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

#define TICK_CENTER_PT_HORIZONTAL 160	// From Left of screen
#define TICK_CENTER_PT_VERTICAL 158		// From Top of screen
#define WHITE_TICK_LENGTH 30			// This is the length of the white tick marks.
#define GREEN_TICK_LENGTH 20			// This is the length of the green tick marks.
#define TICK_LENGTH 100					// This is the total length of the tick from the
										// .. center of the screen.
#define DEGREES_TO_RADIANS (0.01745329252f)

typedef enum {
	STATE_INSERT_MOUTHPIECE = 0,
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
	STATE_RESUME_THERAPY,
	STATE_THERAPY_MOUTHPIECE_FAULT,
	STATE_THERAPY_RECOVER,
	STATE_THERAPY_IS_COMPLETE,
	STATE_OUT_OF_USES,
	STATE_STANDBY,
	STATE_SYSTEM_ERROR
} STATES_ENUM;

//*************************************************************************************
// External References
//*************************************************************************************

//*************************************************************************************
// Local/Global variables
//*************************************************************************************

STATES_ENUM g_State;
STATES_ENUM g_RecoveryState;	// Used to recover from Standby

UINT g_Used = 39;
GX_BOOL g_RingOn = FALSE;
char g_TimeString[16];
char g_TimeTickString[8];
GX_BOOL g_ShowTicks = FALSE;

int g_MouthPiece_SerialNumber = 1;
char g_SerialNumberString[64] = "1";
GX_CHAR *g_PromptString = NULL;

GX_BOOL g_LimitSwitchClosed = FALSE;
INT g_TherapyTime = 0;
GX_BOOL g_TherapyInProcess = FALSE;

struct WIDGET_ATTRIBUTE_STRUCT 
{
	ULONG m_LimitSwitchBtn;
	ULONG m_GoodBtn;
	ULONG m_BadBtn;
	ULONG m_ExpiredBtn;
	ULONG m_12HourBtn;
	ULONG m_WhiteBox;
	ULONG m_WhiteBoxColor;
	ULONG m_TickMark;
} g_WidgetAttibute;

void (*gp_State)(void);

void InsertMouthpieceState(void)
{
}

//*************************************************************************************

void EnableEEPROMPT_Button (GX_BOOL enable_OK, GX_BOOL enable_BAD, GX_BOOL enable_EXPIRED, GX_BOOL enable_12HOUR, GX_BOOL enable_ATTACH)
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
	
	// Enable/Disable EXPIRED button
	if (enable_EXPIRED)
		widgetStyle |= GX_STYLE_ENABLED;
	else
		widgetStyle &= (~GX_STYLE_ENABLED);
	gx_widget_style_set ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_EEPROM_Expired_Button, widgetStyle);

	// Enable/Disable 12 HOUR button
	if (enable_12HOUR)
		widgetStyle |= GX_STYLE_ENABLED;
	else
		widgetStyle &= (~GX_STYLE_ENABLED);
	gx_widget_style_set ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_EEPROM_12HOUR_Button, widgetStyle);

	// Enable/Disable ATTACH/DETACH button
	if (enable_ATTACH)
		widgetStyle |= GX_STYLE_ENABLED;
	else
		widgetStyle &= (~GX_STYLE_ENABLED);
	gx_widget_style_set ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_LimitSwitch_Button, widgetStyle);
}


//*************************************************************************************
// Disable or Enable the Idle Icon button

void EnableIdleButton (GX_BOOL enable)
{
	ULONG widgetStyle;

	gx_widget_style_get ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_IdleTime_Button, &widgetStyle);

	if (enable)
		widgetStyle |= GX_STYLE_ENABLED;
	else
		widgetStyle &= (~GX_STYLE_ENABLED);
	gx_widget_style_set ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_IdleTime_Button, widgetStyle);
}

//*************************************************************************************

