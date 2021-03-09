# belaCycleCountProfileTest
Example project for profiling execution time of primary mode code using Cycle Counter register.

The ARM CPU has a 32 bit cycle counter register (called CCNT), which increments every clock cycle (and can also be set to increment every 64 clock cycles instead, for longer intervals).

By reading this register before and after a piece of code and taking the difference, you can tell how many clock cycles the code took to execute.
If doing this in a primary thread (e.g. in render()) then you will get the true execution time of your code, without interruptions from other tasks.
You can use this to directly measure the execution time of any piece(s) of code you want.

By default, however, this register cannot be read by user programs (attempting to do so generates an illegal instruction error - ouch!).  A kernel module can be used to change the register settings to allow user access.
See project https://github.com/sjbaines/kernelModuleCycleCounter for how to build and install such a kernel module.

Once you have the kernel module installed, this project provides a simple framework for profiling the execution time of blocks of code in primary mode,
either from the setup() function, or from render().

*** If you have not installed the kernel module, this project will generate an Illegal Instruction error when it attempts to access the CCNT register ***

This is a standard Bela project - just drag into the IDE as usual.
