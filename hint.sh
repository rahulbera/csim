BENCHMARK=(perlbench bzip2 gcc cactusADM leslie3d soplex hmmer libquantum omnetpp sphinx3 xalancbmk)

if [ $# != 2 ]
then
	echo "Usage: ./preprocess.sh <benchmark_name> <lookahead_length>"
else
	PROG=$1
	SCALE=$2

	if [ ${PROG} == "all" ]
	then
		echo "You have entered: all"
		for i in "${BENCHMARK[@]}"
		do
			echo ">> Preprocessing: $i"
			./hintgenerator $i ${SCALE}
			sleep 2
		done
	else
		echo ">> Preprocessing: ${PROG}"
		./hintgenerator ${PROG} ${SCALE}
	fi
fi


#speaker-test -t sine -f 1000 -l 1 -r 180000 &> /dev/null

