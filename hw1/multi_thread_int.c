/*
compile: gcc -o multi_thread_int.out multi_thread_int.c -lpthread
run: $ ./multi_thread_int.out [datasize] [thread_num]

NOTE: (datasize <= 100000000, thread_num <= 20)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_THREAD_NUM 20
#define MAX_ARRAY_NUM 100000000
#define key 13

int buffer[MAX_ARRAY_NUM];
int count = 0;

// 加入 Mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// 子執行緒函數
void *child(void *arg) {
    int *input = (int *) arg; // 取得資料
    int thread_count = 0;
    for(int i = input[0]; i < input[1]; i++){
        if(buffer[i] == key){
            thread_count++;
        }
    }
    pthread_mutex_lock( &mutex ); // 上鎖
    count += thread_count;
    pthread_mutex_unlock( &mutex ); // 解鎖

    pthread_exit(NULL);
}

void fill_rand_num(int datasize){
    for(int i = 0; i < datasize; i++){
        buffer[i] = rand() % 1000;
    }
}

int main(int argc, char *argv[]){
    
    struct timeval start, end;
    unsigned long diff;
    unsigned long total_time = 0;
    int datasize = atoi(argv[1]); //input argument of datasize
    int thread_num = atoi(argv[2]); //input argument of thread_num

    if(datasize > MAX_ARRAY_NUM){
        datasize = MAX_ARRAY_NUM;
    }
    if(thread_num > MAX_THREAD_NUM){
        thread_num = MAX_THREAD_NUM;
    }
    printf("Test setting: datasize=%u, thread_num=%d\n", datasize, thread_num);

    fill_rand_num(datasize);

    int seg_input[thread_num][2];
    for(int i = 0; i < thread_num; i++){
        seg_input[i][0] = (datasize / thread_num) * i;
        if(i != thread_num - 1){
            seg_input[i][1] = (datasize / thread_num) * (i + 1);
        }else{
            seg_input[i][1] = datasize;
        }
    }

    for(int repeat_time = 0; repeat_time < 10; repeat_time++){
        count = 0;
        pthread_t t[thread_num];
        gettimeofday(&start, NULL);
        for(int i = 0; i < thread_num; i++){
            pthread_create(&t[i], NULL, child, (void*) seg_input[i]);
        }
        for(int i = 0; i < thread_num; i++){
            pthread_join(t[i], NULL);
        }
        gettimeofday(&end, NULL);

        printf("Integer %d occurs %d times in the array.\n", key, count);
        diff = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
        printf("Compute time: %luus.\n\n", diff);
        total_time += diff;
    }
    printf("Average time cost: %.2fus\n", (total_time / 10.0));
    
    return 0;
}