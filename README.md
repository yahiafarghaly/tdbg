# tdbg

is a tiny debugger for x86_64 platform similar to gdb.
it is for educational purpose.

The learning referrences which the implementation may seem alike:

- [Simon Brand's blog](https://blog.tartanllama.xyz/writing-a-linux-debugger-setup/)
- [low tech blog](http://sigalrm.blogspot.com.eg/2010/07/writing-minimal-debugger.html)

## Available Commands

| *Command* [**Args**]         | Functionality                                                        |
|-----------------|----------------------------------------------------------------------|
| *continue*,*c*,*cont* | Resume the execution of the traced process.                         |
| *break* 0x**ADDRESS** | Set a breakpoint at a certain address of the address space of the traced process. |
| *read register* **Reg Name** | Read the register value of one of supported registers. Value will be shown in decimal notation. |
| *write register* **Reg Name** **Reg Value** | Write a value to a specific register. **Reg Value** can be in decimal or hex notation. |
| *register dump* | Show a list of the processor registers values for the current process. |
| *exit*,*quit* | Terminate the traced process and exit the debugger. |
| *run* | Execute the traced process and stopped it at its entry point. |
| *kill* | Kill the traced process. |
| *next* | Make a single step forward in the traced process execution (i.e: move to the next instruction). |