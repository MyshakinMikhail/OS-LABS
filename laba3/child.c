#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include "common.h"

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		fprintf(stderr, "Usage: %s <shm_name> <sem_write> <sem_read>\n", argv[0]);
		return 1;
	}

	const char *shm_name = argv[1];
	const char *sem_write_name = argv[2];
	const char *sem_read_name = argv[3];

	// подключаемся к существующей shared memory
	int shm_fd = shm_open(shm_name, O_RDWR, 0666);
	if (shm_fd == -1)
	{
		perror("shm_open");
		return 1;
	}

	char *shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (shm_ptr == MAP_FAILED)
	{
		perror("mmap");
		return 1;
	}

	// подключаемся к существующим семафорам
	sem_t *sem_write = sem_open(sem_write_name, 0);
	sem_t *sem_read = sem_open(sem_read_name, 0);
	if (sem_write == SEM_FAILED || sem_read == SEM_FAILED)
	{
		perror("sem_open");
		return 1;
	}

	char line[1024];

	// читаем строки из stdin ( у нас это файл отправленный родителем )
	while (1)
	{
		if (!fgets(line, sizeof(line), stdin))
		{
			if (feof(stdin))
				break; // достигнут конец файла
			continue;  // ошибка чтения — пропускаем
		}

		// убираем возможный '\n' в конце строки
		size_t len = strlen(line);
		if (len > 0 && line[len - 1] == '\n')
			line[len - 1] = '\0';

		// разбираем строку на произвольное количество чисел
		char *ptr = line;
		float sum = 0.0f;
		int hasNumbers = 0;

		while (*ptr)
		{
			char *end;
			float value = strtof(ptr, &end);
			if (end == ptr)
			{
				// если не смогли считать число — двигаемся дальше
				ptr++;
				continue;
			}
			sum += value;
			hasNumbers = 1;
			ptr = end;
		}

		// если в строке были числа — передаём результат в shared memory
		if (hasNumbers)
		{
			char buffer[64];
			snprintf(buffer, sizeof(buffer), "Сумма: %.2f", sum);

			sem_wait(sem_write); // помечаем, что ребенок начал запись shared memory, поэтому блокируем запись от других процессов
			strncpy(shm_ptr, buffer, SHM_SIZE - 1);
			shm_ptr[SHM_SIZE - 1] = '\0';
			sem_post(sem_read); // сигнализируем, что данные готовы, меняем значение семафора на 1, то есть даем родителю возможность читать данные
		}
	}

	// после завершения отправляем сигнал конца
	sem_wait(sem_write);
	strcpy(shm_ptr, "END");
	sem_post(sem_read);

	// освобождаем ресурсы
	munmap(shm_ptr, SHM_SIZE);
	close(shm_fd);
	return 0;
}
