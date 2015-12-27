echo '/src/:' $(cat $(find ./src/ -name '*.cpp' -or -name '*.h') | wc -l) lines
