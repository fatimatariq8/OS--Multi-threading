#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <limits.h>

typedef struct {
    long long int* data;
    size_t start;
    size_t end;
    long long int* partial_sum;
    long long int* partial_min;
    long long int* partial_max;
} Thread_args; //structure for each thread 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* calculation(void* args) {
    Thread_args* thread_args=(Thread_args*)args;
    long long int partial_sum=0; //these are the local variables 
    long long int partial_min=LLONG_MAX;
    long long int partial_max=LLONG_MIN;

    for (int i=thread_args->start; i < thread_args->end; i++) { //checking for min max and sum
        partial_sum += thread_args->data[i];
        if (thread_args->data[i]<partial_min) {
            partial_min = thread_args->data[i];
        }
        if (thread_args->data[i]>partial_max) {
            partial_max = thread_args->data[i];
        }
    }

    pthread_mutex_lock(&mutex); //locking to for thread safety
    *(thread_args->partial_sum) += partial_sum;
    if (partial_min < *(thread_args->partial_min)) {
        *(thread_args->partial_min) = partial_min;
    }
    if (partial_max > *(thread_args->partial_max)) {
        *(thread_args->partial_max) = partial_max;
    }
    pthread_mutex_unlock(&mutex);
    free(args);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 2) { //checking for correct number of arguments
    printf("Format: <program_name> <data_file> <num_threads>\n");
    exit(EXIT_FAILURE);
}

    char* filename=argv[1]; //second argument is filename
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int num; //calculating size of file by counting the number of integers
    size_t data_size = 0; //size_t is used to accomodate all possible sizes 
    while (fscanf(file,"%d",&num) == 1) {
        data_size++;
    }

    long long int* data = (long long int*)malloc(data_size * sizeof(long long int)); //allocate memory dynamically for the data array
    if (data == NULL) {
        perror("Error allocating memory for data");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_SET); //returning to beginning of file 

    size_t i = 0; //reading data from file into the data array
    while (fscanf(file,"%d",&num) == 1) {
        data[i++]=num;
    }
    fclose(file);

    int num_threads = (argc==3)?atoi(argv[2]):4; //number of threads (default is 4 if not provided by the user)

    size_t thread_size=data_size/num_threads; //size for each thread 

    pthread_t threads[num_threads];

    long long int partial_sum = 0;
    long long int partial_min = LLONG_MAX;
    long long int partial_max = LLONG_MIN;
    struct timespec start_time, end_time; //starting time 
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int i = 0; i < num_threads; i++) {
        Thread_args* args = (Thread_args*)malloc(sizeof(Thread_args));
        if (args == NULL) {
            perror("Error allocating memory for thread arguments");
            free(data);
            exit(EXIT_FAILURE);
        }

        args->data = data;
        args->start = i * thread_size;
        args->end = (i==num_threads-1)?data_size:(i+1) * thread_size; //checking if current thread is last thread. if yes, the end is the data array size. if not, the end is found by calculating the end index for the current thread's data  
        args->partial_sum = &partial_sum;
        args->partial_min = &partial_min;
        args->partial_max = &partial_max;

        if (pthread_create(&threads[i], NULL, calculation, args) != 0) {
            perror("Error creating thread");
            free(args);
            free(data);
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Error joining thread");
            free(data);
            exit(EXIT_FAILURE);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time); //ending time calculation

    printf("Sum: %lld\n", partial_sum);
    printf("Min: %lld\n", partial_min);
    printf("Max: %lld\n", partial_max);

    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Multi-threaded processing time: %f seconds\n", elapsed_time);

    free(data);
    return 0;
}
