#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_SIZE 1000

typedef struct
{
	int *arr;
	int i;
	int j;
} ThreadData;

int active_threads = 0;
int max_threads = 2;
pthread_mutex_t mutex;

void *compare_and_swap(void *arg)
{
	ThreadData *data = (ThreadData *)arg;

	pthread_mutex_lock(&mutex);
	while (active_threads >= max_threads)
	{
		pthread_mutex_unlock(&mutex);
		usleep(100); // ждем освобождения потока
		pthread_mutex_lock(&mutex);
	}
	active_threads++;
	pthread_mutex_unlock(&mutex);

	if (data->arr[data->i] > data->arr[data->j])
	{
		int temp = data->arr[data->i];
		data->arr[data->i] = data->arr[data->j];
		data->arr[data->j] = temp;
	}

	pthread_mutex_lock(&mutex);
	active_threads--;
	pthread_mutex_unlock(&mutex);

	free(data);
	return NULL;
}

void odd_even_sort(int *arr, int n)
{
	int sorted = 0;

	while (!sorted)
	{
		sorted = 1;
		pthread_t threads[MAX_SIZE]; // наши потоки
		int thread_count = 0;

		for (int i = 0; i <= n - 2; i += 2)
		{
			ThreadData *data = malloc(sizeof(ThreadData)); // выделяем данные для передачи в поток
			data->arr = arr;
			data->i = i;
			data->j = i + 1;
			pthread_create(&threads[thread_count++], NULL, compare_and_swap, data);
			// здесь функция принимает идентификатор нового потока, атрибуты потока,
			// функцию потока и аргумент для этой функции
			// после запуска поток сразу начинает выполнение compare_and_swap
		}

		for (int i = 0; i < thread_count; i++)
			pthread_join(threads[i], NULL);
		// функция ждет завершения каждого потока, для перехода к следующей фазе

		thread_count = 0;
		for (int i = 1; i <= n - 2; i += 2)
		{
			ThreadData *data = malloc(sizeof(ThreadData));
			data->arr = arr;
			data->i = i;
			data->j = i + 1;
			pthread_create(&threads[thread_count++], NULL, compare_and_swap, data);
		}

		for (int i = 0; i < thread_count; i++)
			pthread_join(threads[i], NULL);

		// Отсортирован ли массив
		for (int i = 0; i < n - 1; i++)
		{
			if (arr[i] > arr[i + 1])
			{
				sorted = 0;
				break;
			}
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Использование: %s <размер массива> <макс потоков>\n", argv[0]);
		return 1;
	}

	int n = atoi(argv[1]);
	max_threads = atoi(argv[2]);
	if (n > MAX_SIZE)
		n = MAX_SIZE;

	int arr[MAX_SIZE];
	srand(time(NULL));
	// инициализатор генерации чисел ( задает seed, по которому генерируются псевдослучайные числа )
	for (int i = 0; i < n; i++)
		arr[i] = rand() % 1000;

	printf("Исходный массив:\n");
	for (int i = 0; i < n; i++)
		printf("%d ", arr[i]);
	printf("\n");

	pthread_mutex_init(&mutex, NULL); // инициализируем наш mutex

	clock_t start = clock();
	odd_even_sort(arr, n);
	clock_t end = clock();

	pthread_mutex_destroy(&mutex); // удаляем mutex после завершения работы

	printf("Отсортированный массив:\n");
	for (int i = 0; i < n; i++)
		printf("%d ", arr[i]);
	printf("\n");

	printf("Время сортировки: %.3f секунд\n", (double)(end - start) / CLOCKS_PER_SEC);

	return 0;
}
