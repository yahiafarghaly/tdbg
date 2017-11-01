#include "registers.h"

#ifdef __x86_64__


const std::array<register_descriptor, (std::size_t)reg_x86_64::NUM_OF_REGISTERS> g_register_descriptors{{
    {reg_x86_64::r15, 15, "r15"},
    {reg_x86_64::r14, 14, "r14"},
    {reg_x86_64::r13, 13, "r13"},
    {reg_x86_64::r12, 12, "r12"},
    {reg_x86_64::rbp, 6, "rbp"},
    {reg_x86_64::rbx, 3, "rbx"},
    {reg_x86_64::r11, 11, "r11"},
    {reg_x86_64::r10, 10, "r10"},
    {reg_x86_64::r9, 9, "r9"},
    {reg_x86_64::r8, 8, "r8"},
    {reg_x86_64::rax, 0, "rax"},
    {reg_x86_64::rcx, 2, "rcx"},
    {reg_x86_64::rdx, 1, "rdx"},
    {reg_x86_64::rsi, 4, "rsi"},
    {reg_x86_64::rdi, 5, "rdi"},
    {reg_x86_64::orig_rax, -1, "orig_rax"},
    {reg_x86_64::rip, -1, "rip"},
    {reg_x86_64::cs, 51, "cs"},
    {reg_x86_64::eflags, 49, "eflags"},
    {reg_x86_64::rsp, 7, "rsp"},
    {reg_x86_64::ss, 52, "ss"},
    {reg_x86_64::fs_base, 58, "fs_base"},
    {reg_x86_64::gs_base, 59, "gs_base"},
    {reg_x86_64::ds, 53, "ds"},
    {reg_x86_64::es, 50, "es"},
    {reg_x86_64::fs, 54, "fs"},
    {reg_x86_64::gs, 55, "gs"},
}};

/** 
 *  @brief      Get the value which exist in register [r] in (decimal) of a process [pid]  
 *  @details    Cast the first address of structure then advance by the required
 *              register [r] and get the value
 * 
 *  @return     register value and Error if exist
 * 
 */
Error get_register_value(pid_t pid, reg_x86_64 r,uint64_t* output ) {
    if(output == nullptr) return OutputIsNULL;
    if(r > reg_x86_64::NUM_OF_REGISTERS) return WrongRegisterNumber;

    user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, nullptr, &regs);

    *output =  *(reinterpret_cast<uint64_t*>(&regs) + (uint64_t)r) ;

    return Success;
}

/** 
 *  @brief      Set a [value] which exist in register [r] of a process [pid]. 
 *  @return     Error if exist
 */
Error set_register_value(pid_t pid, reg_x86_64 r, uint64_t value)
{
    if(r > reg_x86_64::NUM_OF_REGISTERS) return WrongRegisterNumber;
    user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, nullptr, &regs);

    *(reinterpret_cast<uint64_t*>(&regs) + (uint64_t)r) = value;
    ptrace(PTRACE_SETREGS, pid, nullptr, &regs);
    return Success;
}


/** 
 *  @brief      Return Register index in user_regs_struct from its name.
 *  @return     register index or Error if exist.
 */
Error get_register_from_name(const std::string& name,reg_x86_64* output) {

    if(output == nullptr) return OutputIsNULL;

    auto it = std::find_if(begin(g_register_descriptors), end(g_register_descriptors),
                           [name](auto&& rd) { return rd.reg_name == name; });

    if(it == std::end(g_register_descriptors)) return WrongRegisterName;                       
    *output = it->reg_index;
    return Success;
}

#else 
#warning "reading and modifing registers will not supported for x86 32bits"
#endif