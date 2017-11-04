# Bugs List

## BUG-1

Setting a breakpoint at **return 0** statement address of the main, then informing the
debugger to continue the execution with **continue** results in no match between the stored breakpoint
address ***(return 0)*** and the address which triggered the SIGTRAP resulting in printing the Diagnostic messege which exist in 
**void debugger::continue_execution()** . The same happens if inserted bunch of breakpoint addresses and the last one was the address of  **(return 0)** . Otherwise, setting breakpoint works fine. Also this works fine if you inserted bunch of breakpoint addresses and execute  **continue** command then you set a breakpoint at **return 0** address.
