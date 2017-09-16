#include <iostream>
#include <sys/ptrace.h>
#include <unistd.h>
#include "debugger.h"

int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        std::cerr << "Program name not specified\n";
        return -1;
    }

    auto prog = argv[1];
    auto pid = fork();
    
    if (pid == 0) { 
        /*
          In child process, execute the debuggee program
          PTRACE_TRACEME turns the calling thread into a tracee which allow the parent thread to
          examine and change the tracee's memory and registers. 
        */
        ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
        execl(prog, prog, nullptr);
    }
    else if (pid >= 1)  { 
        // The PID of the child process in parent
        // we're in the parent process
        // execute debugger
        debugger dbg{prog, pid};
        dbg.run();
    }
    else
        std::cerr << "tdbg: Failed to launch " << prog << " program\n";

    return 0;
}