//*****************************************************************************
// Filename: TherapyScreen_Handler.c
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

#define TICK_CENTER_PT_HORIZONTAL 160	// From Left of screen
#define TICK_CENTER_PT_VERTICAL 158		// From Top of screen
#define WHITE_TICK_LENGTH 30			// This is the length of the white tick marks.
#define GREEN_TICK_LENGTH 20			// This is the length of the green tick marks.
#define TICK_LENGTH 100					// This is the total length of the tick from the
										// .. center of the screen.
#define DEGREES_TO_RADIANS (0.01745329252f)

//*************************************************************************************
// External References
//*************************************************************************************

//*************************************************************************************
// Local/Global variables
//*************************************************************************************

char g_TimeString[16];
char g_TimeTickString[8];
GX_BOOL g_ShowTicks = FALSE;

//*************************************************************************************

VOID Therapy_Screen_Draw_Function (GX_WINDOW *window)
{
	UINT status;
	INT secondsRemaining;
	GX_BRUSH *brush, *originalBrush;
	GX_VALUE verticalStart, verticalEnd, horizStart, horizEnd;
	float angle, radians, vertLength, horizLength, tempTime;
	GX_RECTANGLE rectangle;
	GX_COLOR customColor;

	gx_window_draw(window);

	if (g_ShowTicks)
	{
		status = gx_context_brush_get(&brush);
		originalBrush = brush;

		brush->gx_brush_width = 4;

		// Translate seconds into angle.
		// 60 seconds = 90 degrees,
		// 45 seconds = 180 degrees,
		// 30 seconds = 270 degrees,
		// 15 seconds = 360 degrees.
		tempTime = (float) (g_TherapyTime % 60);
		if (tempTime == 0.0f)
			tempTime = 60.0f;
		angle = ((60.0f - tempTime) * 6.0f) + 90.0f;
		radians = angle * DEGREES_TO_RADIANS;
		vertLength = (float) sin (radians);
		horizLength = (float) cos (radians);
		//vertLength = (float) sin (0.0f * DEGREES_TO_RADIANS);
		//vertLength = (float) sin (270.0f * DEGREES_TO_RADIANS);
		//vertLength = (float) sin (180.0f * DEGREES_TO_RADIANS);
	
		// Show the current time
		gx_widget_show (&Therapy_Screen.Therapy_Screen_TimeTick_Prompt);
		horizStart = (GX_VALUE) ((TICK_CENTER_PT_HORIZONTAL - 8) - ((TICK_LENGTH - WHITE_TICK_LENGTH-12) * horizLength)); // "8" is half of width
		//horizEnd = TICK_CENTER_PT_HORIZONTAL - ((TICK_LENGTH - WHITE_TICK_LENGTH) * horizLength);
		verticalStart = (GX_VALUE) ((TICK_CENTER_PT_VERTICAL - 7) - ((TICK_LENGTH - WHITE_TICK_LENGTH-10) * vertLength));
		//verticalEnd = TICK_CENTER_PT_VERTICAL - ((TICK_LENGTH - WHITE_TICK_LENGTH) * vertLength);
		rectangle.gx_rectangle_top = verticalStart;
		rectangle.gx_rectangle_bottom = verticalStart + 14;
		rectangle.gx_rectangle_left = horizStart;
		rectangle.gx_rectangle_right = horizStart + 16;
		gx_widget_resize (&Therapy_Screen.Therapy_Screen_TimeTick_Prompt, &rectangle);
		sprintf_s (g_TimeTickString, sizeof(g_TimeTickString), "%d", g_TherapyTime % 60);
		gx_prompt_text_set (&Therapy_Screen.Therapy_Screen_TimeTick_Prompt, g_TimeTickString);

		// Show White current tick mark
		brush->gx_brush_line_color = GX_COLOR_WHITE;
		horizStart = (GX_VALUE) (TICK_CENTER_PT_HORIZONTAL - (TICK_LENGTH * horizLength));
		horizEnd = (GX_VALUE) (TICK_CENTER_PT_HORIZONTAL - ((TICK_LENGTH - WHITE_TICK_LENGTH) * horizLength));
		verticalStart = (GX_VALUE) (TICK_CENTER_PT_VERTICAL - (TICK_LENGTH * vertLength));
		verticalEnd = (GX_VALUE) (TICK_CENTER_PT_VERTICAL - ((TICK_LENGTH - WHITE_TICK_LENGTH) * vertLength));
		gx_canvas_line_draw (horizStart, verticalStart, horizEnd, verticalEnd);

		// Now draw the green shorter ticks from the current seconds to 0.
		brush->gx_brush_line_color = 0xff787c78; // LCU_MainDisplay_theme_1_color_table[GX_COLOR_ID_GREEN_BRIGHT];
		gx_context_color_get (GX_COLOR_ID_GREEN_SUBTLE, &customColor);
		brush->gx_brush_line_color = customColor;
		secondsRemaining = (INT) tempTime - 1;
		while (secondsRemaining > 0)
		{
			angle = (60.0f - (float) secondsRemaining) * 6.0f + 90.0f;
			radians = angle * DEGREES_TO_RADIANS;
			vertLength = (float) sin (radians);
			horizLength = (float) cos (radians);
			// Show green tick marks
			horizStart = (GX_VALUE) (TICK_CENTER_PT_HORIZONTAL - (TICK_LENGTH * horizLength));
			horizEnd = (GX_VALUE) (TICK_CENTER_PT_HORIZONTAL - ((TICK_LENGTH - GREEN_TICK_LENGTH) * horizLength));
			verticalStart = (GX_VALUE) (TICK_CENTER_PT_VERTICAL - (TICK_LENGTH * vertLength));
			verticalEnd = (GX_VALUE) (TICK_CENTER_PT_VERTICAL - ((TICK_LENGTH - GREEN_TICK_LENGTH) * (vertLength)));
			gx_canvas_line_draw (horizStart, verticalStart, horizEnd, verticalEnd);
			--secondsRemaining;
		}
	}
	else
	{
		gx_widget_hide (&Therapy_Screen.Therapy_Screen_TimeTick_Prompt);
	}
}

