from mpi4py import MPI
import numpy as np
import time
from operator import itemgetter
import psutil

comm = MPI.COMM_WORLD
size = comm.Get_size()
rank = comm.Get_rank()

def partition(array, low, high):
    pivot = array[high]
    i = low - 1

    for j in range(low, high):
        if array[j] <= pivot:
            i = i + 1
            (array[i], array[j]) = (array[j], array[i])

    (array[i + 1], array[high]) = (array[high], array[i + 1])

    return i + 1

def quickSort(array, low, high):
    if low < high:
        pi = partition(array, low, high)

        quickSort(array, low, pi - 1)
        quickSort(array, pi + 1, high)

if rank == 0:
    numbers = np.load(f'input.npy')
    arraySize = len(numbers)
    chunks = np.array_split(numbers, size)
else:
    chunks = None

chunk = comm.scatter(chunks, root=0)

# Start timer
start = time.time()

quickSort(chunk, 0, len(chunk) - 1)

end = time.time()

gathered = comm.gather(chunk, root=0)
elapsed = end - start
gatheredTime = comm.gather(elapsed, root=0)

# Sorted array
Arrays = comm.gather(chunk, root=0)
if rank == 0:
    iteratorNumbers = np.zeros((len(Arrays),), dtype=int)
    sortedArray = []
    for myIndex in range(0, int(arraySize)):
        iterator = [
            (i, (99999999 if iteratorNumbers[i] >= len(Arrays[i]) else Arrays[i][iteratorNumbers[i]])) for i
            in range(0, len(Arrays))]
        res = min(iterator, key=itemgetter(1))
        iteratorNumbers[res[0]] = iteratorNumbers[res[0]] + 1
        sortedArray.append(res[1])
        iterator = []

    np.savetxt('mpi_python.txt', sortedArray, fmt='%3.0d')
    totalTime = sum(gatheredTime)
    averageTime = totalTime / comm.size
    print('Quicksort', arraySize, 'ints on', size, 'procs:', averageTime, 'secs')