/*
complie: gcc multi_process_int.out multi_process_int.c
run: $ ./multi_process_int.out [datasize] [process_num]

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
#define key 13

int buffer[MAX_ARRAY_NUM];
int count = 0;

void fill_rand_num(int datasize){
    for(int i = 0; i < datasize; i++){
        buffer[i] = rand() % 1000;
    }
}

int main(int argc, char *argv[]){

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
    
    fill_rand_num(datasize);

    for(int repeat_time = 0; repeat_time < 10; repeat_time++){
        process_num = atoi(argv[2]);
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
                    if(buffer[j] == key){
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
        while (process_num > 0) {
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
            process_num--;
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