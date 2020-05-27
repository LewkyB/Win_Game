#pragma once // only use this header file one time


// this controls the name of the game everywhere
// white space matters? why do I need to tab this?
#define GAME_NAME	"Win_Game"

// declare functions
LRESULT CALLBACK MainWindowProc(_In_ HWND WindowHandle, _In_ UINT Message, _In_ WPARAM WParam, _In_ LPARAM LParam);

DWORD CreateMainGameWindow(void);

// used to keep more than one instance of the game from running
BOOL GameIsAlreadyRunning(void);