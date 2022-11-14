format:
	cd tools && find . -name "*.cpp" -not -path "*/build/*" | xargs clang-format -i
	cd modules && find . -name "*.cpp" -not -path "*/build/*" | xargs clang-format -i
	cd modules && find . -name "*.h" -not -path "*/build/*" | xargs clang-format -i
	cd modules && find . -name "*.h" -not -path "*/build/*" | xargs sed -i -- "s=}// namespace =\} // namespace =g"
	cd modules && find . -name "*.cpp" -not -path "*/build/*" | xargs sed -i -- "s=}// namespace =\} // namespace =g"
	cd modules && find . -name "*.h" -not -path "*/build/*" | xargs sed -i -- "s=\    // ---=\// ---=g"
	cd modules && find . -name "*.cpp" -not -path "*/build/*" | xargs sed -i -- "s=\    // ---=\// ---=g"
	cd modules && find . -name "*.h" -not -path "*/build/*" | xargs sed -i -- "s=\  // ---=\// ---=g"
	cd modules && find . -name "*.cpp" -not -path "*/build/*" | xargs sed -i -- "s=\  // ---=\// ---=g"
	cd modules && find . -name "*.cpp" -not -path "*/build/*" | xargs gawk -i inplace 'NF > 0 {blank=0} NF == 0 {blank++} blank < 2'i
	cd demos && find . -name "*.cpp" -not -path "*/build/*" | xargs clang-format -i
	cd demos && find . -name "*.h" -not -path "*/build/*" | xargs clang-format -i
	cd demos && find . -name "*.h" -not -path "*/build/*" | xargs sed -i -- "s=}// namespace =\} // namespace =g"
	cd demos && find . -name "*.cpp" -not -path "*/build/*" | xargs sed -i -- "s=}// namespace =\} // namespace =g"
	cd demos && find . -name "*.h" -not -path "*/build/*" | xargs sed -i -- "s=\    // ---=\// ---=g"
	cd demos && find . -name "*.cpp" -not -path "*/build/*" | xargs sed -i -- "s=\    // ---=\// ---=g"
	cd demos && find . -name "*.h" -not -path "*/build/*" | xargs sed -i -- "s=\  // ---=\// ---=g"
	cd demos && find . -name "*.cpp" -not -path "*/build/*" | xargs sed -i -- "s=\  // ---=\// ---=g"
	cd demos && find . -name "*.cpp" -not -path "*/build/*" | xargs gawk -i inplace 'NF > 0 {blank=0} NF == 0 {blank++} blank < 2'i

.PHONY: format
