#include "Window.h"

Window::Window(uint width, uint height)	{
	hasInit = false;
	HINSTANCE hInstance = GetModuleHandle( NULL );

	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	if(!GetClassInfoEx(hInstance,WINDOWCLASS,&windowClass))	{
		windowClass.cbSize		= sizeof(WNDCLASSEX);
		windowClass.style		= CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc	= (WNDPROC)StaticWindowProc;
		windowClass.hInstance	= hInstance;
		windowClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
		windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
		windowClass.lpszClassName = WINDOWCLASS;

		if(!RegisterClassEx(&windowClass)) {
			return;
		}
	}

	windowHandle = CreateWindowEx(NULL,
		WINDOWCLASS,    // name of the window class
		"OpenGL Window!",   // title of the window
		WS_OVERLAPPEDWINDOW|WS_POPUP|WS_VISIBLE|WS_SYSMENU|WS_MAXIMIZEBOX|WS_MINIMIZEBOX,    // window style
		(int)100,	// x-position of the window
		(int)100,	// y-position of the window
		(int)width,		// width of the window
		(int)height,		// height of the window
		NULL,				// No parent window!
		NULL,				// No Menus!
		hInstance,			// application handle
		this);				// 

	deviceContext=GetDC(windowHandle);

	RECT    rt; 
	GetClientRect(windowHandle, &rt);

	screenWidth		= rt.right;
	screenHeight	= rt.bottom;

	Keyboard::Initialise(windowHandle);
	Mouse::Initialise(windowHandle);

	hasInit		= true;
	forceQuit	= false;
}


Window::~Window(void)	{
	Keyboard::Destroy();
	Mouse::Destroy();
}

LRESULT Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)	{
	switch(message)	 {
		case(WM_CREATE):{
			Window* w = reinterpret_cast<Window*>(((LPCREATESTRUCT)lParam)->lpCreateParams);
			SetWindowLongPtr(hWnd,GWL_USERDATA,reinterpret_cast<long>(w));
		}break;
		case(WM_DESTROY):	{
			PostQuitMessage(0);
			forceQuit = true;
		}break;
		case (WM_ACTIVATE): {
			if(LOWORD(wParam) == WA_INACTIVE)	{
				ReleaseCapture();
				ClipCursor(NULL);
				if (hasInit) {
					Mouse::instance->Sleep();
					Mouse::instance->Sleep();
				}
			}
			else{
				if(hasInit) {
					Mouse::instance->Wake();	
					Mouse::instance->Wake();
				}
			}
			return 0;
		}break;
		case (WM_LBUTTONDOWN): {

		}break;

		case (WM_MOUSEMOVE): {
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = windowHandle;
			TrackMouseEvent(&tme);
		}break;
		case(WM_SIZE): {
			screenWidth		= LOWORD(lParam);
			screenHeight	= HIWORD(lParam);
			Resize();
		}break;

		case(WM_SETFOCUS) : {
			if (hasInit) {
				Mouse::instance->Wake(); 
				Mouse::instance->Wake();
			}
		}break;
		case(WM_KILLFOCUS) : {
			if (hasInit) {
				Mouse::instance->Sleep();
				Mouse::instance->Sleep();
			}
		}break;
	}
	return DefWindowProc (hWnd, message, wParam, lParam);
}

LRESULT CALLBACK Window::StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)	{
	Window* window = (Window*)GetWindowLongPtr(hWnd,GWL_USERDATA);
	return window->WindowProc(hWnd,message,wParam,lParam);
}

bool	Window::UpdateWindow() {
	Keyboard::instance->UpdateHolds();
	Mouse::instance->UpdateHolds();

	MSG		msg;
	while(PeekMessage(&msg,windowHandle,0,0,PM_REMOVE)) {
		CheckMessages(msg); 
	}

	return !forceQuit;
}

void Window::CheckMessages(MSG &msg)	{
	switch (msg.message)	{				// Is There A Message Waiting?
		case (WM_QUIT):
		case (WM_CLOSE): {					// Have We Received A Quit Message?
			forceQuit = true;
		}break;
		case (WM_INPUT): {
			UINT dwSize;
			GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, NULL, &dwSize,sizeof(RAWINPUTHEADER));

			BYTE* lpb = new BYTE[dwSize];

			GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, lpb, &dwSize,sizeof(RAWINPUTHEADER));
			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (Keyboard::instance && raw->header.dwType == RIM_TYPEKEYBOARD) {
				Keyboard::instance->Update(raw);
			}
			else if (Mouse::instance && raw->header.dwType == RIM_TYPEMOUSE) {
				Mouse::instance->Update(raw);
			}
			delete lpb;
		}break;

		default: {								// If Not, Deal With Window Messages
			TranslateMessage(&msg);				// Translate The Message
			DispatchMessage(&msg);				// Dispatch The Message
		}
	}
}

void Window::RegisterResizeCallback(ResizeCallbackFunction func) {
	resizeListeners.push_back(func);
	func(screenWidth, screenHeight);
}

void Window::Resize() {
	for (std::vector<ResizeCallbackFunction>::iterator i = resizeListeners.begin(); i != resizeListeners.end(); ++i) {
		(*i)(screenWidth, screenHeight);
	}
};