//*************************************************************************************
// This function displays the Therapy information and hides everything else
#if 0
void EnterTherapyInProgress_State (GX_WINDOW *window, STATES_ENUM nextState)
{
	gx_system_timer_stop (window, PAUSE_TIMER_ID);	// we may be coming for the PAUSED state, so let's ensure the PAUSED timer is off.

	gx_widget_hide (&ReadyScreen.ReadyScreen_Information_TextView);
	gx_widget_hide (&ReadyScreen.ReadyScreen_Information_Button);
	gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
	gx_widget_hide (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
	gx_widget_hide (&ReadyScreen.ReadyScreen_Instruction_TextView);

	if (!g_TherapyInProcess)
	{
		g_TherapyInProcess = TRUE;
		if (g_TherapyTime < 10)		// This allows resume from a "Power Cycle".
			g_TherapyTime = 300;
	}
	gx_widget_show (&ReadyScreen.ReadyScreen_Time_Prompt);
	gx_widget_show (&ReadyScreen.ReadyScreen_Minute_Prompt);
	gx_prompt_text_set (&ReadyScreen.ReadyScreen_Time_Prompt, g_TimeString);
	//gx_widget_show (&ReadyScreen.ReadyScreen_GreenTick_Icon);
	g_ShowTicks = TRUE;
	gx_system_timer_start(window, THERAPY_TIMER_ID, 2, 0);
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_GREEN, GX_PIXELMAP_ID_STATUSRING_GREEN);
	EnableEEPROMPT_Button (GX_FALSE, GX_TRUE, GX_FALSE, GX_FALSE, GX_TRUE);
	g_State = nextState;
}
#endif

