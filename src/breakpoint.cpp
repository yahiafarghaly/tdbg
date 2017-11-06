#include "breakpoint.h"

/** 
 *  @brief     Enable setting a breakpoint at a specific address [m_addr] of
 *              process [m_pid].
 * 
 *  @details    In x86, Overwriting an instruction at a specific address by
 *              int3 instruction(opcode = 0xcc) will trigger the CPU to execute
 *              a handler in CPU interrupt vector table which the OS - in case of linux -
 *              pass it the process as a SIGTRAP.
 *              It is one byte long instruction which makes it perfect to be caught by ptrace.
 * 
 *  @Note       Works only for x86 processors.
 *  @return     true if the memory address is a valid address. false,otherwise.
 */
bool breakpoint::enable() {
    errno = 0;
    uint64_t int3 = 0x000000CC;
    uint64_t data_with_int3;
	// Fetch the program instruction at the desired address of a specific process.
    auto data = ptrace(PTRACE_PEEKTEXT, m_pid, m_addr, nullptr);
    if (data < 0)
    {
        switch (errno)
        {
        case EFAULT:
            std::cout << "ptrace error: EFAULT\n";
            goto FAIL;
            break;
        case EIO:
            std::cout << "ptrace error: EIO\n";
            goto FAIL;
            break;
        case ESRCH:
            std::cout << "ptrace error: ESRCH\n";
            goto FAIL;
            break;
        case EPERM:
            std::cout << "ptrace error: EPERM\n";
            goto FAIL;
            break;
        case EBUSY:
            std::cout << "ptrace error: EBUSY\n";
            goto FAIL;
            break;
        case EINVAL:
            std::cout << "ptrace error: EINVAL\n";
            goto FAIL;
            break;
        default:
            // Not an error if the instruction as a value is less than zero.
            break;
        }
    }
    /* Inject the magical byte of making a software interrupt 
       which is specifically defined for use by debuggers in intel processors. */ 
    m_saved_data = static_cast<uint8_t>(data & 0x000000ff); //save bottom byte
    data_with_int3 = ((data & ~0x000000ff) | int3);         //set bottom byte to 0xcc
    ptrace(PTRACE_POKETEXT, m_pid, m_addr, data_with_int3);

    // Enable that (this) object of the class has a breakpoint at [m_addr] of [m_pid] process.
    m_enabled = true;
    return  true;

 FAIL:   return false;
}

/** 
 *  @brief     Disable setting a breakpoint at a specific address [m_addr] of
 *              process [m_pid].
 * 
 *  @details    Restore the old the instruction instead of INT3 instruction.
 * 
 *  @Note       Works only for x86 processors.
 *  @return     void
 */
void breakpoint::disable() {
    // restore instruction
    this->stop_execution();
    // Disbale that (this) object of the class has a breakpoint at [m_addr] of [m_pid] process.
    m_enabled = false;
}

/** 
 *  @brief     Restoring the instruction which was corrupted by injecting INT3 instruction 
 *              at a specific address [m_addr] of process [m_pid].
 *  @return     void
 */
void breakpoint::stop_execution()
{
    // restore instruction
    auto data = ptrace(PTRACE_PEEKTEXT, m_pid, m_addr, nullptr);
    auto restored_data = ((data & ~0x000000ff) | m_saved_data);
    ptrace(PTRACE_POKEDATA, m_pid, m_addr, restored_data);
}