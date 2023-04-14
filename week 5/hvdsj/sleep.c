/*
 * sleep.c
 *
 *  Created on: Sep 12, 2017
 *      Author: Vipraja
 */
#include "sleep.h"
#include "stdbool.h"
#include "em_emu.h"




int sleep_block_counter[4] = {0};                         /* Number of sleep modes = 4  */


void sleep(void)
{
	if (sleep_block_counter[0] > 0)
	{                                                    /* Blocked everything below EM0, so just return */
		return;
	    }
	else if (sleep_block_counter[1] > 0)
	{
	        EMU_EnterEM1();                              /* Blocked everything below EM1, enter EM1 */
	    }
	else if (sleep_block_counter[2] > 0)
	{
	        EMU_EnterEM2(true);                              /* Blocked everything below EM2, enter EM2 */
	}
	else
	{
	        EMU_EnterEM3(true);                              /* Blocked everything below EM3, enter EM3 */
	}
	    return;
	}


void blockSleepMode(sleepstate_enum minimumMode)
{
CORE_AtomicDisableIrq();
sleep_block_counter[minimumMode]++;
CORE_AtomicEnableIrq();
}

void unblockSleepMode(sleepstate_enum minimumMode)
{
CORE_AtomicDisableIrq();
if(sleep_block_counter[minimumMode] > 0) {
sleep_block_counter[minimumMode]--;
}
CORE_AtomicEnableIrq();
}

