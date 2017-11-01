
#include "debugger.h"
#include "debugger-backend-methods.h"
#include <iomanip>

/** 
 *  @brief     Start the debugger
 * 
 *  @details    The entry point of the debugger where it waits for user commands.
 * 
 *  @return     void
 */
void debugger::run() {
    /* 
    Wait until the debuggee program(i.e child) sends a SIGTRAP signal where it
    waits at its entry point till debugger sends it a signal (PTRACE_CONT) for
    Contining its execution 
    */
    wait_for_signal();

    char* line = nullptr;
    // use linenoise for making a nice command line prompt for the debugger.
    while((line = linenoise("tdbg> ")) != nullptr) {
        if(!handle_command(line)) break;
        linenoiseHistoryAdd(line);
        linenoiseFree(line);
    }
}

/** 
 *  @brief     Handling the commands of the debugger
 * 
 *  @return     false when exit command is given,otherwise true.
 */
bool debugger::handle_command(const std::string& line) {

    auto args = split(line,' ');
    auto command = args[0];
    uint64_t register_value;

    if (is_prefix(command, "continue") || is_prefix(command, "c") || is_prefix(command, "cont")) {
        this->continue_execution();
    }
    else if(is_prefix(command, "break")) {
        std::string addr {args[1], 2}; //naively assume that the user has written 0xADDRESS , so take what after 0x
       // std::intptr_t new_address = add_address_offest(m_pid,addr);
       // set_breakpoint_at_address(new_address);
       this->set_breakpoint_at_address(std::stol(addr, 0, 16));
    }
    else if (is_prefix(command, "register"))
    {
        if (is_prefix(args[1], "read"))
        {
            reg_x86_64 r_index;
            if (get_register_from_name(args[2], &r_index) != Success)
                std::cout << "'"<< args[2]<< "'" << " is not exist in processor registers or not supported by the debugger\n";
            else
            {
                get_register_value(m_pid, r_index, &register_value);
                std::cout << register_value << std::endl;
            }
        }
        else if (is_prefix(args[1], "dump")) {
                this->dump_registers();
        }
    }
    else if(is_prefix(command, "exit") || is_prefix(command, "quit"))
    {
        return false;
    }
    else {
        std::cerr << "Unknown command\n";
    }

    return true;
}


/** 
 *  @brief     continue execution of the debuggee program until the next
 *              SIGTRAP from debuggee.
 * 
 *  @details    SIGTRAP: The SIGTRAP signal is sent to a process(in our case:debugger) when an exception
 *              (or trap) occurs: a condition that a debugger has requested to be 
 *              informed of - for example, when a particular function is executed, 
 *              or when a particular variable changes value or at a certain breakpoint.
 * 
 *  @return     void
 */
void debugger::continue_execution() {
    // Resume the execution of the debugee program.
    ptrace(PTRACE_CONT, m_pid, nullptr, nullptr);
    wait_for_signal();
}

/** 
 *  @brief     Set a breakpoint at the address [addr] of a process [m_pid].
 * 
 *  @return     void
 */
void debugger::set_breakpoint_at_address(std::intptr_t addr) {
    if(m_breakpoints.find(addr) == m_breakpoints.end())
    {
        std::cout << "Set a breakpoint at address 0x" << std::hex << addr << std::endl;
        breakpoint bp {m_pid, addr};
        bp.enable();
        m_breakpoints[addr] = bp;
    }
    else
        std::cout << "A breakpoint is already set at 0x" << std::hex << addr << std::endl;
}

/** 
 *  @brief      An encapsulation of the operation of waitpid
 *  @details    Wait the debuggee program to send a SIGTRAP signal where it it is 
 *              got trapped by a breakpoint for example.
 * 
 *  @return     void
 */
void debugger::wait_for_signal()
{
    int wait_status;
    auto options = 0;
    // wait until the debuggee sends a SIGTRAP
    waitpid(m_pid, &wait_status, options);
}

/** 
 *  @brief      Show the current register contents of process with [m_pid](i.e debuggee).
 * 
 *  @return     void
 */
void debugger::dump_registers()
{
    uint64_t register_value;
    std::cout << std::left << "[Register Name]" << std::setw(16) << std::right << "[Value In Hex]\n";
    for (const auto& rd : g_register_descriptors) {
        get_register_value(m_pid, rd.reg_index, &register_value);
        std::cout << std::left<<"["<< rd.reg_name <<"]"<<  std::setw(16) << std::right << std::hex<< register_value<< std::endl;
    }
}