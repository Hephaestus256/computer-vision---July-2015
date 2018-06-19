#ifndef INC_GRAPHICS_2D
#define INC_GRAPHICS_2D
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\math_2d.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\color.h"
#include "stdafx.h"
#include <windows.h>
#include <vector>

class gfx_surface {
protected:
	HDC dest_hdc;
	HBITMAP pixels;
	HBITMAP old_bmp;
	quantum_2d dims;
	HPEN pen;
	HBRUSH brush;
	rgb* background;
public:
	HDC hdc;

	gfx_surface(HDC p_dest_hdc = NULL)
	{
		hdc = CreateCompatibleDC(p_dest_hdc);
		dest_hdc = p_dest_hdc;
		pixels = NULL;
		old_bmp = NULL;
		pen = NULL;
		brush = NULL;
	}

	~gfx_surface()
	{
		if (hdc != NULL) {
			SelectObject(hdc, old_bmp);
			DeleteDC(hdc);
		}

		if (pixels != NULL) {
			DeleteObject(pixels);
		}

		if (pen != NULL) {
			DeleteObject(pen);
		}

		if (brush != NULL) {
			DeleteObject(brush);
		}
	}

	bool init_surface(int width, int height, HBITMAP p_bmp = NULL)
	{
		pixels = p_bmp;
		old_bmp = (HBITMAP)SelectObject(hdc, p_bmp);
		dims = quantum_2d(width, height);

		return (old_bmp != NULL);
	}

	inline quantum_2d get_dims()
	{
		return dims;
	}

	inline quantum_2d get_top_rig()
	{
		return dims - quantum_2d(1, 1);
	}

	bool blit(quantum_2d p_xy, HDC p_dest_hdc = NULL, quantum_2d p_dims = QUANTUM_2D_UNDEF_VALUE, quantum_2d p_uv = QUANTUM_2D_UNDEF_VALUE)
	{
		if (p_dest_hdc == NULL) {
			p_dest_hdc = dest_hdc;
		}

		p_dims = dims;
		p_uv = quantum_2d(0, 0);

		return BitBlt(p_dest_hdc, p_xy.x, p_xy.y, p_dims.x, p_dims.y, hdc, p_uv.x, p_uv.y, SRCCOPY) != FALSE;
	}

	void set_color(rgb color, bool is_solid = true)
	{
		static rgb last_color(RGB_BLACK);
		static rgb last_brush(RGB_BLACK);
		int pen_mode;

		if (is_solid) {
			pen_mode = PS_SOLID;
		} else {
			pen_mode = PS_SOLID;
		}

		pen = CreatePen(pen_mode, 1, RGB(color.r, color.g, color.b));
		SelectObject(hdc, pen);
		last_color = color;
		
		if (is_solid) {
			if (last_brush != color) {
				brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
				SelectObject(hdc, brush);
				last_brush = color;
			}
		}
	}

	void circle(quantum_2d center, int radius, rgb color, bool solid = false)
	{
		set_color(color, solid);
		Ellipse(hdc, center.x - radius, center.y - radius, center.x + radius, center.y + radius);
	}

	void elipse(quantum_2d center, int x_radius, int y_radius, rgb color, bool solid = true)
	{
		set_color(color, solid);
		Ellipse(hdc, center.x - x_radius, center.y - y_radius, center.x + x_radius, center.y + y_radius);
	}

	void rectangle(quantum_2d tl, quantum_2d br, rgb color, bool solid = true)
	{
		set_color(color, solid);
		Rectangle(hdc, tl.x, tl.y, br.x + 2, br.y + 2);
	}

	void poly(quantum_2d* verts, int ct, rgb color, bool solid = true)
	{
		POINT* p_verts = new POINT[ct];

		for (int i = 0; i < ct; i++) {
			p_verts[i].x = verts[i].x;
			p_verts[i].y = verts[i].y;
		}

		set_color(color, solid);
		Polygon(hdc, p_verts, ct);
	}

