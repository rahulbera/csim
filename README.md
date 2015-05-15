===================================================================
								CSIM
						Offline Cache Simulator
===================================================================


1.	All the files names are self explanatory.
	<policy_name>.h file porvides the header, and corresponding .cpp file implements it.

2.	Main file is main.cpp

3.	preprocess.cpp is the preprocessor for the OPT policy.

4.	To compile, execute the following commads from the simulator directory:
	
			$ make clean
			$ make

	It will generate two executables, namely "preprocessor" and "csim"

5.	You can run simulator as
		
			./csim <benchmark_name> <size_mode>

	size_mode = 1 means 512KB cache, 2 means 1MB and so on.

6.	For automation, you can use run.sh script. Just run:
		
			$ bash run.sh <benchmark_name OR all> <mode OR all>

	Giving all as parameter will consider all benchmarks or all size modes.

7.	It needs trace cache file. All trace cache file should be saved in side a new directory named binaries.
	Trace file should be named in a particular fashion: <benchmark_name>_trace.binary

8.	To run OPT, it needs another set of preprocessed file. Those preprocessing can be done by preprocessor

			$ preprocessor <benchmark_name>

	For automation, the preprocess.sh script can be useful.
