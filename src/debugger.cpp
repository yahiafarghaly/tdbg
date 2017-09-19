
#include "debugger.h"


// helpful methods

static std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> out{};
    std::stringstream ss {s};
    std::string item;

    while (std::getline(ss,item,delimiter)) {
        out.push_back(item);
    }

    return out;
}

static bool is_prefix(const std::string& s, const std::string& of) {
    if (s.size() > of.size()) return false;
    return std::equal(s.begin(), s.end(), of.begin());
}

static std::intptr_t add_address_offest(pid_t tracee_pid,std::string addr)
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