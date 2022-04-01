//*****************************************************************************
// Filename: Reading_Screen_Handler.c
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

extern GX_CHAR *g_SerialNumber_Prompt;
extern VOID Enable_Limit_Switch (GX_WIDGET* limitSwitchButton, GX_BOOL enable);

//*************************************************************************************

VOID Reading_Screen_Draw_Function (GX_WINDOW *window)
{
	gx_window_draw(window);
}

//*************************************************************************************
// Function Name: Reading_Screen_Event_Function
//
// Description: This functions handles the Splash screen
//
//*************************************************************************************

UINT Reading_Screen_Event_Function (GX_WINDOW *window, GX_EVENT *event_ptr)
{
	UINT mySize, myBufSize;
	long serialNumber;
	int slot;

    gx_window_event_process(window, event_ptr);

	switch (event_ptr->gx_event_type)
	{
	//--------------------------------------------
	case GX_EVENT_SHOW:
		gx_widget_hide (&Reading_Screen.Reading_Screen_Instruction_TextView);

		// Disable ATTACH button, disable EEPROM buttons
		Enable_EEPROM_Buttons (&Reading_Screen.base, GX_TRUE, GX_TRUE, GX_TRUE, GX_TRUE, GX_TRUE);
		gx_text_button_text_id_set (&Reading_Screen.base.PrimaryTemplate_LimitSwitch_Button, GX_STRING_ID_DETACH);

		// Make Ring Color
		gx_icon_pixelmap_set (&Reading_Screen.Reading_Screen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);

		// Show the box, make it red and populate it.
		gx_widget_show (&Reading_Screen.Reading_Screen_WhiteBox_Icon);
		gx_icon_pixelmap_set (&Reading_Screen.Reading_Screen_WhiteBox_Icon, GX_PIXELMAP_ID_WHITE_TEXT_BOX, GX_PIXELMAP_ID_WHITE_TEXT_BOX);
		//Root_DisplayInstruction (&Error_Screen.Error_Screen_Instruction_TextView, "SYSTEM\rERROR 1", 2, GX_COLOR_ID_RED);
		DisplayInformation_InBox (&Reading_Screen.Reading_Screen_Information_TextView, "Reading...", 1, GX_COLOR_ID_WHITE);
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
		gx_single_line_text_input_buffer_get (&Reading_Screen.base.PrimaryTemplate_SerialNumber_TextInput, &g_SerialNumber_Prompt, &mySize, &myBufSize);
		serialNumber = atol (g_SerialNumber_Prompt);
		// Remove any previously attached Mouthpieces.
		for (slot = 0; slot < MOUTHPIECE_DB_SIZE; ++slot)
			g_Mouthpiece_DB[slot].m_Attached = FALSE;

		// Locate existing Mouthpiece Serial Number or next available slot for new Mouthpiece
		for (slot = 0; slot < MOUTHPIECE_DB_SIZE; ++slot)
		{
			if (g_Mouthpiece_DB[slot].m_SerialNumber == 0)			// This slot is available for a new mouthpiece
			{
				g_Mouthpiece_DB[slot].m_TherapyStatus = THERAPY_READY_TO_START;
				g_Mouthpiece_DB[slot].m_RemainingTherapyTime = 300;
				g_Mouthpiece_DB[slot].m_SerialNumber = serialNumber;
			}
			// Go ahead and process this Mouthpiece.... even its a new one
			if (g_Mouthpiece_DB[slot].m_SerialNumber == serialNumber)			// Same Serial Number?
			{
				g_Mouthpiece_DB[slot].m_Attached = TRUE;
				switch (g_Mouthpiece_DB[slot].m_TherapyStatus)
				{
				case THERAPY_IDLE:
				case THERAPY_READY_TO_START:
					g_Mouthpiece_DB[slot].m_RemainingTherapyTime = 300;
					screen_toggle((GX_WINDOW *)&SerialNumber_Screen, window);	// Go confirm the "new" mouthpiece
					break;
				case THERAPY_IN_PROGRESS:
					screen_toggle((GX_WINDOW *)&SerialNumber_Screen, window);	// Go confirm the "new" mouthpiece
					//screen_toggle((GX_WINDOW *)&PressToResume_Screen, window);
					break;
				case THERAPY_PAUSED:
					screen_toggle((GX_WINDOW *)&Therapy_Screen, window);
					break;
				case THERAPY_COMPLETE:
			        screen_toggle((GX_WINDOW *)&DailyLimitReached_Error_Screen, window);	// Yes, tell the user that we did.
					break;
				} // end switch m_TherapyStatus
				break; // no need to continue looking.
			}
		}
		break;

	//--------------------------------------------
	case GX_SIGNAL (EEPROM_FAIL_BTN_ID, GX_EVENT_CLICKED):
        screen_toggle((GX_WINDOW *)&ReadingError_Screen, window);
		break;

	//--------------------------------------------
	case GX_SIGNAL (EEPROM_EXPIRED_BTN_ID, GX_EVENT_CLICKED):
        screen_toggle((GX_WINDOW *)&Expired_Error_Screen, window);
		break;

	//--------------------------------------------
	case GX_SIGNAL (EEPROM_12HOUR_BTN_ID, GX_EVENT_CLICKED):
        screen_toggle((GX_WINDOW *)&DailyLimitReached_Error_Screen, window);
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


