BENCHMARK=(perlbench bzip2 gcc mcf cactusADM leslie3d soplex hmmer libquantum lbm omnetpp sphinx3 xalancbmk)

if [ $# != 3 ]
then
	echo "Usage: ./hint.sh <benchmark_name> <mode> <lookahead_length>"
else
	PROG=$1
	MODE=$2
	SCALE=$3


	if [ ${PROG} == "all" ]
	then
		echo "You have entered: all"
		for i in "${BENCHMARK[@]}"
		do
			echo ">> Preprocessing: $i"
			./hintgenerator $i ${MODE} ${SCALE}
			sleep 2
		done
	else
		echo ">> Preprocessing: ${PROG}"
		./hintgenerator ${PROG} ${MODE} ${SCALE}
	fi
fi


#speaker-test -t sine -f 1000 -l 1 -r 180000 &> /dev/null

