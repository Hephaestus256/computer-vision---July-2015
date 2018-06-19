#ifndef INC_GRAPHICS_DEBUG
#define INC_GRAPHICS_DEBUG

#define TEST_DS_IN 0
#define TEST_DS_OUT_GFX_1 1
#define TEST_DS_OUT_GFX_2 2
#define TEST_DS_OUT_TEXT_1 3
#define TEST_DS_OUT_TEXT_2 4

#define TEST_WIN_TEXT_1 0
#define TEST_WIN_TEXT_2 1
#define TEST_WIN_OUT 2
#define TEST_WIN_IN 3

#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\win_32.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\my_stl.h"

draw_surface* _test_ds[16];
window* _test_win[16];

int _test_int[16] = {0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0};

void show_windows(int millisecs = -1)
{
	//window::repaint_all();
	_test_win[TEST_WIN_OUT]->repaint();

	if (millisecs < 0) {
		MessageBox(NULL, L"All windows were repainted.", NULL, NULL);
	} else {
		Sleep(millisecs);
	}
}

void show_buffer()
{
	_test_ds[TEST_DS_OUT_GFX_1]->put_buffer();
	show_windows(0);
	
	if (_test_win[TEST_WIN_OUT]->zoom > 1.0) {
		for (double x = 100 + (1.0 - frac(_test_ds[TEST_DS_OUT_GFX_1]->bot_lef.x)) * _test_win[TEST_WIN_OUT]->zoom; x < 100 + 1000; x += _test_win[TEST_WIN_OUT]->zoom) {
			_test_win[TEST_WIN_OUT]->draw_line(point_2d(int(x), 65), point_2d(int(x), 90));
		}
		for (double y = - 13 + 100 + (frac(_test_ds[TEST_DS_OUT_GFX_1]->bot_lef.y)) * _test_win[TEST_WIN_OUT]->zoom; y < 100 + 500; y += _test_win[TEST_WIN_OUT]->zoom) {
			_test_win[TEST_WIN_OUT]->draw_line(point_2d(65, int(y)), point_2d(90, int(y)));
		}
	}
}

void set_view(window& win)
{
	_test_ds[TEST_DS_OUT_GFX_1]->set_view(win.zoom, win.cursor);
}

void show_text(char* s, int line_n = -1)
{
	_test_win[TEST_WIN_TEXT_1]->put_text(s, line_n);
}

void show_text(double n, int line_n = -1)
{
	_test_win[TEST_WIN_TEXT_1]->put_text(n, "", line_n);
}

void show_text(char* s, double n, int line_n = -1)
{
	_test_win[TEST_WIN_TEXT_1]->put_text(n, s, line_n);
}

void show_text(char* s1, char* s2, int line_n = -1)
{
	char s[256];
	strcpy_s(s, s1);
	strcat_s(s, s2);
	_test_win[TEST_WIN_TEXT_1]->put_text(s, line_n);
}

void cull_geo(int n = -1)
{
	_test_ds[TEST_DS_OUT_GFX_1]->cull_geometry(n);
}

void explore_outp(window& w1, window& w2, int exit_key = 27)
{
	int k = -3;
	static int skip_count = -1;

	if (skip_count >= 0) {
		--skip_count;
		return;
	}

	do {
		if (k != -1) {
			set_view(w2);
			show_buffer();
		}

		k = w2.get_last_key();
		//show_text("Key", k, 4);
		//show_text("Pos", w2.cursor.report(), 5);

		if (k == int('C')) {
			// clear all geo
			_test_ds[TEST_DS_OUT_GFX_1]->clear_geo();
		} else if (k == int('O')) {
			// clear all other geo
		} else if (k == int('Q')) {
			exit(0);
		} else if (k >= int('0') && k <= int('9')) {
			skip_count = (int)pow(10.0, double(k - int('0')));
		}

	} while(window::get_message() && (k != exit_key));

}

void color_point(int x, int y, rgb c, double alpha = 1.0, bool scale = false)
{
	_test_ds[TEST_DS_OUT_GFX_1]->write_pxl(x, y, c);
}

void color_point(vect_2d<int> p, rgb c, double alpha = 1.0, bool scale = false)
{
	_test_ds[TEST_DS_OUT_GFX_1]->write_pxl(p.x, p.y, c);
}

void color_seg_horz(int x1, int x2, int y, rgb c)
{
	for (int x = x1; x <= x2; ++x) {
		color_point(x, y, c);
	}
}

void color_seg_vert(int x, int y1, int y2, rgb c)
{
	for (int y = y1; y <= y2; ++y) {
		color_point(x, y, c);
	}
}

void color_point_in(int x, int y, rgb c, double alpha = 1.0)
{
	_test_ds[TEST_DS_IN]->alpha_blend_safe(x, y, (float)alpha, c);
}

