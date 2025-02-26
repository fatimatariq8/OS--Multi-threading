#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    if (argc!=2) { //checking for correct number of arguments
        printf("Format: <program_name> <data_file> \n");
        exit(EXIT_FAILURE);
    }

    char* filename = argv[1]; //second argument is filename
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int num; //calculating size of file by counting the number of integers
    size_t data_size=0; //size_t is used to accomodate all possible sizes 
    while (fscanf(file,"%d",&num) == 1) {
        data_size++;
    }

    long long int* data = (long long int*)malloc(data_size * sizeof(long long int));  //allocate memory dynamically for the data array
    if (data == NULL) {
        perror("Error allocating memory for data");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_SET); //returning to beginning of file 

    size_t i = 0; //reading data from file into the data array
    while (fscanf(file, "%d",&num) == 1) {
        data[i++]=num;
    }
    fclose(file);

    long long int sum = 0;
    long long int min = LLONG_MAX;
    long long int max = LLONG_MIN;

    clock_t start_time = clock(); //starting time

    for (i = 0; i < data_size; i++) {
        sum += data[i];
        if (data[i] < min) {
            min = data[i];
        }
        if (data[i] > max) {
            max = data[i];
        }
    }

    clock_t end_time = clock(); 

    printf("Sum: %I64d\n", sum);
    printf("Min: %I64d\n", min);
    printf("Max: %I64d\n", max);

    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Single-threaded processing time: %f seconds\n", elapsed_time);

    free(data);

    return 0;
}