VOID ReadyScreen_Draw_Function (GX_WINDOW *window)
{
	UINT status;
	INT secondsRemaining;
	GX_BRUSH *brush, *originalBrush;
	GX_VALUE verticalStart, verticalEnd, horizStart, horizEnd;
	float angle, radians, vertLength, horizLength, tempTime;
	GX_RECTANGLE rectangle;
	GX_COLOR customColor;

	gx_window_draw(window);

	gp_State = InsertMouthpieceState;

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
		gx_widget_show (&ReadyScreen.ReadyScreen_TimeTick_Prompt);
		horizStart = (GX_VALUE) ((TICK_CENTER_PT_HORIZONTAL - 8) - ((TICK_LENGTH - WHITE_TICK_LENGTH-12) * horizLength)); // "8" is half of width
		//horizEnd = TICK_CENTER_PT_HORIZONTAL - ((TICK_LENGTH - WHITE_TICK_LENGTH) * horizLength);
		verticalStart = (GX_VALUE) ((TICK_CENTER_PT_VERTICAL - 7) - ((TICK_LENGTH - WHITE_TICK_LENGTH-10) * vertLength));
		//verticalEnd = TICK_CENTER_PT_VERTICAL - ((TICK_LENGTH - WHITE_TICK_LENGTH) * vertLength);
		rectangle.gx_rectangle_top = verticalStart;
		rectangle.gx_rectangle_bottom = verticalStart + 14;
		rectangle.gx_rectangle_left = horizStart;
		rectangle.gx_rectangle_right = horizStart + 16;
		gx_widget_resize (&ReadyScreen.ReadyScreen_TimeTick_Prompt, &rectangle);
		sprintf_s (g_TimeTickString, sizeof(g_TimeTickString), "%d", g_TherapyTime % 60);
		gx_prompt_text_set (&ReadyScreen.ReadyScreen_TimeTick_Prompt, g_TimeTickString);

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
		gx_widget_hide (&ReadyScreen.ReadyScreen_TimeTick_Prompt);
	}
}

//*************************************************************************************
// This function moves the Information Window so the text is centered vertically.
// The widget takes care of horizontal centering.
//*************************************************************************************

void DisplayInformation (GX_WINDOW *window, GX_CHAR *myString, INT numberOfLines, GX_RESOURCE_ID myColor)
{
	GX_RECTANGLE rectangle;

	gx_widget_show (&ReadyScreen.ReadyScreen_Information_TextView);
	// font height is 20 pixels.
	// full height is 100 pixels which is 5 lines.
	rectangle.gx_rectangle_left = ReadyScreen.ReadyScreen_Information_TextView.gx_widget_size.gx_rectangle_left;
	rectangle.gx_rectangle_right = ReadyScreen.ReadyScreen_Information_TextView.gx_widget_size.gx_rectangle_right;

	gx_multi_line_text_view_text_color_set (&ReadyScreen.ReadyScreen_Information_TextView, myColor, myColor, myColor);
	rectangle.gx_rectangle_top = 108 + (10*(5 - numberOfLines));
	rectangle.gx_rectangle_bottom = rectangle.gx_rectangle_top + (20 * numberOfLines);
	gx_widget_show (&ReadyScreen.ReadyScreen_Information_TextView);
	gx_widget_resize (&ReadyScreen.ReadyScreen_Information_TextView, &rectangle);
	gx_multi_line_text_view_text_set (&ReadyScreen.ReadyScreen_Information_TextView, myString);
}

//*************************************************************************************
void DisplayInstruction (GX_WINDOW *window, GX_CHAR *myString, INT numberOfLines, GX_RESOURCE_ID myColor)
{
	GX_RECTANGLE rectangle;

	gx_widget_show (&ReadyScreen.ReadyScreen_Instruction_TextView);
	// font height is 20 pixels.
	// full height is 100 pixels which is 5 lines.
	rectangle.gx_rectangle_left = ReadyScreen.ReadyScreen_Instruction_TextView.gx_widget_size.gx_rectangle_left;
	rectangle.gx_rectangle_right = ReadyScreen.ReadyScreen_Instruction_TextView.gx_widget_size.gx_rectangle_right;

	gx_multi_line_text_view_text_color_set (&ReadyScreen.ReadyScreen_Instruction_TextView, myColor, myColor, myColor);
	rectangle.gx_rectangle_top = 108 + (11*(5 - numberOfLines));
	rectangle.gx_rectangle_bottom = rectangle.gx_rectangle_top + (20 * numberOfLines);
	gx_widget_resize (&ReadyScreen.ReadyScreen_Instruction_TextView, &rectangle);
	gx_multi_line_text_view_text_set (&ReadyScreen.ReadyScreen_Instruction_TextView, myString);
}

//*************************************************************************************
// This function displays two kinds of strings.
// The first string is destined for the "Information" box.
// The second string is destined for the "Instuction" box.
//*************************************************************************************
void DisplayMultiInformation ()
{
}

//*************************************************************************************
// This function displays the INSERT MOUTHPIECE information and hides everything else

