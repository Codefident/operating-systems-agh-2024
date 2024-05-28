#include "santaClaus.h"

int main()
{
    srand(time(NULL) + getpid());

    int reindeers_ids[N_REINDEERS];

    pthread_create(&santa_thread, NULL, santa_handler, NULL);
    for (int i = 0; i < N_REINDEERS; i++)
    {
        reindeers_ids[i] = i;
        pthread_create(&reindeers_threads[i], NULL, reindeer_handler, &reindeers_ids[i]);
    }

    pthread_join(santa_thread, NULL);
    for (int i = 0; i < N_REINDEERS; i++)
        pthread_join(reindeers_threads[i], NULL);

    return EXIT_SUCCESS;
}

void *santa_handler(void *arg)
{
    for (int i = 0; i < N_CYCLES; i++)
    {
        pthread_cond_wait(&santa_cond, &santa_mutex);

        printf("...\nMikolaj: budze sie\nMikolaj: dostarczam zabawki\n");
        sleep(rand() % 3 + 2);

        for (int j = 0; j < N_REINDEERS; j++)
            pthread_mutex_unlock(&reindeers_mutex[j]);

        printf("Mikolaj: zasypiam\n...\n");
    }

    for (int i = 0; i < N_REINDEERS; i++)
        pthread_cancel(reindeers_threads[i]);

    return NULL;
}

void *reindeer_handler(void *arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    int id = *((int *)arg);

    pthread_mutex_lock(&reindeers_mutex[id]);
    while (1)
    {
        sleep(rand() % 6 + 5);

        pthread_mutex_lock(&reindeers_counter_mutex);
        reindeers_counter++;
        printf("Renifer: czeka %d reniferow na Mikolaja, %d\n", reindeers_counter, id);

        if (reindeers_counter == N_REINDEERS)
        {
            printf("Renifer: wybudzam Mikolaja, %d\n", id);
            pthread_cond_signal(&santa_cond);
            reindeers_counter = 0;
        }

        pthread_mutex_unlock(&reindeers_counter_mutex);

        pthread_mutex_lock(&reindeers_mutex[id]);

        printf("Renifer: lece na wakacje, %d\n", id);
    }

    return NULL;
}
