
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
	breakpoint(){}
    breakpoint(pid_t pid, std::intptr_t addr)
        : m_pid{pid}, m_addr{addr}, m_enabled{false}, m_saved_data{}
    {}
	
    void enable();
    void disable();

    auto is_enabled() const -> bool { return m_enabled; }
    auto get_address() const -> std::intptr_t { return m_addr; }

private:
    pid_t m_pid;
    std::intptr_t m_addr;
    bool m_enabled;
    uint8_t m_saved_data; //data which used to be at the breakpoint address
};

#endif