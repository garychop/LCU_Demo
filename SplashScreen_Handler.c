//*****************************************************************************
// Filename: SplashScreen_Handler.c
//
// Date: June 15, 2021
//
// Author: G. Chopcinski, Kg Solutions, LLC
// 
//*****************************************************************************

#include <stdio.h>
#include <stdlib.h>

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

union RGB_color{
	struct {
		byte m_R;
		byte m_G;
		byte m_B;
	};
	struct {
		long m_Long;
	};
}RGB_color_type;

byte g_FileBuffer[4096];

void doNewFileStuff ()
{
#define COLOR_TABLE_SIZE (4096)

	FILE *soureFilePtr, *newFilePtr;
	errno_t errno;
	char errStr[1024];
	//byte colorTable[1024][3];
	union RGB_color colorTable[COLOR_TABLE_SIZE];
	union RGB_color lastPixelColor;
	long startOfBitmap;
	long sizeofBitmap;
	long bitmapBufferPos;
	long totalBytes; 
	size_t itemsRead;
	fpos_t filePos;
	fpos_t bitmapFilePosition;
	int loop;
	int numberOfTableEntries, pixelColorTableIndex;
	BOOL found;
	int heightYdim, widthXdim, pixelCount, lineCount;
	char newFileName[128];
	int readCount;
	int numOfSamePixels;
	//const char fileName[128] = "Blank_60x16.bmp";
	//const char fileName[128] = "MuReva_Logo_Rev7.bmp";
	const char fileName[128] = "MuReva_Logo_Rev2.bmp";
	//const char fileName[128] = "Clock_Image_21_66_152_Rev3.bmp";
	//const char fileName[128] = "Clock_Image_8_65_40_Rev3.bmp";
	//const char fileName[128] = "vertical_color.bmp";
	//const char fileName[128] = "color.bmp";

	if ((errno = fopen_s(&soureFilePtr, fileName, "rb"))) 
	{
		strerror_s (errStr, sizeof (errStr), errno);
		printf("Error! opening file");

		// If file pointer will return NULL
		// Program will exit.
		exit(1);
	}

	// First, let's read the Bitmap File information.
	itemsRead = fread_s (g_FileBuffer, sizeof(g_FileBuffer), 1, 54, soureFilePtr);
	if (itemsRead < 54)
		exit(1);	// Error.

	if (fgetpos (soureFilePtr, &bitmapFilePosition))
		exit(1);

	widthXdim = g_FileBuffer[18] + (0x100 * g_FileBuffer[19]);
	heightYdim = g_FileBuffer[22] + (0x100 * g_FileBuffer[23]);

	// So how big is this bitmap.
	sizeofBitmap = g_FileBuffer[2] + (0x100 * g_FileBuffer[3]) + (0x10000 * g_FileBuffer[4]);

	// Determine where the actual Bitmap is located in the buffer.
	startOfBitmap = g_FileBuffer[10] + (g_FileBuffer[11]);

	// Fill the color table with colors.
	numberOfTableEntries = 0;
	lineCount = 0;
	totalBytes = 0;
	readCount = widthXdim * 3;
	if ((readCount % 4) != 0)
		readCount += 4 - ((widthXdim * 3) % 4);		// Each Line (scan) is a multiple of 4, adjust if necessary.

	for (lineCount = 0; lineCount < heightYdim; ++lineCount)
	{
		//fgetpos (soureFilePtr, &filePos);
		itemsRead = fread_s (g_FileBuffer, sizeof(g_FileBuffer), 1, readCount, soureFilePtr);	// Read x number of pixels, each pixel/color is 3 bytes.
		fgetpos (soureFilePtr, &filePos);
		bitmapBufferPos = 0;
		if (g_FileBuffer[0x84] != 0x20)
			found = TRUE;
		if (numberOfTableEntries == 0)
		{
			// Let's use the first color we encounter as valid.
			colorTable[numberOfTableEntries].m_Long = 0x0l;
			colorTable[numberOfTableEntries].m_R = g_FileBuffer[bitmapBufferPos];
			colorTable[numberOfTableEntries].m_G = g_FileBuffer[bitmapBufferPos+1];
			colorTable[numberOfTableEntries].m_B = g_FileBuffer[bitmapBufferPos+2];
			++numberOfTableEntries;
			//bitmapBufferPos += 3;	// Point to next pixel color.
		}

		// Ok look for an existing color.
		for (pixelCount = 0; pixelCount < widthXdim; ++pixelCount)
		{
			found = FALSE;
			// Find color in color table
			for (loop = 0; loop < numberOfTableEntries; ++loop)
			{
				if ((g_FileBuffer[bitmapBufferPos] == colorTable[loop].m_R) && (g_FileBuffer[bitmapBufferPos+1] == colorTable[loop].m_G) && (g_FileBuffer[bitmapBufferPos+2] == colorTable[loop].m_B))
				{
					found = TRUE;
					break;
				}
			}
			if (!found)
			{
				colorTable[numberOfTableEntries].m_Long = 0x0l;
				colorTable[numberOfTableEntries].m_R = g_FileBuffer[bitmapBufferPos];
				colorTable[numberOfTableEntries].m_G = g_FileBuffer[bitmapBufferPos+1];
				colorTable[numberOfTableEntries].m_B = g_FileBuffer[bitmapBufferPos+2];
				++numberOfTableEntries;
				if (numberOfTableEntries > (COLOR_TABLE_SIZE-2))
					numberOfTableEntries = COLOR_TABLE_SIZE-2;
			}
			bitmapBufferPos += 3;	// Point to next pixel color.
		}
	}
//	fclose (soureFilePtr);

	// Create the new file.
	sprintf_s (newFileName, sizeof (newFileName), "%s.TXT", fileName);
	if ((errno = fopen_s(&newFilePtr, newFileName, "w"))) 
	{
		exit(1);
	}

	// Store the Color Table into the File.
	fprintf_s (newFilePtr, "Color Table Follows\n\n");
	for (loop = 0; loop < numberOfTableEntries; ++loop)
	{
		fprintf_s (newFilePtr, "[%02d] 0x%04x\n", loop, colorTable[loop].m_Long);
	}
	fprintf_s (newFilePtr, "\n");

	//fclose (soureFilePtr); fclose (newFilePtr);

	// Put Source File at beginning of Bitmap Data (pixels)
	if (errno = fsetpos (soureFilePtr, &bitmapFilePosition))
	{
		strerror_s (errStr, sizeof (errStr), errno);
		exit(1);
	}

	// For each line
	//		find the ColorTableIndex for the first pixel/color, maintain in pixelColorTableIndex
	//		
	totalBytes = 0;
	numOfSamePixels = 0;
	for (lineCount = 0; lineCount < heightYdim; ++lineCount)
	{
		itemsRead = fread_s (g_FileBuffer, sizeof(g_FileBuffer), 1, readCount, soureFilePtr);	// Read x number of pixels, each pixel/color is 3 bytes.
		// Locate the Color Table Index of the first Pixel.
		pixelColorTableIndex = 0xff;
		found = FALSE;
		for (loop = 0; loop < numberOfTableEntries; ++loop)
		{
			if ((colorTable[loop].m_R == g_FileBuffer[0]) && (colorTable[loop].m_G == g_FileBuffer[1]) && (colorTable[loop].m_B == g_FileBuffer[2]))
			{
				pixelColorTableIndex = loop;
				found = TRUE;
				break;
			}
		}
		if (!found)	// Didn't find color
			exit(1);

		lastPixelColor = colorTable[loop];
//		lastPixelColor.m_R = g_FileBuffer[0];
//		lastPixelColor.m_G = g_FileBuffer[1];
//		lastPixelColor.m_B = g_FileBuffer[2];
		numOfSamePixels = 1;
		bitmapBufferPos = 3;
		for (pixelCount = 1; pixelCount < widthXdim; ++pixelCount)
		{
			// Check to see if the next pixel is the same as the last one.
			// If so, increment the "Same Pixel Counter" and look at the next pixel in this scan/record
			if ((lastPixelColor.m_R == g_FileBuffer[bitmapBufferPos]) && (lastPixelColor.m_G == g_FileBuffer[bitmapBufferPos+1]) && (lastPixelColor.m_B == g_FileBuffer[bitmapBufferPos+2]))
			{
				++numOfSamePixels;
				bitmapBufferPos += 3;
			}
			else
			{	// It must be a different pixel color.
				// Store the Pixel Color Table Index and number of contigious same pixel colors.
				fprintf_s (newFilePtr, "Table Index = %d, Number of Pixels = %d\n", pixelColorTableIndex, numOfSamePixels);
				totalBytes += 4;
				// Setup for the next Pixel Color
				lastPixelColor.m_R = g_FileBuffer[bitmapBufferPos];
				lastPixelColor.m_G = g_FileBuffer[bitmapBufferPos+1];
				lastPixelColor.m_B = g_FileBuffer[bitmapBufferPos+2];
				numOfSamePixels = 1;
				found = FALSE;
				for (loop = 0; loop < numberOfTableEntries; ++loop)
				{
					if (lastPixelColor.m_Long == colorTable[loop].m_Long)
					{
						pixelColorTableIndex = loop;
						found = TRUE;
						break;
					}
				}
				if (!found)	// Didn't find color
					exit(1);
				bitmapBufferPos += 3;	// Point to next pixel
			}
		}
		fprintf_s (newFilePtr, "Table Index = %d, Number of Pixels = %d\n", pixelColorTableIndex, numOfSamePixels);
		fprintf_s (newFilePtr, "------------------- Line # = %d\n", lineCount);
		totalBytes += 4;
	}

	fprintf_s (newFilePtr, "\n\nTotal Number of Bytes = %d\n", totalBytes);

	fclose (soureFilePtr);
	fclose (newFilePtr);

	exit (0);
}

