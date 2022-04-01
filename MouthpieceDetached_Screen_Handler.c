//*****************************************************************************
// Filename: MouthpieceDetached_Screen_Handler.c
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

VOID MouthpieceDetached_Screen_Draw_Function (GX_WINDOW *window)
{
	gx_window_draw(window);
}

//*************************************************************************************
// Function Name: Reading_Screen_Event_Function
//
// Description: This functions handles the Splash screen
//
//*************************************************************************************

UINT MouthpieceDetached_Screen_Event_Function (GX_WINDOW *window, GX_EVENT *event_ptr)
{
	int slot;

    gx_window_event_process(window, event_ptr);

	switch (event_ptr->gx_event_type)
	{
	//--------------------------------------------
	case GX_EVENT_SHOW:

		// Enable ATTACH button, disable EEPROM buttons
		Enable_EEPROM_Buttons (&MouthpieceDetached_Screen.base, GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);

		// Make Ring Color
		gx_icon_pixelmap_set (&MouthpieceDetached_Screen.MouthpieceDetached_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);

		// Show the box, make it red and populate it.
		gx_widget_show (&MouthpieceDetached_Screen.MouthpieceDetached_Screen_WhiteBox_Icon);
		gx_icon_pixelmap_set (&MouthpieceDetached_Screen.MouthpieceDetached_Screen_WhiteBox_Icon, GX_PIXELMAP_ID_RED_TEXT_BOX, GX_PIXELMAP_ID_RED_TEXT_BOX);
		DisplayInformation_InBox (&MouthpieceDetached_Screen.MouthpieceDetached_Screen_Information_TextView, "MOUTHPIECE\rDETACHED\r\r", 4, GX_COLOR_ID_RED);
		DisplayInstruction_InBox (&MouthpieceDetached_Screen.MouthpieceDetached_Screen_Instruction_TextView, "\r\rReinsert\rMouthpiece", 4, GX_COLOR_ID_RED);
		break;

	//--------------------------------------------
	case GX_SIGNAL (LIMIT_SWITCH_BTN_ID, GX_EVENT_CLICKED):
		if (g_LimitSwitchClosed == FALSE)
		{
			g_LimitSwitchClosed = TRUE;
	        screen_toggle((GX_WINDOW *)&Reading_Screen, window);
		}
		else
		{
			g_LimitSwitchClosed = FALSE;
			for (slot = 0; slot < MOUTHPIECE_DB_SIZE; ++slot)
				g_Mouthpiece_DB[slot].m_Attached = FALSE;
		}
		break;

	//--------------------------------------------
	case GX_SIGNAL (IDLE_TIME_BUTTON_ID, GX_EVENT_CLICKED):
		Set_Standby_Exit_Screen ((GX_WINDOW*) &Splash_Window);
        screen_toggle((GX_WINDOW *)&Standby_Screen, window);
		break;

	//--------------------------------------------
	case GX_SIGNAL (SYSTEM_ERROR_BTN_ID, GX_EVENT_CLICKED):
        screen_toggle((GX_WINDOW *)&Error_Screen, window);
		break;

	} // end switch

	return (GX_SUCCESS);
}


