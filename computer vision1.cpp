#include "stdafx.h"
#include "computer vision1.h"
#include <stdio.h>
#include "C:\Users\Nathan\Documents\Visual Studio 2010\Libraries\win_32.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2010\Libraries\computer_vision2.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2010\Libraries\graphics_debug.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	// put little tests here:
	
	//mess(rgb(55, 44, 192).color_percept().report(), "conv");

	// end of little tests

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	draw_surface ds1(1000, 500);
	draw_surface ds2(1000, 500, 50.0);
	_test_ds[TEST_DS_IN] = &ds1;
	_test_ds[TEST_DS_OUT_GFX_1] = &ds2;

	window w1 = window(L"Input Window", hInstance, nCmdShow, IDC_COMPUTERVISION1);
	window w2 = window(L"Output Window", hInstance, nCmdShow, IDC_COMPUTERVISION1, 0.0, 0.0, 0.75, 1.0);
	window w3 = window(L"Text Window", hInstance, nCmdShow, IDC_COMPUTERVISION1, 0.75, 0.0, 1.0, 1.0);
	_test_win[TEST_WIN_IN] = &w1;
	_test_win[TEST_WIN_OUT] = &w2;
	_test_win[TEST_WIN_TEXT_1] = &w3;

	//bitmap b(LPCWSTR(L"C:\\Users\\Nathan\\Documents\\Visual Studio 2012\\Test Files\\apple2.bmp"));
	bitmap b(LPCWSTR(L"C:\\Users\\Nathan\\Documents\\Visual Studio 2012\\Test Files\\rectangle.bmp"));
						    //C:\\Users\Nathan\Documents\Visual Studio 2012\Test Files
	b.blit(quantum_2d(15,15), ds1.hdc);
	b.blit(quantum_2d(15,15), ds2.hdc);

	w1.blit(&ds1, quantum_2d(100, 100));
	w2.blit(&ds2, quantum_2d(100, 100));
	
	w2.reblit_all();
	ds2.get_background();

	//ds1.circle(quantum_2d(342, 300), 115,  RGB_GREEN);
	//ds1.circle(quantum_2d(-1042, 300), 1390,  RGB_GREEN);
	//		ds1.circle(quantum_2d(351, 160), 151,  RGB_GREEN);
	//ds1.circle(quantum_2d(451, 140), 351,  RGB_GREEN);
	//ds1.circle(quantum_2d(651, -1240), 1551,  RGB_GREEN);
	//ds1.circle(quantum_2d(451, 120), 51,  RGB_GREEN);
	//ds1.rectangle(quantum_2d(200, 160), quantum_2d(500, 460), RGB_GREEN);
	//ds1.rectangle(quantum_2d(200, 150), quantum_2d(300, 250), RGB_BLUE);
	//ds1.rectangle(quantum_2d(210, 160), quantum_2d(270, 180), RGB_BLACK);
	//ds1.rectangle(quantum_2d(260, 160), quantum_2d(270, 230), RGB_BLACK);
	//ds1.rectangle(quantum_2d(240, 190), quantum_2d(245, 200), RGB_BLACK);
	//ds1.rectangle(quantum_2d(240, 190), quantum_2d(245, 200), RGB_RED);
	//ds1.rectangle(quantum_2d(210, 160), quantum_2d(230, 230), RGB_BLACK);
	//ds1.rectangle(quantum_2d(200, 150), quantum_2d(300, 250), RGB_RED);
	//ds1.triangle(quantum_2d(223 - 200, 214 - 25), quantum_2d(457 - 249, 209), quantum_2d(415 - 200, 665), RGB_GREEN);
	//this one: 
	//ds1.triangle(quantum_2d(223 - 200, 314 - 25), quantum_2d(457 - 209, 409), quantum_2d(515 - 200, 265), RGB_GREEN);
	//ds1.triangle(quantum_2d(223 - 200, 114 - 25), quantum_2d(957 - 209, 409), quantum_2d(515 - 100, 65), RGB_GREEN);
	//ds1.line_seg(100, 100, 100, 200, RGB_WHITE);
	show_windows();
	
	group_filter(&ds1);//, quantum_2d(50,15), ds1.get_top_rig() - quantum_2d(100, 250), 5);

	w1.repaint();
	w2.repaint();
	
	w2.zoom = 1.0f;
	w2.cursor = point_2d(0.0, 0.0);

	explore_outp(*_test_win[TEST_WIN_IN], *_test_win[TEST_WIN_OUT], 27);

	return 0;
}