void doFileStuff ()
{
	FILE* fptr, *newFilePtr;
	errno_t errno;
	unsigned char fileBuffer[65535];
	char errStr[256];
	long startOfBitmap;
	long sizeofBitmap;
	long bitmapBufferPos;
	long totalBytes; 
	size_t itemsRead;
	fpos_t filePos;
	unsigned char color[3];
	int colorCounter;
	int heightYdim, widthXdim, pixelCount, lineCount;
	char newFileName[128];
	//const char fileName[128] = "Blank_60x16.bmp";
	//const char fileName[128] = "MuReva_Logo_Rev2.bmp";
	const char fileName[128] = "Clock_Image_21_66_152_Rev3.bmp";

	if ((errno = fopen_s(&fptr, fileName, "rb"))) 
	{
		strerror_s (errStr, sizeof (errStr), errno);
		printf("Error! opening file");

		// If file pointer will return NULL
		// Program will exit.
		exit(1);
	}

	// First, let's read the Bitmap File information.
	itemsRead = fread_s (fileBuffer, sizeof(fileBuffer), 1, 54, fptr);
	if (itemsRead < 54)
		exit(1);	// Error.

	if (fgetpos (fptr, &filePos))
		exit(1);

	widthXdim = fileBuffer[18] + (0x100 * fileBuffer[19]);
	heightYdim = fileBuffer[22] + (0x100 * fileBuffer[23]);

	// So how big is this bitmap.
	sizeofBitmap = fileBuffer[2] + (0x100 * fileBuffer[3]) + (0x10000 * fileBuffer[4]);

	// Determine where the actual Bitmap is located in the buffer.
	startOfBitmap = fileBuffer[10] + (fileBuffer[11]);

	// Create the new file.
	sprintf_s (newFileName, sizeof (newFileName), "%s.TXT", fileName);
	if ((errno = fopen_s(&newFilePtr, newFileName, "w"))) 
	{
		exit(1);
	}

	colorCounter = 0;
	lineCount = 0;
	totalBytes = 0;
	for (lineCount = 0; lineCount < heightYdim; ++lineCount)
	{
		itemsRead = fread_s (fileBuffer, sizeof(fileBuffer), 1, widthXdim * 3, fptr);	// Read x number of pixels, each pixel/color is 3 bytes.
		if (fgetpos (fptr, &filePos))
			exit(1);
		bitmapBufferPos = 0;
		pixelCount = 1;			// Start of new horizontal line
		// Get color in 3 bytes
		color[0] = fileBuffer[bitmapBufferPos++];
		color[1] = fileBuffer[bitmapBufferPos++];
		color[2] = fileBuffer[bitmapBufferPos++];
		colorCounter = 1;
		for (; pixelCount < widthXdim; ++pixelCount)
		{
			if ((color[0] == fileBuffer[bitmapBufferPos]) && (color[1] == fileBuffer[bitmapBufferPos+1]) && (color[2] == fileBuffer[bitmapBufferPos+2]))
			{
				++colorCounter;
				bitmapBufferPos += 3;	// Point to next pixel color.
			}
			else // Must have changed colors.
			{
				// Process color and number of pixel with same color.
				fprintf_s (newFilePtr, "0x%02x%02x%02x 0x%04x (%d)\n", color[0], color[1], color[2], colorCounter, colorCounter);
				totalBytes += 5;
				colorCounter = 1;
				// Store new color
				color[0] = fileBuffer[bitmapBufferPos++];
				color[1] = fileBuffer[bitmapBufferPos++];
				color[2] = fileBuffer[bitmapBufferPos++];
			}
		}
		if (colorCounter > 0)
		{
			fprintf_s (newFilePtr, "0x%02x%02x%02x 0x%04x (%2d)\n", color[0], color[1], color[2], colorCounter, colorCounter);
			colorCounter = 0;
			totalBytes += 5;
		}
	}

	fprintf (newFilePtr, "\n\nTotal Number of Bytes = %d\n", totalBytes);

	fclose (fptr);
	fclose (newFilePtr);

	exit (0);
}

//*************************************************************************************
// Function Name: SplashScreen_Event_Function
//
// Description: This functions handles the Splash screen
//
//*************************************************************************************

UINT SplashScreen_Event_Function (GX_WINDOW *window, GX_EVENT *event_ptr)
{
	//doFileStuff();
	doNewFileStuff();

    switch (event_ptr->gx_event_type)
	{
	case GX_EVENT_SHOW:
		g_LastAttachedSerialNumber = 0;		// Reset the "last used mouthpiece serial number."
		gx_system_timer_start(window, SPLASH_STARTUP_TIMER_ID, 100, 0);
		break;

    case GX_EVENT_TIMER:
        if (event_ptr->gx_event_payload.gx_event_timer_id == SPLASH_STARTUP_TIMER_ID)
		{
	        screen_toggle((GX_WINDOW *)&InsertMouthpiece_Screen, window);
		}

		// Read Bitmap file
		break;
	}

    gx_window_event_process(window, event_ptr);

	return (GX_SUCCESS);
}


