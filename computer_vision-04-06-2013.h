#ifndef INC_COMPUTER_VISION
#define INC_COMPUTER_VISION

#define TEST_DS_IN 0
#define TEST_DS_OUT_GFX_1 1
#define TEST_DS_OUT_GFX_2 2
#define TEST_DS_OUT_TEXT_1 3
#define TEST_DS_OUT_TEXT_2 4

#define TEST_WIN_TEXT_1 0
#define TEST_WIN_TEXT_2 1

#define OVERLAP_CT 3

draw_surface* _test_ds[16];
window* _test_win[16];

int _test_int[16] = {0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0};

/*
pool - a homogenous collection of data in an array, vector, list, etc
row - a horizontal row of pixels
segment - a contiguous horizontal line segment of homogenous characteristics
	there are 2 types of segments:
		row_segment - a segment that comes from a parsed horizontal row of pixels
		poly_segment - the type of segment(s) that form(s) a _polygon_
seg set - a set of segments that make up a single horizontal row
	there are 2 types of seg sets:
		row_seg_set -
		poly_seg_set - 
polygon - a collection of _poly_segments_ that can be parsed into a shape
shape - a collection of _features_ that define the shape
feature - the elements of a _shape_ (curve, angle, vertex)
*/

#include "C:\Users\Nathan\Documents\Visual Studio 2010\Libraries\math_1d.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2010\Libraries\math_2d.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2010\Libraries\my_stl.h"
#include <vector>
#include <list>

void show_text(char* s)
{
	_test_win[TEST_WIN_TEXT_1]->put_text(s);
}

void show_text(double n)
{
	_test_win[TEST_WIN_TEXT_1]->put_text(n);
}

void show_text(char* s, double n)
{
	_test_win[TEST_WIN_TEXT_1]->put_text(n, s);
}

void color_point(int x, int y, rgb c, double alpha = 1.0)
{
	_test_ds[TEST_DS_OUT_GFX_1]->alpha_blend_safe(x, y, alpha, c);
}

void color_point(vect_2d<int> p, rgb c, double alpha = 1.0)
{
	_test_ds[TEST_DS_OUT_GFX_1]->alpha_blend_safe(p.x, p.y, alpha, c);
}

void color_point_in(int x, int y, rgb c, double alpha = 1.0)
{
	_test_ds[TEST_DS_IN]->alpha_blend_safe(x, y, alpha, c);
}

