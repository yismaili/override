#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void decrypt(int key) {
    char buffer[] = "Q}|u`sfg~sf{}|a3";  // encrypted string from the assembly 0x757c7d51, 0x67667360, 0x7b66737e, 0x33617c7d
    size_t len = strlen(buffer);
    int i;
    
    // XOR each character with the key
    for (i = 0; i < len; i++) {
        buffer[i] ^= key;
    }
    
    // compare the decrypted string with expected value
    if (strcmp(buffer, "Congratulations!") == 0) {
        system("/bin/sh");
    } else {
        puts("Invalid Password!");
    }
}

void test(int password, int target) {

    int diff = target - password;// Calculate the difference between target and password
    
    // Check if the difference is within valid range (0-21 or 0x15)
    if (diff <= 0x15) {
        switch (diff) {
            case 0: decrypt(diff); break;
            case 1: decrypt(diff); break;
            case 2: decrypt(diff); break;
            case 3: decrypt(diff); break;
            case 4: decrypt(diff); break;
            case 5: decrypt(diff); break;
            case 6: decrypt(diff); break;
            case 7: decrypt(diff); break;
            case 8: decrypt(diff); break;
            case 9: decrypt(diff); break;
            case 10: decrypt(diff); break;
            case 11: decrypt(diff); break;
            case 12: decrypt(diff); break;
            case 13: decrypt(diff); break;
            case 14: decrypt(diff); break;
            case 15: decrypt(diff); break;
            case 16: decrypt(diff); break;
            case 17: decrypt(diff); break;
            case 18: decrypt(diff); break;
            case 19: decrypt(diff); break;
            case 20: decrypt(diff); break;
            case 21: decrypt(diff); break;
        }
    } else {
        decrypt(rand());
    }
}

int main() {
    int password;
    
    srand(time(NULL));
    
    puts("***********************************");
    puts("*\t\tlevel03\t\t**");
    puts("***********************************");
    
    printf("Password:");
    scanf("%d", &password);
    
    test(password, 0x1337d00d);
    
    return 0;
}