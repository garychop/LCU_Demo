//*****************************************************************************
// Filename: LCU_Demo.h
// Date: June 15, 2021
// Author: G. Chopcinski, KG Solutions, LLC
// 
//*****************************************************************************

#ifndef LCU_DEMO_H
#define LCU_DEMO_H

#define GX_ARC_DRAWING_SUPPORT

#include <stdio.h>
#include "gx_api.h"

#include "MuReva_LCU_demo_resources.h"
#include "MuReva_LCU_demo_specifications.h"

VOID screen_toggle(GX_WINDOW *new_win, GX_WINDOW *old_win);

typedef enum ENUM_TIMER_IDS {SPLASH_STARTUP_TIMER_ID = 1, CABLE_READY_TIMER_ID, THERAPY_TIMER_ID, THERAPY_COMPLETE_TIMER_ID, PAUSE_TIMER_ID};

#endif // LCU_DEMO_H