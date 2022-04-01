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

//-------------------------------------------------------------------------------------
// Forward Declarations

//-------------------------------------------------------------------------------------
// Typedefs and Macros

typedef enum ENUM_TIMER_IDS {SPLASH_STARTUP_TIMER_ID = 1, CABLE_READY_TIMER_ID, THERAPY_TIMER_ID,
	THERAPY_COMPLETE_TIMER_ID, PAUSE_TIMER_ID, ONE_SECOND_TIMER, STANDBY_TIMER};

typedef enum THERAPY_STATUS_IDS_ENUM {THERAPY_IDLE, THERAPY_READY_TO_START, THERAPY_IN_PROGRESS, THERAPY_PAUSED, THERAPY_COMPLETE} THERAPY_STATUS_IDS;

#define MOUTHPIECE_DB_SIZE 16

typedef struct // MOUTHPIECE_DATABASE
{
	long m_SerialNumber;
	int m_RemainingTherapyTime;
	GX_BOOL m_Attached;
	THERAPY_STATUS_IDS m_TherapyStatus;
} MOUTHPIECE_DATABASE_STRUCT;

//-------------------------------------------------------------------------------------
// Global Variables

extern GX_BOOL g_RingOn;
extern GX_BOOL g_LimitSwitchClosed;
//extern INT g_TherapyTime;
//extern THERAPY_STATUS_IDS g_TherapyInProcess;
//extern INT g_ErrorType;
//extern long g_SerialNumber;
extern MOUTHPIECE_DATABASE_STRUCT g_Mouthpiece_DB[MOUTHPIECE_DB_SIZE];

//-------------------------------------------------------------------------------------
// External Global functions

extern VOID screen_toggle(GX_WINDOW *new_win, GX_WINDOW *old_win);
extern VOID Enable_Limit_Switch (GX_WIDGET* limitSwitchButton, GX_BOOL enable);
extern void Enable_EEPROM_Buttons (PRIMARYTEMPLATE_CONTROL_BLOCK* templateBase, GX_BOOL enable_OK, GX_BOOL enable_BAD, GX_BOOL enable_EXPIRED, GX_BOOL enable_12HOUR, GX_BOOL enable_ATTACH);
extern void DisplayInformation_InBox (GX_MULTI_LINE_TEXT_VIEW *textViewWidget, GX_CHAR *myString, INT numberOfLines, GX_RESOURCE_ID myColor);
extern void DisplayInstruction_InBox (GX_MULTI_LINE_TEXT_VIEW *textViewWidget, GX_CHAR *myString, INT numberOfLines, GX_RESOURCE_ID myColor);
extern VOID Set_Standby_Exit_Screen (GX_WINDOW* window);

#endif // LCU_DEMO_H