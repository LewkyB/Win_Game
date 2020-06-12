#pragma once // only use this header file one time


// this controls the name of the game everywhere
// white space matters? why do I need to tab this?
#define GAME_NAME	"Win_Game"

#define GAME_RES_WIDTH	384
#define GAME_RES_HEIGHT 216
#define GAME_BPP		32 //bits per pixel
#define GAME_DRAWING_AREA_SIZE (GAME_RES_WIDTH * GAME_RES_HEIGHT * (GAME_BPP / 8)) // 

typedef struct GAMEBITMAP
{
	// contains members for height, width, and bits per pixel (BPP)
	BITMAPINFO BitmapInfo;

	void* Memory;
} GAMEBITMAP;

// declare functions
LRESULT CALLBACK MainWindowProc(_In_ HWND WindowHandle, _In_ UINT Message, _In_ WPARAM WParam, _In_ LPARAM LParam);

DWORD CreateMainGameWindow(void);

// used to keep more than one instance of the game from running
BOOL GameIsAlreadyRunning(void);

void ProcessPlayerInput(void);

void RenderFrameGraphics(void);
