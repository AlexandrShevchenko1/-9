#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <queue>

int counter;
int sum_index;
std::queue<int> buffer;
std::vector<pthread_t> sum_threads;
pthread_mutex_t mutexCounter;
pthread_cond_t condCounter;

void *GetSum(void* param) {
    int count = *((int*)param);
    int current_index = sum_index++;
    printf("Summer %d created!\n", current_index) ;
    int t_sum = 3 + rand() % 4;
    int sum = 0;
    sleep(t_sum);
    pthread_mutex_lock(&mutexCounter) ;
    while (count > 0) {
        sum += buffer.front();
        buffer.pop();
        --count;
    }
    buffer.push(sum);
    ++counter;
    if (counter > 1)
        pthread_cond_signal(&condCounter);
    printf("Summer %d: Summa read values = %d\n", current_index, sum);
    pthread_mutex_unlock(&mutexCounter);
    return nullptr;
}


void *Creater(void* param) {
    int num = *((int*)param);
    int time = 1 + rand()%7;
    sleep(time);
    int data = 1;
    pthread_mutex_lock(&mutexCounter);
    buffer.push(data);
    counter++;
    if (counter > 1)
        pthread_cond_signal(&condCounter);
    printf("Creater %d / Created value = %d\n", num, data);
    pthread_mutex_unlock(&mutexCounter);
}


void *Processor(void* args) {
    int two = 2;
    while (true) {
        pthread_t newSumer;
        pthread_mutex_lock(&mutexCounter) ;
        while (counter < 2) {
            pthread_cond_wait(&condCounter, &mutexCounter);
        }
        sum_threads.push_back(newSumer);
        pthread_create(&newSumer, nullptr, GetSum, (void *) (&two));
        counter -= 2;
        pthread_mutex_unlock(&mutexCounter);
    }
}

int main() {
    int i;
    pthread_mutex_init(&mutexCounter, nullptr);
    pthread_cond_init(&condCounter, nullptr);

    pthread_t handler;
    pthread_create(&handler, nullptr, Processor, nullptr);

    pthread_t threadP[20];
    int producers[20];
    for (i = 0 ; i < 20; ++i) {
        producers[i] = i + 1;
        pthread_create(&threadP[i], nullptr, Creater, (void *) (producers + i));
    }

    for (i = 0 ; i < 20; ++i) {
        pthread_join(threadP[i], nullptr);
    }

    for(i = 0; i < sum_threads.size(); ++i) {
        pthread_join(sum_threads[i], nullptr);
    }

    sleep(10);
    std::cout << "Result = " << buffer.front();
    pthread_mutex_destroy(&mutexCounter);
    pthread_cond_destroy(&condCounter);
    return 0;
}
