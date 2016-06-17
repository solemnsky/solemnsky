for file in $(find src/ -type f -name *.h) 
do
  mv $file "$file"pp
done
