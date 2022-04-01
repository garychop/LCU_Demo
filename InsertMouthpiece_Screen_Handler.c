//*****************************************************************************
// Filename: InsertMouthpiece_Screen_Handler.c
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

//*************************************************************************************

VOID InsertMouthpiece_Screen_Draw_Function (GX_WINDOW *window)
{
	gx_window_draw(window);
}

//*************************************************************************************
// Function Name: InsertMouthpiece_Screen_Event_Function
//
// Description: This functions handles the Splash screen
//
//*************************************************************************************

UINT InsertMouthpiece_Screen_Event_Function (GX_WINDOW *window, GX_EVENT *event_ptr)
{
	int slot;

    gx_window_event_process(window, event_ptr);

	switch (event_ptr->gx_event_type)
	{
	case GX_EVENT_SHOW:
		//g_LimitSwitchClosed = FALSE;

		gx_multi_line_text_button_text_id_set (&InsertMouthpiece_Screen.base.PrimaryTemplate_SystemError_Button, GX_STRING_ID_STRING_24);	// "Cause System Error"

		// Enable ATTACH button, disable EEPROMP buttons
		Enable_EEPROM_Buttons (&InsertMouthpiece_Screen.base, GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);

		// Make Ring WHITE
		gx_icon_pixelmap_set (&InsertMouthpiece_Screen.InsertMouthpiece_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_WHITE, GX_PIXELMAP_ID_STATUSRING_WHITE);

		// Show the box, make it white and populate it.
		gx_widget_show (&InsertMouthpiece_Screen.InsertMouthpiece_Screen_WhiteBox_Icon);
		gx_icon_pixelmap_set (&InsertMouthpiece_Screen.InsertMouthpiece_Screen_WhiteBox_Icon, GX_PIXELMAP_ID_WHITE_TEXT_BOX, GX_PIXELMAP_ID_WHITE_TEXT_BOX);
		DisplayInstruction_InBox (&InsertMouthpiece_Screen.InsertMouthpiece_Screen_Instruction_TextView, "Insert\rMouthpiece", 2, GX_COLOR_ID_WHITE);

		if (g_LimitSwitchClosed == TRUE)
		{
			gx_text_button_text_id_set (&InsertMouthpiece_Screen.base.PrimaryTemplate_LimitSwitch_Button, GX_STRING_ID_DETACH);
	        screen_toggle((GX_WINDOW *)&Reading_Screen, window);
		}
		else
		{
			gx_text_button_text_id_set (&InsertMouthpiece_Screen.base.PrimaryTemplate_LimitSwitch_Button, GX_STRING_ID_ATTACH);
		}
		break;

	//----------------------------------------------------------------------------------------
	// MOUTHPIECE (Limit) switch
	//----------------------------------------------------------------------------------------
	case GX_SIGNAL (LIMIT_SWITCH_BTN_ID, GX_EVENT_CLICKED):
		// Switch is open, let's close it
		if (g_LimitSwitchClosed == FALSE)
		{
			g_LimitSwitchClosed = TRUE;
	        screen_toggle((GX_WINDOW *)&Reading_Screen, window);
		}
		else
		{
			for (slot = 0; slot < MOUTHPIECE_DB_SIZE; ++slot)
				g_Mouthpiece_DB[slot].m_Attached = FALSE;
			g_LimitSwitchClosed = FALSE;
		}
		break;

	//--------------------------------------------
	case GX_SIGNAL (IDLE_TIME_BUTTON_ID, GX_EVENT_CLICKED):
		Set_Standby_Exit_Screen (window);
        screen_toggle((GX_WINDOW *)&Standby_Screen, window);
		break;

	//--------------------------------------------
	case GX_SIGNAL (SYSTEM_ERROR_BTN_ID, GX_EVENT_CLICKED):
        screen_toggle((GX_WINDOW *)&Error_Screen, window);
		break;

	} // end switch

	return (GX_SUCCESS);
}


