# **Level03**

At this level (`level03`), I found a binary file that prompts the user for a password when executed:

```bash
level03@OverRide:~$ ./level03
***********************************
*          level03                *
***********************************
Password:
```

## **Binary Analysis**

After analyzing the program using GDB, I found that it prompts the user for a password and passes the input to a function named `test`. Inside this function:

1. The input is subtracted from a predefined **magic number** (`0x1337d00d`).
2. The result of this subtraction is then compared to `21`.
3. If the result is **less than or equal to 21**, the program calls the `decrypt` function.
4. The **difference** (i.e., `magic_number - input`) is used as the **decryption key**.
5. The `decrypt` function takes the encrypted string `Q}|u`sfg~sf{}|a3` and attempts to decrypt it using this key.
6. If the decrypted string matches `"Congratulations!"`, the program spawns a shell.

**Main Function Disassembly**

```nasm
0x0804885a <+0>:     push   %ebp                  ; Save old base pointer
0x0804885b <+1>:     mov    %esp,%ebp            ; Set new base pointer
0x0804885d <+3>:     and    $0xfffffff0,%esp     ; Align stack
0x08048860 <+6>:     sub    $0x20,%esp           ; Allocate 32 bytes on stack

; Initialize random seed
0x08048873 <+25>:    call   0x80484b0 <time@plt> ; time(0)
0x0804887b <+33>:    call   0x8048500 <srand@plt>; srand(time(0))

; Get user input
0x080488c1 <+103>:   call   0x8048530 <__isoc99_scanf@plt>
0x080488c6 <+108>:   mov    0x1c(%esp),%eax      ; Store input in eax

; Setup test function call
0x080488ca <+112>:   movl   $0x1337d00d,0x4(%esp); Magic number as 2nd arg
0x080488d2 <+120>:   mov    %eax,(%esp)          ; Input as 1st arg
0x080488d5 <+123>:   call   0x8048747 <test>     ; Call test function

```

**Test Function Disassembly**

```nasm
; Function setup
0x08048747 <+0>:     push   %ebp
0x08048748 <+1>:     mov    %esp,%ebp
0x0804874a <+3>:     sub    $0x28,%esp           ; Allocate 40 bytes

; Get arguments and calculate difference
0x0804874d <+6>:     mov    0x8(%ebp),%eax       ; Get input (1st arg)
0x08048750 <+9>:     mov    0xc(%ebp),%edx       ; Get magic (2nd arg)
0x08048753 <+12>:    mov    %edx,%ecx
0x08048755 <+14>:    sub    %eax,%ecx            ; magic - input
0x08048757 <+16>:    mov    %ecx,%eax
0x08048759 <+18>:    mov    %eax,-0xc(%ebp)      ; Store result

; Compare result with 0x15 (21)
0x0804875c <+21>:    cmpl   $0x15,-0xc(%ebp)     ; Compare with 21
0x08048760 <+25>:    ja     0x804884a <test+259> ; If > 21, jump to failure

; Spawn shell
0x08048715 <+181>:   movl   $0x80489d4,(%esp)
0x0804871c <+188>:   call   0x80484e0 <system@plt>

```

**Decrypt Function** **Disassembly**

```nasm

 ; Encrypted string in memory
0x08048673: movl   $0x757c7d51,-0x1d(%ebp)  ; "Q}|u"
0x0804867a: movl   $0x67667360,-0x19(%ebp)  ; "`sfg"
0x08048681: movl   $0x7b66737e,-0x15(%ebp)  ; "~sf{"
0x08048688: movl   $0x33617c7d,-0x11(%ebp)  ; "}|a3"

; XOR loop with our difference
0x080486d2 <+114>:   mov    0x8(%ebp),%eax      ; Get our input (18)
0x080486d5 <+117>:   xor    %edx,%eax           ; XOR with encrypted char
0x080486d7 <+119>:   mov    %eax,%edx

0x080486df <+127>:   mov    %dl,(%eax)          ; Store result

0x080486f2 <+146>:   mov    $0x80489c3,%eax     ; Expected string `Congratulations!`
0x08048700 <+160>:   repz cmpsb %es:(%edi),%ds:(%esi)  ; Compare
0x08048711 <+177>:   test   %eax,%eax
0x08048713 <+179>:   jne    0x8048723           ; Jump if not equal

; If equal, call system
0x08048715 <+181>:   movl   $0x80489d4,(%esp)   ; "/bin/sh"
0x0804871c <+188>:   call   0x80484e0 <system@plt>
```

## **Vulnerability**

The vulnerability lies in the mathematical condition within the `test` function:

```c
int difference = 0x1337d00d - input;
if (difference <= 21)
    decrypt(difference);  // Success path
else
    decrypt(rand());      // Failure path

```

**Why it's Vulnerable**

- The program reveals a clear mathematical relationship.
- We control the input value, allowing us to manipulate the result.
- Simple arithmetic lets us compute the exact value needed to pass the check.

## **Exploit**

To exploit this vulnerability, we solve for `input` such that:

```c
magic - input ≤ 21
```

Given:

```c
magic = 0x1337d00d  = 322424845
```

**XOR Process:** 

https://md5decrypt.net/en/Xor/

```nasm
Encrypted:   75 7c 7d 51 67 66 73 60 7b 66 73 7e 33 61 7c 7d
Key:         12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12
            XOR
Decrypted:   67 6e 6f 43 75 74 61 72 69 74 61 6c 21 73 6e 6f

In ASCII: "Congratulations!"
```

Let's decrypt first part (0x757c7d51) with key 18 (0x12):

```nasm
75 XOR 12 = 67 ('g')
7c XOR 12 = 6e ('n')
7d XOR 12 = 6f ('o')
51 XOR 12 = 43 ('C')
```

Result: "Cong" (backwards because of little endian)

We choose `n = 18` :

```c
input = magic - n
input = 322424845 - 18
input = 322424827
```

**Execution**

```bash
level03@OverRide:~$ ./level03
***********************************
*          level03                *
***********************************
Password: 322424827
$ whoami
level04
$ cat /home/users/level04/.pass
kgv3tkEb9h2mLkRsPkXRfc2mHbjMxQzvb2FrgKkf
```

## **Flag**

```c
kgv3tkEb9h2mLkRsPkXRfc2mHbjMxQzvb2FrgKkf
```