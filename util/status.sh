for dir in $(find ./src/ -type d) 
do
echo $dir : $(cat $(find $dir -name '*.cpp' -or -name '*.h') | wc -l) lines
done