	void triangle(quantum_2d p1, quantum_2d p2, quantum_2d p3, rgb color, bool solid = true)
	{
		quantum_2d verts[3] = {p3, p2, p1};
		poly(verts, 3, color, solid);
	}

	void test_box(quantum_2d center, rgb color = RGB_WHITE, int radius = 0)
	{
		int offset = radius + 1;
		//rectangle(center - quantum_2d(offset, offset), center + quantum_2d(offset, offset), color, false);
	}
};

class bitmap : public gfx_surface {
	BITMAP bm;
public:
	bitmap(LPCWSTR p_file_name, HDC p_dest_hdc = NULL) : gfx_surface(p_dest_hdc)
	{
		HBITMAP hbmp = (HBITMAP)LoadImage(NULL, p_file_name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (hbmp == NULL) {
			wchar_t tmp[50];
			swprintf(tmp, 50, L"BMP load failed - error %i", ::GetLastError());
			MessageBox(NULL, p_file_name, NULL, NULL);
			MessageBox(NULL, LPCWSTR(tmp), NULL, NULL);
		}

		GetObject(hbmp, sizeof(bm), &bm);
		init_surface(bm.bmWidth, bm.bmHeight, hbmp);
	}
	
	~bitmap()
	{
	}
};

enum geo_object {geo_point, geo_line, geo_circle};

class drawn_geo {
public:

	geo_object obj;
	point_2d pos;
	rgb color;
	double param1;

	drawn_geo()
	{
	}

	drawn_geo(geo_object p_obj, point_2d p_pos, rgb p_color, double p_param1)
	{
		obj = p_obj;
		pos = p_pos;
		color = p_color;
		param1 = p_param1;
	}
};

class draw_surface : public gfx_surface {
protected:
	void* direct_dat;
	rgb* buffer;
	point_2dd cursor;
	double zoom;
	quantum_2d dims;
	std::vector<drawn_geo> geometry;

public:

	point_2dd bot_lef;

	draw_surface(
		int p_width, int p_height, 
		point_2d p_bot_lef = point_2d(0.0, 0.0), 
		HDC p_dest_hdc = NULL) : gfx_surface(p_dest_hdc)
	{
		BITMAPINFO bmp_info;
		bmp_info.bmiHeader.biBitCount = 24;
		bmp_info.bmiHeader.biCompression = BI_RGB;
		bmp_info.bmiHeader.biPlanes = 1;
		bmp_info.bmiHeader.biSize = sizeof(bmp_info.bmiHeader);
		bmp_info.bmiHeader.biWidth = p_width;
		bmp_info.bmiHeader.biHeight = p_height;
		bot_lef = point_2d(0, 0);
		zoom = 1.0;
		buffer = new rgb[p_width * p_height];

		//color_surface(buffer, p_width, p_height);
		init_surface(p_width, p_height, CreateDIBSection(hdc, &bmp_info, DIB_RGB_COLORS, &direct_dat, NULL, 0));
		dims = get_dims();

		get_background();
	}

	draw_surface(HDC p_dest_hdc = NULL) : gfx_surface(p_dest_hdc)
	{
		direct_dat = NULL;
		zoom = 1.0;
		bot_lef = point_2d(0, 0);
		dims = get_dims();
	}

	draw_surface(LPCWSTR p_file_name)
	{
		bitmap bmp(p_file_name);

		BITMAPINFO bmp_info;
		bmp_info.bmiHeader.biBitCount = 24;
		bmp_info.bmiHeader.biCompression = BI_RGB;
		bmp_info.bmiHeader.biPlanes = 1;
		bmp_info.bmiHeader.biSize = sizeof(bmp_info.bmiHeader);
		bmp_info.bmiHeader.biWidth = bmp.get_dims().x;
		bmp_info.bmiHeader.biHeight = bmp.get_dims().y;

		init_surface(bmp_info.bmiHeader.biWidth, bmp_info.bmiHeader.biHeight, CreateDIBSection(hdc, &bmp_info, DIB_RGB_COLORS, &direct_dat, NULL, 0));
		bmp.blit(quantum_2d(0, 0), hdc);

		zoom = 1.0;
		bot_lef = point_2d(0, 0);
		dims = get_dims();
	}

