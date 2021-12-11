all: Parallel mpi clean


Parallel:
	g++ -std=c++11 Parallel.cpp -o Parallel.exe; ./Parallel.exe > Parallel.txt

mpi:
	mpicc mpi.cpp -o mpi.exe; mpirun -np 2 ./mpi.exe; rm -rf mpi.exe

clean:
	rm *.exe sample.txt
