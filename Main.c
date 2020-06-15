/*
	After EP0002:
		
		- added the WindowClass which will allow for the creation of our own window from scratch
			- updated and expanded out variable names

		- created a window for our game by copy and pasting code from winprog.org
			- updated and expanded out variable names
		
		- registered the window that we made with the windows operating system

	After EP0003

		- changed pragma warnings from 0 to 3 to make sure we see buffer overflows to avoid
		  the program from being exploited

			- memcpy() doesn't have any protections when it comes to copying a larger buffer to
			  a smaller buffer and vice versa. This is bad and can be exploited?

				- use memcpy_s() to avoid buffer errors

		- SAL 
			- source code annoation language
			- give compiler tips for how to handle arguments in functions

*/

#include <stdio.h>
#include <stdint.h>

// telling compiler to behave in a certain way
// adjust warning levels within source code without
// having to do anything else
// purpose: turns off all but 3 warnings that come from <windows.h>
#pragma warning(push, 3) 
#include <windows.h>
#pragma warning(pop)

// header files
#include "Main.h"

// global variables are guaranteed to be initialized to zero
BOOL gGameIsRunning; // g prefix denotes global
HANDLE gGameWindow;
GAMEBITMAP gBackBuffer;

GAMEPERFDATA gPerformanceData;

