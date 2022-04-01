//*****************************************************************************
// Filename: Standby_Screen_Handler.c
//
// Date: Mar 31, 2022
//
// Author: G. Chopcinski, Kg Solutions, LLC
// 
//*****************************************************************************

#include "LCU_Demo.h"

#include "math.h"

//*************************************************************************************
// Local Macros
//*************************************************************************************

//*************************************************************************************
// External References
//*************************************************************************************

//*************************************************************************************
// Local/Global variables
//*************************************************************************************

extern MOUTHPIECE_DATABASE_STRUCT g_Mouthpiece_DB[MOUTHPIECE_DB_SIZE];
extern VOID Enable_Limit_Switch (GX_WIDGET* limitSwitchButton, GX_BOOL enable);

GX_WINDOW *g_ReturnWindow = NULL;

//*************************************************************************************

VOID Set_Standby_Exit_Screen (GX_WINDOW* window)
{
	g_ReturnWindow = window;
}

//*************************************************************************************

VOID Standby_Screen_Draw_Function (GX_WINDOW *window)
{
	gx_window_draw(window);
}

//*************************************************************************************
// Function Name: Standby_Screen_Event_Function
//
// Description: This functions handles the Splash screen
//
//*************************************************************************************

UINT Standby_Screen_Event_Function (GX_WINDOW *window, GX_EVENT *event_ptr)
{
	GX_RECTANGLE rect;
	int slot;

    gx_window_event_process(window, event_ptr);

	switch (event_ptr->gx_event_type)
	{
	//--------------------------------------------
	case GX_EVENT_SHOW:
		//gx_multi_line_text_button_text_id_set (&Standby_Screen.base.PrimaryTemplate_SystemError_Button, GX_STRING_ID_STRING_7);	// "Press to Restart"

		// Disable ATTACH button, disable EEPROM buttons
		Enable_EEPROM_Buttons (&Standby_Screen.base, GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);

		if (g_LimitSwitchClosed == TRUE)
			gx_text_button_text_id_set (&Standby_Screen.base.PrimaryTemplate_LimitSwitch_Button, GX_STRING_ID_DETACH);
		else
			gx_text_button_text_id_set (&Standby_Screen.base.PrimaryTemplate_LimitSwitch_Button, GX_STRING_ID_ATTACH);

		// Show Mureva Icon 
		Standby_Screen.Standby_Screen_Mureva_Icon.gx_widget_size.gx_rectangle_top = 134;
		Standby_Screen.Standby_Screen_Mureva_Icon.gx_widget_size.gx_rectangle_bottom = Standby_Screen.Standby_Screen_Mureva_Icon.gx_widget_size.gx_rectangle_top + 53;
		Standby_Screen.Standby_Screen_Mureva_Icon.gx_widget_size.gx_rectangle_left = 142;
		Standby_Screen.Standby_Screen_Mureva_Icon.gx_widget_size.gx_rectangle_top = Standby_Screen.Standby_Screen_Mureva_Icon.gx_widget_size.gx_rectangle_left + 40;

		gx_widget_show (&Standby_Screen.Standby_Screen_Mureva_Icon);

		gx_system_timer_start(window, STANDBY_TIMER, 2, 0);

		break;

	//----------------------------------------------------------------------------------------
	// MOUTHPIECE (Limit) switch
	//----------------------------------------------------------------------------------------
	case GX_SIGNAL (LIMIT_SWITCH_BTN_ID, GX_EVENT_CLICKED):
		if (g_LimitSwitchClosed == FALSE)
		{
			g_LimitSwitchClosed = TRUE;
		}
		else
		{
			g_LimitSwitchClosed = FALSE;
			for (slot = 0; slot < MOUTHPIECE_DB_SIZE; ++slot)
				g_Mouthpiece_DB[slot].m_Attached = FALSE;
		}
		gx_system_timer_stop (window, STANDBY_TIMER);	// Stop the standby timer
		if (g_ReturnWindow)
		{
			screen_toggle(g_ReturnWindow, window);
			g_ReturnWindow = NULL;
		}
		else
			screen_toggle((GX_WINDOW *)&Reading_Screen, window);
		break;

	//--------------------------------------------
    case GX_EVENT_TIMER:
		if (event_ptr->gx_event_payload.gx_event_timer_id == STANDBY_TIMER)
		{
			// We're in standby, float the Standby Icon around the screen.
			rect = Standby_Screen.Standby_Screen_Mureva_Icon.gx_widget_size;
			rect.gx_rectangle_top += 5;
			if (rect.gx_rectangle_top > 148)
				rect.gx_rectangle_top = 70;
			rect.gx_rectangle_bottom = rect.gx_rectangle_top + 80;
			rect.gx_rectangle_left += 5;
			if (rect.gx_rectangle_left > 180)
				rect.gx_rectangle_left = 74;
			rect.gx_rectangle_right = rect.gx_rectangle_left + 60;
			gx_widget_resize (&Standby_Screen.Standby_Screen_Mureva_Icon, &rect);
			gx_system_timer_start(window, STANDBY_TIMER, 20, 0);
		}
		break;

	//--------------------------------------------
	case GX_SIGNAL (SYSTEM_ERROR_BTN_ID, GX_EVENT_CLICKED):
		gx_system_timer_stop (window, STANDBY_TIMER);	// Stop the standby timer
        screen_toggle((GX_WINDOW *)&Splash_Window, window);
		break;

	} // end switch

	return (GX_SUCCESS);
}


