/*
compile: gcc -o multi_thread_string.out multi_thread_string.c -lpthread
run: $ ./multi_thread_string.out [datasize] [thread_num]

NOTE: (datasize <= 100000000, thread_num <= 20)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_THREAD_NUM 20
#define MAX_ARRAY_NUM 100000000

char buffer[MAX_ARRAY_NUM][5];
char key[5] = {'h', 'e', 'l', 'l', 'o'};
int count = 0;

// 加入 Mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// 子執行緒函數
void *child(void *arg) {
    int *input = (int *) arg; // 取得資料
    int thread_count = 0;
    for(int i = input[0]; i < input[1]; i++){
        int check = 1; //assume the string of data and the one of key are the same
        for(int j = 0; j < 5; j++){
            if(buffer[i][j] != key[j]){
                check = 0;
                break;
            }
        }
        if(check == 1){
            thread_count++;
        }
    }
    pthread_mutex_lock( &mutex ); // 上鎖
    count += thread_count;
    pthread_mutex_unlock( &mutex ); // 解鎖

    pthread_exit(NULL);
}

void fill_rand_string(int datasize){
    for(int i = 0; i < datasize; i++){
        for(int j = 0; j < 5; j++){
            buffer[i][j] = rand(); //string[j]
        }
    }
}

int main(int argc, char *argv[]){
    
    FILE *F ;
    F = fopen("multi_thread_string.csv", "a");
    if(F == NULL){
        printf("open file fail.\n");
    }

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

    fill_rand_string(datasize);

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

        printf("String \"%c%c%c%c%c\" occurs %d times in the array.\n", key[0], key[1], key[2], key[3], key[4], count);
        diff = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
        printf("Compute time: %luus.\n\n", diff);
        total_time += diff;
    }
    int avg_time = total_time / 10; 
    printf("Average time cost: %dus\n", avg_time);
    
    //write csv file
    fprintf(F, "%d, %d, %d \n", datasize, thread_num, avg_time);
    fclose(F);

    return 0;
}