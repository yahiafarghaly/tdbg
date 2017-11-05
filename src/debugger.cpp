
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
    int signal_status = wait_for_signal();
    if (WIFSTOPPED(signal_status))
    {
        /*EIP(in x86 mode) or RIP (in 64 mode) register hold the next instruction
         address to be executed by the processor in the traced program.
         Here [rip] variable contains the the current instruction address after
         substracting a one from it.

         Note: RIP reg is multiplied by 8 since each register is 8 byte long in array
         of registers and RIP value intself is the index of RIP register in this array. */
        intptr_t rip = ptrace(PTRACE_PEEKUSER, m_pid, 8 * RIP, NULL) - 1;
        printf("Process %d started and initially stopped at 0x%lx\n", m_pid, rip);
        this->debuggee_captured = true;
    }
    else
    {
        printf("Process %d doesn't send SIGTRAP !\n",m_pid);
        printf("tdbg exits.\n");
        exit(1);
    }

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

/* A small macro to define if the debuggee program is killed or in debug-mode.
  Only be used inside handle_command()
*/
#define IS_TRACED_PROCESS_CAPTURED()                  \
    do                                                \
    {                                                 \
        if (!debuggee_captured)                       \
        {                                             \
            printf("No runnable process to debug\n"); \
            return true;                              \
        }                                             \
    } while (0);

    auto args = split(line,' ');
    auto command = args[0];
    uint64_t register_value;

    if (is_prefix(command, "continue") || is_prefix(command, "c") || is_prefix(command, "cont")) {
        IS_TRACED_PROCESS_CAPTURED();
        this->continue_execution();
    }
    else if(is_prefix(command, "next"))
    {
        IS_TRACED_PROCESS_CAPTURED();
        this->next_instruction();
    }
    else if(is_prefix(command, "break")) {
        IS_TRACED_PROCESS_CAPTURED();
        std::string addr {args[1], 2}; //naively assume that the user has written 0xADDRESS , so take what after 0x
       this->set_breakpoint_at_address(std::stol(addr, 0, 16));
    }
    else if (is_prefix(command, "register"))
    {
        IS_TRACED_PROCESS_CAPTURED();
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
        else if(is_prefix(args[1], "write")) // ex: register write rax 0xdeadbeaf
        {
            reg_x86_64 r_index;
            if (get_register_from_name(args[2], &r_index) != Success)
                std::cout << "'"<< args[2]<< "'" << " is not exist in processor registers or not supported by the debugger\n";
            else
            {
                set_register_value(m_pid,r_index,convert_numerical_string_into_decimal_number(args[3]));
            }
        }
        else if (is_prefix(args[1], "dump")) {
                this->dump_registers();
        }
    }
    else if(is_prefix(command, "kill"))
    {
        IS_TRACED_PROCESS_CAPTURED();
        ptrace(PTRACE_SETOPTIONS, m_pid, nullptr, PTRACE_O_EXITKILL);
        debuggee_captured = false;
        m_breakpoints.clear();
        printf("Process %d is killed\n", m_pid);
    }
    else if(is_prefix(command, "run"))
    {
        if(!debuggee_captured)
        {
            if (this->run_traced_process())
            {
                debuggee_captured = true;
            }
            else
            {
                std::cout << "Failure to run " << m_prog_name << std::endl;
                debuggee_captured = false;
            }
        }
        else
        {
            intptr_t rip = ptrace(PTRACE_PEEKUSER, m_pid, 8 * RIP, NULL) - 1;
            printf("Process %d already has been started from a while and stopped at 0x%lx\n", m_pid, rip);
        }
    }
    else if(is_prefix(command, "exit") || is_prefix(command, "quit"))
    {
        ptrace(PTRACE_SETOPTIONS, m_pid, nullptr, PTRACE_O_EXITKILL);
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
void debugger::continue_execution()
{
    // Resume the execution of the debugee program.
    ptrace(PTRACE_CONT, m_pid, nullptr, nullptr);
    int signal_status = wait_for_signal();

    if (WIFSTOPPED(signal_status)) // such as SIGTRAP
    {
        /*EIP(in x86 mode) or RIP (in 64 mode) register hold the next instruction
         address to be executed by the processor in the traced program.
         Here [rip] variable contains the the current instruction address after
         substracting a one from it.

         Note: RIP reg is multiplied by 8 since each register is 8 byte long in array
         of registers and RIP value intself is the index of RIP register in this array. */
        intptr_t rip = ptrace(PTRACE_PEEKUSER, m_pid, 8 * RIP, NULL) - 1;
        // check if the current instruction address is a stored breakpoint.
        if (m_breakpoints.find(rip) != m_breakpoints.end())
        {
            // restore the instruction instead of breakpoint instruction.
            m_breakpoints[rip].stop_execution();
            // Set RIP reg to the decrement rip value so the debugger points to current restored instruction.
            ptrace(PTRACE_POKEUSER, m_pid, 8 * RIP, rip);
            printf("Process %d stopped at 0x%lx\n", m_pid, rip);
        }
        else
        {
            printf("---------- F O R Diagnostic only ----------\n");
            printf("RIP reg value doesn't match a stored breakpoint.\n");
            printf("RIP Value: 0x%lx\n",rip);
            printf("Available breakpoints addresses:\n");
            std::for_each(m_breakpoints.begin(), m_breakpoints.end(),
            [](const std::pair<std::intptr_t, breakpoint>& p) {
                printf("0x%lx\n",p.first);
            });
            printf("-------------------------------------------\n");
        }
    }
    else
    {
        printf("Debugged process is not running any more.\n");
        this->debuggee_captured = false;
        m_breakpoints.clear();
    }
}

/** 
 *  @brief     Set a breakpoint at the address [addr] of a process [m_pid].
 * 
 *  @return     void
 */
void debugger::set_breakpoint_at_address(std::intptr_t addr) {
    if(m_breakpoints.find(addr) == m_breakpoints.end())
    {
        breakpoint bp {m_pid, addr};
        if(bp.enable())
        {
            m_breakpoints[addr] = bp;
            std::cout << "Set a breakpoint at address 0x" << std::hex << addr << std::endl;
        }
        else
            std::cout << "Not valid address to set a breakpoint.\n";

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
int debugger::wait_for_signal()
{
    int wait_status;
    auto options = 0;
    // wait until the debuggee sends a SIGTRAP
    waitpid(m_pid, &wait_status, options);
    return  wait_status;
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

/** 
 *  @brief      Execute the traced process to run, mainly used if the debugged process
 *              is killed and an intention to re-run again is exist.
 * 
 *  @return     true, if successful start.
 */
bool debugger::run_traced_process()
{
    auto pid = fork();
    if (pid == 0) { 
        ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
        errno = 0;
        if (personality(ADDR_NO_RANDOMIZE) < 0)
        {
            if (EINVAL == errno)
                std::cout << "The kernel was unable to change the personality.\n";
        }
        /*
         Calling exec from the traced process causes a SIGTRAP being sent to it,
         also causing it to stop. */
        execl(m_prog_name.c_str(), m_prog_name.c_str(), nullptr);
    }
    else if (pid >= 1)  { 
        // The PID of the child process in parent
        // we're in the parent process
        // execute debugger
        this->m_pid = pid;
        int signal_status = wait_for_signal();
        if (WIFSTOPPED(signal_status))
        {
            intptr_t rip = ptrace(PTRACE_PEEKUSER, m_pid, 8 * RIP, NULL) - 1;
            printf("Process %d started and initially stopped at 0x%lx\n", m_pid, rip);
        }
        else
        {
            printf("Process %d doesn't send SIGTRAP !\n",m_pid);
            printf("tdbg exits.\n");
            exit(1);
        }
    }
    else
    {
        std::cerr << "tdbg: Failed to launch " << m_prog_name << " program\n";
        return false;
    }
    return true;
}

void debugger::next_instruction()
{
    ptrace(PTRACE_SINGLESTEP, m_pid, nullptr, nullptr);
    int signal_status = wait_for_signal();

    if (WIFSTOPPED(signal_status)) // such as SIGTRAP
    {
        printf("Process %d stopped at 0x%lx\n", m_pid, this->get_current_instruction_address());
    }
    else
    {
        printf("Debugged process is not running any more.\n");
        this->debuggee_captured = false;
        m_breakpoints.clear();
    }
}

std::intptr_t debugger::get_current_instruction_address()
{
    return (ptrace(PTRACE_PEEKUSER, m_pid, 8 * RIP, NULL) - 1);
}