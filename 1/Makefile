all: parallel mpi clean

parallel:
	g++ -std=c++11 Parallel.cpp -lgomp -o parallel.exe; ./parallel.exe > parallel.txt

mpi:
	mpicc mpi.cpp -o mpi.exe; mpirun -np 2 ./mpi.exe; rm -rf mpi.exe

clean:
	rm *.exe sample.txt
