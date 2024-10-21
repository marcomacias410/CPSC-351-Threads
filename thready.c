#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

pthread_mutex_t print_mutex;

typedef struct {
    int num;

} thread1_t;

void* pattern_1_child(void* vptr) {
    int random = rand() % 8 + 1;
    thread1_t* stuff = (thread1_t*)vptr;

    pthread_mutex_lock(&print_mutex);

    printf("Process %d (%ld) beginning... sleeping for %d seconds\n",
           stuff->num, pthread_self(), random);

    pthread_mutex_unlock(&print_mutex);

    sleep(random);

    pthread_mutex_lock(&print_mutex);

    printf("Process %d (%ld) exiting  \n", stuff->num, pthread_self());

    pthread_mutex_unlock(&print_mutex);

    free(stuff);

    return NULL;
}

void pattern1(int num) {
    srand(time(NULL));
    int status;

    pthread_t threads[num];
    pthread_mutex_init(&print_mutex, NULL);

    for (int i = 0; i < num; ++i) {
        thread1_t* stuff = (thread1_t*)malloc(sizeof(thread1_t));
        stuff->num = i;

        pthread_create(&threads[i], NULL, pattern_1_child, stuff);
    }

    for (int i = 0; i < num; ++i) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&print_mutex);
}

typedef struct {
    int thread_num;
    int num_of_threads;
    int sleep_time;
    pthread_t child_tid;
} thread_t;

void* pattern_2_child(void* vptr) {
    thread_t* data = (thread_t*)vptr;
    printf("Process %d (%lu): starting\n", data->thread_num, pthread_self());

    if (data->thread_num < data->num_of_threads - 1) {
        thread_t* child_data = malloc(sizeof(thread_t));
        child_data->thread_num = data->thread_num + 1;
        child_data->num_of_threads = data->num_of_threads;
        child_data->sleep_time = data->sleep_time;

        pthread_create(&data->child_tid, NULL, pattern_2_child,
                       (void*)child_data);

        printf(
            "Process %d (%lu), sleeping %d seconds after creating process %d "
            "(%lu)\n",
            data->thread_num, pthread_self(), data->sleep_time,
            child_data->thread_num, data->child_tid);
        sleep(data->sleep_time);

        printf("Child %d (%lu), waiting for child %d (%lu) to finish\n",
               data->thread_num, pthread_self(), child_data->thread_num,
               data->child_tid);
        pthread_join(data->child_tid, NULL);

        free(child_data);
    } else {
        printf("Process %d (%lu) [no process created], sleeping %d seconds\n",
               data->thread_num, pthread_self(), data->sleep_time);
        sleep(data->sleep_time);
    }
    printf("Process %d (%lu) exiting.\n", data->thread_num, pthread_self());

    return NULL;
}

void pattern2(int num) {
    int random = rand() % 8 + 1;

    thread_t* data = malloc(sizeof(thread_t));
    data->thread_num = 0;
    data->num_of_threads = num;
    data->sleep_time = random;

    pthread_t first_thread;
    pthread_create(&first_thread, NULL, pattern_2_child, (void*)data);

    pthread_join(first_thread, NULL);
    printf("Parent: process 0 (%lu) has exited\n", first_thread);

    free(data);
    
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s [123] <num>\n", argv[0]);
        return 1;
    }

    if (argv[1][0] != '1' && argv[1][0] != '2' && argv[1][0] != '3') {
        printf("Invalid pattern number. Gotta be 1/2/3\n");
        return 1;
    }

    int pattern_num = argv[1][0] - '0';

    int num_of_processes;
    if (0 == sscanf(argv[2], "%d", &num_of_processes) || num_of_processes < 1) {
        printf("Invalid number of processes (1+)\n");
        return 1;
    }

    if (pattern_num == 1 && num_of_processes >= 1 && num_of_processes <= 256)
        pattern1(num_of_processes);
    if (pattern_num == 2 && num_of_processes >= 1 && num_of_processes <= 256)
        pattern2(num_of_processes);

    return 0;
}