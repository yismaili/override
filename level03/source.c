#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void decrypt(int input) {
    char encrypted_str[] = {0x75, 0x7c, 0x7d, 0x51, 
                            0x67, 0x66, 0x73, 0x60, 
                            0x7b, 0x66, 0x73, 0x7e, 
                            0x33, 0x61, 0x7c, 0x7d, 
                            0x00};

    // XOR decryption
    for (int i = 0; i < strlen(encrypted_str); i++) {
        encrypted_str[i] ^= input;
    }

    if (strcmp(encrypted_str, "Congratulations!") == 0) {
        system("/bin/sh");
    } else {
        puts("Invalid input!");
    }
}

void test(int input, int magic_number) {
    int diff = magic_number - input;

    if (diff > 21) {
        srand(time(NULL));
        decrypt(rand());
        return;
    }

  
    static void *jump_table[] = {
        // multiple decrypt calls with different inputs
        &&label_0, &&label_1, &&label_2, &&label_3, 
        // ... more labels
    };

    goto *jump_table[diff];

    label_0: decrypt(input); return;
    label_1: decrypt(input); return;
    label_2: decrypt(input); return;
    label_3: decrypt(input); return;
    // ... more labels
}

int main() {
    int input;

    srand(time(NULL));

    puts("***********************************");
    puts("*\t     -Level03 -\t\t  *");
    puts("***********************************");

    printf("Enter password: ");
    scanf("%d", &input);

    test(input, 0x1337d00d);

    return 0;
}