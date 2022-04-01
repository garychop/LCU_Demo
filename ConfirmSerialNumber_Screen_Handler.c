//*****************************************************************************
// Filename: ConfirmSerialNumber_Screen_Handler.c
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

GX_CHAR *g_SerialNumber_Prompt;
char g_SerialNumberString[64] = "1";

//*************************************************************************************

VOID SerialNumber_Screen_Draw_Function (GX_WINDOW *window)
{
	gx_window_draw(window);
}

//*************************************************************************************
// Function Name: SerialNumber_Screen_Event_Function
//
// Description: This functions handles the Splash screen
//
//*************************************************************************************

UINT SerialNumber_Screen_Event_Function (GX_WINDOW *window, GX_EVENT *event_ptr)
{
	GX_RECTANGLE rect;
	int slot;

    gx_window_event_process(window, event_ptr);

	switch (event_ptr->gx_event_type)
	{
	//--------------------------------------------
	case GX_EVENT_SHOW:

		// Make Ring Color
		gx_icon_pixelmap_set (&SerialNumber_Screen.SerialNumber_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);

		// Display "Serial Number: MCAxxxxxx" in the information box
		for (slot = 0; slot < MOUTHPIECE_DB_SIZE; ++slot)
		{
			if (g_Mouthpiece_DB[slot].m_Attached == TRUE)
			{
				break;
			}
		}
		sprintf_s (g_SerialNumberString, sizeof (g_SerialNumberString), "Serial Number:\rMCA%06d\r\r", g_Mouthpiece_DB[slot].m_SerialNumber);

		// Show the box, color it and populate it.
		gx_widget_show (&SerialNumber_Screen.SerialNumber_Screen_WhiteBox_Icon);
		gx_icon_pixelmap_set (&SerialNumber_Screen.SerialNumber_Screen_WhiteBox_Icon, GX_PIXELMAP_ID_WHITE_TEXT_BOX, GX_PIXELMAP_ID_WHITE_TEXT_BOX);
		DisplayInformation_InBox (&SerialNumber_Screen.SerialNumber_Screen_Information_TextView, g_SerialNumberString, 4, GX_COLOR_ID_WHITE);

		// Display Press in the Instruction box
		DisplayInstruction_InBox (&SerialNumber_Screen.SerialNumber_Screen_Instruction_TextView, "\r\r Press       \rto Continue", 4, GX_COLOR_ID_WHITE);
		rect.gx_rectangle_top = 160;
		rect.gx_rectangle_bottom = rect.gx_rectangle_top + 18;
		rect.gx_rectangle_left = 176;
		rect.gx_rectangle_right = rect.gx_rectangle_left + 26;
		gx_widget_resize (&SerialNumber_Screen.SerialNumber_Screen_PauseIcon_Button, &rect);
		gx_widget_show (&SerialNumber_Screen.SerialNumber_Screen_PauseIcon_Button);

		gx_text_button_text_id_set (&SerialNumber_Screen.base.PrimaryTemplate_LimitSwitch_Button, GX_STRING_ID_DETACH);
		Enable_EEPROM_Buttons (&SerialNumber_Screen.base, GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);
		break;

	//--------------------------------------------
	case GX_SIGNAL (PLAY_BTN_ID, GX_EVENT_CLICKED):
		for (slot = 0; slot < MOUTHPIECE_DB_SIZE; ++slot)
		{
			if (g_Mouthpiece_DB[slot].m_Attached == TRUE)
			{
				if (g_Mouthpiece_DB[slot].m_TherapyStatus == THERAPY_IN_PROGRESS)
				{
					screen_toggle((GX_WINDOW *)&PressToResume_Screen, window);
				}
				else
				{
					screen_toggle((GX_WINDOW *)&PressToStart_Screen, window);
				}
			}
		}

		break;

	//--------------------------------------------
	case GX_SIGNAL (LIMIT_SWITCH_BTN_ID, GX_EVENT_CLICKED):
		g_LimitSwitchClosed = FALSE;
		for (slot = 0; slot < MOUTHPIECE_DB_SIZE; ++slot)
			g_Mouthpiece_DB[slot].m_Attached = FALSE;
        screen_toggle((GX_WINDOW *)&InsertMouthpiece_Screen, window);
		break;

	//--------------------------------------------
	case GX_SIGNAL (EEPROM_OK_BTN_ID, GX_EVENT_CLICKED):
		// Process serial number
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


