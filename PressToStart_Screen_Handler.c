//*****************************************************************************
// Filename: PressToStart_Screen_Handler.c
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

VOID PressToStart_Screen_Draw_Function (GX_WINDOW *window)
{
	gx_window_draw(window);
}

//*************************************************************************************
// Function Name: PressToStart_Screen_Event_Function
//
// Description: This functions handles the Splash screen
//
//*************************************************************************************

UINT PressToStart_Screen_Event_Function (GX_WINDOW *window, GX_EVENT *event_ptr)
{
	GX_RECTANGLE rect;
	int slot;

    gx_window_event_process(window, event_ptr);

	switch (event_ptr->gx_event_type)
	{
	//--------------------------------------------
	case GX_EVENT_SHOW:

		// Make Ring Color
		gx_icon_pixelmap_set (&PressToStart_Screen.PressToStart_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);

		// Show the box, color it and populate it.
		gx_widget_show (&PressToStart_Screen.PressToStart_Screen_WhiteBox_Icon);
		gx_icon_pixelmap_set (&PressToStart_Screen.PressToStart_Screen_WhiteBox_Icon, GX_PIXELMAP_ID_WHITE_TEXT_BOX, GX_PIXELMAP_ID_WHITE_TEXT_BOX);

		// Display Press in the Instruction box
		DisplayInstruction_InBox (&PressToStart_Screen.PressToStart_Screen_Instruction_TextView, "Press     \nto Start\nTherapy", 3, GX_COLOR_ID_WHITE);
		// Display Play/pause icon
		rect.gx_rectangle_top = 128 + 2;	// "10" is used for 2 lines, "2" is for 3 lines
		rect.gx_rectangle_bottom = rect.gx_rectangle_top + 18;
		rect.gx_rectangle_left = 176; // + 12;
		rect.gx_rectangle_right = rect.gx_rectangle_left + 26;
		gx_widget_resize (&PressToStart_Screen.PressToStart_Screen_PauseIcon_Button, &rect);
		gx_widget_show (&PressToStart_Screen.PressToStart_Screen_PauseIcon_Button);

		gx_text_button_text_id_set (&PressToStart_Screen.base.PrimaryTemplate_LimitSwitch_Button, GX_STRING_ID_DETACH);
		Enable_EEPROM_Buttons (&PressToStart_Screen.base, GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);
		break;

	//--------------------------------------------
	case GX_SIGNAL (PLAY_BTN_ID, GX_EVENT_CLICKED):
        screen_toggle((GX_WINDOW *)&Therapy_Screen, window);
		break;

	//--------------------------------------------
	case GX_SIGNAL (LIMIT_SWITCH_BTN_ID, GX_EVENT_CLICKED):
		g_LimitSwitchClosed = FALSE;
		for (slot = 0; slot < MOUTHPIECE_DB_SIZE; ++slot)
			g_Mouthpiece_DB[slot].m_Attached = FALSE;
        screen_toggle((GX_WINDOW *)&InsertMouthpiece_Screen, window);
		break;

	//--------------------------------------------
	case GX_SIGNAL (IDLE_TIME_BUTTON_ID, GX_EVENT_CLICKED):
		Set_Standby_Exit_Screen ((GX_WINDOW *)&InsertMouthpiece_Screen);
        screen_toggle((GX_WINDOW *)&Standby_Screen, window);
		break;

	//--------------------------------------------
	case GX_SIGNAL (SYSTEM_ERROR_BTN_ID, GX_EVENT_CLICKED):
        screen_toggle((GX_WINDOW *)&Error_Screen, window);
		break;

	} // end switch

	return (GX_SUCCESS);
}