void show_point(vect_2d<int> p, rgb c = RGB_RED, int radius = 3)
{
	for (int x = p.x - radius; x <= p.x + radius; x++) {
		color_point(x, p.y - radius, c);
	}

	for (int y = p.y - radius + 1; y <= p.y + radius -1; y++) {
		color_point(p.x - radius, y, c);
		color_point(p.x + radius, y, c);
	}

	for (int x = p.x - radius; x <= p.x + radius; x++) {
		color_point(x, p.y + radius, c);
	}

//	blit_windows(0);
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

rgb read_point_in(int x, int y)
{
	return _test_ds[TEST_DS_IN]->read_pxl_safe(x, y);
}

void show_line(line_2d_dydx l, rgb color = RGB_RED)
{
	static quantum_2d d = _test_ds[TEST_DS_OUT_GFX_1]->get_dims();
	double m;
	double dep;
	int ind_comp;

	if (abs(l.m) < 1.0) {
		ind_comp = VECT_2D_X;
		m = l.m;
		dep = l.b;
	} else {
		ind_comp = VECT_2D_Y;
		m = 1.0 / l.m;
		dep = l.b * m;
	}

	show_text(l.m);

	for (int ind = 0; ind < d.get_component(ind_comp); ind++) {
		vect_2d<int> p = point_2d(double(ind), dep).alias_pixel();
		color_point(p.get_component(ind_comp), p.get_other_component(ind_comp), color);
		dep += m;
	}
}

class pixel_info {
public:
	int r, g, b;

	pixel_info()
	{
	}

	pixel_info(int p_r, int p_g, int p_b)
	{
		r = p_r;
		g = p_g;
		b = p_b;
	}

	int extent()
	{
		return max(max(abs(g), abs(r)), abs(b));
	}

	rgb report()
	{
		int prim = extent();
		return rgb(prim, prim, prim);
	}
};

class pixel : public rgb
{
public:
	pixel () : rgb()
	{
	}

	pixel (rgb p_rgb)
	{
		r = p_rgb.r;
		g = p_rgb.g;
		b = p_rgb.b;
	}

	pixel (int p_r, int p_g, int p_b) : rgb(p_r, p_g, p_b)
	{
	}

	pixel_info operator - (pixel p_to)
	{
		return pixel_info(p_to.r - r, p_to.g - g, p_to.b - b);
	}
};

class pixel_parser {
	draw_surface* in;
	int x, x_end, y;
public:
	pixel_parser(int p_x1, int p_x2, int p_y, draw_surface* p_in)
	{
		x = p_x1;
		x_end = p_x2;
		y = p_y;
		in = p_in;
	}

	~pixel_parser()
	{
	}

	inline void advance()
	{
		x++;
	}

	inline int get_x()
	{
		return x;
	}

	inline int get_y()
	{
		return y;
	}

	inline int get_left()
	{
		return x - 1;
	}

	inline int get_right()
	{
		return x - 1;
	}

	inline bool at_end()
	{
		return x >= x_end;
	}
	
	inline pixel read(int offset_x = 0, int offset_y = 0)
	{
		return in->read_pxl(x + offset_x, y + offset_y);
	}

	inline pixel read_l()
	{
		return read(-1);
	}

	inline pixel read_r()
	{
		return read(+1);
	}

	inline pixel read_t()
	{
		return read(0, -1);
	}

	inline pixel read_b()
	{
		return read(0, +1);
	}

	inline pixel_info diff(int offset_x, int offset_y = 0)
	{
		return read() - read(offset_x, offset_y);
	}

	inline pixel_info diff_l()
	{
		return diff(-1);
	}

	inline pixel_info diff_r()
	{
		return diff(+1);
	}

	inline pixel_info diff_t()
	{
		return diff(0, -1);
	}

	inline pixel_info diff_b()
	{
		return diff(0, +1);
	}
};

typedef quantum_2d tracer_unit;
typedef std::list<tracer_unit> tracer_type;
typedef appendable_element<tracer_type*> tracer_base;

class tracer : public tracer_base {
public:
	tracer* connects;
	inline tracer ()
	{
		data = new tracer_type;
		connects = NULL;
	}

	inline ~tracer ()
	{
		data->clear();
		delete(data);
	}

	inline void store_lef(int x, int y)
	{
		data->push_front(tracer_unit(x, y));
	}

	inline void store_rig(int x, int y)
	{
		data->push_back(tracer_unit(x, y));
	}

	inline tracer_unit delete_lef()
	{
		tracer_unit ret = data->front();
		data->pop_front();
		return ret;
	}

	inline tracer_unit delete_rig()
	{
		tracer_unit ret = data->back();
		data->pop_back();
		return ret;
	}

	inline void add_inny(tracer* trace)
	{
		// todo: differentiate between an inny on the same level and an inny inside this tracer
		// do this with two separate pointers - within, beside?
		if (connects == NULL) { // nothing connected to this tracer (but could still be connected to tracer in this circuit)
			connects = trace;
		} else { // tracer already connected.  stick tracer in between
			trace->connects = connects;
			connects = trace;
		}
	}
};

// the segments that result from parsing a horizontal row of screen pixels
class row_segment {
public:
	int lef_x;
	int rig_x;
	tracer* lef_trace;
	tracer* rig_trace;
	row_segment* last_connection; // last connection below this segment

	row_segment()
	{
		lef_trace = rig_trace = NULL;
		last_connection = NULL;
	}

	row_segment(int p_lef_x, int p_rig_x, tracer* p_lef_trace, tracer* p_rig_trace)
	{
		lef_x = p_lef_x;
		rig_x = p_rig_x;
		lef_trace = p_lef_trace;
		rig_trace = p_rig_trace;
		last_connection = NULL;
	}
};
typedef std::vector<row_segment*> row_seg_set;
typedef row_seg_set::iterator row_segment_it;


template <class element_type>
class linear_tracer {

	double threshold;
	double ct;
	point_2dd vert_acc;
	line_2d_dydx curr_line;
	//line_2d_gen<element_type> curr_line;
	element_type start;
	element_type prev;

public:

	linear_tracer(double p_threshhold = 1.01)
	{
		threshold = p_threshhold;
		init();
	}

	~linear_tracer()
	{
	}

	void init()
	{
		vert_acc = VECT_2D_DOUBLE_ORIG;
		ct = 0.0;
	}

	bool compare(element_type p_p)
	{
		double offset = curr_line.distance(p_p.center());
		//show_text("offset", offset);
		return ((offset > threshold) || (offset < -threshold));
	}

	bool update(element_type curr)
	{
		bool ret = false;

		if (ct == 0.0) {
			start = curr;
		} else if (prev != start) {			
			curr_line = line_2d_dydx(start.center(), prev.center(), vert_acc / ct);

			/* process corner */
			if (compare(curr)) {
				ct = 1.0;
				start = prev;
				vert_acc = start.center();
				ret = true;
			} 
		}

		vert_acc += curr.center();
		ct++;
		prev = curr;

		return ret;
	}

	element_type get_corner()
	{
		return prev;
	}

	line_2d_gen<element_type> get_curr_line()
	{
		return curr_line;
	}
};

template <class gen>
class linear_auditor {
	
	line_2d_gen<gen> line; 
	double tolerance;

public:

	linear_auditor(line_2d_gen<gen> p_line, double p_tolerance = 0.51)
	{
		line = p_line;
		tolerance = p_tolerance;
	}

	bool audit(gen p_p)
	{
		double offset = line.offset(p_p.center());
		//show_point(gen(int(line.func(p_p) + .5), int(p_p.y)), RGB_BLUE, 0);
		if (offset > tolerance || offset < -tolerance) {
			//show_text(line.get_dxdy().func(p_p.y + .5) - (p_p.x + .5));
			//show_text(p_p.x + .5);
			//show_text("");
			//show_text(offset);
		}
		return (offset > tolerance) || (offset < -tolerance);
	}

	/*
	bool audit_dydx(gen p_p)
	{
		point_2d p = p_p.center();
		double y = line.dydx.func(p.x);
		//if ((fabs(p.y - y) > tolerance) || (fabs(p.y - y) < -tolerance)) show_text(p.y - y);
		//show_text(line.dydx.m);
		//show_text(line.dxdy.m);
		//show_point(vect_2d<int>(p.x, y), RGB_RED, 1);
		return (p.y - y > tolerance) || (p.y - y < -tolerance);
	}
	*/

	/*
	bool audit(gen p_p)
	{
		double dep = line.func(p_p);
		//if ((fabs(p.x - x) > tolerance) || (fabs(p.x - x) < -tolerance)) show_text(p.x - x);
		show_text(line.dxdy.m);
		show_text(line.dydx.m);
		//show_point(vect_2d<int>(x, p.y), RGB_RED, 1);
		return (p.x - x > tolerance) || (p.x - x < -tolerance);
	}

	bool audit(gen p_p)
	{
		if (line.is_dydx()) {
			return audit_dydx(p_p);
		} else {
			return audit_dxdy(p_p);
		}
	}
	*/
};

class feature {
	// a corner is a curve with radius of (0, 0) and arc = angle
	// an edge is a curve with radius of (+inf, +inf) and arc = segment length
	float radius[2];
	float arc;

	inline feature(float p_arc, float p_r1, float p_r2)
	{
		radius[0] = p_r1;
		radius[1] = p_r2;
		arc = p_arc;
	}
};

typedef std::vector<feature> feature_bank;
typedef std::vector<tracer_unit> circuit_type;

class vertex {
public:
	tracer_unit pixel;
	int index;

	vertex (tracer_unit p_pixel, int p_index)
	{
		pixel = p_pixel;
		index = p_index;
	}
};

/* this is a class that holds a complete circuit of tracers once
     a blob has been completely parsed */
class shape {
public:
	std::vector<vertex> verts;
	tracer_unit prev_el;
	linear_tracer<tracer_unit> lt;
	circuit_type circuit;

	void log_vertex(tracer_unit v, int i)
	{
		verts.push_back(vertex(v, i));
		//show_point(v, RGB_RED);
		//show_windows();
	}
	
	void process_element(tracer_unit u)
	{
		if (read_point_in(u.x, u.y) == RGB_WHITE) {
			color_point(u.x, u.y, RGB_RED);
		} else {
			color_point(u.x, u.y, RGB_BLUE);
		}

		circuit.push_back(u);
		prev_el = u;
	}

	void process_corners()
	{
		int o_ct = 0;
		// circuit_type::iterator start_e;

		//show_point(circuit.begin()->alias(), RGB_PURPLE);
		// start_e = circuit.begin();

		for (int i = 0; i < 2; i++) {
			//for (circuit_type::iterator e = circuit.begin(); !(i == 1 && o_ct == 30) && (e != circuit.end()); ++e) {
		//for (circular_iterator<circuit_type::iterator> j(circuit.end(), circuit.begin()); j.cont_end() && (j.get_overlap_ct() < 30); j.iterate()) {
			for (int j = 0; !(i == 1 && o_ct == 30) && j < circuit.size(); j++) {


				/*
				if (i) {
					color_point(e->x, e->y, RGB_RED);
				} else {
					color_point(e->x, e->y, RGB_GREEN);
				}
				*/

			if (lt.update(circuit[j])) {
					//line_2d_dydx conn(*start_e, *e);
					//show_point(circuit[j].alias(), RGB_YELLOW);
					//show_windows();

					//max_value<double, circuit_type::iterator> real_corner(0.0, start_e);
					
					// circuit_type::iterator ov = start_e;
					/*
					for (ov = start_e; (ov != e) && (ov != circuit.end()); ++ov) {
						//show_point(ov->alias(), RGB_GREEN);
						//real_corner.update(fabs(conn.distance(*ov)), ov);
						//show_text(conn.distance(*ov));
						//show_windows();
					}

					if (ov != circuit.end()) {
						show_point(ov->alias(), RGB_GREEN);
						real_corner.update(fabs(conn.distance(*ov)), ov);
						//show_text(conn.distance(*ov));
						//show_windows();
					}
					*/

				log_vertex(circuit[j], j);
					//show_windows();
					//start_e = e;
				}
			
				//prev_el = *e;
				o_ct += i;
			}
		}
	}

	void correct_verts()
	{
		for (circular_iterator<unsigned> i(verts.size(), 0, verts.size() - 1); i.cont(); i.iterate()) {
			max_value<double> mx;

			for (
				circular_iterator<unsigned> j(circuit.size(), verts[i.wrap_iter()].index, verts[i.wrap_iter(1)].index); 
				j.cont(); 
				j.iterate()) {
				
				//double dist1 = verts[i.wrap_iter()].pixel.center().dist(circuit[j.wrap_iter()].center());
				//double dist2 = verts[i.wrap_iter(2)].pixel.center().dist(circuit[j.wrap_iter()].center());
				//mx.update(dist1 + dist2, j.wrap_iter());
				tri_2d<point_2d> tri(
					verts[i.wrap_iter()].pixel,
					circuit[j.wrap_iter()],
					verts[i.wrap_iter(2)].pixel
				);
				mx.update(abs(tri.area_x2()), j.wrap_iter());
			}
			
			if (mx.report_key() != -1) {
				vect_2d<int> p = circuit[mx.report_key()].alias();
				show_point(p, RGB_PURPLE);
				show_windows();
			} else {
				show_text("I iter", i.get_iter());
			}
		}
	}

	void correct_verts2()
	{
		for (circular_iterator<unsigned> i(verts.size()); i.not_wrap(); i.iterate()) {
			max_value<double> perim;

			/*
			show_text("s", i.get_iter());
			show_text("s", i.wrap_iter(1));
			show_text("s", i.wrap_iter(2));
			show_text("next");
			*/

			//show_text("e", verts[i.wrap_iter(2)].index);
			//show_text("start correct");
			bool first = false;

			for (circular_iterator<unsigned> j(circuit.size(), verts[i.get_iter()].index); j.get_iter() != verts[i.wrap_iter(2)].index; j.iterate()) {
			//for (int j = verts[i].index; j < verts[(i + 2) % verts.size()].index; j++) {
				double dist1 = circuit[j.get_iter()].alias().dist(verts[i.get_iter()].pixel.alias());
				double dist2 = circuit[j.get_iter()].alias().dist(verts[i.wrap_iter(2)].pixel.alias());
				double dist_total = dist1 + dist2;

				if (j.get_iter() == verts[i.wrap_iter(1)].index) {
					first = true;
				}

				/*
				if (first) {
					show_point(circuit[j.get_iter()].alias(), RGB_BLUE);
				} else {
					show_point(circuit[j.get_iter()].alias(), RGB_WHITE);
				}
				*/

				//show_windows();

				//if (j.get_iter() % 10 == 0)
					//show_text("j", j.get_iter());

				//show_point(verts[i].pixel.alias_pixel(), RGB_GREEN);
				//show_windows();

				//show_point(verts[(i + 1) % verts.size()].pixel.alias_pixel(), RGB_BLUE);
				//show_point(circuit[j].alias_pixel(), RGB_GREY);

				perim.update(dist_total, j.get_iter());
				
				//if (j.get_iter() == 0)
				//show_text("j", j.get_iter());
				//show_text("2", dist2);
				//if (dist_total > 112.0)
			}

			if (perim.report_key() >= 0) {
				//show_text("x", circuit[perim.report_key()].alias().x);
				//show_text("y", circuit[perim.report_key()].alias().y);
				show_point(circuit[perim.report_key()].alias(), RGB_GREEN);
				show_windows();
			} else {
				//show_text("didn't find it");
			}
			
		}
	}

	void process_test()
	{
		for (unsigned i = 0; i < circuit.size(); i++) {
			point_2d p1 = circuit[0].to_cart_coord();
			point_2d p2 = circuit[i % circuit.size()].to_cart_coord();
			point_2d bot = p1.center(p2);
			point_2d pc = circuit[int(i * 0.5 + 0.5)].to_cart_coord();

			double c = pc.dist(bot);
			double s = 0.5 * p1.dist(p2);
			double r = s / sin(2 * atan(c / s));

			//if 
		}
	}

	/*
	void process_test4()
	{
		mean<quantum_2d> ave;

		for (unsigned i = 0; i < circuit.size(); i++) {
			ave.update(circuit[i]);
			line_2d_dydx l(
		}
	}
	*/

	/*
	void process_test3()
	{
		for (unsigned int i = 0; i < verts.size(); i++) {
			//show_point(verts[i]);
		}

		//return;

		for (unsigned int i = 1; i < verts.size() - 1; i++) {
			point_2d p1 = verts[i].to_cart_coord();
			point_2d p2 = verts[i - 1].to_cart_coord();
			point_2d p3 = verts[i + 1].to_cart_coord();
			line_2d_dydx l1(p1, p2);
			line_2d_dydx l2(p1, p3);
			point_2d mid1 = p1.center(p2);
			point_2d mid2 = p1.center(p3);
			line_2d_dydx ray1 = l1.perp_line(mid1);
			line_2d_dydx ray2 = l2.perp_line(mid2);
			show_line(ray1, RGB_BLUE);
			show_line(ray2, RGB_GREEN);
			show_windows();
			point_2d c = ray1.intersect(ray2);
			//show_point(c.alias_pixel());
			show_point(verts[i]);
		}
	}
	*/

	/*
	void process_test2()
	{
		mean<double> rad;
		double prev = 0.0;
		double ac = 0.0;
		double ta = 0.0;

		for (unsigned int i = 0; i < verts.size(); i++) {
			//show_point(verts[i], RGB_BLUE);
			angle_2d<double> a(verts[i % verts.size()], verts[(i + 1) % verts.size()], verts[(i + 2) % verts.size()]);
			double dist1 = verts[i % verts.size()].dist(verts[(i + 1) % verts.size()]);
			double dist2 = verts[(i + 1) % verts.size()].dist(verts[(i + 2) % verts.size()]);
			double f1 = dist1 * a.supplement().angle;
			double f2 = dist2 * a.supplement().angle;
			//show_text(a.supplement().to_degrees());
			//show_text(dist1 / a.supplement().angle);
			//show_text(dist2 / a.supplement().angle);
			//show_text(f);
			bool b1 = fabs(f1) > 15.0;
			bool b2 = fabs(f2) > 15.0;

			if (b1 && b2) {
				// corner
				show_point(verts[(i + 1) % verts.size()], RGB_RED);
				prev = 0.0;
				ac = 0.0;
				ta = 0.0;
			} else if (!b1 && !b2) {
				double r = dist2 / a.supplement().angle;
				double change;
				
				r = ac / ta;

				//show_text((rad.calc() - r) / rad.calc());
				rad.update(r);
				
				//show_text(r);

				if (prev > r) {
					change = (prev - r) / r;
				} else {
					change = (r - prev) / prev;
				}

				if (change > 0.6 && (prev != 0.0)) {
					// curve - change in radius
					//prev = 0.0;
					ac = 0.0;
					ta = 0.0;
					show_point(verts[(i + 1) % verts.size()], RGB_BLUE);
				} else {
					// curve
					show_point(verts[(i + 1) % verts.size()], RGB_GREEN);
				}
				
				show_text("radius", r);
				show_text("change", change * 100.0);
				//show_text("");
				prev = r;
				//show_text(dist2);
			} else {
				show_point(verts[(i + 1) % verts.size()], RGB_YELLOW);
				prev = 0.0;
				ac = 0.0;
				ta = 0.0;
			}
			
			if (i > 25) {
				show_text("f1", f1);
				show_text("f2", f2);
			}

			show_windows();

			//show_text(verts[(i + 1) % verts.size()].y);
			//show_windows();
			//show_text("");
			//show_text((i + 10) % verts.size());
			ac += dist1;
			ta += a.supplement().angle;
		}
	}
	*/

	void process_curves()
	{
		mean<point_2d> ac(POINT_2D_ORIG);
		mean<double> ar;

		for (unsigned i = 0; i < circuit.size(); i++) {
			point_2d p1 = circuit[0].to_cart_coord();
			point_2d p2 = circuit[i % circuit.size()].to_cart_coord();
			point_2d bot = p1.center(p2);
			point_2d pc = circuit[int(i * 0.5 + 0.5)].to_cart_coord();
			ray_2d se(p1, p2);
			ray_2d norm(float(se.angle - PI * 0.5), pc);
			double c = pc.dist(bot);

			if (c > 2.0) {
				double s = 0.5 * p1.dist(p2);
				double r = s / sin(2 * atan(c / s));
				ar.update(r);
				point_2d cn = norm.extend(ar.calc()).alias_pixel();
				ac.update(cn);

				show_text(r);
				//show_point(p2.alias_pixel(), RGB_GREEN);
				show_point(ac.calc());
				//show_windows();
			} else {
				//show_point(p2.alias_pixel(), RGB_BLUE);
			}
		}
	}

	void process_between(tracer_unit p1, tracer_unit p2)
	{
		if (p1.x < p2.x) {
			if (p1.y < p2.y) { // case 1
				process_element(tracer_unit(p1.x, p1.y));
				for (int x = p1.x + 1; x < p2.x; x++) {
					process_element(tracer_unit(x, p2.y));
				}
			} else if (p1.y > p2.y) { // case 2
				for (int x = p1.x; x < p2.x; x++) {
					process_element(tracer_unit(x, p1.y));
				}
			} else { // case 3
				for (int x = p1.x; x < p2.x; x++) {
					process_element(tracer_unit(x, p1.y));
				}
			}
		} else if (p1.x > p2.x) {
			if (p1.y < p2.y) { // case 4
				for (int x = p1.x; x > p2.x; x--) {
					process_element(tracer_unit(x, p1.y));
				}
			} else if (p1.y > p2.y) { // case 5
				process_element(tracer_unit(p1.x, p1.y));
				for (int x = p1.x - 1; x > p2.x; x--) {
					process_element(tracer_unit(x, p2.y));
				}
			} else { // case 6
				for (int x = p1.x; x > p2.x; x--) {
					process_element(tracer_unit(x, p1.y));
				}
			}
		} else {
			process_element(tracer_unit(p1.x, p1.y));
		}
	}

	void process_circuit(feature_bank* p_bank, tracer* p_circuit_start, rgb p_color)
	{
		tracer_unit first, last, p(VECT_2D_INT_UNDEF_VALUE);
		tracer_unit prev(VECT_2D_INT_UNDEF_VALUE);
		tracer* prev_tracer;
		tracer_type::iterator prev_corner;
		bool rep;
		prev_tracer = p_circuit_start;
		prev_corner = p_circuit_start->data->begin();

		show_text("begin");
		//show_windows();

		// iterate through tracers in circuit
		rep = false;
		for (tracer* t = p_circuit_start; t != (tracer*)_alt_null.get_null(ALT_NULL_NA); t = (tracer*)t->next) {
			// iterate through all points in tracer
			for (tracer_type::iterator u = t->data->begin(); u != t->data->end(); ++u) {				
				if (p != VECT_2D_INT_UNDEF_VALUE) {
					process_between(p, *u);
				}
				p = *u;
			}

			// process connector (inny) here on the fly? 
			// or maybe queue them to process after outy?
			if (t->connects != NULL) {
				//shape sh(t->connects);
			}

			rep = true;
		}
	
		process_between(p, *p_circuit_start->data->begin());
		p = *p_circuit_start->data->begin();

		process_corners();
		//correct_verts();
		//process_curves();
		//process_test3();
		//process_test2();
		//identify();
}

/*
void identify()
{
	mean<point_2d> mea(POINT_2DD_ORIG);
	point_2d start = circuit[0];

	for (int i = 0; i < circuit.size(); i++) {
		color_point(circuit[i], RGB_RED);
		mea.update(circuit[i]);
		color_point(mea.calc(), RGB_GREEN);
		tri_2d<point_2d> tri(start, mea.calc(), circuit[i]);
	}
}
*/

public:

	feature_bank perimeter;
	std::vector<feature_bank*> voids;

	// shape is created when the perimeter is completed
	// and so the proto-shape is completed
	shape(tracer* p_outy)
	{
		// process perimeter (outie)
		process_circuit(&perimeter, p_outy, RGB_GREY);		
	}

	inline void add_inny(tracer* p_inny)
	{
		voids.push_back(new feature_bank());
		process_circuit(voids.back(), p_inny, RGB_RED);
	}
};

class circuit_bank {
	std::vector<shape*> shapes;
	std::vector<row_segment*> connections; // temp buffer for segments connected above segment
public:
	void clear_connections()
	{
		connections.clear();
	}

	void add_connection(row_segment* seg)
	{
		connections.push_back(seg);
	}

	/*
	void trace2(int lef_x, int rig_x, int y, tracer*& lef_trace, tracer*& rig_trace)
	{
		// connections are segments above the current scan line that connect to scan line
		// last_connection is  the 
		if (connections.size()) { // if there were connections above segment
			lef_trace = connections.front()->lef_trace;
			lef_trace->store_lef(lef_x, y);
			//show_point(quantum_2d(lef_x, y), RGB_BLUE);
			//show_point(quantum_2d(unit.x, unit.y), RGB_GREEN);
			//show_windows();

			std::vector<row_segment*>::iterator i = connections.begin();
			while (true) {
				if ((*i)->last_connection != NULL) { // diverge case - has a previous connection
					(*i)->last_connection->lef_trace->delete_lef();
					tracer_unit unit = (*i)->last_connection->rig_trace->delete_rig();
					(*i)->last_connection->rig_trace = lef_trace = new tracer();
					// when we do an inny, we go in the reverse direction
					lef_trace->store_lef(lef_x, y);
					//show_point(quantum_2d(lef_x, y), RGB_GREEN);
					//lef_trace->store_lef(lef_x - 1, y - 1);
					//lef_trace->store_rig(unit.x + 1, unit.y - 1);
					lef_trace->store_rig(unit.x, unit.y); 
					//show_point(quantum_2d(unit.x, unit.y), RGB_ORANGE);
					//show_point(quantum_2d(unit.x, y), RGB_GREY);
					//show_windows();
				}
				

				std::vector<row_segment*>::iterator prev = i;
				++i;
				if (i == connections.end()) {
					break;
				}

				color_point_in(lef_x + 1, y + 1, RGB_BLACK);
				color_point_in(rig_x - 1, y + 1, RGB_BLACK);
				show_text("lef_x", lef_x);
				show_text("rig_x", rig_x);
				show_text("y", y);

				// converge case
				if ((*prev)->rig_trace->append((*i)->lef_trace)) {	// complete inny circuit
					connections.front()->lef_trace->add_inny((*i)->lef_trace);
				}
			}

			rig_trace = connections.back()->rig_trace;
			rig_trace->store_rig(rig_x, y);

			//show_point(quantum_2d(rig_x, y), RGB_BLUE);
		} else { // new tracer
			lef_trace = rig_trace = new tracer();
			lef_trace->store_lef(lef_x, y);
			//show_point(quantum_2d(lef_x, y), RGB_GREY);
			lef_trace->store_rig(rig_x, y);
			//show_point(quantum_2d(rig_x, y), RGB_RED);
		}
	}
	*/
	
	void trace(int lef_x, int rig_x, int y, tracer*& lef_trace, tracer*& rig_trace)
	{
		std::vector<row_segment*>::iterator prev;

		// connections are segments above the current scan line that connect to scan line

		if (connections.size()) { // if there were connections above segment
			lef_trace = connections.front()->lef_trace;
			lef_trace->store_lef(lef_x, y);

			std::vector<row_segment*>::iterator i = connections.begin();
			while (true) {
				if ((*i)->last_connection != NULL) { // diverge case - has a previous connection
					connections.front()->lef_trace->delete_lef();
					tracer_unit unit = (*i)->last_connection->rig_trace->delete_rig();

					(*i)->last_connection->rig_trace = lef_trace = new tracer();
					// when we do an inny, we go in the reverse direction
					lef_trace->store_lef(lef_x, y);
					lef_trace->store_lef(lef_x - 1, y - 1);
					lef_trace->store_rig(unit.x + 1, unit.y - 1);
					lef_trace->store_rig(unit.x, unit.y); 
				} else {
				}

				if (i != connections.begin()) {
					//color_point_in((*prev)->rig_x + 1, y, RGB_BLACK);
					//color_point_in((*i)->lef_x - 1, y, RGB_BLUE);
					(*prev)->rig_trace->store_rig((*prev)->rig_x + 1, y);
					(*i)->lef_trace->store_lef((*i)->lef_x - 1, y);
					//show_windows();
				}

				prev = i;
				++i;
				if (i == connections.end()) {
					break;
				}

				// converge case
				if ((*prev)->rig_trace->append((*i)->lef_trace)) {	// complete inny circuit
					connections.front()->lef_trace->add_inny((*i)->lef_trace);
				} else {
				}
			}

			rig_trace = connections.back()->rig_trace;
			rig_trace->store_rig(rig_x, y);

			/*
			color_point_in(rig_x, y, RGB_RED);
			show_windows();
			*/
		} else { // new tracer
			lef_trace = rig_trace = new tracer();
			lef_trace->store_lef(lef_x, y);
			lef_trace->store_rig(rig_x, y);
		}
	}

	// last connection is the most downward and rightward connection to the "connection" segment
	void set_connections(row_segment* seg)
	{
		for (std::vector<row_segment*>::iterator i = connections.begin(); i != connections.end(); ++i) {
			(*i)->last_connection = seg;
		}
	}

	void tie_ends(row_seg_set* segs)
	{
		for (row_seg_set::iterator i = segs->begin(); i != segs->end(); ++i) {
			if ((*i)->last_connection == NULL) {
				if ((*i)->rig_trace->append((*i)->lef_trace)) {
					new_shape((*i)->lef_trace);
				}
			}
		}
	}

	void new_shape(tracer* p_perimeter)
	{
		shapes.push_back(new shape(p_perimeter));
	}
};

class row_parser {
	row_seg_set segments;
	bool seg_at_end;
	int xcut;
public:
	int test;
	row_segment_it locate_segment_it;

	inline row_parser(int p_xcut = 0)
	{
		seg_at_end = true;
		xcut = p_xcut;
	}

	inline ~row_parser()
	{
		clear();
	}

	inline row_seg_set* get_segs()
	{
		return &segments;
	}

	inline row_segment* add_segment(int p_x1, int p_x2, tracer* p_lef_trace, tracer* p_rig_trace)
	{
		row_segment* seg = new row_segment(p_x1, p_x2, p_lef_trace, p_rig_trace);
		segments.push_back(seg);
		return seg;
	}

	inline void init_locate_segment()
	{
		locate_segment_it = segments.begin();
	}

	row_segment_it locate_segment(int x)
	{
		while (locate_segment_it != segments.end() && (*locate_segment_it)->rig_x < x) {
			++locate_segment_it;
		}

		seg_at_end = (locate_segment_it == segments.end());
		return locate_segment_it;
	}

	inline bool segment_found()
	{
		return !seg_at_end;
	}

	inline bool segment_null()
	{
		return locate_segment_it == segments.end();
	}

	void clear()
	{
		for (row_segment_it i = segments.begin(); i != segments.end(); ++i) {
			delete *i;
		}
		segments.clear();
	}

	inline bool parse_pixel(pixel_parser parse, int extent, int offset_x = 0, int offset_y = 0)
	{
		if (parse.get_x() + offset_x < xcut ) {
			return true;
		} else {
			int ext = parse.diff(offset_x, offset_y).extent();
			return ext <= extent;
		}
	}

	void parse(draw_surface* in, row_parser* prev, circuit_bank& circuits, int extent, int y, int x1, int x2)
	{
		pixel_parser parse(x1, x2, y, in);
		int left, right;

		clear();
		prev->init_locate_segment();

		while (!parse.at_end()) {
			// while broken color
			while (!parse.at_end() && !parse_pixel(parse, extent, -1)) {
				parse.advance();
			}			
			left = parse.get_left();

			//poly = NULL;
			circuits.clear_connections();
			row_segment* prev_it = NULL;

			// while continuous color
			while (!parse.at_end() && parse_pixel(parse, extent, -1)) {
				if (parse_pixel(parse, extent, 0, -1)) {
					row_segment_it it = prev->locate_segment(parse.get_x());
					if (prev->segment_found() && (*it != prev_it)) {
						circuits.add_connection(*it);
						prev_it = *it;
					}
				}
				parse.advance();
			}			
			right = parse.get_right();

			if (left <= right) {
				tracer* lef_tracer = NULL;
				tracer* rig_tracer = NULL;
				circuits.trace(left, right, y, lef_tracer, rig_tracer);
				row_segment* seg = add_segment(left, right, lef_tracer, rig_tracer);
				circuits.set_connections(seg);
			}
		}

		circuits.tie_ends(prev->get_segs());
	}

	void report(draw_surface* p_in, int p_y, rgb color = RGB_GREEN)
	{
		for (row_segment_it i = segments.begin(); i != segments.end(); ++i) {
			p_in->line_seg((int)(*i)->lef_x, p_y, (int)(*i)->rig_x, p_y, color);
		}
	}
};

void group_filter(draw_surface* in, draw_surface* out = NULL)
{
	int length = in->get_dims().x;
	int end = length - 1;
	row_parser row1, row2;
	row_parser* curr = &row1;
	row_parser* prev = &row2;
	int extent = 10;
	circuit_bank circuits;

	for (int y = 0; y < in->get_dims().y - 1; y++) {
		curr->parse(in, prev, circuits, extent, y, 0, length);
		swap(curr, prev);
	}
	circuits.tie_ends(prev->get_segs());
}

#endif // !INC_COMPUTER_VISION