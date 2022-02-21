//*****************************************************************************
// Filename: ReadyScreen_Handler.c
//
// Date: June 16, 2021
//
// Author: G. Chopcinski, Kg Solutions, LLC
// 
//*****************************************************************************

#include "LCU_Demo.h"

#include "math.h"

//*************************************************************************************
// Local Macros
//*************************************************************************************

#define GRAPH_CENTER_PT_XPOS 160	// From Left of screen
#define GRAPH_CENTER_PT_YPOS 110	// From Top of screen

typedef enum {
	STATE_READY = 0,
	STATE_CABLE_INSERTED,
	STATE_READING_EEPROM,
	STATE_CABLE_OK,
	STATE_MOUTHPIECE_EXPIRED,
	STATE_SERIAL_NUMBER_PROMPT,
	STATE_USAGE,
	STATE_PREPARE_PATIENT,
	STATE_PATIENT_IS_READY,
	STATE_CABLE_FAULT,
	STATE_READY_TO_GO,
	STATE_THERAPY_IN_PROCESS,
	STATE_THERAPY_PAUSED,
	STATE_THERAPY_CONTINUE,
	STATE_THERAPY_MOUTHPIECE_FAULT,
	STATE_THERAPY_RECOVER,
	STATE_THERAPY_IS_COMPLETE,
	STATE_OUT_OF_USES,
	STATE_SYSTEM_ERROR
} STATES_ENUM;

//*************************************************************************************
// External References
//*************************************************************************************

//*************************************************************************************
// Local/Global variables
//*************************************************************************************

STATES_ENUM g_State;
UINT g_Used = 39;
GX_BOOL g_RingOn = FALSE;
char g_TimeString[16];
GX_BOOL g_FullTimeGraphic = FALSE;

int g_MouthPiece_SerialNumber = 1;
char g_SerialNumberString[64] = "1";
GX_CHAR *g_PromptString = NULL;

GX_BOOL g_LimitSwitchClosed = FALSE;
INT g_TherapyTime = 0;
GX_BOOL g_TherapyInProcess = FALSE;

void (*gp_State)(void);

void InsertMouthpieceState(void)
{
}

//*************************************************************************************
// Function Name: ReadyScreen_Event_Function
//
// Description: This functions handles the Splash screen
//
//*************************************************************************************

//*************************************************************************************

void EnableEEPROMPT_Button (GX_BOOL enable_OK, GX_BOOL enable_BAD, GX_BOOL enable_EXPIRED)
{
	ULONG widgetStyle;

	gx_widget_style_get ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_EEPROM_OK_Button, &widgetStyle);

	// Enable/Disable OK button
	if (enable_OK)
		widgetStyle |= GX_STYLE_ENABLED;
	else
		widgetStyle &= (~GX_STYLE_ENABLED);
	gx_widget_style_set ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_EEPROM_OK_Button, widgetStyle);

	// Enable/Disable BAD button
	if (enable_BAD)
		widgetStyle |= GX_STYLE_ENABLED;
	else
		widgetStyle &= (~GX_STYLE_ENABLED);
	gx_widget_style_set ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_EEPROM_Fail_Button, widgetStyle);
	
	// Enable/Disable EXPIERED button
	if (enable_EXPIRED)
		widgetStyle |= GX_STYLE_ENABLED;
	else
		widgetStyle &= (~GX_STYLE_ENABLED);
	gx_widget_style_set ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_EEPROM_Expired_Button, widgetStyle);
}

//*************************************************************************************

VOID ReadyScreen_Draw_Function (GX_WINDOW *window)
{
	//UINT status;

	gx_window_draw(window);

	gp_State = InsertMouthpieceState;
	//status = gx_context_brush_get(&brush);
	//originalBrush = *brush;

	//brush->gx_brush_width = 2;
	//brush->gx_brush_fill_color = GX_COLOR_GREEN;
	//brush->gx_brush_line_color = GX_COLOR_GREEN;
	//status = gx_canvas_arc_draw(100, 100, 100, 0, 90); 
}

//*************************************************************************************