	~draw_surface()
	{		
		if (buffer != NULL) {
			delete buffer;
		}
	}

	void clear_geo()
	{
		geometry.clear();
	}

	void set_view(float p_zoom, point_2d p_cursor)
	{
		zoom = p_zoom;
		
		point_2dd dims_zoom = (dims.cv_double() / zoom);
		point_2dd dims_h_zoom = (dims.cv_double() / zoom) * 0.5;

		bot_lef = point_2dd(p_cursor.x - 0.0, p_cursor.y) - dims_h_zoom;
	}

	void* get_direct_ptr()
	{
		return direct_dat;
	}

	void color_surface(rgb* surface, int w, int h, rgb color = RGB_BLACK)
	{
		rgb* final = surface + w * h;

		for (; surface < final; ++surface) {
			*surface = color;
		}
	}

	void get_surface()
	{
		quantum_2d dims = get_dims();
		rgb* src = (rgb*)get_direct_ptr();
		rgb* final = buffer + dims.x * dims.y;
		
		for (rgb* dest = buffer; dest < final; ++dest) {
			*dest = *src;
			++src;
		}
	}

	inline rgb read_window(int offset)
	{
		rgb* start = (rgb*)get_direct_ptr();
		rgb ret = *(start + offset);
		return ret.reverse();
	}
	
	inline void write_window(int offset, rgb color)
	{
		rgb* start = (rgb*)get_direct_ptr();
		*(start + offset) = color.reverse();
	}

	inline void write_window(int x, int y, rgb color)
	{
		write_window(x + y * dims.y, color);
	}

	inline void write_window_safe(int x, int y, rgb color)
	{
		if (x >= 0 && x < dims.x && y >= 0 && y < dims.y) {
			write_window(x + y * dims.x, color);
		}
	}

	inline void write_buff(int offset, rgb color)
	{
		buffer[offset] = color;
	}

	inline void write_buff(int x, int y, rgb color)
	{
		if (x < 0) {
			;
		} else if (x >= dims.x) {
			;
		} else if (y < 0) {
			;
		} else if (y >= dims.y) {
			;
		} else {
			// (dims.y - 1 - 
			write_buff(x + y * dims.x, color);
		}
	}

	inline rgb read_pxl(int x, int y)
	{
		// (dims.y - 1 - 
		return read_window(x + y * dims.x);
	}

	inline rgb read_pxl_safe(int x, int y)
	{
		// (dims.y - 1 - 
		if ((x >= 0) && (x < dims.x) && (y >= 0) && (y < dims.y)) {
			return read_window(x + y * dims.x);
		} else {
			return rgb(-1, -1, -1);
		}
	}
	
	inline void write_pxl(int x, int y, rgb color)
	{
		// dims.y - 1 - 
		write_buff(x + y * dims.x, color);
	}

	inline void write_pxl_safe(int x, int y, rgb color)
	{
		// dims.y - 1 - 
		if ((x >= 0) && (x < dims.x) && (y >= 0) && (y < dims.y)) {
			write_buff(x + dims.x * y, color);
		}
	}

	template <class gen1>
	inline void write_pxl(vect_2d<gen1> p, rgb color)
	{
		write_pxl_safe((int)p.x, (int)p.y, color);
	}

	inline void alpha_blend(int x, int y, float alpha = 0.5, rgb color = RGB_BLACK)
	{
		rgb exist = read_pxl(x, y);
		write_pxl_safe(x, y, rgb(interpolate(alpha, exist.r, color.r), interpolate(alpha, exist.g, color.g), interpolate(alpha, exist.b, color.b)));
	}

