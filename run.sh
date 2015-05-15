BENCHMARK=(perlbench bzip2 gcc mcf cactusADM leslie3d soplex hmmer libquantum lbm omnetpp sphinx3 xalancbmk)
SIZE=(1 2 4 8 16)

if [ $# != 2 ]
then
	echo "Usage: ./run.sh <all> <mode>"
else
	PROG=$1
	MODE=$2

	if [ ${PROG} == "all" ]
	then
		if [ ${MODE} == "all" ]
		then
			for i in "${BENCHMARK[@]}"
			do
				for j in "${SIZE[@]}"
				do
					n=$(($j * 512))
					echo ">> Simulating: $i Cache: $n KB"
					./csim $i $j
				done
			done
		else
			for i in "${BENCHMARK[@]}"
			do
				echo "Simulating: $i Cache: $((${MODE}*512)) KB"
				./csim $i ${MODE}
			done
		fi
	else
		if [ ${MODE} == "all" ]
		then
			for i in "${SIZE[@]}"
			do
				n=$(($i * 512))
				echo ">> Simulating: ${PROG} Cache: $n KB"
				./csim ${PROG} $i
			done
		fi
	fi
fi


speaker-test -t sine -f 1000 -l 1 -r 180000 &> /dev/null

