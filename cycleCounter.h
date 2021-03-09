/***** cycleCounter.h *****/

#ifndef CYCLE_COUNTER_H__
#define CYCLE_COUNTER_H__


#include <Bela.h>

#include <utility> // For 'std::pair'
#include <vector>


typedef void (*funcPtr)(void);

// To call any of these, need to have enabled user mode access to Cycle Count (CCNT) register via e.g. a kernel module,
// otherwise an Illegal Instruction error will be generated.

// Simple wrapper to profile the function 'codeToProfile' and return the number of cycles it took.
unsigned int getCycleCountForPrimaryExecution(funcPtr codeToProfile, AuxiliaryTask& auxTask);

// Run code 'numRun' times and return the full set of cycle times as a vector
std::vector<unsigned int> getCycleCountsForPrimaryExecution(unsigned int numRuns, funcPtr codeToProfile, AuxiliaryTask& auxTask);

// Run code 'numRun' times and return the min and max cycle counts
std::pair<unsigned int, unsigned int> getMinMaxCycleCountForPrimaryExecution(unsigned int numRuns, funcPtr codeToProfile, AuxiliaryTask& auxTask);



// AuxTask which executes the function at *profileFuncPtr,
// and stores the number of cycles it took in codeBeingProfiled_cycles
void auxTask_rtCodeProfiler(void* dummy);


// ====================================================================================
// The following code is largely taken from https://stackoverflow.com/q/3247373

inline unsigned int get_cyclecount (void)
{
  unsigned int value;
  asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(value)); // Reads CCNT Register into value
  return value;
}


// Simple counter init - no reset, no divider
inline void init_cyclecounter()
{
  unsigned int value = 17;
  // program the performance-counter control-register:
  asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(value));  
  // enable all counters:  
  asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));  
  // clear overflows:
  asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));
}


// do_reset resets counter to zero.
// enable_divider counts every 64 cycles instead of every cycle - to allow longer measurements without overflow
inline void init_perfcounters (bool do_reset = false, bool enable_divider = false)
{
  // in general enable all counters (including cycle counter)
  unsigned int value = 1;

  // peform reset:  
  if (do_reset)
  {
    value |= 2;     // reset all counters to zero.
    value |= 4;     // reset cycle counter to zero.
  } 

  if (enable_divider)
    value |= 8;     // enable "by 64" divider for CCNT.

  value |= 16;

  // program the performance-counter control-register:
  asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(value));  

  // enable all counters:  
  asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));  

  // clear overflows:
  asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));
}


#endif // CYCLE_COUNTER_H__


