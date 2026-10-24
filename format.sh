git ls-files '*.cpp' '*.cc' '*.h' '*.hpp' | xargs clang-format -i --style=file
