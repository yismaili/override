#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <signal.h>

int main() {
    char buffer[32];    // esp+0x20
    int status;          //  esp+0x1c
    pid_t pid;           // esp+0xac
    int ptrace_ret;      // esp+0xa8
    
    pid = fork();
    
    memset(buffer, 0, 128);
    
    if (pid == 0) {
        // child process
        
        prctl(PR_SET_PDEATHSIG, 1); // PR_SET_PDEATHSIG: sets the parent death signal of the calling process
        ptrace(PTRACE_TRACEME, 0, 0, 0);   // start ptrace - allow the parent process to trace this process
        
        puts("Give me some shellcode");
        gets(buffer);  // vulnerable function - buffer overflow
        
    } else {
        // parent process
        
        while (1) {

            wait(&status); // wait for child to stop or exit
            
            // check if child exited normally
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                puts("child is exiting...");
                break;
            }
            
            ptrace_ret = ptrace(PTRACE_PEEKUSER, pid, 44, 0);  // 44 = offsetof(struct user, regs.orig_eax)
            
            if (ptrace_ret == 11) {
                puts("no exec() for you");
                kill(pid, SIGKILL);
                break;
            }
        }
    }
    
    return 0;
}