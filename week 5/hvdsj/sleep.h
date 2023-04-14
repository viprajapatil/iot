/*
 * sleep.h
 *
 *  Created on: Sep 12, 2017
 *      Author: Vipraja
 */

#ifndef SRC_SLEEP_H_
#define SRC_SLEEP_H_

typedef enum {
    EM0 = 0,
    EM1 = 1,
    EM2 = 2,
    EM3 = 3,
    EM4 = 4
} sleepstate_enum;


#include <stdint.h>
#include <stdbool.h>

void sleep();
void blockSleepMode();
void unblockSleepMode();

#endif /* SRC_SLEEP_H_ */
