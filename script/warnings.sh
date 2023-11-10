INCLUDE="-Iimage -Isolver -Igui -Iai -Iutils"
LIBS="$(pkg-config --cflags gtk+-3.0) $(pkg-config --cflags zlib)"
FLAGS="-std=c99"

for file in $(find . -name *.c)
do
	clang-tidy $file -- $INCLUDE $LIBS $FLAGS
done


for file in $(find . -name *.h)
do
	clang-tidy $file -- $INCLUDE $LIBS $FLAGS
done
