//*****************************************************************************
// Filename: LCU_Simulator_Main.h
// Date: June 15, 2021
// Author: G. Chopcinski, KG Solutions, LLC
// 
//*****************************************************************************

#ifndef LCU_SIMULATOR_MAIN_H
#define LCU_SIMULATOR_MAIN_H

#include "LCU_Demo.h"

//extern DIAGNOSTICSCREEN_CONTROL_BLOCK DiagnosticScreen;
//
extern UINT win32_graphics_driver_setup_24xrgb(GX_DISPLAY *display);
VOID *memory_allocate(ULONG size);
void memory_free(VOID *mem);

GX_WINDOW_ROOT           *root;

#endif // LCU_SIMULATOR_MAIN_H