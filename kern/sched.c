
#include <inc/assert.h>

#include <kern/env.h>
#include <kern/pmap.h>


// Choose a user environment to run and run it.
void
sched_yield(void)
{
	// Your code here to implement simple round-robin scheduling.
	// Search through envs array for a runnable environment,
	// in circular fashion starting from the previously running env,
	// and switch to the first such environment found.
	// But never choose envs[0], the idle environment,
	// unless NOTHING else is runnable.

	// Run the special idle environment when nothing else is runnable.
	assert(envs[0].env_status == ENV_RUNNABLE);
	env_run(&envs[0]);
}