	inline void alpha_blend_safe(int x, int y, float alpha = 0.5, rgb color = RGB_BLACK, bool scale = false)
	{
		/*
		if (scale) {
			int x1 = int((float)x * zoom);
			int x2 = int(((float)x + 1.0f) * zoom);
			int y1 = int(float(y) * zoom);
			int y2 = int((float(y) + 1.0f) * zoom);

			for (int x = x1; x < x2; x++) {
				for (int y = y1; y < y2; y++) {
					rgb exist = read_pxl_safe(x, y);
					write_pxl_safe(x, y, rgb(interpolate(alpha, exist.r, color.r), interpolate(alpha, exist.g, color.g), interpolate(alpha, exist.b, color.b)));
				}
			}
		} else {
		*/
			int x1 = x;//int(((float)x + 0.5f) * zoom);
			int y1 = y;//int(((float)y + 0.5f) * zoom);

			rgb exist = read_pxl_safe(x1, y1);
			write_pxl_safe(x, y, rgb(interpolate(alpha, exist.r, color.r), interpolate(alpha, exist.g, color.g), interpolate(alpha, exist.b, color.b)));
		//}
	}

	inline void line_seg(int x1, int y1, int x2, int y2, rgb color)
	{
		double dx = double(x2 - x1);
		double dy = double(y2 - y1);

		if (dx > dy) {
			double y = y1;
			double m = dy / dx;
			for (int x = x1; x <= x2; x++) {
				write_pxl_safe(x, (int)floor(y), color);
				y += m;
			}
		} else {
			double x = x1;
			double m = dx / dy;
			for (int y = y1; y <= y2; y++) {
				write_pxl_safe((int)x, y, color);
				x += m;
			}
		}
	}

	void color_point(int x, int y, rgb c, double alpha = 1.0, bool wind = true, bool scale = false)
	{
		if (wind) {
			write_pxl(x, y, c);
		} else {
			write_window_safe(x, y, c);
		}
	}

	quantum_2d trans_point(point_2d p)
	{
		point_2dd tp = point_2dd(p.x, dims.y - 1 - p.y);
		point_2dd dims_zoom = dims * zoom;
		point_2dd t = ((tp * point_2dd(1.0, -1.0)) - bot_lef) * zoom;// + (dims * 0.5);
		return quantum_2d(int(t.x), int(dims_zoom.y + (t.y)));
	}

	void disp_point(point_2d p, rgb c = RGB_RED, int radius = 3, bool scale = false, bool repos = true)
	{
		quantum_2d ctr = trans_point(p);
		
		for (int x = ctr.x - radius; x <= ctr.x + radius; x++) {
			color_point(x, ctr.y - radius, c, 1.0, scale);
		}

		for (int y = ctr.y - radius + 1; y <= ctr.y + radius - 1; y++) {
			color_point(ctr.x - radius, y, c, 1.0, scale);
			color_point(ctr.x + radius, y, c, 1.0, scale);
		}

		for (int x = ctr.x - radius; x <= ctr.x + radius; x++) {
			color_point(x, ctr.y + radius, c, 1.0, scale);
		}
	}

	void disp_line(line_2d_dydx l, rgb color = RGB_RED, bool scale = false)
	{
		double m;
		double dep;

		if (l.is_asmtote()) {
			int x = int((l.b  - bot_lef.x) * zoom);
			for (int y = 0; y < dims.y; y++) {
				color_point(x, y, color, 1.0, scale);
			}
		} else if (abs(l.m) < 1.0) {
			m = -l.m;
			dep = (-l.b + bot_lef.x * m - bot_lef.y + dims.y) * zoom;

			for (int x = 0; x < dims.x; x++) {
				color_point(x, int(dep), color, 1.0, scale);
				dep += m;
			}
		} else {
			m = -1.0 / l.m;
			dep = (-l.b / l.m  + bot_lef.y * m - bot_lef.x + dims.y / l.m) * zoom;

			for (int y = 0; y < dims.y; y++) {
				color_point(int(dep), y, color, 1.0, scale);
				dep += m;
			}
		}
	}

