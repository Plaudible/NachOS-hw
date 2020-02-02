/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

int SysOpen(char *buffer)
{
    return open(buffer, O_CREAT | O_RDWR, 0666);
}

void SysClose(OpenFileId id){
    close((int)id);
}


void SysExec(char *progName){
    pid_t pid;
    pid = fork();
    if(pid != 0){
        //nothing
    }else{
        char * argv_list[] = {"./nachos", "-x", progName, NULL};
        execvp("./nachos",argv_list);
    }
}

void SysHalt()
{
  kernel->interrupt->Halt();
}


int SysAdd(int op1, int op2)
{
  return op1 + op2;
}






#endif /* ! __USERPROG_KSYSCALL_H__ */