void show_point(point_2d p, rgb c = RGB_RED, int radius = 3, bool nav = true)
{
	_test_ds[TEST_DS_OUT_GFX_1]->log_point(p, radius, c);

	if (nav) {
		_test_win[TEST_WIN_OUT]->cursor = p;
		explore_outp(*_test_win[TEST_WIN_IN], *_test_win[TEST_WIN_OUT], 13);
	}
}

void show_line(point_2d p1, point_2d p2, rgb c = RGB_RED, bool nav = true)
{
	_test_ds[TEST_DS_OUT_GFX_1]->log_line(p1, p2, c);

	if (nav) {
		_test_win[TEST_WIN_OUT]->cursor = p1;
		explore_outp(*_test_win[TEST_WIN_IN], *_test_win[TEST_WIN_OUT], 13);
	}
}

void show_line(point_2d p, line_2d_dydx l, rgb c = RGB_RED, bool nav = true)
{
	_test_ds[TEST_DS_OUT_GFX_1]->log_line(l.func_point(0.0f), l, c);

	if (nav) {
		_test_win[TEST_WIN_OUT]->cursor = p;
		explore_outp(*_test_win[TEST_WIN_IN], *_test_win[TEST_WIN_OUT], 13);
	}
}

void show_circle(circle_2d<> circ, rgb c = RGB_RED, bool nav = true)
{
	_test_ds[TEST_DS_OUT_GFX_1]->log_circle(circ, c);

	if (nav) {
		_test_win[TEST_WIN_OUT]->cursor = circ.center;
		explore_outp(*_test_win[TEST_WIN_IN], *_test_win[TEST_WIN_OUT], 13);
	}
}

void disp_point(window& win, point_2d p, rgb c = RGB_RED, int rad = 3, bool scale = false, bool repos = true)
{
	_test_ds[TEST_DS_OUT_GFX_1]->disp_point(p, c, rad, scale, repos);
}

void show_point_in(vect_2d<int> p, rgb c = RGB_RED, int radius = 3)
{
	for (int x = p.x - radius; x <= p.x + radius; x++) {
		color_point_in(x, p.y - radius, c);
	}

	for (int y = p.y - radius + 1; y <= p.y + radius -1; y++) {
		color_point_in(p.x - radius, y, c);
		color_point_in(p.x + radius, y, c);
	}

	for (int x = p.x - radius; x <= p.x + radius; x++) {
		color_point_in(x, p.y + radius, c);
	}

//	blit_windows(0);
}

/* show round robin element */
template <class gen1, class gen2>
void show_rre(round_robin_element<gen1> e, std::vector<gen2>& vect)
{
	rgb color;

	if (e.lap == 0) {
		color = RGB_WHITE;
	} else if (e.lap < 0) {
		color = RGB_RED;
	} else {
		color = RGB_GREEN;
	}

	show_point(vect[e.i], color);
}

rgb read_point_in(int x, int y)
{
	return _test_ds[TEST_DS_IN]->read_pxl_safe(x, y);
}

void init_background()
{
	_test_ds[TEST_DS_OUT_GFX_1]->init_background();
}

void get_background()
{
	_test_ds[TEST_DS_OUT_GFX_1]->get_background();
}

void put_background()
{
	//_test_ds[TEST_DS_OUT_GFX_1]->put_background();
}

void clear_windows()
{
	put_background();
}

void mess(int num, char* text = NULL)
{
	char in_text[1000];

	if (text == NULL) {
		text = "value = ";
	}

	sprintf_s(in_text, "%s %i", text, num);
	LPTSTR str = char_to_lptstr(in_text);
	MessageBox(NULL, str, L"Info", MB_OK);

	delete [] str;
}

void mess(char* text1, char* text2)
{
	char in_text[1000];

	if (text2 == NULL) {
		text2 = "";
	}

	sprintf_s(in_text, "%s %s", text2, text1);
	LPTSTR str = char_to_lptstr(in_text);
	MessageBox(NULL, str, L"Info", MB_OK);

	delete [] str;
}

void mess(char* text)
{
	char in_text[1000];

	sprintf_s(in_text, "%s", text);
	LPTSTR str = char_to_lptstr(in_text);
	MessageBox(NULL, str, L"Info", MB_OK);

	delete [] str;
}

template <class gen>
void mess(gen num, char* text)
{
	char in_text[1000];

	if (text == NULL) {
		text = "value = ";
	}

	sprintf_s(in_text, "%s %f", text, (float)num);
	LPTSTR str = char_to_lptstr(in_text);
	MessageBox(NULL, str, L"Info", MB_OK);

	delete [] str;
}

#endif