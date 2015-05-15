BENCHMARK=(perlbench bzip2 gcc cactusADM leslie3d soplex hmmer libquantum omnetpp sphinx3 xalancbmk)

if [ $# != 1 ]
then
	echo "Usage: ./preprocess.sh <benchmark_name>"
else
	PROG=$1

	if [ ${PROG} == "all" ]
	then
		echo "You have entered: all"
		for i in "${BENCHMARK[@]}"
		do
			echo ">> Preprocessing: $i"
			./preprocessor $i
			sleep 5
		done
	else
		echo ">> Preprocessing: ${PROG}"
		./preprocessor ${PROG}
	fi
fi


speaker-test -t sine -f 1000 -l 1 -r 180000 &> /dev/null