//*************************************************************************************
// This function displays the PAUSED information and hides everything else
#if 0
void EnterPaused_State (GX_WINDOW *window, STATES_ENUM nextState)
{
	gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
	gx_widget_hide (&ReadyScreen.ReadyScreen_Instruction_TextView);
	gx_widget_hide (&ReadyScreen.ReadyScreen_GreenTick_Icon);
	gx_widget_hide (&ReadyScreen.ReadyScreen_WhiteBox_Icon);

	gx_widget_show (&ReadyScreen.ReadyScreen_Information_TextView);
	//gx_widget_show (&ReadyScreen.ReadyScreen_Information_Button);
	gx_widget_show (&ReadyScreen.ReadyScreen_Time_Prompt);
	gx_widget_show (&ReadyScreen.ReadyScreen_Minute_Prompt);

	sprintf_s (g_TimeString, sizeof (g_TimeString), "%d", g_TherapyTime / 60);
	gx_prompt_text_set (&ReadyScreen.ReadyScreen_Time_Prompt, g_TimeString);
	DisplayInformation (window, "PAUSED", 4, GX_COLOR_ID_WHITE);
	gx_system_timer_stop (window, THERAPY_TIMER_ID);
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_OFF, GX_PIXELMAP_ID_STATUSRING_OFF);
	g_RingOn = FALSE;
	g_ShowTicks = TRUE;
	EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);
	gx_system_timer_start(window, PAUSE_TIMER_ID, 2, 0);
	g_State = nextState;
}
#endif
//*************************************************************************************
// Function Name: ReadyScreen_Event_Function
//
// Description: This functions handles the Splash screen
//
//*************************************************************************************

