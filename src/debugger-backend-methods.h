/** 
 *  @file    debugger-backend-methods.h
 *  @author  Yahia Farghaly
 *  @date    10/21/2017
 *  @version 1.0 
 *  
 *  @brief This file contains the functions which are used interally by 
 *         the debugger program.
 * 
 *  @section DESCRIPTION
 *   Helpful functions
 */
#ifndef __DEBUGGER_BACKEND_METHODS_H
#define __DEBUGGER_BACKEND_METHODS_H

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

/** 
 *  @brief     Split each element in a given string [s] by dimiter [delimiter]
 *              and return a vector of these elements   
 *  
 *  @param     [s]          The string to be splited.
 *  @param     [delimiter]  The given delimiter which the string will be split based on. 
 * 
 *  @return    A vector of strings where each string is a seperated element from the given [s]
 *                          by delimiter [delimiter].
 */
std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> out{};
    std::stringstream ss {s};
    std::string item;

    while (std::getline(ss,item,delimiter)) {
        out.push_back(item);
    }

    return out;
}

/** 
 *  @brief     Checks if a string [of] is a pre-fix of another string [s]
 *  @details    Input example: s = continue , of = contin , return false.
 *                           : s = continue , of = continue 5 , return true.
 *                           : s = continue , of = contin   1 , return false.
 *  @param      [s]         The string being scanned against [of].
 *  @param      [of]        The pre-fix word to search for.
 * 
 *  @return     true if it is pre-fix, otherwise false.
 */
bool is_prefix(const std::string& s, const std::string& of) {
    if (s.size() > of.size()) return false;
    return std::equal(s.begin(), s.end(), of.begin());
}

/** 
 *  @brief     Add the physical address of a process [tracee_pid] to process' memory
 *              stack address [addr]. where [addr] is an offest address inside process'
 *              stack physical addresses.
 *             
 *  @details   Get the the real physical first address of stack' process [tracee_pid] from
 *              /proc/<pid>/maps and add(+) it to [addr].
 *  
 *  @param     [tracee_pid]     The process ID.
 *  @param     [addr]           The offest address of the process in its stack memory region.  
 * 
 *  @return     Total real physical address of a certain location in process' memory stack.
 *              (e.g: 0x7f0c35ea8000 + 0x0000000000400915 = 0x0x7F0C362A8915)
 *  @Bug        seems to not working well, needs modification.
 */
std::intptr_t add_address_offest(pid_t tracee_pid,std::string addr)
{
    auto exec = [](const char* cmd) -> std::string {
            std::array<char, 128> buffer;
            std::string result;
            std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
            if (!pipe) throw std::runtime_error("popen() failed!");
            while (!feof(pipe.get())) {
                if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
                    result += buffer.data();
            }
            return result;
        };

    std::string map_path = "cat /proc/" + std::to_string(tracee_pid) + "/maps";
    std::string sh_command = map_path + "  | grep 'stack' | cut -d- -f1";
    auto start_offest = exec(sh_command.c_str());
    std::intptr_t output  = std::stol(start_offest, 0, 16) + std::stol(addr, 0, 16);
    return output;
}

/** 
 *  @brief                 Convert the string wether it is in hex format or decimal to decimal format.
 *             
 *  @details               The string on formal of 0xA or 10 to 10
 *  
 *  @param     [value]     A numerical value in form of string.
 * 
 *  @return                A signed numerical value.
 */
int64_t convert_numerical_string_into_decimal_number(std::string value)
{
    if((value.compare(0, 2, "0x") == 0 || value.compare(0, 2, "0X") == 0)
    && (value.size() > 2 )
    && (value.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos) )
    {
        // hex notation
        std::string hex_value = {value,2};
        return std::stol(hex_value, 0, 16);
    }
    else
    {
        // decimal notation
        return std::stol(value, 0, 10);
    }
}


#endif /*__DEBUGGER_BACKEND_METHODS_H*/