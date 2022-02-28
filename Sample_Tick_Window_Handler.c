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
//*************************************************************************************
// External References
//*************************************************************************************

//*************************************************************************************
// Local/Global variables
//*************************************************************************************

int g_TimeInSeconds;
GX_CHAR g_TimeString[8] = "hi";

//*************************************************************************************
// DrawFunction
//
// Vertical is Y, Horizontal is X
//*************************************************************************************

VOID Sample_Tick_Window_draw_function (GX_WINDOW *window)
{
	UINT status;
	INT timeRemaining;
	GX_BRUSH *brush, *originalBrush;
	GX_VALUE verticalStart, verticalEnd, horizStart, horizEnd;
	float angle, radians, vertLength, horizLength;
	GX_RECTANGLE rectangle;
	GX_COLOR customColor;

	gx_window_draw(window);

	status = gx_context_brush_get(&brush);
	originalBrush = brush;

	brush->gx_brush_width = 4;

	// Translate seconds into angle.
	// 60 seconds = 90 degrees,
	// 45 seconds = 180 degrees,
	// 30 seconds = 270 degrees,
	// 15 seconds = 360 degrees.
	angle = (60.0f - (float) g_TimeInSeconds) * 6.0f + 90.0f;
	radians = angle * DEGREES_TO_RADIANS;
	vertLength = (float) sin (radians);
	horizLength = (float) cos (radians);
	//vertLength = (float) sin (0.0f * DEGREES_TO_RADIANS);
	//vertLength = (float) sin (270.0f * DEGREES_TO_RADIANS);
	//vertLength = (float) sin (180.0f * DEGREES_TO_RADIANS);
	
	// Show White current tick mark
	brush->gx_brush_line_color = GX_COLOR_WHITE;
	horizStart = (GX_VALUE) (TICK_CENTER_PT_HORIZONTAL - (TICK_LENGTH * horizLength));
	horizEnd = (GX_VALUE) (TICK_CENTER_PT_HORIZONTAL - ((TICK_LENGTH - WHITE_TICK_LENGTH) * horizLength));
	verticalStart = (GX_VALUE) (TICK_CENTER_PT_VERTICAL - (TICK_LENGTH * vertLength));
	verticalEnd = (GX_VALUE) (TICK_CENTER_PT_VERTICAL - ((TICK_LENGTH - WHITE_TICK_LENGTH) * vertLength));
	gx_canvas_line_draw (horizStart, verticalStart, horizEnd, verticalEnd);
	// Show the current time
	horizStart = (GX_VALUE) ((TICK_CENTER_PT_HORIZONTAL - 18) - ((TICK_LENGTH - WHITE_TICK_LENGTH-18) * horizLength));
	//horizEnd = TICK_CENTER_PT_HORIZONTAL - ((TICK_LENGTH - WHITE_TICK_LENGTH) * horizLength);
	verticalStart = (GX_VALUE) ((TICK_CENTER_PT_VERTICAL - 12) - ((TICK_LENGTH - WHITE_TICK_LENGTH-12) * vertLength));
	//verticalEnd = TICK_CENTER_PT_VERTICAL - ((TICK_LENGTH - WHITE_TICK_LENGTH) * vertLength);
	rectangle.gx_rectangle_top = verticalStart;
	rectangle.gx_rectangle_bottom = verticalStart + 24;
	rectangle.gx_rectangle_left = horizStart;
	rectangle.gx_rectangle_right = horizStart + 36;
	gx_widget_resize (&Sample_Tick_Window.Sample_Tick_Window_Time_Prompt, &rectangle);
	sprintf_s (g_TimeString, 8, "%d", g_TimeInSeconds);
	gx_prompt_text_set (&Sample_Tick_Window.Sample_Tick_Window_Time_Prompt, g_TimeString);

	// Now draw the green shorter ticks from the current seconds to 0.
	brush->gx_brush_line_color = 0xff787c78; // LCU_MainDisplay_theme_1_color_table[GX_COLOR_ID_GREEN_BRIGHT];
	gx_context_color_get (GX_COLOR_ID_GREEN_SUBTLE, &customColor);
	brush->gx_brush_line_color = customColor;
	timeRemaining = g_TimeInSeconds - 1;
	while (timeRemaining > 0)
	{
		angle = (60.0f - (float) timeRemaining) * 6.0f + 90.0f;
		radians = angle * DEGREES_TO_RADIANS;
		vertLength = (float) sin (radians);
		horizLength = (float) cos (radians);
		// Show green tick marks
		horizStart = (GX_VALUE) (TICK_CENTER_PT_HORIZONTAL - (TICK_LENGTH * horizLength));
		horizEnd = (GX_VALUE) (TICK_CENTER_PT_HORIZONTAL - ((TICK_LENGTH - GREEN_TICK_LENGTH) * horizLength));
		verticalStart = (GX_VALUE) (TICK_CENTER_PT_VERTICAL - (TICK_LENGTH * vertLength));
		verticalEnd = (GX_VALUE) (TICK_CENTER_PT_VERTICAL - ((TICK_LENGTH - GREEN_TICK_LENGTH) * vertLength));
		gx_canvas_line_draw (horizStart, verticalStart, horizEnd, verticalEnd);
		--timeRemaining;
	}

	//switch (g_TimeInSeconds)
	//{
	//case 60:
	//	brush->gx_brush_line_color = GX_COLOR_WHITE;
	//	horizStart = TICK_CENTER_PT_HORIZONTAL;
	//	horizEnd = TICK_CENTER_PT_HORIZONTAL;
	//	verticalStart = TICK_CENTER_PT_VERTICAL - TICK_LENGTH;
	//	verticalEnd = TICK_CENTER_PT_VERTICAL - (TICK_LENGTH - WHITE_TICK_LENGTH);
	//	gx_canvas_line_draw (horizStart, verticalStart, horizEnd, verticalEnd);
	//	break;
	//case 59:
	//	brush->gx_brush_line_color = GX_COLOR_BLUE;
	//	horizStart = TICK_CENTER_PT_HORIZONTAL + (TICK_LENGTH - GREEN_TICK_LEGNTH);
	//	horizEnd = TICK_CENTER_PT_HORIZONTAL + TICK_LENGTH;
	//	verticalStart = TICK_CENTER_PT_VERTICAL;
	//	verticalEnd = TICK_CENTER_PT_VERTICAL;
	//	gx_canvas_line_draw (horizStart, verticalStart, horizEnd, verticalEnd);
	//	break;
	//case 58:
	//	brush->gx_brush_line_color = GX_COLOR_RED;
	//	horizStart = TICK_CENTER_PT_HORIZONTAL;
	//	horizEnd = TICK_CENTER_PT_HORIZONTAL;
	//	verticalStart = TICK_CENTER_PT_VERTICAL;
	//	verticalEnd = TICK_CENTER_PT_VERTICAL + TICK_LENGTH;
	//	gx_canvas_line_draw (horizStart, verticalStart, horizEnd, verticalEnd);
	//	break;
	//case 57:
	//	brush->gx_brush_line_color = GX_COLOR_GREEN;
	//	horizStart = TICK_CENTER_PT_HORIZONTAL;
	//	horizEnd = TICK_CENTER_PT_HORIZONTAL - TICK_LENGTH;
	//	verticalStart = TICK_CENTER_PT_VERTICAL;
	//	verticalEnd = TICK_CENTER_PT_VERTICAL;
	//	gx_canvas_line_draw (horizStart, verticalStart, horizEnd, verticalEnd);
	//	break;
	//default:
	//	g_TimeInSeconds = 60;
	//	break;
	//} // end switch
}


//*************************************************************************************
// Function Name: ReadyScreen_Event_Function
//
// Description: This functions handles the Splash screen
//
//*************************************************************************************

UINT Sample_Tick_Window_event_function (GX_WINDOW *window, GX_EVENT *event_ptr)
{
	switch (event_ptr->gx_event_type)
	{
	case GX_EVENT_SHOW:
		g_TimeInSeconds = 60;
		//gx_system_timer_start(window, ONE_SECOND_TIMER, 2, 0);
		break;

	case GX_SIGNAL (PLAY_BTN_ID, GX_EVENT_CLICKED):
		--g_TimeInSeconds;
		if (g_TimeInSeconds < 0)
			g_TimeInSeconds = 60;
        gx_system_dirty_mark(&Sample_Tick_Window);      // This forces the gauge to be updated and redrawn
		break;
	}

    gx_window_event_process(window, event_ptr);

	return (GX_SUCCESS);
}