	void disp_circle(circle_2d<> circ, rgb color = RGB_GREEN, bool scale = false)
	{
		int ind;
		double dep;
		double r = circ.radius * zoom;
		int trans = int(r * 0.707);
		point_2d cnt((circ.center.x - bot_lef.x) * zoom, ((dims.y - circ.center.y) - bot_lef.y) * zoom);

		for (ind = -trans; ind <= trans; ind++) {
			dep = sqrt(r * r - double(ind) * double(ind));
			color_point(int(cnt.x + ind), int(cnt.y - dep), color, 1.0, scale);
			color_point(int(cnt.x + ind), int(cnt.y + dep), color, 1.0, scale);
			color_point(int(cnt.x - dep), int(cnt.y + ind), color, 1.0, scale);
			color_point(int(cnt.x + dep), int(cnt.y + ind), color, 1.0, scale);
		}
	}

	void disp_geometry(drawn_geo g)
	{
		if (g.obj == geo_point) {
			disp_point(g.pos, g.color, int(g.param1)); 
		} else if (g.obj == geo_line) {
			disp_line(line_2d_dydx(1.0, g.param1, g.pos), g.color);
		} else if (g.obj == geo_circle) {
			disp_circle(circle_2d<>(g.pos, g.param1), g.color);
		}
	}

	void init_background()
	{
		background = new rgb[dims.x * dims.y];
	}

	void get_background()
	{
		for (int y = 0; y < dims.y; y++) {
			for (int x = 0; x < dims.x; x++) {
				// dims.y - 1
				buffer[x + y * dims.x] = read_pxl(x, y);
			}
		}
	}

	void cull_geometry(int n = -1)
	{
		if (n == -1) {
			geometry.clear();
		} else {
			for (int i = 0; i < min(n, (int)geometry.size()); i++) {
				geometry.pop_back();
			}
		}
	}

	/*
	void put_background()
	{
		quantum_2d dims = get_dims();
		
		for (int y = 0; y < dims.y; y++) {
			for (int x = 0; x < dims.x; x++) {
				write_window(x, y, background[x + y * dims.x]);
			}
		}
	}
	*/

	void put_buffer()
	{
		point_2dd source;
		point_2dd delta =  point_2dd(1.0 / zoom, 1.0 / zoom);

		source.y = bot_lef.y;
		for (int y = 0; y < dims.y; y++) {
			source.x = bot_lef.x;
			for (int x = 0; x < dims.x; x++) {

				int u = int(source.x);
				int v = int(source.y);
				rgb c;

				if (u >= 0 && u < dims.x && v >= 0 && v <= dims.y) {
					c = buffer[u + v * dims.x];
				} else {
					c = RGB_GREY;
				}

				// (dims.y - 1 - 
				write_window(x + y * dims.x, c);
				source.x += delta.x;
			}
			source.y += delta.y;
		}

		for (unsigned int i = 0; i < geometry.size(); i++) {
			disp_geometry(geometry[i]);
		}
	}

	void show_view(point_2d p_center, float zoom = 1.0f)
	{
		point_2d dims_conv = dims / zoom;
		point_2d dims_conv_h = dims_conv / 2.0;

		bot_lef = point_2d(p_center.x, float(dims.y - 1) - p_center.y);
	}

	void log_point(point_2d p, double rad, rgb color)
	{
		geometry.push_back(drawn_geo(geo_point, p, color, rad));
	}

	void log_line(point_2d p1, point_2d p2, rgb color)
	{
		line_2d_dydx l(p1, p2);
		geometry.push_back(drawn_geo(geo_line, p1, color, l.m));
	}

	void log_line(point_2d p, line_2d_dydx l, rgb color)
	{
		geometry.push_back(drawn_geo(geo_line, p, color, l.m));
	}

	void log_circle(circle_2d<> circ, rgb color)
	{
		geometry.push_back(drawn_geo(geo_circle, circ.center, color, circ.radius));
	}
};

class sprite {
public:
	gfx_surface* surf;
	quantum_2d pos;

	sprite(gfx_surface* p_surf, quantum_2d p_pos)
	{
		surf = p_surf;
		pos = p_pos;
	}

	~sprite()
	{
	}

	gfx_surface* get_bmp()
	{
		return surf;
	}

	bool blit(HDC p_dest_hdc)
	{
		return surf->blit(pos, p_dest_hdc);
	}
};

#endif // !INC_GRAPHICS_2D