#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int num_strings = 3;

    // Allocate memory for the array of string pointers.
    char **string_array = (char **)malloc(num_strings * sizeof(char *));

    // Check if memory allocation was successful.
    if (string_array == NULL) {
        perror("Memory allocation failed");
        return 1;
    }
 char *arr[] = {"Geek", "Geeks", "Geekfor"};
    // Initialize the individual strings using a loop.
    for (int i = 0; i < num_strings; i++) {
        // Allocate memory for each individual string.
        string_array[i] = arr[i];
        
    }

    // Print the array of strings.
    for (int i = 0; i < num_strings; i++) {
        printf("string_array[%d] = %s\n", i, string_array[i]);
    }

    // Don't forget to free the allocated memory when you're done.
    for (int i = 0; i < num_strings; i++) {
        free(string_array[i]);
    }
    free(string_array);

    return 0;
}
