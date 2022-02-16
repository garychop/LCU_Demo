//*****************************************************************************
// Filename: SplashScreen_Handler.c
//
// Date: June 15, 2021
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

//*************************************************************************************
// Function Name: SplashScreen_Event_Function
//
// Description: This functions handles the Splash screen
//
//*************************************************************************************

UINT SplashScreen_Event_Function (GX_WINDOW *window, GX_EVENT *event_ptr)
{
	switch (event_ptr->gx_event_type)
	{
	case GX_EVENT_SHOW:
		gx_prompt_text_id_set (&Splash_Window.base.PrimaryTemplate_LimitSwitchStatus_prompt, GX_ID_NONE);	// none
		gx_system_timer_start(window, SPLASH_STARTUP_TIMER_ID, 100, 0);
		break;

    case GX_EVENT_TIMER:
        if (event_ptr->gx_event_payload.gx_event_timer_id == SPLASH_STARTUP_TIMER_ID)
		{
	        screen_toggle((GX_WINDOW *)&ReadyScreen, window);
		}
		break;
	}

    gx_window_event_process(window, event_ptr);

	return (GX_SUCCESS);
}

