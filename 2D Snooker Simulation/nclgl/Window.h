#pragma once
#include "Common.h"

#include "Mouse.h"
#include "Keyboard.h"
#include "GameTimer.h"

#include <vector>

#include <windows.h>
#include <fcntl.h>

//These two defines cut a lot of crap out of the Windows libraries
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#define WINDOWCLASS "WindowClass"

typedef void(*ResizeCallbackFunction) (int x, int y);

//This is the OS-specific crap required to render our pixel blocks on screen
class Window	{
public:
	Window(uint width, uint height);
	~Window(void);

	bool	UpdateWindow();	

	HWND	GetHandle() { return windowHandle; }

	void RegisterResizeCallback(ResizeCallbackFunction func);

	GameTimer* GetTimer() {
		return &timer;
	}
protected:
	void CheckMessages(MSG &msg);

	void Resize();

	//Windows requires a static callback function to handle certain incoming messages.
	static LRESULT CALLBACK StaticWindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	LRESULT WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	HWND	windowHandle;	//OS handle
	HDC		deviceContext;

	uint	screenWidth;
	uint	screenHeight;

	bool	forceQuit;
	bool	hasInit;

	GameTimer timer;

	std::vector<ResizeCallbackFunction> resizeListeners;
};

