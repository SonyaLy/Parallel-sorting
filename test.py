import numpy as np


def test_pthread(expected_c):
    actual_c = np.loadtxt("pthread.txt")
    actual_c = [int(item) for item in actual_c]
    if np.array_equal(actual_c, expected_c[0]):
        print("test_pthread COMPLETED")
    else:
        print("test_pthread FAILED")


def test_mpi_c(expected_c):
    actual_c = np.loadtxt("mpi_c.txt")
    actual_c = [int(item) for item in actual_c]
    if np.array_equal(actual_c, expected_c[0]):
        print("test_mpi_c COMPLETED")
    else:
        print("test_mpi_c FAILED")


def test_mpi_python(expected_c):
    actual_c = np.loadtxt("mpi_python.txt")
    actual_c = [int(item) for item in actual_c]
    if np.array_equal(actual_c, expected_c[0]):
        print("test_mpi_python COMPLETED")
    else:
        print("test_mpi_python FAILED")

def test_openmp(expected_c):
    actual_c = np.loadtxt("openmp.txt")
    actual_c = [int(item) for item in actual_c]
    if np.array_equal(actual_c, expected_c[0]):
        print("test_openmp COMPLETED")
    else:
        print("test_openmp FAILED")


if __name__ == '__main__':
    #array = np.loadtxt("input.txt")

    array = []

    with open("input.txt") as f:
        size = f.readline()
        array.append([int(x) for x in f.readline().split()])

    expected_c = np.sort(array)


    test_pthread(expected_c)
    test_mpi_c(expected_c)
    test_mpi_python(expected_c)
    test_openmp(expected_c)

    f.close