import argparse
import os.path
import re

verbose = False
defaultLineLength = 79

class OpenSourceFile(object):

    def __init__(self, src_file):
        self.src_file = src_file

    def modify(self, text):
        with open(self.src_file, "r", encoding="UTF8") as file:
            file_content = file.read()
            return file_content


class MergeHeaderModifier(object):

    def __init__(self, src_file, include_paths):
        self.src_file = src_file
        self.include_regex = re.compile(
            r'''#\s*include\s+["<]([^">]+)[">]\s*''')
        self.include_paths = include_paths
        self.include_file_map = set()

    def resolve_include(self, include_name):
        if os.path.isabs(include_name):
            return include_name
        for include_path in self.include_paths:
            if os.path.exists(os.path.join(include_path, include_name)):
                return os.path.abspath(os.path.join(include_path, include_name))
        return None

    def resolve(self, current_file, text):
        lines = text.splitlines()
        line_number = 1
        text = "#line {} \"{}\"".format(line_number, current_file)
        for line in lines:
            if line.strip().startswith("#pragma once"):
                text = text + "\n" + "// PROTEST: #pragma once"
            else:
                include_match = self.include_regex.fullmatch(line)
                if include_match:
                    include_file = include_match.group(1)
                    resolved_include_file = self.resolve_include(include_file)
                    if resolved_include_file:
                        if resolved_include_file not in self.include_file_map:
                            self.include_file_map.add(resolved_include_file)
                            if verbose:
                                print("resolved -> " + resolved_include_file)
                            with open(resolved_include_file, "r", encoding="UTF8") as file:
                                include_text = self.resolve(
                                    resolved_include_file, file.read())
                                text = text + "\n" + include_text
                                text = text + \
                                    "// PROTEST: end of file {}\n".format(
                                        resolved_include_file)
                                text = text + "\n" + \
                                    "#line {} \"{}\"".format(
                                        line_number, current_file)
                        else:
                            # include file already merged
                            # #include can be removed -> do not add current line
                            text = text + "\n"
                    else:
                        # header file was not -> must be external header like string.h
                        # add line so that it exists when actually compiling the protest cpp file
                        text = text + "\n" + line
                else:
                    # a source code line -> add it
                    text = text + "\n" + line
            line_number += 1
        return text

    def modify(self, text):
        text = "#define PROTEST_ORIGIN_FILE (\"{}\")\n".format(
            self.src_file) + text
        return self.resolve(self.src_file, text)


def split(list_a, chunk_size):
    for i in range(0, len(list_a), chunk_size):
        yield list_a[i:i + chunk_size]


class PrivateToPublicModifier(object):

    def handle_block(self, block):
        if len(block) == 1:
            # suffix
            return block[0]
        else:
            if block[1]:
                # match_block_comment
                return block[0] + block[1]
            if block[3]:
                # match_string_literal
                return block[0] + block[3]
            if block[5]:
                # match_private
                return block[0] + block[6] + "public" + block[8]
            if block[9]:
                # match line comment
                return block[0] + block[9]
        raise ValueError("block must have a match or must be a suffix")

    def modify(self, text):
        # TODO (jreinking) a comment can also look like:
        #
        # // this a comment \
        # this is still part of the comment and > private: < will be replaced
        # 
        # currently the regex will not match this as comment?

        # do not replace string 'private' in comments
        match_block_comment = "(\\/\\*((?!\\*\\/).)*\\*\\/)"
        # do not replace string 'private' in line comments
        match_line_comment = "(\\/\\/[^\\n]*\\n)"
        # do not replace string 'private' in string literals
        match_string_literal = "(\"([^\"\\\\]|\\\\.)*\")"
        # replace it in the rest of the document
        match_private = "(([^a-zA-Z])(private)(\\s*:))"
        regex = "(?s){}|{}|{}|{}".format(match_block_comment,
                                         match_string_literal, match_private, match_line_comment)
        regex = re.compile(regex)
        blocks = re.split(regex, text)
        blocks = list(split(blocks, 10))
        new_blocks = []
        for b in blocks:
            new_blocks.append(self.handle_block(b))
        return "".join(new_blocks)


