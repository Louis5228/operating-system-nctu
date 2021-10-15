/*
complie: gcc -o multi_process_struct.out multi_process_struct.c
run: $ ./multi_process_struct.out [datasize] [process_num]

NOTE: (datasize <= 100000000, process_num <= 20)
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESS_NUM 20
#define MAX_ARRAY_NUM 100000000

typedef struct {
    int a;
} _struct;

_struct buffer[MAX_ARRAY_NUM];
_struct key = {.a = 13};
int count = 0;

void fill_rand_struct(int datasize){
    for(int i = 0; i < datasize; i++){
        buffer[i].a = rand() % 1000;
    }
}

int main(int argc, char *argv[]){

    FILE *F ;
    F = fopen("multi_process_struct.csv", "a");
    if(F == NULL){
        printf("open file fail.\n");
    }

    unsigned long diff;
    unsigned long total_time = 0;
    int datasize = atoi(argv[1]); //input argument of datasize
    int process_num = atoi(argv[2]); //input argument of process_num

    if(datasize > MAX_ARRAY_NUM){
        datasize = MAX_ARRAY_NUM;
    }
    if(process_num > MAX_PROCESS_NUM){
        process_num = MAX_PROCESS_NUM;
    }
    printf("Test setting: datasize=%u, process_num=%d\n", datasize, process_num);
    
    fill_rand_struct(datasize);

    for(int repeat_time = 0; repeat_time < 10; repeat_time++){
        count = 0;
        pid_t pids[process_num];
        struct timeval start, end;

        gettimeofday(&start, NULL);
        /* Start children. */
        for(int i = 0; i < process_num; i++) {
            if((pids[i] = fork()) < 0) {
                printf("fork() error.\n");
            }else if(pids[i] == 0) {
                int process_count = 0;
                int start_index = (datasize / process_num) * i;
                int end_index;
                if(i != process_num - 1){
                    end_index = (datasize / process_num) * (i + 1);
                }else{
                    end_index = datasize;
                }

                for(int j = start_index; j < end_index; j++){
                    if(buffer[j].a == key.a){
                        process_count++;
                    }
                }
                exit(process_count); //return local count for each child process
            }
        }

        /* Wait for children to exit. */
        int seg_count;
        int status;
        pid_t pid;
        for(int i = 0; i < process_num; i++){
            if((pid = wait(&status)) == -1){
                printf("wait() error.\n");
            }else{
                // printf("the pid of the process that ended was %ld\n", (long) pid);
                if (WIFEXITED(status)){
                    seg_count = (int) WEXITSTATUS(status);
                    count += seg_count;
                }else if(WIFSIGNALED(status)){
                    printf("child was terminated by signal %d\n", WTERMSIG(status));
                }else if(WIFSTOPPED(status)){
                    printf("child was stopped by signal %d\n", WSTOPSIG(status));
                }else{
                    printf("reason unknown for child termination\n");
                }
            }
        }
        gettimeofday(&end, NULL);

        printf("Integer %d occurs %d times in the array.\n", key.a, count);
        diff = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
        printf("Compute time: %luus.\n\n", diff);
        total_time += diff;
    }
    int avg_time = total_time / 10;
    printf("Average time cost: %d us\n", avg_time);
    
    //write csv file
    fprintf(F, "%d, %d, %d \n", datasize, process_num, avg_time);
    fclose(F);

    return 0;
}