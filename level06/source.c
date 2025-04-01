#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>

int auth(char *username, int serial) {
    int len;
    int i;
    unsigned int hash;
    
    username[strcspn(username, "\n")] = '\0';
    
    len = strnlen(username, 32);
    
    if (len <= 5) {
        return 1;
    }
    
    // anti-debugging check using ptrace
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
        return 1;
    }
    
    // calculate hash from username
    // start with a seed based on the 4th character of username
    hash = (username[3] ^ 0x1337) + 0x5eeded;
    
    // iterate through each character in username
    for (i = 0; i < len; i++) {
        // check if character is printable (> 31)
        if (username[i] <= 31) {
            return 1;
        }
        
        // update hash based on current character
        // complex calculation that's a modulo operation equivalent to:
        // hash += (username[i] ^ hash) % 0x539
        int xored = username[i] ^ hash;
        int mod = xored % 0x539;
        hash += mod;
    }
    
    // compare calculated hash with provided serial
    if (serial == hash) {
        return 0; 
    } else {
        return 1;
    }
}

int main(int argc, char **argv) {
    char username[32];
    int serial;
    
    puts("***********************************");
    puts("*\t\tlevel06\t\t  *");
    puts("***********************************");
    
    printf("-> Enter Login: ");
    fgets(username, 32, stdin);
    
    puts("***********************************");
    puts("***** NEW ACCOUNT DETECTED ********");
    puts("***********************************");
    
    printf("-> Enter Serial: ");
    scanf("%d", &serial);
    
    // Check authentication
    if (auth(username, serial) == 0) {
        puts("Authenticated!");
        system("/bin/sh");
        return 0;
    } else {
        return 1;
    }
}