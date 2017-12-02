
#ifndef __BREAKPOINT_H
#define __BREAKPOINT_H

#include <iostream>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>

class breakpoint
{
  public:
    // removing the default constructor causes compilation error ?.
    breakpoint() {}
    /* Paramterized constructor with address of breakpoint [addr] at a process
     [pid].*/
    breakpoint(pid_t pid, std::intptr_t breakpointAddress)
        : mPID{pid}, mBPAddress{breakpointAddress}, mBPEnabled{false},
         mSavedLowerInstructionByte{0}
    {
    }

    /* Enable setting a breakpoint at a specific address [m_addr] of process
     [m_pid].*/
    bool setBP();
    /* Deleting the breakpoint at a specific address [m_addr] of process [m_pid]
     and restore the old instruction. */
    void removeBP();
    /* Restore the instruction which the breakpoint was set without deleting the
     breakpoint location. */
    void restoreOriginalInstructionWithoutRemovingBP();

    // is (this) object of the class has an active breakpoint.
    auto BPEnabled() const -> bool { return mBPEnabled; }
    // return the address of the breakpoint.
    auto getBPAddress() const -> std::intptr_t { return mBPAddress; }

  private:
    // pid of the process which has a breakpoint.
    pid_t mPID;
    // the address which the process has a breakpoint at.
    std::intptr_t mBPAddress;
    // is (this) object of the class has an active breakpoint.
    bool mBPEnabled;
    /* the lower byte of the instruction at address [m_addr] which is replaced
     with INT3 byte for making a breakpoint in x86 processors.*/
    uint8_t mSavedLowerInstructionByte;
};

#endif