// Even though we don't use 32 bit computers anymore we still use 
// the name win32. The curse of having a good name. Despite it actually
// being 64 bit we still refer to the API as win32. 
INT __stdcall WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, PSTR CommandLine, INT CmdShow)
{
	// disable 	warnings? need to clarify what this block does
	UNREFERENCED_PARAMETER(Instance);
	UNREFERENCED_PARAMETER(PreviousInstance);
	UNREFERENCED_PARAMETER(CommandLine);
	UNREFERENCED_PARAMETER(CmdShow);

	if (GameIsAlreadyRunning() == TRUE)
	{
		MessageBoxA(NULL, "Game already running!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		goto Exit;
	}

	if (CreateMainGameWindow(NULL) != ERROR_SUCCESS)
		goto Exit;

	// retrieves frequency of performance counter. that frequency is set at
	// boot time so it only needs to be queried once on app initialization
	QueryPerformanceFrequency(&gPerformanceData.PerfFrequency);

	// setup drawing surface (back buffer?)
	gBackBuffer.BitmapInfo.bmiHeader.biSize = sizeof(gBackBuffer.BitmapInfo.bmiHeader);
	gBackBuffer.BitmapInfo.bmiHeader.biWidth = GAME_RES_WIDTH;
	gBackBuffer.BitmapInfo.bmiHeader.biHeight = GAME_RES_HEIGHT;
	gBackBuffer.BitmapInfo.bmiHeader.biBitCount = GAME_BPP;
	gBackBuffer.BitmapInfo.bmiHeader.biCompression = BI_RGB;
	gBackBuffer.BitmapInfo.bmiHeader.biPlanes = 1;

	// VirtualAlloc() reserves memory in pretty large chunks, use it when allocating for large amounts
	// HeapAlloc() should be used when reserving small chunks of memory
	
	// Allocate game memory
	gBackBuffer.Memory = VirtualAlloc(NULL, GAME_DRAWING_AREA_MEMORY_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	// Test if memory allocation was successful
	if (gBackBuffer.Memory == NULL)
	{
		MessageBox(NULL, "Failed to allocate memory for drawing surface!", "Error!", MB_ICONEXCLAMATION | MB_OK);

		return 0;
	}

	memset(gBackBuffer.Memory, 0x7F, GAME_DRAWING_AREA_MEMORY_SIZE);

	MSG Message = { 0 }; // init all messages to 0 to avoid any message errors

	// 
	gGameIsRunning = TRUE; 

	while (gGameIsRunning == TRUE)
	{
		// when you want to measure timing this needs to go at the start of whatever 
		// you intend to time
		QueryPerformanceCounter(&gPerformanceData.FrameStart);

		// PeekMessageA() takes a look in the message queue
		// and if nothing is in the queue it will continue to run.
		//
		// PM_REMOVE will remove the message from queue and then
		// the message will be dispatched.
		while (PeekMessageA(&Message, gGameWindow, 0, 0, PM_REMOVE))
		{
			DispatchMessageA(&Message);
		}

		// Every 16.67 ms this function is called to achieve 60 FPS.
		ProcessPlayerInput();

		RenderFrameGraphics();

		// whenever you start measuring time you also need to end measuring time
		QueryPerformanceCounter(&gPerformanceData.FrameEnd);

		// calculate time
		gPerformanceData.ElapsedMicrosecondsPerFrame.QuadPart = gPerformanceData.FrameEnd.QuadPart - gPerformanceData.FrameStart.QuadPart;
		gPerformanceData.ElapsedMicrosecondsPerFrame.QuadPart *= 1000000;
		gPerformanceData.ElapsedMicrosecondsPerFrame.QuadPart /= gPerformanceData.PerfFrequency.QuadPart;

		// Allows other threads to run, but lacks stability. Setting
		// Sleep() to 1 significantly decreases CPU usage.
		// does not actually sleep for 1ms. the windows system clock is approx. 15ms cycles
		// so it will go to sleep and windows will do stuff, come back and say you've 
		// been asleep at least 1ms so you can wake back up
		Sleep(1);

		gPerformanceData.TotalFramesRendered++;

		if (gPerformanceData.TotalFramesRendered % CALCULATE_AVG_FPS_EVERY_X_FRAMES == 0)
		{
			char str[64] = { 0 }; // temporary string
			
			// _TRUNCATE means this function will only copy as much data as will fit in the string
			// leaving the destination buffer null-terminated
			_snprintf_s(str, _countof(str), _TRUNCATE, "Elapsed microseconds: %lli\n", gPerformanceData.ElapsedMicrosecondsPerFrame.QuadPart);

			OutputDebugStringA();
		}


	}

	while (GetMessageA(&Message, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Message);

		// each time a message comes it the message will be dispatched
		// to the WindowClass defined window procedure
		DispatchMessageA(&Message);
	}

Exit:
	return 0;
}

// processes all messages received by window
LRESULT CALLBACK MainWindowProc(
	_In_ HWND WindowHandle,			// handle to window
	_In_ UINT Message,				// message identifier
	_In_ WPARAM WParam,				// first message parameter
	_In_ LPARAM LParam)    // second message parameter
{

	LRESULT Result = 0;

	// all processing handles in the switch block
	switch (Message)
	{
		case WM_CLOSE:
		{
			// Put another window message into queue that is 0.
			//
			// Purpose: To send 0 to while loop GetMessageA to end the loop.
			PostQuitMessage(0);

			gGameIsRunning = FALSE;

			break;
		}

		default:
		{
			// when message comes in pass it along to next procedure
			// this handles all the default behaviors like minimize or close
			Result = DefWindowProcA(WindowHandle, Message, WParam, LParam);
		}
	}
	return Result;
}

// _In_ is SAL that indicates that the parameter is an input
DWORD CreateMainGameWindow(void)
{
	DWORD Result = ERROR_SUCCESS;

	// when programming in windows, everything is a window
	WNDCLASSEXA WindowClass = { 0 };

	
	// removed to fix messaging issue, replaced with gGameWindow
	//HWND WindowHandle = 0;

	// 0xccccccccccccccc is the op code for init(3) which kicks the line into debug mode
	WindowClass.cbSize = sizeof(WNDCLASSEXA);

	WindowClass.style = 0;

	WindowClass.lpfnWndProc = MainWindowProc;

	WindowClass.cbClsExtra = 0;

	WindowClass.cbWndExtra = 0;

	// using GetModuleHandleA(NULL) instead of HINSTANCE Instance because things 
	// can get hairy if you store this in a .dll
	WindowClass.hInstance = GetModuleHandleA(NULL);

	// IDI_APPLICATION is a way to customize an application icon?
	WindowClass.hIcon = LoadIconA(NULL, IDI_APPLICATION);

	WindowClass.hIconSm = LoadIconA(NULL, IDI_APPLICATION);

	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	// Using purple color for debugging purposes
	WindowClass.hbrBackground = CreateSolidBrush(RGB(255, 0, 255));

	WindowClass.lpszMenuName = NULL; // used for drop down menus in window

	WindowClass.lpszClassName = GAME_NAME "_WINDOWCLASS";
	
	// REMOVED AFTER CHANGING MANIFEST FILE DPI SETTINGS
	//
	// ignore virtual resolution and get display's actual resolution
	//SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	
	// register window class with windows
	if (RegisterClassExA(&WindowClass) == 0) // look up the doc of the function before deciding boolean results
	{
		Result = GetLastError();

		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

		goto Exit;
	}

	gGameWindow = CreateWindowEx(
		0, // disables extended window style
		WindowClass.lpszClassName,
		"Gimme Dog Pics", // title of the window
		WS_OVERLAPPEDWINDOW | // window is an overlapped window meaning has title bar, border, and client area
		WS_VISIBLE, // show window without using ShowWindow()
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		640,
		480,
		NULL,
		NULL,
		GetModuleHandleA(NULL),
		NULL
	);

	if (gGameWindow == NULL)
	{
		// need to fix this by adding an error code
		Result = GetLastError();

		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

		goto Exit;
	}
	
	gPerformanceData.MonitorInfo.cbSize = sizeof(MONITORINFO);

	// default to primary monitor if gGameWindow is not available
	// then send info to MonitorInfo struct
	if (GetMonitorInfoA(MonitorFromWindow(gGameWindow, MONITOR_DEFAULTTOPRIMARY), &gPerformanceData.MonitorInfo) == 0)
	{
		Result = ERROR_MONITOR_NO_DESCRIPTOR;
		goto Exit;
	}

	// calculate monitor screen resolution using manifest file with DPI settings
	gPerformanceData.MonitorWidth = gPerformanceData.MonitorInfo.rcMonitor.right - gPerformanceData.MonitorInfo.rcMonitor.left;
	gPerformanceData.MonitorHeight = gPerformanceData.MonitorInfo.rcMonitor.bottom - gPerformanceData.MonitorInfo.rcMonitor.top;

	// change window behavior to frameless and remove title bar
	// bitwise operations here achieve multiple window settings at once
	if (SetWindowLongPtrA(gGameWindow, GWL_STYLE, (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~WS_OVERLAPPEDWINDOW) == 0)
	{
		Result = GetLastError();
		goto Exit;
	}

	// set window to fullscreen
	if (SetWindowPos(
		gGameWindow, 
		HWND_TOP,
		gPerformanceData.MonitorInfo.rcMonitor.left,
		gPerformanceData.MonitorInfo.rcMonitor.top,
		gPerformanceData.MonitorWidth,
		gPerformanceData.MonitorHeight, 
		SWP_NOOWNERZORDER | SWP_FRAMECHANGED) == 0)
	{
		Result = GetLastError();
		goto Exit;
	}

Exit:
	return Result;
}

BOOL GameIsAlreadyRunning(void)
{
	HANDLE Mutex = NULL;

	Mutex = CreateMutexA(NULL, FALSE, GAME_NAME "_GameMutex");

	if (GetLastError() == ERROR_ALREADY_EXISTS)
		return TRUE;
	else
		return FALSE;
}

void ProcessPlayerInput(void)
{
	 int16_t EscapeKeyIsDown = GetAsyncKeyState(VK_ESCAPE);

	 if (EscapeKeyIsDown)
		 SendMessageA(gGameWindow, WM_CLOSE, 0, 0);
}

void RenderFrameGraphics(void)
{
	//memset(gBackBuffer.Memory, 0xFF, (GAME_RES_HEIGHT * 4)*4);

	PIXEL32 Pixel = { 0 };
	Pixel.Blue = 0;
	Pixel.Green = 0x7f;
	Pixel.Red = 0;
	Pixel.Alpha = 0xff;

	for (int x = 0; x < GAME_RES_WIDTH * GAME_RES_HEIGHT; x++)
	{
		memcpy_s((PIXEL32*)gBackBuffer.Memory + x, sizeof(PIXEL32), &Pixel, sizeof(PIXEL32));
	}


	// when getting device context always remember to release it
	HDC DeviceContext = GetDC(gGameWindow);

	// DI = Device Independent
	// destination for this function refers to the monitors
	// source refers to the game or the backbuffer
	StretchDIBits(
		DeviceContext,
		0,
		0, 
		gPerformanceData.MonitorWidth,
		gPerformanceData.MonitorHeight, 
		0,
		0,
		GAME_RES_WIDTH,
		GAME_RES_HEIGHT,
		gBackBuffer.Memory,
		&gBackBuffer.BitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);

	ReleaseDC(gGameWindow, DeviceContext);
}



