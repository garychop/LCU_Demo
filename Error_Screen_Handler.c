//*****************************************************************************
// Filename: Error_Screen_Handler.c
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

VOID Error_Screen_Draw_Function (GX_WINDOW *window)
{
	gx_window_draw(window);
}

//*************************************************************************************
// Function Name: Error_Screen_Event_Function
//
// Description: This functions handles the Splash screen
//
//*************************************************************************************

UINT Error_Screen_Event_Function (GX_WINDOW *window, GX_EVENT *event_ptr)
{
    gx_window_event_process(window, event_ptr);

	switch (event_ptr->gx_event_type)
	{
	//--------------------------------------------
	case GX_EVENT_SHOW:
		g_LimitSwitchClosed = FALSE;

		gx_multi_line_text_button_text_id_set (&Error_Screen.base.PrimaryTemplate_SystemError_Button, GX_STRING_ID_STRING_7);	// "Press to Restart"

		gx_widget_hide (&Error_Screen.Error_Screen_Instruction_TextView);
		// Disable ATTACH button, disable EEPROM buttons
		Enable_EEPROM_Buttons (&Error_Screen.base, GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE);

		// Make Ring WHITE
		gx_icon_pixelmap_set (&Error_Screen.Error_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);

		// Show the box, make it red and populate it.
		gx_widget_show (&Error_Screen.Error_Screen_WhiteBox_Icon);
		gx_icon_pixelmap_set (&Error_Screen.Error_Screen_WhiteBox_Icon, GX_PIXELMAP_ID_RED_TEXT_BOX, GX_PIXELMAP_ID_RED_TEXT_BOX);
		DisplayInformation_InBox (&Error_Screen.Error_Screen_Information_TextView, "SYSTEM\rERROR 01", 2, GX_COLOR_ID_RED);
		break;

	//--------------------------------------------
	case GX_SIGNAL (SYSTEM_ERROR_BTN_ID, GX_EVENT_CLICKED):
        screen_toggle((GX_WINDOW *)&Splash_Window, window);
		break;

	} // end switch

	return (GX_SUCCESS);
}