void EnterInsertMouthpieceState (GX_WINDOW *window, STATES_ENUM nextState)
{
	ULONG widgetStyle;

	g_State = nextState;
	g_LimitSwitchClosed = FALSE;
	g_TherapyInProcess = FALSE;
	g_ShowTicks = FALSE;

	//gx_widget_hide (&ReadyScreen.ReadyScreen_TherpayTime_RadialProgressBar);
	gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
	gx_widget_hide (&ReadyScreen.ReadyScreen_TimeTick_Prompt);
	gx_widget_hide (&ReadyScreen.ReadyScreen_Time_Prompt);
	gx_widget_hide (&ReadyScreen.ReadyScreen_Minute_Prompt);
	gx_widget_hide (&ReadyScreen.ReadyScreen_GreenTick_Icon);
	gx_widget_hide (&ReadyScreen.ReadyScreen_ScreenSaver_Icon);
	gx_widget_hide (&ReadyScreen.ReadyScreen_Information_TextView);

	//gx_widget_show (&ReadyScreen.ReadyScreen_Instruction_TextView);
	gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);

	// The limit switch button may have been disabled.... we need to enable it.
	gx_widget_style_get ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_LimitSwitch_Button, &widgetStyle);
	widgetStyle |= GX_STYLE_ENABLED;
	gx_widget_style_set ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_LimitSwitch_Button, widgetStyle);
	gx_text_button_text_id_set (&ReadyScreen.base.PrimaryTemplate_LimitSwitch_Button, GX_STRING_ID_ATTACH);

	gx_multi_line_text_button_text_id_set (&ReadyScreen.base.PrimaryTemplate_SystemError_Button, GX_STRING_ID_STRING_24);	// "Cause System Error"

	//sprintf_s (g_TimeString, sizeof (g_TimeString), "%d:%02d", g_TherapyTime / 60, g_TherapyTime % 60);
	EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_WHITE, GX_PIXELMAP_ID_STATUSRING_WHITE);
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_WhiteBox_Icon, GX_PIXELMAP_ID_WHITE_TEXT_BOX, GX_PIXELMAP_ID_WHITE_TEXT_BOX);
	//DisplayInformation (window, "Insert\rMouthpiece", 2, GX_COLOR_ID_WHITE);
	DisplayInstruction (window, "Insert\rMouthpiece", 2, GX_COLOR_ID_WHITE);
}

//*************************************************************************************
// This function displays the READING.... information and hides everything else

void EnterReadingEEPROM_State (GX_WINDOW *window, STATES_ENUM nextState)
{
	g_ShowTicks = FALSE;

	gx_widget_hide (&ReadyScreen.ReadyScreen_Instruction_TextView);
	gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);

	gx_icon_button_pixelmap_set (&ReadyScreen.base.PrimaryTemplate_Attach_IconButton, GX_PIXELMAP_ID_RADIO_ON);
	gx_text_button_text_id_set (&ReadyScreen.base.PrimaryTemplate_LimitSwitch_Button, GX_STRING_ID_DETACH);

	EnableEEPROMPT_Button (GX_TRUE, GX_TRUE, GX_TRUE, GX_TRUE, GX_TRUE);

	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_WhiteBox_Icon, GX_PIXELMAP_ID_WHITE_TEXT_BOX, GX_PIXELMAP_ID_WHITE_TEXT_BOX);

	DisplayInformation (window, "Reading...", 1, GX_COLOR_ID_WHITE);
	g_State = nextState;
}

//*************************************************************************************
// This function displays the INSERT MOUTHPIECE information and hides everything else

void EnterSerialNumber_State (GX_WINDOW *window, STATES_ENUM nextState)
{
	GX_RECTANGLE rect;
	UINT mySize, myBufSize;

	g_ShowTicks = FALSE;

	// Set LED ring to Blue
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);
	// Set Text Box to White
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_WhiteBox_Icon, GX_PIXELMAP_ID_WHITE_TEXT_BOX, GX_PIXELMAP_ID_WHITE_TEXT_BOX);
	gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);

	// Display "Serial Number: MCAxxxxxx" in the information box
	gx_single_line_text_input_buffer_get (&ReadyScreen.base.PrimaryTemplate_SerialNumber_TextInput, &g_PromptString, &mySize, &myBufSize);
	g_MouthPiece_SerialNumber = atoi (g_PromptString);
	sprintf_s (g_SerialNumberString, sizeof (g_SerialNumberString), "Serial Number:\rMCA%06d\r\r", g_MouthPiece_SerialNumber);
	DisplayInformation (window, g_SerialNumberString, 4, GX_COLOR_ID_WHITE);
	// Display Press in the Instruction box
	DisplayInstruction (window, "\r\r  Press      \rto Continue", 4, GX_COLOR_ID_WHITE);
	rect.gx_rectangle_top = 160;
	rect.gx_rectangle_bottom = rect.gx_rectangle_top + 18;
	rect.gx_rectangle_left = 176;
	rect.gx_rectangle_right = rect.gx_rectangle_left + 26;
	gx_widget_resize (&ReadyScreen.ReadyScreen_PauseIcon_Button, &rect);
	gx_widget_show (&ReadyScreen.ReadyScreen_PauseIcon_Button);

	EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);

	g_State = nextState;
}

