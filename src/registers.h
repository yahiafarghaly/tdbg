#ifndef __REGISTERS_H
#define __REGISTERS_H

#include <sys/user.h>
#include <sys/ptrace.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <algorithm>
#include <array>
#include "error_enum.h"

#ifdef __x86_64__

/*  On the same mapping of user_regs_struct as ptrace returns 
 *  so we can access it easily  */
enum class reg_x86_64
{
  r15 = 0,
  r14,
  r13,
  r12,
  rbp,
  rbx,
  r11,
  r10,
  r9,
  r8,
  rax,
  rcx,
  rdx,
  rsi,
  rdi,
  orig_rax,
  rip,
  cs,
  eflags,
  rsp,
  ss,
  fs_base,
  gs_base,
  ds,
  es,
  fs,
  gs,
  NUM_OF_REGISTERS /* A silly and not safe way for figuring out
                     how many registers exist in this enum automatically */  
};


struct register_descriptor
{
    reg_x86_64 reg_index;
    int reg_dwarf_number;
    std::string reg_name;
};

/*  Get the value which exist in register [r] of a process [pid]  */
Error get_register_value(pid_t pid, reg_x86_64 r,uint64_t* output );
/*  Return Register index in user_regs_struct from its name       */
Error get_register_from_name(const std::string& name,reg_x86_64* output);

extern const std::array<register_descriptor, (std::size_t)reg_x86_64::NUM_OF_REGISTERS> g_register_descriptors;

#else 
#warning "reading and modifing registers will not supported for x86 32bits"
#endif


#endif /* __REGISTERS_H */