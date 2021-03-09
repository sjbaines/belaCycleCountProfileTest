/***** cycleCounter.cpp *****/

#include "cycleCounter.h"


funcPtr profileFuncPtr = nullptr;


// Number of cycles taken by most recent profiled function
unsigned int codeBeingProfiled_cycles = 0;


// AuxTask which executes the function at *profileFuncPtr,
// and stores the number of cycles it took in codeBeingProfiled_cycles
void auxTask_rtCodeProfiler(void* dummy)
{
	if (profileFuncPtr == nullptr)
	{ // Prevent illegal access if pointer not set
		codeBeingProfiled_cycles = 1;
		return;
	}
	
	unsigned int preSec = get_cyclecount();
	(*profileFuncPtr)(); // Run the code being profiled
	unsigned int postSec = get_cyclecount();

	codeBeingProfiled_cycles = postSec - preSec;
}





// Simple wrapper to profile the function 'codeToProfile' and return the number of cycles it took.
unsigned int getCycleCountForPrimaryExecution(funcPtr codeToProfile, AuxiliaryTask& auxTask)
{
	// Point to code to test:
	profileFuncPtr = codeToProfile;

	// Mark test as not complete:
	codeBeingProfiled_cycles = 0;

	// Schedule the test:
	Bela_scheduleAuxiliaryTask(auxTask);

	// We can't directly wait for the task to complete, so we just kill time and watch for its result.
	while(codeBeingProfiled_cycles == 0) 
	{
		sleep(100);
	}
	
	return codeBeingProfiled_cycles;
}


// Run code 'numRun' times and return the min and max cycle counts
std::pair<unsigned int, unsigned int> getMinMaxCycleCountForPrimaryExecution(unsigned int numRuns, funcPtr codeToProfile, AuxiliaryTask& auxTask)
{
	unsigned int minCycles = 0;
	unsigned int maxCycles = 0;

	for (int i = 0; i != numRuns; ++i)
	{
		unsigned int cycleCount = getCycleCountForPrimaryExecution(codeToProfile, auxTask);
		if (i == 0 || cycleCount < minCycles)
			minCycles = cycleCount;
		if (cycleCount > maxCycles)
			maxCycles = cycleCount;
	}
	
	return std::pair<unsigned int, unsigned int>(minCycles, maxCycles);
}


// Run code 'numRun' times and return the full set of cycle times as a vector
std::vector<unsigned int> getCycleCountsForPrimaryExecution(unsigned int numRuns, funcPtr codeToProfile, AuxiliaryTask& auxTask)
{
	std::vector<unsigned int> cycles; 

	for (int i = 0; i != numRuns; ++i)
	{
		unsigned int cycleCount = getCycleCountForPrimaryExecution(codeToProfile, auxTask);
		cycles.push_back(cycleCount);
	}
	return cycles;
}




