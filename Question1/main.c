#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
    int maximum;
    int minimum;
    float average;
    int* even_values;
    int even_values_count;
}arrayStats;


arrayStats parse_array(const int array[], int n){
    arrayStats stats = {
        .maximum = 0,
        .minimum = 0,
        .average = 0.0f,
        .even_values = NULL,
        .even_values_count = 0
    };

    if(n <= 0){
        return stats;
    }

    stats.maximum = array[0];
    stats.minimum = array[0];
    stats.even_values = malloc(n * sizeof(int));
    if(stats.even_values == NULL){
        return stats;
    }

    long sum = 0;

    for(int i = 0; i < n; i++){
        if(array[i] > stats.maximum){
            stats.maximum = array[i];
        }
        if(array[i] < stats.minimum){
            stats.minimum = array[i];
        }
        if(array[i] % 2 == 0)
        {
            stats.even_values[stats.even_values_count++] = array[i];
        }
        sum += array[i];
    }

    stats.average = (float)sum / n;

    // To avoid over-allocation
    int* shrunk = realloc(stats.even_values, stats.even_values_count * sizeof(int));
    if(shrunk != NULL || stats.even_values_count == 0){
        stats.even_values = shrunk;
    }

    return stats;
}

int main(){

    int array[] = {1, 2, 4, -1, 2, -2};
    int size = sizeof(array) / sizeof(array[0]);

    arrayStats stats = parse_array(array, size);

    printf("Array stats\n");
    printf("Average: %.2f\n", stats.average);
    printf("Maximum: %d\n", stats.maximum);
    printf("Minimum: %d\n", stats.minimum);
    if(stats.even_values_count == 0)
    {
        printf("No even values in array");
    }
    else{
        printf("%d even values: ", stats.even_values_count);
        for(int i = 0; i < stats.even_values_count; i++){
            printf("%d ", stats.even_values[i]);
        }
    }

    free(stats.even_values);

    return 0;
}