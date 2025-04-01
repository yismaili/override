# **Level00**

At this level (level00), I also found a binary file. When run, it asks for a username and password

```
level00@OverRide:~$ ./level00
***********************************
*        -Level00 -         *
***********************************
Password:
```

## **Binary Analysis**

Using GDB, I disassembled the main function and found that the program compares the input with the hex value **`0x149c`**. If it matches, the program executes a system command. If wrong, it prints a failure message.

## **Vulnerability**

The program accepts a command-line argument and compares it to 5276 (0x149c). If the input matches 5276, the program spawns a shell.

```nasm
(gdb) disas main
Dump of assembler code for function main:
[...]
0x080484b5 <+33>:	movl   $0x80485f0,(%esp)
0x080484bc <+40>:	call   0x8048390 [puts@plt](mailto:puts@plt)
0x080484c1 <+45>:	mov    $0x804862c,%eax ;Password:
0x080484c6 <+50>:	mov    %eax,(%esp)
0x080484c9 <+53>:	call   0x8048380 [printf@plt](mailto:printf@plt)
0x080484ce <+58>:	mov    $0x8048636,%eax ; "%d"
0x080484d3 <+63>:	lea    0x1c(%esp),%edx
0x080484d7 <+67>:	mov    %edx,0x4(%esp)
0x080484db <+71>:	mov    %eax,(%esp)
0x080484de <+74>:	call   0x80483d0 [__isoc99_scanf@plt](mailto:__isoc99_scanf@plt)
0x080484e3 <+79>:	mov    0x1c(%esp),%eax
0x080484e7 <+83>:	cmp    $0x149c,%eax ;Compare with 0x149c (5276 in decimal)
0x080484ec <+88>:	jne    0x804850d <main+121> ;Jump if not equal
0x080484ee <+90>:	movl   $0x8048639,(%esp) ;"\nAuthenticated!"
0x080484f5 <+97>:	call   0x8048390 [puts@plt](mailto:puts@plt)
0x080484fa <+102>:	movl   $0x8048649,(%esp) ;"/bin/sh"
0x08048501 <+109>:	call   0x80483a0 [system@plt](mailto:system@plt)
[...]
End of assembler dump.
```

## Exploit

By providing 5276 as an argument, we can exploit this vulnerability to obtain a privileged shell:

```bash
level00@OverRide:~$ ./level00
***********************************
* 	     -Level00 -		  *
***********************************
Password:5276

Authenticated!
$ whoami
level01
$ cat /home/users/level01/.pass
uSq2ehEGT6c9S24zbshexZQBXUGrncxn5sD5QfGL
$
```

This demonstrates a basic privilege escalation vulnerability. By supplying the correct "magic number" (5276), the program grants elevated access and spawns a shell, allowing us to read the password for the next level.

## Flag

```bash
uSq2ehEGT6c9S24zbshexZQBXUGrncxn5sD5QfGL
```