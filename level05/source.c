#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char buffer[100];    // 0x64 byte buffer at esp+0x28
    int i = 0;
    
    fgets(buffer, 100, stdin);
    
    while (i < strlen(buffer)) {
        // check if the character is an uppercase letter between 'A' and 'Z'
        if (buffer[i] > '@' && buffer[i] <= 'Z') {
            //XOR with 0x20
            buffer[i] ^= 0x20;
        }
        i++;
    } 
    printf(buffer);  // format string vulnerability!
    exit(0);
}