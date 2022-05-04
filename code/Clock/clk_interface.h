#ifndef CLK_INTERFACE_H
#define CLK_INTERFACE_H

#include "../Defs.h"

void initClk();
int getClk();
void destroyClk(bool terminateAll);

#endif