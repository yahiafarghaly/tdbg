
#include "debugger.h"
#include "debugger-backend-methods.h"

// debugger Class methods
void debugger::run() {
    int wait_status;
    auto options = 0;
    // wait until the traced process sends a SIGTRAP,then forward
    waitpid(m_pid, &wait_status, options);

    char* line = nullptr;
    while((line = linenoise("tdbg> ")) != nullptr) {
        if(!handle_command(line)) break;
        linenoiseHistoryAdd(line);
        linenoiseFree(line);
    }
}


bool debugger::handle_command(const std::string& line) {

    auto args = split(line,' ');
    auto command = args[0];

    if (is_prefix(command, "continue") || is_prefix(command, "c") || is_prefix(command, "cont")) {
        continue_execution();
    }
    else if(is_prefix(command, "break")) {
        std::string addr {args[1], 2}; //naively assume that the user has written 0xADDRESS , so take what after 0x
        std::intptr_t new_address = add_address_offest(m_pid,addr);
        //set_breakpoint_at_address(std::stol(addr, 0, 16));
        set_breakpoint_at_address(new_address);
    }
    else if(is_prefix(command, "exit"))
    {
        return false;
    }
    else {
        std::cerr << "Unknown command\n";
    }

    return true;
}



void debugger::continue_execution() {
    // resume the execution of the debugee program.
    ptrace(PTRACE_CONT, m_pid, nullptr, nullptr);

    int wait_status;
    auto options = 0;
    // wait until the debuggee finish
    waitpid(m_pid, &wait_status, options);

}

void debugger::set_breakpoint_at_address(std::intptr_t addr) {
    std::cout << "Set a breakpoint at address 0x" << std::hex << addr << std::endl;
    breakpoint bp {m_pid, addr};
    bp.enable();
    m_breakpoints[addr] = bp;
}