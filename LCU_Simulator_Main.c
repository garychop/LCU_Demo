//***********************************************************************************
// Project: LCU Simulator
//
// Filename: LCU_Simulator_Main.c
//
// Description: This project is a Windows based project that is used to convey the
//		MuReva Display Unit.
//
// Author: G. Chopcinski
//
// Creation Date: 6/15/21
//
//***********************************************************************************

#define GX_ARC_DRAWING_SUPPORT

#include "time.h"			// Windows supported timed functions.
#include "LCU_Simulator_Main.h"

#define SCREEN_STACK_SIZE  5
#define SCRATCHPAD_PIXELS  (LCU_MAINDISPLAY_X_RESOLUTION * LCU_MAINDISPLAY_Y_RESOLUTION * 2)

//--------------------------------------------------------------------
// Define prototypes.
VOID start_guix(VOID);

//--------------------------------------------------------------------
// Global Variables
TX_BYTE_POOL       memory_pool;
GX_COLOR           scratchpad[SCRATCHPAD_PIXELS];

GX_BOOL g_LimitSwitchClosed = FALSE;
GX_BOOL g_RingOn = FALSE;
long g_LastAttachedSerialNumber = 0;

MOUTHPIECE_DATABASE_STRUCT g_Mouthpiece_DB[MOUTHPIECE_DB_SIZE];

//*************************************************************************************
// GLOBAL VARIABLES
//*************************************************************************************

//*************************************************************************************
// Provided by GUIX to support Windows environment.
//*************************************************************************************

VOID *memory_allocate(ULONG size)
{
    VOID *memptr;

    if (tx_byte_allocate(&memory_pool, &memptr, size, TX_NO_WAIT) == TX_SUCCESS)
    {
        return memptr;
    }
    return NULL;
}

//*************************************************************************************
// Provided by GUIX to support Windows environment.
//*************************************************************************************
void memory_free(VOID *mem)
{
    tx_byte_release(mem);
}

//*************************************************************************************
// Provided by GUIX to support Windows environment.
//*************************************************************************************
int main(int argc, char ** argv)
{
    tx_kernel_enter();
    return(0);
}

//*************************************************************************************
// Provided by GUIX to support Windows environment.
//*************************************************************************************
VOID tx_application_define(void *first_unused_memory)
{
    /* create byte pool*/
    tx_byte_pool_create(&memory_pool, "scratchpad", scratchpad,
        SCRATCHPAD_PIXELS * sizeof(GX_COLOR));

    start_guix();
}

