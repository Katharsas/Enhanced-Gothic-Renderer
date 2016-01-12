#include "Window.h"
#include <REngine.h>

Window::Window()
{
	Handle = NULL;
}


Window::~Window(void)
{
	DestroyWindow(Handle);
}

/** Non-static callback */
LRESULT Window::OnWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		/*RECT ur;
		if(GetUpdateRect(hWnd,&ur,false))
		{
			ValidateRect(hWnd,&ur);
		}*/
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return 0;
}

/** Creates the window */
bool Window::Create(LPCSTR WindowName, int PosX, int PosY, int SizeX, int SizeY, HINSTANCE hInstance, long WindowStyle, DWORD ExWindowStyle, HWND Parent)
{
	RECT r = {0, 0, SizeX, SizeY};
	AdjustWindowRect(&r, WindowStyle, false);

	//r.right-=PosX;
	//r.bottom-=PosY;

	// Register window class
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = Window::WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = sizeof(LONG_PTR);
	wcex.hInstance     = hInstance;
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = "Wnd";
	
	RegisterClassEx(&wcex);

	// Create window
	Handle = CreateWindowEx(ExWindowStyle,
		"Wnd",
		WindowName,
		WindowStyle,
		PosX,
		PosY,
		r.right-r.left,
		r.bottom-r.top,
		Parent,
		NULL,
		hInstance,
		this
		);

	return Handle != NULL;
}

LRESULT Window::WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	LRESULT result = 0;
	if (msg == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		Window* Wnd = (Window *)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hWnd,
			GWLP_USERDATA,
			PtrToUlong(Wnd)
			);

		result = 0;
	}
	else
	{
		Window *Wnd = reinterpret_cast<Window *>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
			hWnd,
			GWLP_USERDATA
			)));

		bool WasHandled = false;

		if (Wnd && Wnd->GetHandle())
		{
			WasHandled = Wnd->OnWindowMessage(hWnd,msg,wParam,lParam) != 0;
		}

		

		if(!WasHandled)
		{
			result = DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}

	return result;
}

/** Runs the game */
void Window::Run(rendercallback callback)
{
	bool bGotMsg;
	MSG msg;
	msg.message = WM_NULL;
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	while( WM_QUIT != msg.message )
	{
		// Use PeekMessage() so we can use idle time to render the scene. 
		bGotMsg = ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );

		if( bGotMsg )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			REngine::RenderingDevice->OnFrameStart();

			callback();

			REngine::RenderingDevice->OnFrameEnd();

			// Logic
			REngine::RenderingDevice->Present();		
		}
	}
}