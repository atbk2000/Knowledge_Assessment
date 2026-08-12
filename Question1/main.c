#include <stdio.h>
#include <stdlib.h>

// Structure containing array statistics
typedef struct 
{
    int maximum;
    int minimum;
    float average;
    int* even_values;
    int even_values_count;
}arrayStats;


// ============================================================================
// ARRAY PARSING & STATISTICS
// ============================================================================

/**
 * @brief Parses an integer array and computes basic statistics.
 *
 * This function computes the maximum, minimum and average of the provided
 * array. It also collects all even values into a dynamically allocated
 * array stored in the returned `arrayStats` structure. The caller is
 * responsible for freeing `even_values` when no longer needed.
 *
 * @param array Pointer to the input integer array.
 * @param n Number of elements in `array`.
 * @return arrayStats Structure containing computed statistics and a
 *         heap-allocated array of even values (or NULL if none / on error).
 */
arrayStats parse_array(const int array[], int n){
    arrayStats stats = {
        .maximum = 0,
        .minimum = 0,
        .average = 0.0f,
        .even_values = NULL,
        .even_values_count = 0
    };

    // Guard: empty or invalid input
    if(n <= 0){
        return stats;
    }

    // Initialize min/max from first element
    stats.maximum = array[0];
    stats.minimum = array[0];

    // Allocate worst-case storage for all elements being even.
    stats.even_values = malloc(n * sizeof(int));
    if(stats.even_values == NULL){
        // Allocation failed; return stats with even_values == NULL
        return stats;
    }

    long sum = 0; // Use larger type to avoid overflow for sum

    // Iterate array to gather stats and collect even numbers
    for(int i = 0; i < n; i++){
        if(array[i] > stats.maximum){
            stats.maximum = array[i];
        }
        if(array[i] < stats.minimum){
            stats.minimum = array[i];
        }
        if(array[i] % 2 == 0)
        {
            // Append even value to the dynamic buffer
            stats.even_values[stats.even_values_count++] = array[i];
        }
        sum += array[i];
    }

    // Compute average as float
    stats.average = (float)sum / n;

    // Shrink the even_values buffer to the exact number of collected items.
    // If realloc fails we keep the original buffer (still valid) unless
    // there were zero even values in which case we free and set to NULL.
    if (stats.even_values_count == 0) {
        free(stats.even_values);
        stats.even_values = NULL;
    } else {
        int* shrunk = realloc(stats.even_values, stats.even_values_count * sizeof(int));
        if(shrunk != NULL){
            stats.even_values = shrunk;
        }
        // if realloc fails, keep the original buffer to preserve data
    }

    return stats;
}

// ============================================================================
// MAIN
// ============================================================================
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