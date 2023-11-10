
for file in $(find . -name *.c)
do
	echo $file
	clang-format -i -style=file $file
done

for file in $(find . -name *.h)
do
	echo $file
	clang-format -i -style=file $file
done

