#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include "common.h"

int main()
{
	char shm_name[64], sem_write_name[64], sem_read_name[64];
	pid_t pid = getpid();

	// создаёт уникальное имя для общей памяти
	snprintf(shm_name, sizeof(shm_name), "%s%d", SHM_NAME_PREFIX, pid);
	snprintf(sem_write_name, sizeof(sem_write_name), "%s%d", SEM_WRITE_PREFIX, pid);
	snprintf(sem_read_name, sizeof(sem_read_name), "%s%d", SEM_READ_PREFIX, pid);

	// создаёт именованный объект общей памяти в ядре; 0666 - остальные процессы имеют права на чтение и запись
	int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
	if (shm_fd == -1)
	{
		perror("shm_open");
		exit(1);
	}

	// задает размер области общей памяти в ядре
	ftruncate(shm_fd, SHM_SIZE);

	// создает виртуальную память с которой взаимодействует программа, а также таблицу MMU для соответствия учатсков памяти.
	char *shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); // memorymap
	{
		perror("mmap");
		exit(1);
	}

	// создаёт или открывает именованный семафор с разрешением на запись
	sem_t *sem_write = sem_open(sem_write_name, O_CREAT, 0666, 1);

	// создаёт или открывает именованный семафор без разрешения на чтение
	sem_t *sem_read = sem_open(sem_read_name, O_CREAT, 0666, 0);
	if (sem_write == SEM_FAILED || sem_read == SEM_FAILED)
	{
		perror("sem_open");
		exit(1);
	}

	printf("Введите имя файла с числами: ");
	char filename[256];
	scanf("%255s", filename);

	// открываем файл для чтения
	int file_fd = open(filename, O_RDONLY);
	if (file_fd == -1)
	{
		perror("open");
		exit(1);
	}

	// создаём дочерний процесс
	pid_t child_pid = fork();
	if (child_pid == -1)
	{
		perror("fork");
		exit(1);
	}

	if (child_pid == 0)
	{
		// перенаправляем ввод дочернего процесса из файла
		dup2(file_fd, STDIN_FILENO);
		close(file_fd);

		// заменяет выполнение текущего процесса на дочерний и передает ему соответствующие аргументы
		execl("./child", "child", shm_name, sem_write_name, sem_read_name, NULL);
		perror("execl");
		exit(1);
	}

	close(file_fd);

	// родитель читает данные из shared memory
	printf("\nРезультаты вычислений:\n");

	while (1)
	{
		// ждём пока ребёнок что-то запишет
		sem_wait(sem_read); // переводим его в 0, то есть родитель теперь может читать, но внешние процессы не могут этого делать

		if (strcmp(shm_ptr, "END") == 0)
			break;

		printf("%s\n", shm_ptr);

		// разрешаем следующую запись
		sem_post(sem_write);
	}

	// очищаем ресурсы
	wait(NULL);
	munmap(shm_ptr, SHM_SIZE);
	close(shm_fd);

	shm_unlink(shm_name);
	sem_unlink(sem_write_name);
	sem_unlink(sem_read_name);

	printf("\nРабота завершена.\n");
	return 0;
}
