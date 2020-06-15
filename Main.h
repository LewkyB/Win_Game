#pragma once // only use this header file one time


// this controls the name of the game everywhere
// white space matters? why do I need to tab this?
#define GAME_NAME	"Win_Game"

#define GAME_RES_WIDTH	384
#define GAME_RES_HEIGHT 240
#define GAME_BPP		32 //bits per pixel
#define GAME_DRAWING_AREA_MEMORY_SIZE (GAME_RES_WIDTH * GAME_RES_HEIGHT * (GAME_BPP / 8))

#define CALCULATE_AVG_FPS_EVERY_X_FRAMES 100 // calculate FPS every 100 frames

typedef struct GAMEBITMAP
{
	// contains members for height, width, and bits per pixel (BPP)
	BITMAPINFO BitmapInfo;

	void* Memory;
} GAMEBITMAP;

typedef struct PIXEL32
{
	uint8_t Blue;
	uint8_t Green;
	uint8_t Red;
	uint8_t Alpha;
} PIXEL32;

// structure for holding game performance data
typedef struct GAMEPERFDATA
{
	uint64_t TotalFramesRendered;

	uint32_t RawFramesPerSecondAverage; // frames that do not get reduced
	uint32_t CookedFramesPerSecondAverage; // frames that get reduced to 60 fps

	// timing variables
	// LARGE_INTEGER is a 64 bit int that is a union between high and low bits
	LARGE_INTEGER PerfFrequency;
	LARGE_INTEGER FrameStart;
	LARGE_INTEGER FrameEnd;
	LARGE_INTEGER ElapsedMicrosecondsPerFrame;


	MONITORINFO MonitorInfo; // used to get more details about the display

	// Manifest file I/O has been set to "Per Monitor High DPI Aware"
	int32_t MonitorWidth;
	int32_t MonitorHeight;

} GAMEPERFDATA;


// declare functions
LRESULT CALLBACK MainWindowProc(_In_ HWND WindowHandle, _In_ UINT Message, _In_ WPARAM WParam, _In_ LPARAM LParam);

DWORD CreateMainGameWindow(void);

// used to keep more than one instance of the game from running
BOOL GameIsAlreadyRunning(void);

void ProcessPlayerInput(void);

void RenderFrameGraphics(void);
