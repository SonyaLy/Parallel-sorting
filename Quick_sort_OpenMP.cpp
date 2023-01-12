#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include<sys/time.h>
using namespace std;

// �������� ����� ��������� �� ��� ������� ��������
volatile int arr_volume = 0;
int* volatile arr;

// ������� ������������ ���� �����
void swap(int* a, int* b)
{
	int t = *a;
	*a = *b;
	*b = t;
}

// ������� ��� ���������� ���������� ������� arr[]
int partition(int arr[], int start, int end)
{
	int pivot = arr[end];
	int i = (start - 1);

	// ������������ �������
	for (int j = start; j <= end - 1; j++) {
		if (arr[j] < pivot) {
			i++;
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[end]);

	// ���������� ��������������� ������
	return (i + 1);
}

// ������� ��� ���������� quicksort c �������������� openmp
void quicksort(int arr[], int start, int end, int n)
{
	if (start < end) {

		// ��������� ������� pivot ����� ����������
		int index = partition(arr, start, end);

		// ������������ ������
#pragma omp parallel sections num_threads(n)
		{
#pragma omp section
			{
				// ����� ��������
				quicksort(arr, start, index - 1, n);
			}
#pragma omp section
			{
				// ������ ��������
				quicksort(arr, index + 1, end, n);
			}
		}
	}
}

int main(int argc, char* argv[])
{
	float time_use = 0;
	struct timeval start_time;
	struct timeval end_time;

	int n = atoi(argv[1]);
	//omp_set_num_threads(n);
	
	FILE* open_file = fopen("input.txt", "r+");
	if (open_file)
	{
		fscanf(open_file, "%d", &arr_volume);

		arr = (int*)malloc(arr_volume * sizeof(int));
		for (int i = 0; i < arr_volume; i++)
		{
			fscanf(open_file, "%d", &arr[i]);
		}
		fclose(open_file);
	}

	gettimeofday(&start_time, NULL);
	
	// ����� ������� ���������� � ������������ �����������
	quicksort(arr, 0, arr_volume - 1, n);

	gettimeofday(&end_time, NULL);
	time_use = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec);


	FILE* close_file = fopen("openmp.txt", "w+");
	for (int i = 0; i < arr_volume; i++)
		fprintf(close_file, "%d ", arr[i]);
	
	printf("Quicksort %d ints on %d procs: %lf secs\n",
		arr_volume, n, time_use / 1000000);

	return 0;
}