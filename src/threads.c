#include <pthread.h>
#include <stdio.h>


void* thread_worker(void* args);

struct thread_args {
    pthread_mutex_t *mutex;
    int *i;
};

int main(void)
{
    pthread_t thread;
    pthread_mutex_t mutex;
    int i = 0;
    struct thread_args args;
    args.i = &i;
    args.mutex = &mutex;

    printf("Start main\n");

    pthread_create(&thread, NULL, &thread_worker, &args);
    pthread_mutex_init(&mutex, NULL);
    while (1) {
        pthread_mutex_lock(&mutex);
        i += 1;
        pthread_mutex_unlock(&mutex);
    }
    pthread_join(thread, NULL);
    pthread_mutex_destroy(&mutex);
    printf("End main\n");
    return 0;    
}


void* thread_worker(void* args)
{
    struct thread_args *my_args = (struct thread_args*)args;
    while (1) {
        printf("%d ", *my_args->i);
        pthread_mutex_lock(my_args->mutex);
        *my_args->i += 1;
        //pthread_mutex_unlock(my_args->mutex);
    }
}