//*************************************************************************************
// This function displays the Start Therapy information and hides everything else

void EnterStartTherapy_State (GX_WINDOW *window, STATES_ENUM nextState)
{
	GX_RECTANGLE rect;

	g_ShowTicks = FALSE;

	gx_widget_hide (&ReadyScreen.ReadyScreen_Information_TextView);

	// Set LED ring to Blue
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);
	// Set Text Box to White
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_WhiteBox_Icon, GX_PIXELMAP_ID_WHITE_TEXT_BOX, GX_PIXELMAP_ID_WHITE_TEXT_BOX);
	gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
	DisplayInstruction (window, "Press     \nto Start\nTherapy", 3, GX_COLOR_ID_WHITE);
	// Display Play/pause icon
	rect.gx_rectangle_top = 128 + 2;	// "10" is used for 2 lines, "2" is for 3 lines
	rect.gx_rectangle_bottom = rect.gx_rectangle_top + 18;
	rect.gx_rectangle_left = 176; // + 12;
	rect.gx_rectangle_right = rect.gx_rectangle_left + 26;
	gx_widget_resize (&ReadyScreen.ReadyScreen_PauseIcon_Button, &rect);
	gx_widget_show (&ReadyScreen.ReadyScreen_PauseIcon_Button);
	
	g_State = nextState;
}

//*************************************************************************************
// This function displays the Resume Therapy information and hides everything else

void EnterResumeTherapy_State (GX_WINDOW *window, STATES_ENUM nextState)
{
	GX_RECTANGLE rect;

	g_ShowTicks = FALSE;

	gx_widget_hide (&ReadyScreen.ReadyScreen_Information_TextView);

	gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);

	// Set to Blue LED ring
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_BLUE, GX_PIXELMAP_ID_STATUSRING_BLUE);
	// Set to White Text Box
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_WhiteBox_Icon, GX_PIXELMAP_ID_WHITE_TEXT_BOX, GX_PIXELMAP_ID_WHITE_TEXT_BOX);

	DisplayInstruction (window, "Press     \nto Resume\nTherapy", 3, GX_COLOR_ID_WHITE);
	rect.gx_rectangle_top = 128 + 2;	// "10" is used for 2 lines, remove for 3 lines
	rect.gx_rectangle_bottom = rect.gx_rectangle_top + 18;
	rect.gx_rectangle_left = 176; //  + 12;
	rect.gx_rectangle_right = rect.gx_rectangle_left + 26;
	gx_widget_resize (&ReadyScreen.ReadyScreen_PauseIcon_Button, &rect);
	gx_widget_show (&ReadyScreen.ReadyScreen_PauseIcon_Button);
	
	g_State = nextState;
}

//*************************************************************************************
// This function displays the Therapy information and hides everything else

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

//*************************************************************************************
// This function displays the PAUSED information and hides everything else

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

//*************************************************************************************
// This function displays the Therapy Complete information and hides everything else

void EnterTherapyComplete_State (GX_WINDOW *window, STATES_ENUM nextState)
{
	gx_widget_hide (&ReadyScreen.ReadyScreen_Minute_Prompt);
	gx_widget_hide (&ReadyScreen.ReadyScreen_Time_Prompt);
	gx_widget_hide (&ReadyScreen.ReadyScreen_GreenTick_Icon);
	g_ShowTicks = FALSE;

	//gx_widget_show (&ReadyScreen.ReadyScreen_Information_TextView);
	gx_widget_hide (&ReadyScreen.ReadyScreen_Instruction_TextView);
	DisplayInformation (window, "Therapy\rComplete", 2, GX_COLOR_ID_WHITE);
	//DisplayInstruction (window, "\r\rRemove Mouthpiece", 4, GX_COLOR_ID_WHITE);

	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_WhiteBox_Icon, GX_PIXELMAP_ID_WHITE_TEXT_BOX, GX_PIXELMAP_ID_WHITE_TEXT_BOX);
	gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
	gx_widget_show (&ReadyScreen.ReadyScreen_Information_Button);
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_GREEN, GX_PIXELMAP_ID_STATUSRING_GREEN);
	gx_system_timer_start(window, THERAPY_COMPLETE_TIMER_ID, 20, 0);	// Resume the timer
	g_RingOn = TRUE;
	EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);
	g_TherapyTime = 0;
	g_TherapyInProcess = FALSE;
	g_State = nextState;
}

