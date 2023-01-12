# Parallel "QuickSort" algorithm
Laboratory work on the course supercomputers SPbPU

The QuickSort algorithm is known as one of the fastest and most efficient sorting algorithms. Quicksort uses a divide-and-conquer approach. Its separation aspects make QuickSort possible for parallelization.

## How it works:

### Uploading data to a supercomputer
pscp -P 22 -i C:\Users\Public\super_key.ppk -r "C:\Users\Public\..." tm5u...@login1.hpc.spbstu.ru:...

### Data generation
python3 gen_data.py 1000 input.txt

### MPI:
mpicc Quick_sort_MPI.cpp -o Quick_sort_MPI

mpirun -np 4 ./Quick_sort_MPI input.txt mpi_c.txt

### OpenMP:
gcc -fopenmp Quick_sort_OpenMP.cpp -o Quick_sort_OpenMP

//// export OMP_NUM_THREADS=8

./Quick_sort_OpenMP

### Pthread:
gcc Quick_sort_pthread.cpp -o Quick_sort_pthread -lpthread

./Quick_sort_pthread

### MPI Python
mpirun -n 4 python3 python_mpi.py

### Testing
python3 test.py

### Loading modules
module load mpi/openmpi/4.0.1/gcc/8

module load mpi/impi/5.1.3.210

module load python/3.9

### Comand on supecomputers
sinfo

salloc -N1 -p tornado-k40 -t 00:10:00

squeue

scancel ...

logout