//*************************************************************************************
// Provided by GUIX to support Windows environment.
//*************************************************************************************
VOID  start_guix(VOID)
{
	UINT myError;
	int i;

    /* Initialize GUIX. */
    gx_system_initialize();

    /* Assign memory alloc/free functions. */
    gx_system_memory_allocator_set(memory_allocate, memory_free);

	myError = -1;
    myError = gx_studio_display_configure(LCU_MAINDISPLAY, win32_graphics_driver_setup_24xrgb, 
        LANGUAGE_ENGLISH, LCU_MAINDISPLAY_THEME_1, &root);

	// Clear out the Mouthpiece Database
	for (i = 0; i < MOUTHPIECE_DB_SIZE; ++i)
	{
		g_Mouthpiece_DB[i].m_TherapyStatus = THERAPY_IDLE;
		g_Mouthpiece_DB[i].m_RemainingTherapyTime = 0;
		g_Mouthpiece_DB[i].m_SerialNumber = 0L;
		g_Mouthpiece_DB[i].m_Attached = FALSE;
	}

	//g_TherapyInProcess = THERAPY_IDLE;

    myError = gx_studio_named_widget_create("ReadyScreen", GX_NULL, GX_NULL);
    myError = gx_studio_named_widget_create("Reading_Screen", GX_NULL, GX_NULL);
    myError = gx_studio_named_widget_create("Expired_Error_Screen", GX_NULL, GX_NULL);
    myError = gx_studio_named_widget_create("Error_Screen", GX_NULL, GX_NULL);
    myError = gx_studio_named_widget_create("ReadingError_Screen", GX_NULL, GX_NULL);
    myError = gx_studio_named_widget_create("PressToStart_Screen", GX_NULL, GX_NULL);
    myError = gx_studio_named_widget_create("PressToResume_Screen", GX_NULL, GX_NULL);
    myError = gx_studio_named_widget_create("Therapy_Screen", GX_NULL, GX_NULL);
    myError = gx_studio_named_widget_create("TherapyComplete_Screen", GX_NULL, GX_NULL);
    myError = gx_studio_named_widget_create("MouthpieceDetached_Screen", GX_NULL, GX_NULL);
	myError = gx_studio_named_widget_create("DailyLimitReached_Error_Screen", GX_NULL, GX_NULL);
	myError = gx_studio_named_widget_create("SerialNumber_Screen", GX_NULL, GX_NULL);
    myError = gx_studio_named_widget_create("InsertMouthpiece_Screen", GX_NULL, GX_NULL);
    myError = gx_studio_named_widget_create("Sample_Tick_Window", GX_NULL, GX_NULL);
    myError = gx_studio_named_widget_create("Standby_Screen", GX_NULL, GX_NULL);
    myError = gx_studio_named_widget_create("Splash_Window", (GX_WIDGET *)root, GX_NULL);

	/* Show the root window.  */
    gx_widget_show(root);

    /* start GUIX thread */
    gx_system_start();
}

//*************************************************************************************

void Enable_EEPROM_Buttons (PRIMARYTEMPLATE_CONTROL_BLOCK* templateBase, GX_BOOL enable_OK, GX_BOOL enable_BAD, GX_BOOL enable_EXPIRED, GX_BOOL enable_12HOUR, GX_BOOL enable_ATTACH)
{
	ULONG widgetStyle;

	gx_widget_style_get ((GX_WIDGET*) &templateBase->PrimaryTemplate_EEPROM_OK_Button, &widgetStyle);

	// Enable/Disable OK button
	if (enable_OK)
		widgetStyle |= GX_STYLE_ENABLED;
	else
		widgetStyle &= (~GX_STYLE_ENABLED);
	gx_widget_style_set ((GX_WIDGET*) &templateBase->PrimaryTemplate_EEPROM_OK_Button, widgetStyle);

	// Enable/Disable BAD button
	if (enable_BAD)
		widgetStyle |= GX_STYLE_ENABLED;
	else
		widgetStyle &= (~GX_STYLE_ENABLED);
	gx_widget_style_set ((GX_WIDGET*) &templateBase->PrimaryTemplate_EEPROM_Fail_Button, widgetStyle);
	
	// Enable/Disable EXPIRED button
	if (enable_EXPIRED)
		widgetStyle |= GX_STYLE_ENABLED;
	else
		widgetStyle &= (~GX_STYLE_ENABLED);
	gx_widget_style_set ((GX_WIDGET*) &templateBase->PrimaryTemplate_EEPROM_Expired_Button, widgetStyle);

	// Enable/Disable 12 HOUR button
	if (enable_12HOUR)
		widgetStyle |= GX_STYLE_ENABLED;
	else
		widgetStyle &= (~GX_STYLE_ENABLED);
	gx_widget_style_set ((GX_WIDGET*) &templateBase->PrimaryTemplate_EEPROM_12HOUR_Button, widgetStyle);

	// Enable/Disable ATTACH/DETACH button
	if (enable_ATTACH)
		widgetStyle |= GX_STYLE_ENABLED;
	else
		widgetStyle &= (~GX_STYLE_ENABLED);
	gx_widget_style_set ((GX_WIDGET*) &templateBase->PrimaryTemplate_LimitSwitch_Button, widgetStyle);
}

//******************************************************************************************

