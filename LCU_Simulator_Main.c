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

extern void CreateFeatureWidgets (GX_VERTICAL_LIST *list);

//--------------------------------------------------------------------
// Global Variables
TX_BYTE_POOL       memory_pool;
GX_COLOR           scratchpad[SCRATCHPAD_PIXELS];

GX_RECTANGLE g_HiddenRectangle = {0,0,0,0};

//GX_RECTANGLE g_FeatureLocation[] = {
//	{10, 16, 300, 86},
//	{30, 94, 290, 130},
//	{30, 130, 290, 162},
//	{30, 166, 290, 198},
//	{0,0,0,0}};

int g_ChangeScreen_WIP;

GX_WINDOW *g_CalibrationScreen = GX_NULL;
//extern GX_RECTANGLE g_CalibrationPadLocations[];
extern GX_RECTANGLE g_CalibrationPromptLocations[];

int g_CalibrationPadNumber;
int g_CalibrationStepNumber;
int g_PadValue = 35;
int g_DeltaValue;

int g_ClicksActive = FALSE;
int g_PowerUpInIdle = FALSE;
int g_TimeoutValue = 20;
int g_RNet_Active = TRUE;
//MODE_SWITCH_SCHEMA_ENUM g_Mode_Switch_Schema = MODE_SWITCH_PIN5;

//*************************************************************************************
// GLOBAL VARIABLES
//*************************************************************************************

int g_MinimumDriveValue = 20;		// Percentage, Minimum Drive value
char g_MinimuDriveString[8] = "20%";
unsigned char g_HA_Version_Major, g_HA_Version_Minor, g_HA_Version_Build, g_HA_EEPROM_Version = 5;

//MAIN_SCREEN_FEATURE g_MainScreenFeatureInfo[NUM_FEATURES];

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

    /* Initialize GUIX. */
    gx_system_initialize();

    /* Assign memory alloc/free functions. */
    gx_system_memory_allocator_set(memory_allocate, memory_free);

	myError = -1;
    myError = gx_studio_display_configure(LCU_MAINDISPLAY, win32_graphics_driver_setup_24xrgb, 
        LANGUAGE_ENGLISH, LCU_MAINDISPLAY_THEME_1, &root);

    myError = gx_studio_named_widget_create("ReadyScreen", GX_NULL, GX_NULL);
    myError = gx_studio_named_widget_create("Splash_Window", (GX_WIDGET *)root, GX_NULL);

	/* Show the root window.  */
    gx_widget_show(root);

    /* start GUIX thread */
    gx_system_start();
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

