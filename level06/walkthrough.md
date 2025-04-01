# **Level06**

At this level (`level06`), I found a binary file that prompts the user for a username and Serial number  when executed:

```nasm
level06@OverRide:~$ ./level06
***********************************
*		level06		  *
***********************************
-> Enter Login:
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial:
```

## **Binary Analysis**

The program reads user input containing a username and a serial number. It then prints a prompt and calls the `auth` function. This function uses `ptrace` to prevent debugging and calculates the expected serial number based on the provided username. It then compares the computed serial number with the user-inputted serial number. If the comparison is successful, the program spawns a shell.

**Main Function**

```nasm
;Stack Setup
0x08048879 <+0>:     push   %ebp
0x0804887f <+6>:     sub    $0x50,%esp        ;Allocate 80 bytes

; Security Measures
0x08048889 <+16>:    mov    %gs:0x14,%eax     ; Stack canary setup
0x0804888f <+22>:    mov    %eax,0x4c(%esp)   ; Store canary

; Input Handling
; Username input
0x080488d8 <+95>:    movl   $0x20,0x4(%esp)   ; Size = 32 bytes
0x080488e0 <+103>:   lea    0x2c(%esp),%eax   ; Buffer location
0x080488e7 <+110>:   call   fgets@plt         ; Read username

; Serial input
0x0804892d <+180>:   call   scanf@plt         ; Read serial
0x08048932 <+185>:   mov    0x28(%esp),%eax   ; Load serial

; Authentication
0x08048941 <+200>:   call   auth              ; Check credentials
0x08048946 <+205>:   test   %eax,%eax         ; Check return value
```

**Auth Function**

```nasm
; Function Setup
0x08048748 <+0>:     push   %ebp
0x0804874b <+3>:     sub    $0x28,%esp        ; 40 bytes local storage

; Anti-Debug Protection
0x080487b5 <+109>:   call   ptrace@plt        ; Anti-debugging check
0x080487ba <+114>:   cmp    $0xffffffff,%eax  ; Check ptrace result
0x080487bd <+117>:   jne    0x80487ed         ; Exit if debugger detected

; Serial Calculation
0x080487f0 <+168>:   add    $0x3,%eax         ; Get 4th char
0x080487f9 <+177>:   xor    $0x1337,%eax      ; XOR with 0x1337
0x080487fe <+182>:   add    $0x5eeded,%eax    ; Add constant

;Final Comparison
0x08048866 <+286>:   cmp    -0x10(%ebp),%eax  ; Compare with input serial
```

## **Vulnerability**

The program uses a predictable algorithm to generate the serial number, making it vulnerable. The weak debugging protection using `ptrace` can be bypassed by modifying the return value. Additionally, memory access allows us to read the calculated serial number, making it possible to retrieve or bypass authentication.

```c
// Pseudocode of hash generation
hash = (username[3] ^ 0x1337) + 0x5eeded;
// Result can be read from memory

if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1)
     return(1);
// Can be bypassed by modifying return value

// We can read calculated serial at:
-0x10(%ebp)    // Location of generated hash
```

## **Exploit**

To exploit this, we:

1. Bypass the `ptrace` anti-debugging check.
2. Set a breakpoint at the comparison instruction to extract the correct serial number.
3. Use the extracted serial number to authenticate and gain shell access.

**Step 1: Bypass ptrace**

```nasm
(gdb) catch syscall ptrace
Catchpoint 2 (syscall 'ptrace' [26])
;configure automatic bypass
(gdb) commands
Type commands for breakpoint(s) 2, one per line.
End with a line saying just "end".
>set $eax=0
>continue
>end
;set breakpoint at comparison (auth+286)
(gdb) b *0x08048866
Breakpoint 3 at 0x8048866
(gdb)
```

**Step 2:  Retrieve the Serial Number**

```nasm
;run program
(gdb) r
Starting program: /home/users/level06/level06

Breakpoint 1, 0x0804887c in main ()
(gdb) c
Continuing.
***********************************
*		level06		  *
***********************************
-> Enter Login: youyou ;# use simple login > 5 chars
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 0 ;any number for now

Catchpoint 2 (call to syscall ptrace), 0xf7fdb440 in __kernel_vsyscall ()

Catchpoint 2 (returned from syscall ptrace), 0xf7fdb440 in __kernel_vsyscall ()

Breakpoint 3, 0x08048866 in auth ()
 ;at breakpoint, read calculated serial
(gdb) x/x $ebp-0x10
0xffffd678:	0x005f1ae1 ;serial = 6232801 in decimal
(gdb)
```

**Step 3: Authentication**

```nasm
Login: youyou
Serial: 6232801
```

**Final Result**

```nasm
level06@OverRide:~$ ./level06
***********************************
*		level06		  *
***********************************
-> Enter Login: youyou
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 6232801
Authenticated!
$ whoami
level07
$ cat /home/users/level07/.pass
GbcPDRgsFK77LNnnuh7QyFYA2942Gp8yKj9KrWD8
$
```

## Flag

```nasm
GbcPDRgsFK77LNnnuh7QyFYA2942Gp8yKj9KrWD8
```