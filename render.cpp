// Profile test example code by Steve Baines, 9th March 2021
// Feel free to do anything you want with this code.
//
// Example project for profiling using Cycle Count (CCNT) register.
// Before running this code you MUST first enable user access to the register, e.g. via a kernel module.
// If you do not, then this code will generate an Illegal Instruction error and fail.
//
// To load the module (once you have built it), use the following at the command line, pointing to wherever your kernel module is:
// # insmod ./projects/kernelModuleCycleCounter/cycleCounter.ko
// Then, to check that it loaded, check for a log message:
// # dmesg | tail -1


#include "cycleCounter.h"

#include <Bela.h>

#include <cmath>
#include <libraries/math_neon/math_neon.h>

// =============================================

// How many times to run each profile test
constexpr int numRuns = 16;

// An AuxTask handle which we will use to run the task in primary from within setup.
AuxiliaryTask auxTask;

// =============================================
// Example bits of code to test

// Note use of a static here - otherwise the code being tested gets optimised away to nothing!
static float a = 0.0f;

constexpr int iters = 100;

// Some pieces of code to test
void profileTest_empty()
{
	// Do nothing
}

void profileTest_sin()
{
	a = 1.0f;
	for (int i = 0; i < iters; ++i)
		a = sin(a);
}

void profileTest_sinf()
{
	a = 1.0f;
	for (int i = 0; i < iters; ++i)
		a = sinf(a);
}

void profileTest_sinf_neon()
{
	a = 1.0f;
	for (int i = 0; i < iters; ++i)
		a = sinf_neon(a);
}



//=============================================
// Used to run all profile tests from setup using an AuxTask:
void setup_runAllProfilingTests()
{
	std::vector<unsigned int> vecCycles;

	vecCycles = getCycleCountsForPrimaryExecution(numRuns, profileTest_empty, auxTask);
	printf("SETUP: Cycles for 'profileTest_empty':"); for (unsigned int cycles : vecCycles) {	printf("\t%d", cycles); } printf("\n");

	vecCycles = getCycleCountsForPrimaryExecution(numRuns, profileTest_sin, auxTask);
	printf("SETUP: Cycles for 'profileTest_sin':"); for (unsigned int cycles : vecCycles) {	printf("\t%d", cycles); } printf("\n");

	vecCycles = getCycleCountsForPrimaryExecution(numRuns, profileTest_sinf, auxTask);
	printf("SETUP: Cycles for 'profileTest_sinf':"); for (unsigned int cycles : vecCycles) {	printf("\t%d", cycles); } printf("\n");

	vecCycles = getCycleCountsForPrimaryExecution(numRuns, profileTest_sinf_neon, auxTask);
	printf("SETUP: Cycles for 'profileTest_sinf_neon':"); for (unsigned int cycles : vecCycles) {	printf("\t%d", cycles); } printf("\n");
}


//=============================================
// Used to run profile tests from render:

extern funcPtr profileFuncPtr;
// Number of cycles taken by most recent profiled function
extern unsigned int codeBeingProfiled_cycles;

void render_runNextProfileTest()
{
	// We only run one test per invocation, to limit the print rate to something acceptable
	static int nextProfileTest = 0;
	static int subIdx = 0;
	if (nextProfileTest < 4)
	{
		codeBeingProfiled_cycles = 0;
		if (subIdx == 0)
		{
			switch(nextProfileTest)
			{
				case 0:
					profileFuncPtr = profileTest_empty;
					rt_printf("RENDER: Cycles for 'profileTest_empty':");
					break;
				case 1:
					profileFuncPtr = profileTest_sin;
					rt_printf("RENDER: Cycles for 'profileTest_sin':");
					break;
				case 2:
					profileFuncPtr = profileTest_sinf;
					rt_printf("RENDER: Cycles for 'profileTest_sinf':");
					break;
				case 3:
					profileFuncPtr = profileTest_sinf_neon;
					rt_printf("RENDER: Cycles for 'profileTest_sinf_neon':");
					break;
			}
		}

		switch(subIdx++)
		{ // Run one test per render call, to limit rate of messages
			default:
				auxTask_rtCodeProfiler(nullptr);
				rt_printf("\t%d", codeBeingProfiled_cycles);
				break;
			case numRuns:
				rt_printf("\n");
				++nextProfileTest;
				subIdx = 0;
				break;
		}
	}
}
//=============================================





bool setup(BelaContext *context, void *userData)
{
	// To call this, need to have 'cycleCount' kernel module installed.
	init_cyclecounter(); // Start the counter running

	// Create profiler task which runs at same priority as audio 'render' tasks.
	// Because we are still in setup, audio tasks are not yet running, so we are not competing with them for time.
	auxTask = Bela_createAuxiliaryTask(auxTask_rtCodeProfiler, BELA_AUDIO_PRIORITY, "auxTask_rtCodeProfiler", NULL);

	// Run all the tests
	setup_runAllProfilingTests();

	return true;
}



void render(BelaContext *context, void *userData)
{
	// Run tests directly on render thread, to compare times with those from setup:
	// (In general, you wouldn't use this - only here for comparison)
	render_runNextProfileTest();
}


void cleanup(BelaContext *context, void *userData)
{
}