VOID Enable_Limit_Switch (GX_WIDGET* limitSwitchButton, GX_BOOL enable)
{
	ULONG widgetStyle;

	gx_widget_style_get ((GX_WIDGET*) limitSwitchButton, &widgetStyle);
	widgetStyle |= GX_STYLE_ENABLED;
	gx_widget_style_set ((GX_WIDGET*) limitSwitchButton, widgetStyle);
	gx_text_button_text_id_set ((GX_TEXT_BUTTON*) limitSwitchButton, GX_STRING_ID_ATTACH);
}

//*************************************************************************************
// This function moves the Information Window so the text is centered vertically.
// The widget takes care of horizontal centering.
//*************************************************************************************

void DisplayInformation_InBox (GX_MULTI_LINE_TEXT_VIEW *textViewWidget, GX_CHAR *myString, INT numberOfLines, GX_RESOURCE_ID myColor)
{
	GX_RECTANGLE rectangle;

	gx_widget_show (textViewWidget);
	// font height is 20 pixels.
	// full height is 100 pixels which is 5 lines.
	rectangle.gx_rectangle_left = textViewWidget->gx_widget_size.gx_rectangle_left;
	rectangle.gx_rectangle_right = textViewWidget->gx_widget_size.gx_rectangle_right;

	gx_multi_line_text_view_text_color_set (textViewWidget, myColor, myColor, myColor);
	rectangle.gx_rectangle_top = 108 + (10*(5 - numberOfLines));
	rectangle.gx_rectangle_bottom = rectangle.gx_rectangle_top + (20 * numberOfLines);
	gx_widget_show (textViewWidget);
	gx_widget_resize (textViewWidget, &rectangle);
	gx_multi_line_text_view_text_set (textViewWidget, myString);
}

//*************************************************************************************
void DisplayInstruction_InBox (GX_MULTI_LINE_TEXT_VIEW *textViewWidget, GX_CHAR *myString, INT numberOfLines, GX_RESOURCE_ID myColor)
{
	GX_RECTANGLE rectangle;

	gx_widget_show (textViewWidget);
	// font height is 20 pixels.
	// full height is 100 pixels which is 5 lines.
	rectangle.gx_rectangle_left = textViewWidget->gx_widget_size.gx_rectangle_left;
	rectangle.gx_rectangle_right = textViewWidget->gx_widget_size.gx_rectangle_right;

	gx_multi_line_text_view_text_color_set (textViewWidget, myColor, myColor, myColor);
	rectangle.gx_rectangle_top = 108 + (11*(5 - numberOfLines));
	rectangle.gx_rectangle_bottom = rectangle.gx_rectangle_top + (20 * numberOfLines);
	gx_widget_resize (textViewWidget, &rectangle);
	gx_multi_line_text_view_text_set (textViewWidget, myString);
}

//******************************************************************************************
// Detach one window and attach anothe window to root.
//******************************************************************************************

VOID screen_toggle(GX_WINDOW *new_win, GX_WINDOW *old_win)
{
	int status;
    if (!new_win->gx_widget_parent)
    {
        status = gx_widget_attach(root, (GX_WIDGET *)new_win);
    }
    else
    {
        status = gx_widget_show((GX_WIDGET *)new_win);
    }
    status = gx_widget_detach((GX_WIDGET *)old_win);
}

////******************************************************************************************
//VOID screen_switch(GX_WIDGET *parent, GX_WIDGET *new_screen)
//{
//    gx_widget_detach(current_screen);
//    gx_widget_attach(parent, new_screen);
//    current_screen = new_screen;
//}
//*************************************************************************************
// Function Name: Template_event_function
//
// Description: This handles any messages sent to the template window
//
//*************************************************************************************

UINT Template_event_function (GX_WINDOW *window, GX_EVENT *event_ptr)
{
	UINT myErr = -1;

	switch (event_ptr->gx_event_type)
	{
    case GX_EVENT_TIMER:
		break;
	case GX_EVENT_PEN_UP:
		break;
	} // end switch

    myErr = gx_window_event_process(window, event_ptr);

	return myErr;
}