//*************************************************************************************
// This function displays the READING ERROR REINSERT MOUTHPIECE information in RED
// and hides everything else

void EnterReadingError_State (GX_WINDOW *window, STATES_ENUM nextState)
{
	gx_system_timer_stop (window, THERAPY_TIMER_ID);	// We may have gotten here during Therapy

	gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
	gx_widget_hide (&ReadyScreen.ReadyScreen_Minute_Prompt);
	gx_widget_hide (&ReadyScreen.ReadyScreen_Time_Prompt);
	gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
	//gx_widget_show (&ReadyScreen.ReadyScreen_Instruction_TextView);
	DisplayInformation (window, "READING\nERROR\r\r\r", 4, GX_COLOR_ID_RED);
	DisplayInstruction (window, "\r\rREINSERT\rMOUTHPIECE", 4, GX_COLOR_ID_RED);
	//DisplayInstruction (window, "\r\rReinsert\rMouthpiece", 4, GX_COLOR_ID_RED);
	g_ShowTicks = FALSE;
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_WhiteBox_Icon, GX_PIXELMAP_ID_RED_TEXT_BOX, GX_PIXELMAP_ID_RED_TEXT_BOX);
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);
	EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);
	g_State = nextState;
}

void EnterMouthpieceDetachedError_State (GX_WINDOW *window, STATES_ENUM nextState)
{
	gx_system_timer_stop (window, THERAPY_TIMER_ID);	// We may have gotten here during Therapy

	gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
	gx_widget_hide (&ReadyScreen.ReadyScreen_Minute_Prompt);
	gx_widget_hide (&ReadyScreen.ReadyScreen_Time_Prompt);
	gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
	//gx_widget_show (&ReadyScreen.ReadyScreen_Instruction_TextView);
	DisplayInformation (window, "MOUTHPIECE\nDETACHED\r\r\r", 4, GX_COLOR_ID_RED);
	DisplayInstruction (window, "\r\rREINSERT\rMOUTHPIECE", 4, GX_COLOR_ID_RED);
	//DisplayInstruction (window, "\r\rReinsert\rMouthpiece", 4, GX_COLOR_ID_RED);
	g_ShowTicks = FALSE;
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_WhiteBox_Icon, GX_PIXELMAP_ID_RED_TEXT_BOX, GX_PIXELMAP_ID_RED_TEXT_BOX);
	gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);
	EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);
	g_State = nextState;
}

//*************************************************************************************
// Function Name: ReadyScreen_Event_Function
//
// Description: This functions handles the Splash screen
//
//*************************************************************************************

