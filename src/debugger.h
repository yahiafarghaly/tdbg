
#ifndef __DEBUGGER_H
#define __DEBUGGER_H

#include <iostream>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <linenoise.h>

class debugger {
public:
    debugger (std::string prog_name, pid_t pid)
        : m_prog_name{std::move(prog_name)}, m_pid{pid} {}

    void run();
    void handle_command(const std::string& line);
    void continue_execution();

private:
    std::string m_prog_name;
    pid_t m_pid;
};

#endif /* __DEBUGGER_H */