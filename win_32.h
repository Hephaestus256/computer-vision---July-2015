#ifndef INC_WIN_32
#define INC_WIN_32

#include <stdio.h>
#include <windows.h>
#include "stdafx.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\graphics_2d.h"
#include <list>
#include <vector>

#define MAX_LOADSTRING 100
#define WIN_32_MAX_WINDOWS 256

typedef std::list<sprite> sprite_bank;
typedef std::vector<LPTSTR> text_bank;

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

//HDC hdc = GetDC(NULL);
int lf_height = 15;//-MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72);
HFONT hf = CreateFont(lf_height, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, L"Times New Roman");
//ReleaseDC(NULL, hdc);

LPTSTR char_to_lptstr(const char* in_text)
{
	LPTSTR str = new TCHAR[strlen(in_text) + 1];
	MultiByteToWideChar(0, 0, in_text, -1, str, int(strlen(in_text)) + 1);
	return str;
}

quantum_2d get_screen_dims()
{
	RECT rc;
	GetWindowRect(GetDesktopWindow(), &rc);
	return quantum_2d(rc.right, rc.bottom);
}

class window {
	HINSTANCE instance;
	HWND wnd;
	HDC hdc;
	WNDCLASSEX wcex;
	sprite_bank sprites;
	text_bank texts;
	static int window_ct;
	static window* pool[256];
	static HACCEL hAccelTable;
public:
	bool user_closed;
	point_2d cursor;
	float zoom;
	int last_key;

	window(
		LPCWSTR p_title, HINSTANCE p_instance, int p_cmd_show, int idc,
		double x1 = 0.0, double y1 = 0.0, double x2 = 1.0, double y2 = 1.0)
	{
		static bool init = false;

		if (!init) {
			for (int i = 0; i < 256; i++) {
				pool[i] = NULL;
			}
			hAccelTable = LoadAccelerators(p_instance, MAKEINTRESOURCE(idc));

			init = true;
		}

		user_closed = false;
		instance = p_instance;

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra	= sizeof(long);
		wcex.hInstance = p_instance;
		wcex.hIcon = LoadIcon(p_instance, MAKEINTRESOURCE(idc));
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName = MAKEINTRESOURCE(idc);
		wcex.lpszClassName = L"Window Class";
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
		RegisterClassEx(&wcex);
		
		quantum_2d dims = get_screen_dims();
		cursor = POINT_2D_ORIG;

		wnd = CreateWindow(wcex.lpszClassName, p_title, 
			WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL, 
			int(x1 * dims.x), int(y1 * dims.y), int((x2 - x1) * dims.x), int((y2 - y1) * dims.y), NULL, NULL, p_instance, NULL
		);

		if (!wnd) {
		  MessageBox(NULL, L"Window creation failed", NULL, NULL);
		}

		hdc = GetDC(wnd);
		if (!hdc) {
			MessageBox(NULL, L"Could not get DC", NULL, NULL);
		}

		pool[window_ct] = this;
		SetWindowLong(wnd, 0, window_ct + 1);

		ShowWindow(wnd, SW_SHOW);
		UpdateWindow(wnd);

		window_ct++;
		last_key = -1;

		zoom = 1.0f;
	}

	~window()
	{
		clear_texts();
	}

	bool repaint()
	{
		InvalidateRect(wnd, NULL, TRUE);
		return UpdateWindow(wnd) == TRUE;
	}

	bool blit(gfx_surface* p_surf, quantum_2d p_pos)
	{
		sprites.push_front(sprite(p_surf, p_pos));
		return repaint();
	}

	bool put_text(const char* p_text, int line_n = -1)
	{
		LPTSTR str = char_to_lptstr(p_text);

		if (line_n < 0) {
			texts.push_back(str);
		} else {
			if ((int)texts.size() <= line_n) {
				for (int i = (int)texts.size(); i <= line_n; i++) {
					texts.push_back(L"");
				}
			}
			texts[line_n] = str;
		}

		return repaint();
	}

	bool put_text(int n, const char* p_text = "", int line_n = -1)
	{
		char line[100];
		sprintf_s(line, "%s %i", p_text, n);
		return put_text(line, line_n);
	}

	bool put_text(double n, const char* p_text = "", int line_n = -1)
	{
		char line[100];
		sprintf_s(line, "%s %f", p_text, n);
		return put_text(line, line_n);
	}

	void draw_line(point_2d p1, point_2d p2)
	{
		MoveToEx(hdc, int(p1.x), int(p1.y), NULL);
		LineTo(hdc, int(p2.x), int(p2.y));
	}

	void clear_texts()
	{
		for (text_bank::iterator i = texts.begin(); i != texts.end(); i++) {
			delete *i;
		}
		texts.clear();
	}

	int get_last_key()
	{
		int ret = last_key;
		last_key = -1;
		return ret;
	}

	static bool get_message()
	{
		MSG msg;
		bool ret = GetMessage(&msg, NULL, 0, 0) == TRUE;

		if (ret) {
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return ret;
	}

	static window* get_window(int index)
	{
		return pool[index];
	}

	static bool set_messages(HWND p_wnd, UINT p_message, WPARAM p_w_param, LPARAM p_l_param)
	{
		int n = GetWindowLong(p_wnd, 0);

		if (n != NULL) {
			window* win = pool[n - 1];
			float delta = 30.0f / win->zoom;

			if (win != NULL) {
				switch (p_message) {
					case WM_KEYDOWN:
						win->last_key = int(p_w_param);

						if (p_w_param == 37) {
							win->cursor.x -= delta;
						} else if (p_w_param == 38) {
							win->cursor.y += delta;
						} else if (p_w_param == 39) {
							win->cursor.x += delta;
						} else if (p_w_param == 40) {
							win->cursor.y -= delta;
						} else if (p_w_param == 189) {
							win->zoom *= 0.5;
						} else if (p_w_param == 187) {
							win->zoom *= 2.0;
						}

						break;
					case WM_KEYUP:
						break;
					case WM_DESTROY:
						win->user_closed = true;
						break;
					case WM_PAINT:
						PAINTSTRUCT ps;
						HDC hdc = BeginPaint(p_wnd, &ps);

						for (sprite_bank::iterator i = win->sprites.begin(); i != win->sprites.end(); i++) {
							i->blit(hdc);
						}

						int y = 0;
						for (text_bank::iterator i = win->texts.begin(); i != win->texts.end(); i++) {
							int x = 0;
							TextOut(hdc, x, y, *i, (int)_tcslen(*i));
							y += 15;
						}

						EndPaint(p_wnd, &ps);
						
						break;
				}
			}
		}

		return true;
	}

	static void repaint_all()
	{
		for (int i = 0; i < window_ct; i++) {
			pool[i]->repaint();
		}
	}

	static void reblit_all()
	{
		for (int w = 0; w < window_ct; w++) {
			for (sprite_bank::iterator i = pool[w]->sprites.begin(); i != pool[w]->sprites.end(); i++) {
				i->blit(pool[w]->hdc);
			}
		}
	}
};
int window::window_ct = 0;
window* window::pool[256];
HACCEL window::hAccelTable = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	window::set_messages(hWnd, message, wParam, lParam);
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void blit_windows(int millisecs = -1)
{
	window::reblit_all();

	if (millisecs < 0) {
		MessageBox(NULL, L"All windows were repainted.", NULL, NULL);
	} else {
		Sleep(millisecs);
	}
}

void msg(double n, char* text = "")
{
	char line[100];
	sprintf_s(line, "%s %f", text, n);
	MessageBox(NULL, LPCWSTR(line), NULL, NULL);
}

#endif // !INC_WIN_32