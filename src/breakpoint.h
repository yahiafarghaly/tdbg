
#ifndef __BREAKPOINT_H
#define __BREAKPOINT_H

#include <iostream>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>

class breakpoint {
public:
    // removing the default constructor causes compilation error ?.
    breakpoint(){}
    // Paramterized constructor with address of breakpoint [addr] at a process [pid].
    breakpoint(pid_t pid, std::intptr_t addr)
        : m_pid{pid}, m_addr{addr}, m_enabled{false}, m_saved_data{}
    {}
    
    // Enable setting a breakpoint at a specific address [m_addr] of process [m_pid].
    void enable();
    // Disable setting a breakpoint at a specific address [m_addr] of process [m_pid].
    void disable();

    // is (this) object of the class has an active breakpoint.
    auto is_enabled() const -> bool { return m_enabled; }
    // return the address of the breakpoint.
    auto get_address() const -> std::intptr_t { return m_addr; }

private:
    // pid of the process which has a breakpoint.
    pid_t m_pid;
    // the address which the process has a breakpoint at.
    std::intptr_t m_addr;
    // is (this) object of the class has an active breakpoint.
    bool m_enabled;
    // the lower byte of the instruction at address [m_addr] which is replaced
    // with INT3 byte for making a breakpoint in x86 processors.
    uint8_t m_saved_data;
};

#endif