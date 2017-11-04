
#ifndef __DEBUGGER_H
#define __DEBUGGER_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <sys/personality.h>
#include <linenoise.h>
#include "breakpoint.h"
#include "registers.h"
#include "error_enum.h"

class debugger {
public:
    debugger (std::string prog_name, pid_t pid)
        : m_prog_name{std::move(prog_name)}, m_pid{pid} {debuggee_captured = false;}

    // Start the debugger
    void run();
    // Handle the debugger user commands.
    bool handle_command(const std::string& line);
    // wait until the debuggee sends a SIGTRAP signal.
    int wait_for_signal();

private:
    // The debuggee program name
    std::string m_prog_name;
    // The debuggee program Process ID
    pid_t m_pid;
    /* An un-order map of breakpoint objects to be access by its addresses hashes.
         m_breakpoints[breakpoint address] -> breakpoint object. */
    std::unordered_map<std::intptr_t, breakpoint> m_breakpoints;
    // To determine if traced process is runnable or not.
    bool debuggee_captured;

    /*****  Debugger Control functions on debuggee  *****/

    // Continue execution of debuggee program with process ID [m_pid]
    void continue_execution();
    // Set a breakpoint at the process ID [m_pid].
    void set_breakpoint_at_address(std::intptr_t addr);
    // Show the current register values of process with [m_pid].
    void dump_registers();
    // start the debuggee program 
    bool run_traced_process();
};

#endif /* __DEBUGGER_H */