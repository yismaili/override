#include <stdio.h>
#include <string.h>

int verify_user_name(char *input) {
    printf("Enter login name: ");
    
    char *expected_name = "dat_wil";
    return strcmp(input, expected_name) != 0;
}

int verify_user_pass(char *input) {
    char *expected_pass = "admin";
    return strcmp(input, expected_pass) != 0;
}

int main() {
    char username[0x10];
    char password[0x64];
    int auth_result = 0;

    printf("***********************************\n");
    printf("*\t     -Level01 -\t\t  *\n");
    printf("***********************************\n");

    printf("login: ");
    fgets(username, 0x100, stdin);
    username[strcspn(username, "\n")] = 0;  // remove newline

    if (verify_user_name(username) != 0) {
        printf("nope, incorrect username\n");
        return 1;
    }

    printf("Password: ");
    fgets(password, 0x64, stdin);
    password[strcspn(password, "\n")] = 0; 

    if (verify_user_pass(password) != 0) {
        printf("nope, incorrect password\n");
        return 1;
    }

    printf("Authenticated!\n");
    return 0;
}