UINT ReadyScreen_Event_Function (GX_WINDOW *window, GX_EVENT *event_ptr)
{
	ULONG widgetStyle;
	UINT mySize, myBufSize;
	GX_RECTANGLE rect;
	INT thisSerialNumber;

    gx_window_event_process(window, event_ptr);

	switch (event_ptr->gx_event_type)
	{
	case GX_EVENT_SHOW:
		EnterInsertMouthpieceState(window, STATE_INSERT_MOUTHPIECE);
		break;

	//----------------------------------------------------------------------------------------
	// Pause/Play button is pushed
	//----------------------------------------------------------------------------------------
	case GX_SIGNAL (PLAY_BTN_ID, GX_EVENT_CLICKED):

		switch (g_State)
		{
		case STATE_SERIAL_NUMBER_PROMPT:
			if (g_TherapyTime > 0)
				EnterResumeTherapy_State (window, STATE_RESUME_THERAPY);
			else
				EnterStartTherapy_State (window, STATE_READY_TO_GO);
			break;

		case STATE_READY_TO_GO:
			EnableIdleButton (FALSE);
			EnterTherapyInProgress_State(window, STATE_THERAPY_IN_PROCESS);
			break;

		case STATE_THERAPY_IN_PROCESS:
			EnableIdleButton (TRUE);
			EnterPaused_State(window, STATE_THERAPY_PAUSED);
			break;

		case STATE_THERAPY_PAUSED:
			EnableIdleButton (FALSE);
			EnterTherapyInProgress_State(window, STATE_THERAPY_IN_PROCESS);
			break;

		case STATE_RESUME_THERAPY:
			EnableIdleButton (FALSE);
			EnterPaused_State(window, STATE_THERAPY_PAUSED);
			//EnterTherapyInProgress_State(window, STATE_THERAPY_PAUSED); // STATE_THERAPY_IN_PROCESS);
			break;

		case STATE_STANDBY:
			// Re-enable IDLE button
			EnableIdleButton (TRUE);
			// Hide Standby "floating" icon
			gx_widget_hide (&ReadyScreen.ReadyScreen_ScreenSaver_Icon);
			gx_system_timer_stop (window, STANDBY_TIMER);	// Stop the standby timer

			switch (g_RecoveryState)
			{
			case STATE_INSERT_MOUTHPIECE:
				EnterInsertMouthpieceState(window, STATE_INSERT_MOUTHPIECE);
				break;
			case STATE_READING_EEPROM:
				EnterReadingEEPROM_State(window, STATE_READING_EEPROM);
				break;
			case STATE_SERIAL_NUMBER_PROMPT:
				EnterSerialNumber_State(window, STATE_SERIAL_NUMBER_PROMPT);
				break;
			case STATE_READY_TO_GO:
				EnterStartTherapy_State (window, STATE_READY_TO_GO);
				break;
			case STATE_THERAPY_PAUSED:
				EnterPaused_State(window, STATE_THERAPY_PAUSED);
				break;
			case STATE_THERAPY_IS_COMPLETE:
				EnterTherapyComplete_State(window, STATE_THERAPY_IS_COMPLETE);
				break;
			default:
				g_State = g_RecoveryState;
				break;
			} // end switch g_RecoveryState
			break;
		} // end switch of Play Button
		break;

	//----------------------------------------------------------------------------------------
	// MOUTHPIECE (Limit) switch
	//----------------------------------------------------------------------------------------
	case GX_SIGNAL (LIMIT_SWITCH_BTN_ID, GX_EVENT_CLICKED):
		gx_system_timer_stop (window, PAUSE_TIMER_ID);
		gx_widget_hide (&ReadyScreen.ReadyScreen_ScreenSaver_Icon);
		gx_system_timer_stop (window, STANDBY_TIMER);	// Stop the standby timer
		// Enable the Idle Button
		EnableIdleButton(TRUE);

		// Switch is open, let's close it
		if (g_LimitSwitchClosed == FALSE)
		{
			g_LimitSwitchClosed = TRUE;
			// If we are in a "Fault during Therapy Session" state, let's resume the Therapy
			if (g_TherapyInProcess)
			{
				EnterReadingEEPROM_State(window, STATE_THERAPY_CONTINUE);
			}
			else
			{
				EnterReadingEEPROM_State(window, STATE_READING_EEPROM);
			}
		}
		else	// Switch is CLOSED, let's open it.
		{
			g_ShowTicks = FALSE;
			gx_system_timer_stop(window, THERAPY_COMPLETE_TIMER_ID);
			g_LimitSwitchClosed = FALSE;
			gx_text_button_text_id_set (&ReadyScreen.base.PrimaryTemplate_LimitSwitch_Button, GX_STRING_ID_ATTACH);
			gx_widget_hide (&ReadyScreen.ReadyScreen_Minute_Prompt);
			gx_widget_hide (&ReadyScreen.ReadyScreen_Time_Prompt);
			if ((g_State == STATE_THERAPY_IN_PROCESS) || (g_State == STATE_THERAPY_PAUSED))
			{
				EnterMouthpieceDetachedError_State (window, STATE_THERAPY_MOUTHPIECE_FAULT);
			}
			else if (g_State == STATE_THERAPY_RECOVER)
			{
				g_State = STATE_THERAPY_MOUTHPIECE_FAULT;
			}
			else // STATE_READING_EEPROM
			{
				EnterInsertMouthpieceState(window, STATE_INSERT_MOUTHPIECE);
			}
		}
		break;

	case GX_SIGNAL (EEPROM_OK_BTN_ID, GX_EVENT_CLICKED):
		if (g_State == STATE_READING_EEPROM)
		{
			EnterSerialNumber_State(window, STATE_SERIAL_NUMBER_PROMPT);
		}
		else if (g_State == STATE_THERAPY_CONTINUE)	// We are recovering from a Mouthpiece detached condition.
		{
			gx_single_line_text_input_buffer_get (&ReadyScreen.base.PrimaryTemplate_SerialNumber_TextInput, &g_PromptString, &mySize, &myBufSize);
			thisSerialNumber = atoi (g_PromptString);
			if (thisSerialNumber == g_MouthPiece_SerialNumber)
			{
				EnterPaused_State(window, STATE_THERAPY_PAUSED);
			}
			else // Different serial number
			{
				g_TherapyInProcess = FALSE;
				EnterSerialNumber_State(window, STATE_SERIAL_NUMBER_PROMPT);
			}
			break;
		}
		break;

	case GX_SIGNAL (EEPROM_EXPIRED_BTN_ID, GX_EVENT_CLICKED):
		if (g_State == STATE_READING_EEPROM)
		{
			gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_WhiteBox_Icon, GX_PIXELMAP_ID_RED_TEXT_BOX, GX_PIXELMAP_ID_RED_TEXT_BOX);
			DisplayInformation (window, "MOUTHPIECE\rEXPIRED", 2, GX_COLOR_ID_RED);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);
			EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);
			g_State = STATE_MOUTHPIECE_EXPIRED;
		}
		else if (g_State == STATE_THERAPY_CONTINUE)	// We are recovering from a Mouthpiece detached condition.
		{
			gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_WhiteBox_Icon, GX_PIXELMAP_ID_RED_TEXT_BOX, GX_PIXELMAP_ID_RED_TEXT_BOX);
			DisplayInformation (window, "MOUTHPIECE\rDAILY LIMIT REACHED", 3, GX_COLOR_ID_RED);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);
			EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);
			g_State = STATE_MOUTHPIECE_EXPIRED;
			break;
		}
		break;

	case GX_SIGNAL (EEPROM_FAIL_BTN_ID, GX_EVENT_CLICKED):
		if (g_State == STATE_READING_EEPROM)
		{
			EnterReadingError_State(window, STATE_CABLE_FAULT);
		}
		else if (g_State == STATE_THERAPY_IN_PROCESS)
		{	// We are doing therapy and we determined that we cannot write to the mouthpiece
			// Indicate a Red "Mouthpiece Error", "Reinsrt Mouthpiece".
			// change to STATE_THERAPY_RECOVER state
			EnterReadingError_State(window, STATE_THERAPY_RECOVER);
		}
		else if (g_State == STATE_THERAPY_CONTINUE)	// We are recovering from a Mouthpiece detached condition.
		{
			DisplayInformation (window, "MOUTHPIECE\rFAULT", 2, GX_COLOR_ID_RED);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_WhiteBox_Icon, GX_PIXELMAP_ID_RED_TEXT_BOX, GX_PIXELMAP_ID_RED_TEXT_BOX);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);
			EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);
			g_State = STATE_THERAPY_RECOVER;
		}
		break;

	case GX_SIGNAL (EEPROM_12HOUR_BTN_ID, GX_EVENT_CLICKED):
		{	// We have determined that the mouthpiece is being used within a 12 hour period.
			// Indicate a Red "Mouthpiece Error", "Reinsrt Mouthpiece".
			// change to STATE_THERAPY_RECOVER state
			gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
			gx_widget_hide (&ReadyScreen.ReadyScreen_Minute_Prompt);
			gx_widget_hide (&ReadyScreen.ReadyScreen_Time_Prompt);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_WhiteBox_Icon, GX_PIXELMAP_ID_RED_TEXT_BOX, GX_PIXELMAP_ID_RED_TEXT_BOX);
			gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
			// Update USER Information
			DisplayInformation (window, "MOUTHPIECE DAILY LIMIT REACHED", 3, GX_COLOR_ID_RED);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);
			g_ShowTicks = FALSE;
			gx_system_timer_stop (window, THERAPY_TIMER_ID);
			EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_TRUE);
			g_State = STATE_CABLE_FAULT;
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
				EnableIdleButton (TRUE);
				EnterTherapyComplete_State(window, STATE_THERAPY_IS_COMPLETE);
			}
			else
			{
				// Display only the remaining minutes.
				sprintf_s (g_TimeString, sizeof (g_TimeString), "%d", g_TherapyTime / 60);
				gx_prompt_text_set (&ReadyScreen.ReadyScreen_Time_Prompt, g_TimeString);
				g_ShowTicks = TRUE;
			    gx_system_dirty_mark(&ReadyScreen);      // This forces the gauge to be updated and redrawn
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
		else if (event_ptr->gx_event_payload.gx_event_timer_id == STANDBY_TIMER)
		{
			// We're in standby, float the Standby Icon around the screen.
			rect = ReadyScreen.ReadyScreen_ScreenSaver_Icon.gx_widget_size;
			rect.gx_rectangle_top += 5;
			if (rect.gx_rectangle_top > 148)
				rect.gx_rectangle_top = 70;
			rect.gx_rectangle_bottom = rect.gx_rectangle_top + 80;
			rect.gx_rectangle_left += 5;
			if (rect.gx_rectangle_left > 180)
				rect.gx_rectangle_left = 74;
			rect.gx_rectangle_right = rect.gx_rectangle_left + 60;
			gx_widget_resize (&ReadyScreen.ReadyScreen_ScreenSaver_Icon, &rect);
			gx_system_timer_start(window, STANDBY_TIMER, 20, 0);
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
			gx_widget_hide (&ReadyScreen.ReadyScreen_Minute_Prompt);
			gx_widget_hide (&ReadyScreen.ReadyScreen_GreenTick_Icon);
			gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
			gx_widget_hide (&ReadyScreen.ReadyScreen_ScreenSaver_Icon);
			gx_widget_hide (&ReadyScreen.ReadyScreen_Instruction_TextView);
			g_ShowTicks = FALSE;
			gx_multi_line_text_button_text_id_set (&ReadyScreen.base.PrimaryTemplate_SystemError_Button, GX_STRING_ID_STRING_7);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_WhiteBox_Icon, GX_PIXELMAP_ID_RED_TEXT_BOX, GX_PIXELMAP_ID_RED_TEXT_BOX);
			gx_widget_show (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
			EnableEEPROMPT_Button (GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE, GX_FALSE);
			// Disable the MOUTHPIECE Button
			gx_widget_style_get ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_LimitSwitch_Button, &widgetStyle);
			widgetStyle &= (~GX_STYLE_ENABLED);
			gx_widget_style_set ((GX_WIDGET*) &ReadyScreen.base.PrimaryTemplate_LimitSwitch_Button, widgetStyle);

			// Update USER Information
			DisplayInformation (window, "SYSTEM\rERROR 01", 2, GX_COLOR_ID_RED);
			gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_RED, GX_PIXELMAP_ID_STATUSRING_RED);

			// Stop all timers
			gx_system_timer_stop (window, THERAPY_TIMER_ID);
			gx_system_timer_stop (window, THERAPY_COMPLETE_TIMER_ID);
			gx_system_timer_stop (window, PAUSE_TIMER_ID);
			g_State = STATE_SYSTEM_ERROR;
		}
		break;

	case GX_SIGNAL (IDLE_TIME_BUTTON_ID, GX_EVENT_CLICKED):
		EnableIdleButton (FALSE);		// Disable the button
		g_RecoveryState = g_State;
		g_State = STATE_STANDBY;

		// Stop all timers
		gx_system_timer_stop (window, THERAPY_TIMER_ID);
		gx_system_timer_stop (window, THERAPY_COMPLETE_TIMER_ID);
		gx_system_timer_stop (window, PAUSE_TIMER_ID);

		// Hide all widgets
		gx_widget_hide (&ReadyScreen.ReadyScreen_Time_Prompt);
		gx_widget_hide (&ReadyScreen.ReadyScreen_Minute_Prompt);
		gx_widget_hide (&ReadyScreen.ReadyScreen_GreenTick_Icon);
		gx_widget_hide (&ReadyScreen.ReadyScreen_PauseIcon_Button);
		gx_widget_hide (&ReadyScreen.ReadyScreen_WhiteBox_Icon);
		gx_widget_hide (&ReadyScreen.ReadyScreen_Information_TextView);
		gx_widget_hide (&ReadyScreen.ReadyScreen_Instruction_TextView);
		g_ShowTicks = FALSE;

		// Turn off LED ring
		gx_icon_pixelmap_set (&ReadyScreen.ReadyScreen_StatusRing_Icon, GX_PIXELMAP_ID_STATUSRING_OFF, GX_PIXELMAP_ID_STATUSRING_OFF);

		// Show Screensaver Icon 
		ReadyScreen.ReadyScreen_ScreenSaver_Icon.gx_widget_size.gx_rectangle_top = 134;
		ReadyScreen.ReadyScreen_ScreenSaver_Icon.gx_widget_size.gx_rectangle_bottom = ReadyScreen.ReadyScreen_ScreenSaver_Icon.gx_widget_size.gx_rectangle_top + 53;
		ReadyScreen.ReadyScreen_ScreenSaver_Icon.gx_widget_size.gx_rectangle_left = 142;
		ReadyScreen.ReadyScreen_ScreenSaver_Icon.gx_widget_size.gx_rectangle_top = ReadyScreen.ReadyScreen_ScreenSaver_Icon.gx_widget_size.gx_rectangle_left + 40;

		gx_widget_show (&ReadyScreen.ReadyScreen_ScreenSaver_Icon);

		gx_system_timer_start(window, STANDBY_TIMER, 2, 0);

		break;
	}

	return (GX_SUCCESS);
}


