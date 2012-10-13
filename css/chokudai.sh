DIR=./chinso
if [ $# -ne 2 ]; then
	echo "error"
	exit -1
fi
$DIR $1 1 1 1 1 10000 1 10000 1 10000
$DIR $2 1 1 1 1 10000 1 10000 1 10000
