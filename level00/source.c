#include <stdio.h>
#include <stdlib.h>

int main() {
    puts("***********************************");
    puts("*\t     level00    \t  *");
    puts("***********************************");

    printf("Password:");

    int user_input;

    scanf("%d", &user_input);

    if (user_input == 0x149c) {
        puts("Congratulations, you have successfully logged in!");
        
        system("/bin/sh");
        
        return 0;
    } else {
        puts("Invalid Password!");
        return 1;
    }
}