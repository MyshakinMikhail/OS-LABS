#pragma once
#include <semaphore.h>

#define SHM_SIZE 1024

// префиксы имён для shared memory и семафоров
#define SHM_NAME_PREFIX "/my_shm_"
#define SEM_WRITE_PREFIX "/sem_write_"
#define SEM_READ_PREFIX "/sem_read_"