class PrintCommendModifier(object):

    def handle_block(self, block, limit):
        new_lines = []
        new_line = ""
        for word in block:
            if new_line == "":
                new_line = word
            elif len(new_line) + len(word) + 1 <= limit:
                new_line = new_line + " " + word
            else:
                new_lines.append(new_line)
                new_line = word
        if new_line != "":
            new_lines.append(new_line)
        return new_lines

    def modify(self, text):
        matches = re.split(
            '([\t ]*/\*\* @info(\(format\))?\n(((?!\*/).)*)?\*/)', text, flags=re.S)
        text = ""
        while len(matches) > 3:
            limit = 0
            if matches[2]:
                limit = defaultLineLength
            m = matches[1]
            ms = m.split("\n")
            n = len(ms)
            ms = ms[1:]
            ms = ms[:-1]
            i = 0
            for line in ms:
                line = re.sub('^([ \t]*)\*[ \t]*', '',
                              line, flags=re.MULTILINE)
                line = line.strip()
                if "@seperator" == line:
                    line = "-" * defaultLineLength
                ms[i] = line
                i += 1
            if limit:
                new_lines = []
                block = []
                for line in ms:
                    if line == "":
                        new_lines += self.handle_block(block, limit)
                        new_lines.append(line)
                        block = []
                    else:
                        block += line.split()
                new_lines += self.handle_block(block, limit)
                ms = new_lines

            m = "\n".join(ms)
            text += matches[0] + "logger() << \"" + \
                m.replace("\n", "\\n") + "\" << \"\\n\";"
            for j in range(n - 1):
                text += "\n"
            matches = matches[5:]
        text += matches[0]
        return text


class Cpp11AnnotationToClangAnnotation(object):

    def handle_block(self, block):
        if len(block) == 1:
            # suffix
            return block[0]
        else:
            if block[1]:
                # match_block_comment
                return block[0] + block[1]
            if block[3]:
                # match_string_literal
                return block[0] + block[3]
            if block[5]:
                # match_annotation
                ms = block[7].split("\n")
                # TODO (jreinking) what about already escaped '"'?
                retval = block[0] + "PROTEST_HIDE([[clang::annotate(\"" + block[7].replace("\n", "").replace(" ", "").replace("\"", "\\\"") + "\")]])"
                for j in range(len(ms) - 1):
                    retval += "\n"
                return retval
            if block[8]:
                # match line comment
                return block[0] + block[8]
        raise ValueError("block must have a match or must be a suffix")

    def modify(self, text):
        # TODO (jreinking) code duplication
        # TODO (jreinking) a comment can also look like:
        #
        # // this a comment \
        # this is still part of the comment and > private: < will be replaced
        # 
        # currently the regex will not match this as comment?

        # do not replace string 'private' in comments
        match_block_comment = "(\\/\\*((?!\\*\\/).)*\\*\\/)"
        # do not replace string 'private' in line comments
        match_line_comment = "(\\/\\/[^\\n]*\\n)"
        # do not replace string 'private' in string literals
        match_string_literal = "(\"([^\"\\\\]|\\\\.)*\")"
        # replace it in the rest of the document
        match_annotation = "((\[\[([^\]]*)\]\]))"
        regex = "(?s){}|{}|{}|{}".format(match_block_comment,
                                         match_string_literal,
                                         match_annotation,
                                         match_line_comment)
        regex = re.compile(regex)
        blocks = re.split(regex, text)
        blocks = list(split(blocks, 9))
        new_blocks = []
        for b in blocks:
            new_blocks.append(self.handle_block(b))
        return "".join(new_blocks)


class PreCompiler(object):

    def __init__(self):
        parser = argparse.ArgumentParser()
        parser.add_argument('--include-path', nargs="*")
        parser.add_argument('--src-file')
        parser.add_argument('--output-file')
        parser.add_argument('-v', '--verbose', action='store_true')

        self.args = parser.parse_args()
        global verbose
        verbose = self.args.verbose
        if not os.path.isabs(self.args.src_file):
            self.args.src_file = os.path.abspath(
                os.path.join(os.curdir, self.args.src_file))

        self.src_include_dir = self.args.include_path
        index = 0
        for include_path in self.args.include_path:
            if not os.path.isabs(include_path):
                include_path = os.path.abspath(
                    os.path.join(os.curdir, include_path))
                self.src_include_dir[index] = include_path
            index += 1
        if verbose:
            print("include paths: {}".format(self.src_include_dir))

        self.parsed = set()
        self.modifiers = []
        self.modifiers.append(OpenSourceFile(self.args.src_file))
        self.modifiers.append(MergeHeaderModifier(
            self.args.src_file, self.src_include_dir))
        self.modifiers.append(PrivateToPublicModifier())
        self.modifiers.append(PrintCommendModifier())
        self.modifiers.append(Cpp11AnnotationToClangAnnotation())
        self.run()

    def run(self):
        with open(self.args.src_file, "r", encoding="UTF8") as src_file:
            text = src_file.read()
            for m in self.modifiers:
                text = m.modify(text)
            with open(self.args.output_file, "w", encoding="UTF8") as dst_file:
                dst_file.write(text)


if __name__ == "__main__":
    PreCompiler()
