import argparse
import subprocess
import re
import json


def find_address_of(target, name):
    command = "nm --defined-only --demangle {} | grep \"{}\"".format(target, name)
    print(command)
    nm_process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    nm_output, nm_error = nm_process.communicate()
    print(nm_output)
    string_value = nm_output.decode('utf-8')
    return (list(map(lambda x: int(x[0], 16), map(lambda x: x.split(' '), string_value.split('\n')[0:-1]))))


if __name__ == "__main__":
    print("post executable")
    parser = argparse.ArgumentParser()
    parser.add_argument('--target')
    parser.add_argument('--sources', nargs="*")

    args = parser.parse_args()

    print(args.target)
    print(args.sources)

    protest_files = filter(lambda x: "protest.pt.cpp" in x, args.sources)

    data = {}
    data["protest::dummyVariable"] = find_address_of(args.target, "protest::dummyVariable")
    data["protest::dummyFunction"] = find_address_of(args.target, "protest::dummyFunction")

    for file in protest_files:
        with open(file, 'r') as file:
            content = file.read()
            pattern = r"@protest::getStaticVariable\(\"(.+)\"\)"
            matches = re.findall(pattern, content)
            
            for name in matches:
                data[name] = find_address_of(args.target, name)

    with open(args.target + ".json", "w") as file:
        json.dump(data, file, indent=4)