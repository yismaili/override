#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char username[100];     // -0x70(%rbp)
    char file_buffer[100];  // -0xa0(%rbp)
    char password[100];     // -0x110(%rbp)
    FILE *file;


    file = fopen("/home/users/level03/.pass", "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }

    size_t bytes_read = fread(file_buffer, 1, 41, file);
    file_buffer[bytes_read] = '\0';

    file_buffer[strcspn(file_buffer, "\n")] = '\0';

    if (bytes_read != 41) {
        fprintf(stderr, "Error reading file\n");
        exit(1);
    }


    fclose(file);

    puts("***********************************");
    puts("*\t     -Level02 -\t\t  *");
    puts("***********************************");
    puts("Enter login name: ");

    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';
    printf(username);

    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';

    if (strncmp(password, file_buffer, 41) == 0) {
        printf("Authenticated user: %s\n", username);
        system("/bin/sh");
        return 0;
    } else {
        printf("%s\n", username);
        puts("Invalid Password!");
        exit(1);
    }
}