#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#define MAX_WORKERS 100
#define MAX_PIVOTS 10

struct Pivot
{
	int index;
	int value;
};

struct WorkerData
{
	int id;
	int* start;
	int n;
	int size;
};

int size_array;
int* g_arrayData;
int num_workers;
pthread_t workers[MAX_WORKERS];

struct WorkerData g_workerData[MAX_WORKERS];
pthread_attr_t g_attr;

int g_activeWorkers = 0;
pthread_mutex_t g_lock;

double g_startTime;
double g_finalTime;

void initWorkerData()
{
	for (int i = 0; i < num_workers; i++) {
		g_workerData[i].id = 0;
		g_workerData[i].start = 0;
		g_workerData[i].n = 0;
		g_workerData[i].size = 0;
	}
}

int compare(const void* a, const void* b)
{
	return (*(int*)a - *(int*)b);
}

// Функция перестановки двух чисел
void swap(int* a, int* b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

// функция сравнения, которая сортирует значения в порядке возрастания
int comparePivot(const void* a, const void* b)
{
	return (((struct Pivot*)a)->value - ((struct Pivot*)b)->value);
}

// выберите MAX_PIVOTS случайных pivot и выберите одну из них 
int getPivot(int* start, int n)
{
	struct Pivot pivots[MAX_PIVOTS];
	int maxPivots = (MAX_PIVOTS > n) ? n : MAX_PIVOTS;

	for (int i = 0; i < maxPivots; i++){
		int index = rand() % n;
		pivots[i].index = index;
		pivots[i].value = start[index];
	}

	// сортируем pivot
	qsort(&pivots[0], maxPivots, sizeof(struct Pivot), comparePivot);

	int pivot = pivots[maxPivots / 2].index;
#ifdef DEBUG
	printf("pivot is %d\n", pivot);
	printf("pivot value is %d\n", start[pivot]);
#endif
	return pivot;
}

void parallel_quicksort(int* start, int n, int size);

// точка входа потока
void* startThread(void* data)
{
	struct WorkerData* p = (struct WorkerData*)data;
	int id = p->id;

#ifdef DEBUG
	printf("worker %d (pthread id %lu) has started\n", id, pthread_self());
#endif

	parallel_quicksort(p->start, p->n, p->size);
	pthread_exit(0);
}

// Параллельная quicksort принимает массив, начинающийся с указателя start и содержащий количество элементов size
void parallel_quicksort(int* start, int n, int size)
{
	// блокируем мьютекс, чтобы проверить количество работников
	pthread_mutex_lock(&g_lock);
	if (g_activeWorkers < num_workers){
		// получаем id
		pthread_t worker = g_activeWorkers;
		g_activeWorkers++;
		pthread_mutex_unlock(&g_lock);

		// получаем pivot
		int pivotIndex = getPivot(start, n);
		int right = n - 1;
		
		// перемещаем pivot в конец
		swap(&start[pivotIndex], &start[right]);

		int storeIndex = 0;
		for (int i = 0; i < right; i++){
			if (start[i] < start[right]){
				swap(&start[i], &start[storeIndex]);
				storeIndex++;
			}
		}
		swap(&start[storeIndex], &start[right]);
		pivotIndex = storeIndex;

		// делим текущий массив на два подмассива
		g_workerData[worker].id = worker;
		g_workerData[worker].start = start + pivotIndex;
		g_workerData[worker].n = n - pivotIndex;
		g_workerData[worker].size = size;

		// создаем отдельный поток для работы с одним из вложенных массивов
		pthread_create(&workers[worker], &g_attr, startThread, (void*)&g_workerData[worker]);

		// этот поток работает с другим вложенным массивом
		parallel_quicksort(start, pivotIndex, size);

		// дождаться завершения другого потока
		void* status;
		int rc = pthread_join(workers[worker], &status);
		if (rc){
			printf("ERROR; return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
#ifdef DEBUG
		printf("Main: completed join with worker %ld (pthread id %lu) having a status of %ld\n",
			worker, workers[worker], (long)status);
#endif
	}
	else{
		pthread_mutex_unlock(&g_lock);
		// нет доступных потоков, выполняем обычную сортировку
		qsort(start, n, sizeof(int), compare);
	}
}

double readTimer()
{
	static bool initialized = false;
	static struct timeval start;
	struct timeval end;
	if (!initialized){
		gettimeofday(&start, NULL);
		initialized = true;
	}
	gettimeofday(&end, NULL);
	return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

int main(int argc, const char* argv[])
{
	if (argc > 1)
		num_workers = atoi(argv[1]);
	else
		printf("Need number of workers....\n");

	// Инициализируем данные
	initWorkerData();

	// Чтение данных
	FILE* open_file = fopen("input.txt", "r+");
	if (open_file){
		fscanf(open_file, "%d", &size_array);

		g_arrayData = (int*)malloc(size_array * sizeof(int));
		for (int i = 0; i < size_array; i++){
			fscanf(open_file, "%d", &g_arrayData[i]);
		}
		fclose(open_file);
	}

	// Устанвливаем глобальные атрибуты потока
	pthread_attr_init(&g_attr);
	pthread_attr_setscope(&g_attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&g_attr, PTHREAD_CREATE_JOINABLE);

	// Инициализируем мьютекс
	pthread_mutex_init(&g_lock, NULL);

	// Инициализируем таймер и установливаем время начала
	g_startTime = readTimer();

	// Начинаем прараллельную quicksort
	parallel_quicksort(&g_arrayData[0], size_array, sizeof(int));

	// Остановка таймера
	g_finalTime = readTimer() - g_startTime;

	//printf("The execution time is %g sec\n", g_finalTime);
	printf("Quicksort %d ints on %d procs: %g secs\n",
		size_array, num_workers, g_finalTime);

	// Выводим массив в файл
	FILE* close_file = fopen("pthread.txt", "w+");
	for (int i = 0; i < size_array; i++) {
		fprintf(close_file, "%d ", g_arrayData[i]);
	}

#ifdef DEBUG
	printf("Sorted array is: \n");
	for (int i = 0; i < size_array; i++) {
		printf("%d ", g_arrayData[i]);
	}
	printf("\n");
#endif

	return 0;
}