UINT ReadyScreen_Event_Function (GX_WINDOW *window, GX_EVENT *event_ptr)
{
	ULONG widgetStyle;
	UINT mySize, myBufSize;
	ULONG checkedStatus;

	switch (event_ptr->gx_event_type)
	{
	case GX_EVENT_SHOW:
		g_State = STATE_READY;
		g_LimitSwitchClosed = FALSE;
		g_TherapyInProcess = FALSE;
		gx_widget_hide (&ReadyScreen.ReadyScreen_TherpayTime_RadialProgressBar);
		gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
		gx_widget_hide (&ReadyScreen.ReadyScreen_Time_Prompt);
		gx_widget_hide (&ReadyScreen.ReadyScreen_GreenTick_Icon);
		// This button may have been disabled.... we need to enable it.
		gx_widget_style_get ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_LimitSwitch_Button, &widgetStyle);
		widgetStyle |= GX_STYLE_ENABLED;
		gx_widget_style_set ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_LimitSwitch_Button, widgetStyle);

		gx_multi_line_text_button_text_id_set (&ReadyScreen.base.PrimaryTemplate_SystemError_Button, GX_STRING_ID_STRING_24);	// "Cause System Error"

		sprintf_s (g_TimeString, sizeof (g_TimeString), "%d:%02d", g_TherapyTime / 60, g_TherapyTime % 60);
		EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE);
		gx_prompt_text_id_set (&ReadyScreen.base.PrimaryTemplate_LimitSwitchStatus_prompt, GX_ID_NONE);	// none
		gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_WHITE, GX_PIXELMAP_ID_STATUSRING_WHITE);
		gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_10);	// "INSERT MOUTHPIECE"
		break;

	//----------------------------------------------------------------------------------------
	// Pause/Play button is pushed
	//----------------------------------------------------------------------------------------
	case GX_SIGNAL (PLAY_BTN_ID, GX_EVENT_CLICKED):
		checkedStatus = ReadyScreen.ReadyScreen_AlternateTime_CheckBox.gx_widget_style;
		g_FullTimeGraphic = (checkedStatus & 0x10) ? TRUE : FALSE;	// If "1" then checkbox is checked.

		switch (g_State)
		{
		case STATE_SERIAL_NUMBER_PROMPT:
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);
			gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_9);	// "READY PRESS PLAY?"
			gx_widget_show (&ReadyScreen.ReadyScreen_PauseIcon_Button);
			g_State = STATE_READY_TO_GO;
			break;

		case STATE_READY_TO_GO:
			gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_11);	// "THERAPY IN PROCESS"
			gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
			gx_widget_hide (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
			g_State = STATE_THERAPY_IN_PROCESS;
			if (!g_TherapyInProcess)
			{
				g_TherapyInProcess = TRUE;
				g_TherapyTime = 300;
			}
			gx_widget_show (&ReadyScreen.ReadyScreen_Time_Prompt);
			gx_prompt_text_set (&ReadyScreen.ReadyScreen_Time_Prompt, g_TimeString);
			gx_widget_show (&ReadyScreen.ReadyScreen_GreenTick_Icon);
			gx_widget_show (&ReadyScreen.ReadyScreen_TherpayTime_RadialProgressBar);
			gx_radial_progress_bar_value_set(&ReadyScreen.ReadyScreen_TherpayTime_RadialProgressBar, g_TherapyTime);
			gx_system_timer_start(window, THERAPY_TIMER_ID, 2, 0);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_GREEN, GX_PIXELMAP_ID_STATUSRING_GREEN);
			EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE);
			break;

		case STATE_THERAPY_IN_PROCESS:
			g_State = STATE_THERAPY_PAUSED;
			gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
			gx_widget_show (&ReadyScreen.ReadyScreen_Time_Prompt);
			gx_widget_hide (&ReadyScreen.ReadyScreen_GreenTick_Icon);
			gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_29);	// "PAUSED"
			gx_system_timer_stop (window, THERAPY_TIMER_ID);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_OFF, GX_PIXELMAP_ID_STATUSRING_OFF);
			g_RingOn = FALSE;
			gx_system_timer_start(window, PAUSE_TIMER_ID, 2, 0);
			break;

		case STATE_THERAPY_PAUSED:
			gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_11);	// "THERAPY IN PROCESS"
			gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
			gx_widget_hide (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
			gx_widget_show (&ReadyScreen.ReadyScreen_Time_Prompt);
			gx_widget_show (&ReadyScreen.ReadyScreen_GreenTick_Icon);
			gx_widget_show (&ReadyScreen.ReadyScreen_TherpayTime_RadialProgressBar);
			gx_radial_progress_bar_value_set(&ReadyScreen.ReadyScreen_TherpayTime_RadialProgressBar, g_TherapyTime);
			gx_system_timer_start(window, THERAPY_TIMER_ID, 2, 0);
			gx_system_timer_stop (window, PAUSE_TIMER_ID);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_GREEN, GX_PIXELMAP_ID_STATUSRING_GREEN);
			g_State = STATE_THERAPY_IN_PROCESS;
			break;

		//case STATE_CABLE_FAULT:
		//	g_State = STATE_READY;
		//	EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE);
		//	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_GREEN, GX_PIXELMAP_ID_STATUSRING_GREEN);
		//	gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_10);	// "CONNECT MOUTHPIECE"
		//	break;

		} // end switch
		break;

	//----------------------------------------------------------------------------------------
	// MOUTHPIECE (Limit) switch
	//----------------------------------------------------------------------------------------
	case GX_SIGNAL (LIMIT_SWITCH_BTN_ID, GX_EVENT_CLICKED):
		gx_system_timer_stop (window, PAUSE_TIMER_ID);
		if (g_LimitSwitchClosed == FALSE)		// Switch is open, let's close it
		{
			g_LimitSwitchClosed = TRUE;
			// If we are in a "Fault during Therapy Session" state, let's resume the Therapy
			if (g_State == STATE_THERAPY_MOUTHPIECE_FAULT)
			{
				gx_icon_button_pixelmap_set (&ReadyScreen.base.PrimaryTemplate_LimitSwitchStatus_IconButton, GX_PIXELMAP_ID_RADIO_ON);
				gx_prompt_text_id_set (&ReadyScreen.base.PrimaryTemplate_LimitSwitchStatus_prompt, GX_STRING_ID_STRING_20);	// "INSERTED"
				//gx_widget_show (&ReadyScreen.ReadyScreen_PauseIcon_Button);
				gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
				// Change to "READING..."
				gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);
				gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_5);	// "READING..."
				EnableEEPROMPT_Button (GX_TRUE, GX_TRUE, GX_TRUE);
				g_State = STATE_THERAPY_CONTINUE;
			}
			else
			{
				gx_icon_button_pixelmap_set (&ReadyScreen.base.PrimaryTemplate_LimitSwitchStatus_IconButton, GX_PIXELMAP_ID_RADIO_ON);
				gx_prompt_text_id_set (&ReadyScreen.base.PrimaryTemplate_LimitSwitchStatus_prompt, GX_STRING_ID_STRING_20);	// "INSERTED"
				EnableEEPROMPT_Button (GX_TRUE, GX_TRUE, GX_TRUE);
				// Change to "READING..."
				gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);
				gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_5);	// "READING..."
				g_State = STATE_CABLE_INSERTED;
			}
		}
		else	// Switch is CLOSED, let's open it.
		{
			gx_widget_hide (&ReadyScreen.ReadyScreen_TherpayTime_RadialProgressBar);
			gx_system_timer_stop(window, THERAPY_COMPLETE_TIMER_ID);
			g_LimitSwitchClosed = FALSE;
			gx_icon_button_pixelmap_set (&ReadyScreen.base.PrimaryTemplate_LimitSwitchStatus_IconButton, GX_PIXELMAP_ID_RADIO_OFF);
			gx_prompt_text_id_set (&ReadyScreen.base.PrimaryTemplate_LimitSwitchStatus_prompt, GX_ID_NONE);	// none
			gx_widget_hide (&ReadyScreen.ReadyScreen_Time_Prompt);
			if ((g_State == STATE_THERAPY_IN_PROCESS)  || (g_State == STATE_THERAPY_RECOVER) || (g_State == STATE_CABLE_INSERTED))
			{
				gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_10);	// "INSERT MOUTHPIECE"
				gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_WHITE, GX_PIXELMAP_ID_STATUSRING_WHITE);
				gx_system_timer_stop(window, THERAPY_TIMER_ID);
				gx_widget_hide (&ReadyScreen.ReadyScreen_GreenTick_Icon);
				gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
				g_State = STATE_THERAPY_MOUTHPIECE_FAULT;
			}
			//else if (g_State == STATE_CABLE_FAULT)
			//{
			//	gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_13);		// "MOUTHPIECE DETACHED"
			//	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);
			//	gx_system_timer_stop(window, THERAPY_TIMER_ID);
			//	gx_widget_hide (&ReadyScreen.ReadyScreen_Time_Prompt);
			//	gx_widget_hide (&ReadyScreen.ReadyScreen_GreenTick_Icon);
			//	gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
			//	g_State = STATE_THERAPY_MOUTHPIECE_FAULT;
			//}
			else
			{
				gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_10);	// "INSERT MOUTHPIECE"
				gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
				gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_WHITE, GX_PIXELMAP_ID_STATUSRING_WHITE);
				g_State = STATE_READY;
			}
		}
		break;

	case GX_SIGNAL (EEPROM_OK_BTN_ID, GX_EVENT_CLICKED):
		if (g_State == STATE_CABLE_INSERTED)
		{
			gx_single_line_text_input_buffer_get (&ReadyScreen.base.PrimaryTemplate_SerialNumber_TextInput, &g_PromptString, &mySize, &myBufSize);
			g_MouthPiece_SerialNumber = atoi (g_PromptString);
			sprintf (g_SerialNumberString, "SN:\rMCA%06d\rPress          \rif correct", g_MouthPiece_SerialNumber);
			//gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_8); // "SN: xxxxxxxxx, OK? PRESS PLAY"
			gx_multi_line_text_button_text_set (&ReadyScreen.ReadyScreen_Information_Button, g_SerialNumberString);
			gx_widget_show (&ReadyScreen.ReadyScreen_PauseIcon_Button);
			g_State = STATE_SERIAL_NUMBER_PROMPT;
			EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE);
		}
		else if (g_State == STATE_THERAPY_CONTINUE)	// We are recovering from a Mouthpiece detached condition.
		{
			EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE);
			gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
			gx_widget_hide (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
			gx_widget_show (&ReadyScreen.ReadyScreen_Time_Prompt);
			gx_widget_show (&ReadyScreen.ReadyScreen_TherpayTime_RadialProgressBar);
			gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_29);	// "PAUSED"
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);
			gx_system_timer_start(window, PAUSE_TIMER_ID, 2, 0);
			g_State = STATE_THERAPY_PAUSED;
			break;
		}
		break;

	case GX_SIGNAL (EEPROM_EXPIRED_BTN_ID, GX_EVENT_CLICKED):
		if (g_State == STATE_CABLE_INSERTED)
		{
			gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_15); // "MOUTHPIECE EXPIRED"
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);
			g_State = STATE_MOUTHPIECE_EXPIRED;
			EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE);
		}
		else if (g_State == STATE_THERAPY_CONTINUE)	// We are recovering from a Mouthpiece detached condition.
		{
			EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE);
			gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_15); // "MOUTHPIECE EXPIRED"
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);
			g_State = STATE_MOUTHPIECE_EXPIRED;
			break;
		}
		break;

	case GX_SIGNAL (EEPROM_FAIL_BTN_ID, GX_EVENT_CLICKED):
		if (g_State == STATE_CABLE_INSERTED)
		{
			gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_27);	// "MOUTHPIECE FAULT"
			//gx_widget_hide (&ReadyScreen.ReadyScreen_Time_Prompt);
			//gx_widget_hide (&ReadyScreen.ReadyScreen_GreenTick_Icon);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);
			EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE);
			g_State = STATE_CABLE_FAULT;
		}
		else if (g_State == STATE_THERAPY_CONTINUE)	// We are recovering from a Mouthpiece detached condition.
		{
			gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_27);	// "MOUTHPIECE FAULT"
			//gx_widget_hide (&ReadyScreen.ReadyScreen_Time_Prompt);
			//gx_widget_hide (&ReadyScreen.ReadyScreen_GreenTick_Icon);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);
			EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE);
			g_State = STATE_THERAPY_RECOVER;
		}
		break;

    case GX_EVENT_TIMER:
        if (event_ptr->gx_event_payload.gx_event_timer_id == THERAPY_COMPLETE_TIMER_ID)
		{
			gx_system_timer_start(window, THERAPY_COMPLETE_TIMER_ID, 20, 0);	// Resume the timer
			if (g_RingOn)
			{
				gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_OFF, GX_PIXELMAP_ID_STATUSRING_OFF);
				g_RingOn = FALSE;
			}
			else
			{
				gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_GREEN, GX_PIXELMAP_ID_STATUSRING_GREEN);
				g_RingOn = TRUE;
			}
		}
		else if (event_ptr->gx_event_payload.gx_event_timer_id == THERAPY_TIMER_ID)	// Therapy is done.
		{
			--g_TherapyTime;
			if (g_TherapyTime <= 1)
			{
				gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_12); // "THERAPY COMPLETE"
				gx_widget_hide (&ReadyScreen.ReadyScreen_Time_Prompt);
				gx_widget_hide (&ReadyScreen.ReadyScreen_GreenTick_Icon);
				gx_widget_hide (&ReadyScreen.ReadyScreen_TherpayTime_RadialProgressBar);
				gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
				gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_GREEN, GX_PIXELMAP_ID_STATUSRING_GREEN);
				gx_system_timer_start(window, THERAPY_COMPLETE_TIMER_ID, 20, 0);	// Resume the timer
				g_RingOn = TRUE;
				g_TherapyInProcess = FALSE;
				g_State = STATE_THERAPY_IS_COMPLETE;
			}
			else
			{
				if (g_FullTimeGraphic)
				{
				// Display the time in "MIN:SEC"
					sprintf_s (g_TimeString, sizeof (g_TimeString), "%d:%02d", g_TherapyTime / 60, g_TherapyTime % 60);
					gx_prompt_text_set (&ReadyScreen.ReadyScreen_Time_Prompt, g_TimeString);
					// The following draws a starting with 0 going counterclockwise to a full ring
					//NOT BAD: gx_radial_progress_bar_value_set(&ReadyScreen.ReadyScreen_TherpayTime_RadialProgressBar, 360-(g_TherapyTime * 360/300));
					gx_radial_progress_bar_value_set(&ReadyScreen.ReadyScreen_TherpayTime_RadialProgressBar, (g_TherapyTime * 360/300));
				}
				else
				{
					// Display only the remaining minutes.
					sprintf_s (g_TimeString, sizeof (g_TimeString), "%d", g_TherapyTime / 60 + 1);
					gx_prompt_text_set (&ReadyScreen.ReadyScreen_Time_Prompt, g_TimeString);
					// The following draws a full green ring and "erases" it going clockwise.
					gx_radial_progress_bar_value_set(&ReadyScreen.ReadyScreen_TherpayTime_RadialProgressBar, (g_TherapyTime % 60) * 6);
				}
				gx_system_timer_start(window, THERAPY_TIMER_ID, 2, 0);	// Resume the timer
			}
		}
		else if (event_ptr->gx_event_payload.gx_event_timer_id == PAUSE_TIMER_ID)	// Pause is active.
		{
			gx_system_timer_start(window, PAUSE_TIMER_ID, 20, 0);	// Resume the timer
			if (g_RingOn)
			{
				gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_OFF, GX_PIXELMAP_ID_STATUSRING_OFF);
				g_RingOn = FALSE;
			}
			else
			{
				gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);
				g_RingOn = TRUE;
			}
		}
		break;

	case GX_SIGNAL (SYSTEM_ERROR_BTN_ID, GX_EVENT_CLICKED):
		if (g_State == STATE_SYSTEM_ERROR)
		{
	        screen_toggle((GX_WINDOW *)&Splash_Window, window);
		}
		else
		{
			gx_widget_hide (&ReadyScreen.ReadyScreen_Time_Prompt);
			gx_widget_hide (&ReadyScreen.ReadyScreen_GreenTick_Icon);
			gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
			gx_multi_line_text_button_text_id_set (&ReadyScreen.base.PrimaryTemplate_SystemError_Button, GX_STRING_ID_STRING_7);
			gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
			EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE);
			// Disable the MOUTHPIECE Button
			gx_widget_style_get ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_LimitSwitch_Button, &widgetStyle);
			widgetStyle &= (~GX_STYLE_ENABLED);
			gx_widget_style_set ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_LimitSwitch_Button, widgetStyle);

			// Update USER Information
			gx_multi_line_text_button_text_id_set (&ReadyScreen.ReadyScreen_Information_Button, GX_STRING_ID_STRING_14); // "CONTROL UNIT ERROR 01"
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);

			// Stop all timers
			gx_system_timer_stop(window, THERAPY_TIMER_ID);
			gx_system_timer_stop(window, THERAPY_COMPLETE_TIMER_ID);
			g_State = STATE_SYSTEM_ERROR;
		}
		break;

	}

    gx_window_event_process(window, event_ptr);

	return (GX_SUCCESS);
}