UINT Therapy_Screen_Event_Function (GX_WINDOW *window, GX_EVENT *event_ptr)
{

    gx_window_event_process(window, event_ptr);

	switch (event_ptr->gx_event_type)
	{
	//----------------------------------------------------------------------------------------
	case GX_EVENT_SHOW:
		if (g_TherapyInProcess == THERAPY_PAUSED)
		{
			// Display "PAUSED"
			gx_widget_show (&Therapy_Screen.Therapy_Screen_Information_TextView);
			DisplayInformation_InBox (&Therapy_Screen.Therapy_Screen_Information_TextView, "PAUSED", 4, GX_COLOR_ID_WHITE);

			gx_system_timer_start(window, PAUSE_TIMER_ID, 20, 0);	// Start the timer to blink the ring
			gx_icon_pixelmap_set (&Therapy_Screen.Therapy_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);
			g_RingOn = TRUE;
			Enable_EEPROM_Buttons (&Therapy_Screen.base, GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);
		}
		else
		{
			gx_system_timer_start(window, THERAPY_TIMER_ID, 2, 0);		// Start the therapy timer 

			g_ShowTicks = TRUE;
			//g_TherapyTime = 300;
			g_TherapyInProcess = THERAPY_IN_PROGRESS;

			gx_icon_pixelmap_set (&Therapy_Screen.Therapy_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_GREEN, GX_PIXELMAP_ID_STATUSRING_GREEN);

			gx_widget_hide (&Therapy_Screen.Therapy_Screen_WhiteBox_Icon);
			gx_widget_hide (&Therapy_Screen.Therapy_Screen_Information_TextView);
			gx_widget_hide (&Therapy_Screen.Therapy_Screen_Instruction_TextView);

			Enable_EEPROM_Buttons (&Therapy_Screen.base, GX_FALSE, GX_TRUE, GX_FALSE, GX_FALSE, GX_TRUE);
		}
		if (g_LimitSwitchClosed == TRUE)
			gx_text_button_text_id_set (&Therapy_Screen.base.PrimaryTemplate_LimitSwitch_Button, GX_STRING_ID_DETACH);
		else
			gx_text_button_text_id_set (&Therapy_Screen.base.PrimaryTemplate_LimitSwitch_Button, GX_STRING_ID_ATTACH);
		break;

	//----------------------------------------------------------------------------------------
	case GX_SIGNAL (PLAY_BTN_ID, GX_EVENT_CLICKED):
		if (g_TherapyInProcess == THERAPY_IN_PROGRESS)		// We are going to PAUSED
		{
			// The operator wants to pause the process.
			gx_system_timer_stop (window, THERAPY_TIMER_ID);	// Stop the Therapy timer
			// Display "PAUSED"
			gx_widget_show (&Therapy_Screen.Therapy_Screen_Information_TextView);
			DisplayInformation_InBox (&Therapy_Screen.Therapy_Screen_Information_TextView, "PAUSED", 4, GX_COLOR_ID_WHITE);

			gx_system_timer_start(window, PAUSE_TIMER_ID, 20, 0);	// Start the timer to blink the ring
			gx_icon_pixelmap_set (&Therapy_Screen.Therapy_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);
			g_RingOn = TRUE;

			Enable_EEPROM_Buttons (&Therapy_Screen.base, GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);
			g_TherapyInProcess = THERAPY_PAUSED;
		}
		else // We are resuming the Therapy
		{
			gx_widget_hide (&Therapy_Screen.Therapy_Screen_Information_TextView);
			gx_system_timer_start (window, THERAPY_TIMER_ID, 2, 0);		// start the Therapy timer
			gx_system_timer_stop (window, PAUSE_TIMER_ID);	// Stop the Paused timer 
			gx_icon_pixelmap_set (&Therapy_Screen.Therapy_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_GREEN, GX_PIXELMAP_ID_STATUSRING_GREEN);
			Enable_EEPROM_Buttons (&Therapy_Screen.base, GX_FALSE, GX_TRUE, GX_FALSE, GX_FALSE, GX_TRUE);
			g_TherapyInProcess = THERAPY_IN_PROGRESS;
		}


		break;

	//----------------------------------------------------------------------------------------
	case GX_SIGNAL (LIMIT_SWITCH_BTN_ID, GX_EVENT_CLICKED):
		if (g_LimitSwitchClosed == TRUE)	// It better be true... but justincase.
		{
			g_LimitSwitchClosed = FALSE;
			gx_system_timer_stop (window, THERAPY_TIMER_ID);
			g_TherapyInProcess = THERAPY_PAUSED;
			screen_toggle((GX_WINDOW *)&MouthpieceDetached_Screen, window);
		}
		else
		{
			g_LimitSwitchClosed = TRUE;
		}
		break;

	//--------------------------------------------
	case GX_SIGNAL (EEPROM_FAIL_BTN_ID, GX_EVENT_CLICKED):
		gx_system_timer_stop (window, THERAPY_TIMER_ID);
		g_TherapyInProcess = THERAPY_PAUSED;
        screen_toggle((GX_WINDOW *)&ReadingError_Screen, window);
		break;

	//--------------------------------------------
    case GX_EVENT_TIMER:
		if (event_ptr->gx_event_payload.gx_event_timer_id == THERAPY_TIMER_ID)	// Therapy is done.
		{
			--g_TherapyTime;
			if (g_TherapyTime <= 1)
			{
		        screen_toggle((GX_WINDOW *)&TherapyComplete_Screen, window);
			}
			else
			{
				// Display only the remaining minutes.
				sprintf_s (g_TimeString, sizeof (g_TimeString), "%d", g_TherapyTime / 60);
				gx_prompt_text_set (&Therapy_Screen.Therapy_Screen_Time_Prompt, g_TimeString);
				g_ShowTicks = TRUE;
			    gx_system_dirty_mark(&Therapy_Screen);      // This forces the gauge to be updated and redrawn
				gx_system_timer_start(window, THERAPY_TIMER_ID, 2, 0);	// Resume the timer
			}
		}
        else if (event_ptr->gx_event_payload.gx_event_timer_id == PAUSE_TIMER_ID)
		{
			gx_system_timer_start(window, PAUSE_TIMER_ID, 20, 0);	// Resume the timer
			if (g_RingOn)
			{
				gx_icon_pixelmap_set (&Therapy_Screen.Therapy_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_OFF, GX_PIXELMAP_ID_STATUSRING_OFF);
				g_RingOn = FALSE;
			}
			else
			{
				gx_icon_pixelmap_set (&Therapy_Screen.Therapy_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);
				g_RingOn = TRUE;
			}
		}
		break;

	//--------------------------------------------
	case GX_SIGNAL (IDLE_TIME_BUTTON_ID, GX_EVENT_CLICKED):
		// Stop all timers
		gx_system_timer_stop (window, THERAPY_TIMER_ID);
		Set_Standby_Exit_Screen ((GX_WINDOW *)&InsertMouthpiece_Screen);
        screen_toggle((GX_WINDOW *)&Standby_Screen, window);
		break;

	//--------------------------------------------
	case GX_SIGNAL (SYSTEM_ERROR_BTN_ID, GX_EVENT_CLICKED):
		// Stop all timers
		gx_system_timer_stop (window, THERAPY_TIMER_ID);
        screen_toggle((GX_WINDOW *)&Error_Screen, window);
		break;

	}

	return (GX_SUCCESS);
}


