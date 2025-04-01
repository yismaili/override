#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

void clear_stdin() {
    int c;

    do {
        c = getchar(); 
    } while (c != '\n' && c != EOF);
}
unsigned int get_unum() {
    unsigned int number = 0;

    // flush the output buffer
    fflush(stdout);
    printf("Enter a number: ");

    scanf("%u", &number);
    clear_stdin();

    return number;
}

int read_number(int *array) {
    unsigned int index;
    int value;
    printf("Enter index: ");
    index = get_unum();

    value = array[index];
    printf("Value at index %u is %d\n", index, value);

    return 0;
}

int store_number(int *array) {
    unsigned int index, value;

    printf("Enter index: ");
    index = get_unum();

    printf("Enter value: ");
    value = get_unum();

    // Check if the index is divisible by 3
    if (index % 3 == 0 || (value >> 24) == 0xb7) {
        puts("Error: Invalid index or value");
        return 1;
    }

    // Store the value in the array
    array[index] = value;
    return 0;
}

int main(int argc, char *argv[]) {
    char buffer[400]; 
    int numbers[100];
    int *ptr = numbers;
    int result = 0;

    memset(numbers, 0, sizeof(numbers));

    for (int i = 1; i < argc; i++) {
        memset(argv[i], 0, strlen(argv[i]));
    }

    while (1) {
        printf("Input command: ");
        fgets(buffer, sizeof(buffer), stdin);

        // check for specific commands
        if (strncmp(buffer, "store", 5) == 0) {
            result = store_number(numbers);
        } else if (strncmp(buffer, "read", 4) == 0) {
            result = read_number(numbers);
        } else if (strncmp(buffer, "quit", 4) == 0) {
            break;
        }

        if (result) {
            printf("Success\n");
        } else {
            printf("Failure\n");
        }
        memset(buffer, 0, sizeof(buffer));
    }

    return 0;
}