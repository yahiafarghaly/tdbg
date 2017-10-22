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
 *  @return     void
 */
void breakpoint::enable() {
    errno = 0;
	// Fetch the program instruction at the desired address of a specific process.
    auto data = ptrace(PTRACE_PEEKTEXT, m_pid, m_addr, nullptr);
    if (data < 0)
    {
        switch (errno)
        {
        case EFAULT:
            std::cout << "ptrace error: EFAULT\n";
            break;
        case EIO:
            std::cout << "ptrace error: EIO\n";
            break;
        case ESRCH:
            std::cout << "ptrace error: ESRCH\n";
            break;
        case EPERM:
            std::cout << "ptrace error: EPERM\n";
            break;
        case EBUSY:
            std::cout << "ptrace error: EBUSY\n";
            break;
        case EINVAL:
            std::cout << "ptrace error: EINVAL\n";
            break;
        default:
            break;
        }
    }
    // Save the lower byte which will be replaced with INT3 instruction.
    m_saved_data = static_cast<uint8_t>(data & 0xff);
    uint64_t int3 = 0xcc;
    // Set the lower byte to INT3 instruction
    uint64_t data_with_int3 = ((data & ~0xff) | int3);
    // Push the modified instruction with the breakpoint to the same address it was fetched.
    ptrace(PTRACE_POKETEXT, m_pid, m_addr, data_with_int3);
    // Enable that (this) object of the class has a breakpoint at [m_addr] of [m_pid] process.
    m_enabled = true;
}

/** 
 *  @brief     Disable setting a breakpoint at a specific address [m_addr] of
 *              process [m_pid].
 * 
 *  @details    Restore the old lower byte of the instruction instead of INT3 instruction.
 * 
 *  @Note       Works only for x86 processors.
 *  @return     void
 */
void breakpoint::disable() {
    // Fetch the INT3 instruction.
    auto data = ptrace(PTRACE_PEEKTEXT, m_pid, m_addr, nullptr);
    // Restore the old lower byte.
    auto restored_data = ((data & ~0xff) | m_saved_data);
    // Push the old instruction without 0xcc byte.
    ptrace(PTRACE_POKETEXT, m_pid, m_addr, restored_data);
    // Disbale that (this) object of the class has a breakpoint at [m_addr] of [m_pid] process.
    m_enabled = false;
}