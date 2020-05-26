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

*/

#include <stdio.h>


// telling compiler to behave in a certain way
// adjust warning levels within source code without
// having to do anything else
// purpose: turns off all but 3 warnings that come from <windows.h>
#pragma warning(push, 3) 
#include <windows.h>
#pragma warning(pop)


// declare functions
LRESULT CALLBACK MainWindowProc(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);


// Even though we don't use 32 bit computers anymore we still use 
// the name win32. The curse of having a good name. Despite it actually
// being 64 bit we still refer to the API as win32. 
INT WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, PSTR CommandLine, INT CmdShow)
{
	// disable 	warnings? need to clarify what this block does
	UNREFERENCED_PARAMETER(PreviousInstance);
	UNREFERENCED_PARAMETER(CommandLine);
	UNREFERENCED_PARAMETER(CmdShow);


	// when programming in windows, everything is a window
	WNDCLASSEXA WindowClass = { 0 };
	HWND WindowHandle = 0;
	//MSG Msg;

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

	WindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	WindowClass.lpszMenuName = NULL; // used for drop down menus in window

	WindowClass.lpszClassName = "Win_Game_WINDOWCLASS"; // placeholding string 



	// register window class with windows
	if (RegisterClassExA(&WindowClass) == 0) // look up the doc of the function before deciding boolean results
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

		return 0;
	}


	WindowHandle = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		WindowClass.lpszClassName,
		"Window Title", // title of the window
		WS_OVERLAPPEDWINDOW | // window is an overlapped window meaning has title bar, border, and client area
		WS_VISIBLE, // show window without using ShowWindow()
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		240,
		120,
		NULL,
		NULL,
		Instance,
		NULL
	);

	if (WindowHandle == NULL)
	{
		// need to fix this by adding an error code
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// this causes the window to actually open
	ShowWindow(WindowHandle, TRUE);

	MSG Message;

	while (GetMessageA(&Message, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Message);

		// each time a message comes it the message will be dispatched
		// to the WindowClass defined window procedure
		DispatchMessageA(&Message);
	}

	return 0;
}


// processes all messages received by window
LRESULT CALLBACK MainWindowProc(
	HWND WindowHandle,        // handle to window
	UINT Message,        // message identifier
	WPARAM WParam,    // first message parameter
	LPARAM LParam)    // second message parameter
{

	LRESULT Result = 0;

	// all processing handles in the switch block
	switch (Message)
	{
		default:
		{
			// when message comes in pass it along to next procedure
			// this handles all the default behaviors like minimize or close
			Result = DefWindowProcA(WindowHandle, Message, WParam, LParam);
		}
	}
	return 0;
}