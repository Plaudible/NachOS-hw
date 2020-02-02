// exception.cc
//  Entry point into the Nachos kernel from user programs.
//  There are two kinds of things that can cause control to
//  transfer back to here from user code:
//
//  syscall -- The user code explicitly requests to call a procedure
//  in the Nachos kernel.  Right now, the only function we support is
//  "Halt".
//
//  exceptions -- The user code does something that the CPU can't handle.
//  For instance, accessing memory that doesn't exist, arithmetic errors,
//  etc.
//
//  Interrupts (which can also cause control to transfer from user
//  code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
//  Entry point into the Nachos kernel.  Called when a user program
//  is executing, and either does a syscall, or generates an addressing
//  or arithmetic exception.
//
//  For system calls, the following is the calling convention:
//
//  system call code -- r2
//    arg1 -- r4
//    arg2 -- r5
//    arg3 -- r6
//    arg4 -- r7
//
//  The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//  "which" is the kind of exception.  The list of possible exceptions
//  is in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
  int type = kernel->machine->ReadRegister(2);
  
  DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");
  
  switch (which) {
    case SyscallException:
      switch(type) {
        case SC_Halt:
          DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
          
          SysHalt();
          break;
          
          case SC_Add: {
          DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
          
          /* Process SysAdd Systemcall*/
          int result;
          result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
                          /* int op2 */(int)kernel->machine->ReadRegister(5));
          
          DEBUG(dbgSys, "Add returning with " << result << "\n");
          /* Prepare Result */
          kernel->machine->WriteRegister(2, (int)result);
          
          
          break;
          }
              
          case SC_Open: {
          printf("Open system call made by %s\n", kernel->currentThread->getName());
              int buffer = (int)kernel -> machine -> ReadRegister(4);
              char programName[100];
              int charBuffer = 1;
              int i = 0;
              
               while((char)charBuffer != '\0'){
                  kernel -> machine -> ReadMem(buffer, 1, &charBuffer);
                  buffer +=1;
                  programName[i] = (char)charBuffer;
                  i++;
               }
              printf("%s",programName);
              OpenFileId id = (OpenFileId)SysOpen(programName);
              kernel -> machine -> WriteRegister(2,(int)id);
          break;
          }
              
          case SC_Close: {
          printf("Close system call made by %s\n", kernel->currentThread->getName());
              OpenFileId id = (OpenFileId)kernel -> machine -> ReadRegister(4);
              SysClose(id);
          break;
          }
          
          case SC_Write: {
          printf("Write system call made by %s\n", kernel->currentThread->getName());
              int buffer = (int)kernel -> machine -> ReadRegister(4);
              int size = (int)kernel -> machine -> ReadRegister(5);
              OpenFileId id = (OpenFileId)kernel -> machine -> ReadRegister(6);
              
              int charBuffer = 1;
              char outputBuffer[size];
              for (int x = 0; x < size; x++){
                  kernel -> machine -> ReadMem(buffer, 1, &charBuffer);
                  buffer +=1;
                  outputBuffer[x] = (char)charBuffer;
              }
              if(id == 1){
                  for(int x = 0; x < size; x++){
                      printf("%c", outputBuffer[x]);
                  }
              }
              if(id > 1){
                  write(id, outputBuffer, size-1);
              }
          break;
          }
              
          case SC_Exec: {
              char buffer[50];
              int x = 1;
              int i = 0;
              int programName = (int)kernel -> machine -> ReadRegister(4);
              while ((char)x != '\0'){
                  kernel -> machine -> ReadMem(programName, 1, &x);
                  programName++;
                  buffer[i] = (char)x;
                  i++;
              }
              SysExec(buffer);
          break;
          }
              
          case SC_Exit:
          printf("Exit system call made by %s\n", kernel->currentThread->getName());
          kernel->currentThread->Finish();
          break;
          
          default:
          cerr << "Unexpected system call " << type << "\n";
          break;
      }
      break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
  }
  
  /* Modify return point */
  {
    /* set previous programm counter (debugging only)*/
    kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
    
    /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
    kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
    
    /* set next programm counter for brach execution */
    kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
  }
}
