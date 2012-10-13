DIR=../css/chinso
if [ $# -ne 1 ]; then
	echo "error"
fi
./do.sh $DIR && $DIR $1 1 1 1
