#ifndef _ENERGY_H
#define _ENERGY_H

#include "CS5550.h"


void Energy_Init(void);
void Energy_GetAMP(u8 *AMPData);
void Energy_SetAMP(u8  AMPData);
void Energy_Read(void);
u16 Get_Ad_Data(void);
u16 Get_Ad_Data_8(void);

#endif


