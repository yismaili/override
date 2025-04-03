#include <stdio.h>
#include <string.h>

char a_user_name[256]; // Global variable at 0x0804a040

int verify_user_name() {
    puts("verifying username....");
    
    return strcmp(a_user_name, "dat_wil");
}

int verify_user_pass(char *password) {
    return strcmp(password, "admin");
}

int main() {
    char password_buffer[64]; 
    int auth_flag = 0;      
    
    memset(password_buffer, 0, 64);
    
    puts("********* ADMIN LOGIN PROMPT *********");
    printf("Enter Username: ");
   
    fgets(a_user_name, 256, stdin);
    
    auth_flag = verify_user_name();
    
    if (auth_flag != 0) {
        puts("nope, incorrect username...");
        return 1;
    }
    
    puts("Enter Password: ");
    
    fgets(password_buffer, 100, stdin);
    
    auth_flag = verify_user_pass(password_buffer);
    
    if (auth_flag != 0) {
        puts("nope, incorrect password...");
        return 1;
    }
    
    puts("Authenticated!");
    return 0;
}