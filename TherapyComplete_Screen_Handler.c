//*****************************************************************************
// Filename: TherapyComplete_Screen_Handler.c
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

//*************************************************************************************

VOID TherapyComplete_Screen_Draw_Function (GX_WINDOW *window)
{
	gx_window_draw(window);
}

//*************************************************************************************
// Function Name: TherapyComplete_Screen_Event_Function
//
// Description: This functions handles the Splash screen
//
//*************************************************************************************

UINT TherapyComplete_Screen_Event_Function (GX_WINDOW *window, GX_EVENT *event_ptr)
{
    gx_window_event_process(window, event_ptr);

	switch (event_ptr->gx_event_type)
	{
	//--------------------------------------------
	case GX_EVENT_SHOW:
		gx_widget_hide (&TherapyComplete_Screen.TherapyComplete_Screen_Instruction_TextView);

		// Disable ATTACH button, disable EEPROM buttons
		Enable_EEPROM_Buttons (&TherapyComplete_Screen.base, GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);

		// Make Ring Color
		gx_icon_pixelmap_set (&TherapyComplete_Screen.TherapyComplete_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_GREEN, GX_PIXELMAP_ID_STATUSRING_GREEN);
		g_RingOn = TRUE;

		// Show the box, make it red and populate it.
		gx_widget_show (&TherapyComplete_Screen.TherapyComplete_Screen_WhiteBox_Icon);
		gx_icon_pixelmap_set (&TherapyComplete_Screen.TherapyComplete_Screen_WhiteBox_Icon, GX_PIXELMAP_ID_WHITE_TEXT_BOX, GX_PIXELMAP_ID_WHITE_TEXT_BOX);
		DisplayInformation_InBox (&TherapyComplete_Screen.TherapyComplete_Screen_Information_TextView, "Therapy\rComplete", 2, GX_COLOR_ID_WHITE);

		gx_system_timer_start(window, THERAPY_COMPLETE_TIMER_ID, 20, 0);	// Start the timer to blink the ring
		if (g_LimitSwitchClosed == TRUE)
			gx_text_button_text_id_set (&TherapyComplete_Screen.base.PrimaryTemplate_LimitSwitch_Button, GX_STRING_ID_DETACH);
		else
			gx_text_button_text_id_set (&TherapyComplete_Screen.base.PrimaryTemplate_LimitSwitch_Button, GX_STRING_ID_ATTACH);
		break;

	//--------------------------------------------
    case GX_EVENT_TIMER:
        if (event_ptr->gx_event_payload.gx_event_timer_id == THERAPY_COMPLETE_TIMER_ID)
		{
			gx_system_timer_start(window, THERAPY_COMPLETE_TIMER_ID, 20, 0);	// Resume the timer
			if (g_RingOn)
			{
				gx_icon_pixelmap_set (&TherapyComplete_Screen.TherapyComplete_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_OFF, GX_PIXELMAP_ID_STATUSRING_OFF);
				g_RingOn = FALSE;
			}
			else
			{
				gx_icon_pixelmap_set (&TherapyComplete_Screen.TherapyComplete_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_GREEN, GX_PIXELMAP_ID_STATUSRING_GREEN);
				g_RingOn = TRUE;
			}
		}
		break;

	//--------------------------------------------
	case GX_SIGNAL (LIMIT_SWITCH_BTN_ID, GX_EVENT_CLICKED):
		g_LimitSwitchClosed = FALSE;
		g_TherapyInProcess = THERAPY_IDLE;
        screen_toggle((GX_WINDOW *)&InsertMouthpiece_Screen, window);
		break;

	//--------------------------------------------
	case GX_SIGNAL (IDLE_TIME_BUTTON_ID, GX_EVENT_CLICKED):
		Set_Standby_Exit_Screen ((GX_WINDOW*) &InsertMouthpiece_Screen);
        screen_toggle((GX_WINDOW *)&Standby_Screen, window);
		break;

	//--------------------------------------------
	case GX_SIGNAL (SYSTEM_ERROR_BTN_ID, GX_EVENT_CLICKED):
        screen_toggle((GX_WINDOW *)&Error_Screen, window);
		break;

	} // end switch

	return (GX_SUCCESS);
}


