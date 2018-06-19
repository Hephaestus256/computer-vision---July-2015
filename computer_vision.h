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

#ifndef INC_COMPUTER_VISION
#define INC_COMPUTER_VISION

#define OVERLAP_CT 3

#define DEFAULT_LINEAR_TOL 0.51
#define DEFAULT_ARC_TOL 1.01

#define TENT_VERTEX_NULL tent_vertex(ROUND_ROBIN_ELEMENT_NULL)

#include <string.h>
#include <math.h>
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\graphics_debug.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\math_1d.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\math_2d.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\my_stl.h"
#include <vector>
#include <list>

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

	bool threshhold(pixel p_to, int threshhold)
	{
		return (p_to - *this).extent() > threshhold;
	}
};

template <class sample_type>
class sampler {
	
	class cannon {
	public:

		sample_type sample;
		int pos;

		cannon(sample_type p_sample, int p_pos = 0)
		{
			sample = p_sample;
			pos = p_pos;
		}
	};

	sample_type* source;
	sample_type* curr;
	quantum_2d pos;
	quantum_2d first_pos;
	quantum_2d last_pos;
	int source_width;
	cannon cannon_x;
	cannon* cannon_y;

	sample_type read_sample()
	{
		return *curr;
	}

public:

	void init(sample_type* p_source, quantum_2d c1, quantum_2d c2, int p_source_width = -1)
	{
		clear();

		curr = source = p_source + c1.x + (c1.y * source_width); // init to top left corner of source
		first_pos = pos = c1;
		last_pos = c2;
		source_width = p_source_width;
		cannon_x = cannon(source[0]); // init to first sample
		cannon_y = new cannon[c2.x - c1.x + 1];

		for (int i = 0; i <= c2.x - c1.x; i++) {
			cannon_y[i] = cannon(sample[i]);
		}
	}

	void clear()
	{
		if (source != NULL) {
			delete cannon_y;
		}

		source = NULL;
	}

	sampler(sample_type* p_source, quantum_2d c1, quantum_2d c2, int source_width = -1)
	{
		init(p_source, c1, c2, source_width);
	}

	~sampler()
	{
		clear();
	}

	// increments iteration
	// returns if a new row has been entered
	inline bool iterate()
	{
		pos.x++;
		curr++;

		if (pos.x > last_pos.x) {
			pos.x = first_pos.x;
			curr += source_width;
			pos.y++;
			return true;
		}

		return false;
	}

	inline bool cont()
	{
		return pos.y <= last_pos.y;
	}

	bool test_horz()
	{
		source_type sample = read_sample();

		if (sample.threshhold(cannon_x)) {
			cannon_x = cannon(sample);
			return true;
		} else {
			return false;
		}
	}

	bool test_vert()
	{
		return read_sample().threshhold(cannon_y[pos.x - first_pos.x]);
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

	inline tracer_unit get_last_lef()
	{
		return data->front();
	}

	inline tracer_unit get_last_rig()
	{
		return data->back();
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

class vertex {
public:
	point_2d pixel;
	int index;
	double area;

	vertex()
	{
	}

	vertex (int p_index)
	{
		index = p_index;
	}

	vertex (point_2d p_pixel, int p_index, double p_area = 0.0)
	{
		pixel = p_pixel;
		index = p_index;
		area = p_area;
	}

	inline void invalidate()
	{
		index = -1;
	}

	inline bool valid()
	{
		return index != -1;
	}
};

class tent_vertex {
public:
	round_robin_element<> index;
	double area;

	tent_vertex()
	{
	}

	tent_vertex(round_robin_element<> p_index)
	{
		index = p_index;
	}

	tent_vertex(round_robin_element<> p_index, double p_area)
	{
		index = p_index;
		area = p_area;
	}

	tent_vertex(vertex v)
	{
		index = v.index;
		area = v.area;
	}

	inline bool valid()
	{
		return index.valid();
	}

	inline void invalidate()
	{
		index.invalidate();
	}

	inline char* report()
	{
		sprintf(s_report_text, "area: %f, index: %f, lap: %f", float(area), float(index.i), float(index.lap));
		return s_report_text;
	}
};

/*
template <class unit_type = point_2d, class container_type = std::vector<unit_type>>
class area_tracer {
public:
	acc_area<unit_type> acc;
	round_robin_iterator<unit_type, container_type> start, end;
	int first;

	inline void init(container_type& p_container, int p_start = 0)
	{
		start = end = round_robin_iterator<unit_type, container_type>(p_start, p_container);
		acc.init(get_element(p_start));
	}

	inline void init_rev(tracer_type* p_tracer)
	{
	}

	inline void set_start(int p_start)
	{
		start.e = p_start;
		acc.init(get_element(p_start));
	}

	area_tracer ()
	{
		first = -2;
	}

	area_tracer (container_type& p_container, int p_start = 0)
	{
		init(p_container, p_start);
	}

	unit_type get_element(int n)
	{
		return start.get_element(n);
	}

	void reset()
	{
		start = end;
		acc.init(*start);
	}

	void reset(int n)
	{
		start.e = end.e = n;
		acc.init(*start);
	}

	void iterate_tail()
	{
		++start;
		acc.sub_col_x2(*start);
	}

	inline void iterate_head()
	{
		++end;
		acc.add_col_x2(*end);
	}

	inline void iterate_head_rev()
	{
		--end;
		acc.sub_col_x2(*end);
	}

	inline bool nonlinear(double th = 0.5)
	{
		return fabs(calc_ave_height()) > th;
	}

	inline bool parse_span(double th = 0.5)
	{
		if (nonlinear(th)) {
			reset();

			//return true;

			if (first < 0) {
				first = end.e.i - 1;
				return true;
			} else {
				return true;
			}
		} else {
			return false;
		}
	}

	bool iterate_span(double th = 0.5)
	{
		iterate_head();
		return parse_span(th);
	}

	bool iterate_span_rev(double th = 0.5)
	{
		iterate_head_rev();
		return parse_span(th);
	}

	int correct_vert(int tent, int e1, int e2)
	{
		max_value<double> mx;
		round_robin_element<> i1 = e1;
		round_robin_element<> i2 = round_robin_element<>(e2).fix_end(e1, start.vect->size());

		//put_background();

		for (round_robin_element<> i = i1; i < i2; i.inc(start.vect->size())) {
		
			int n1 = i.interpol(i1, start.vect->size()).i;
			int n2 = i.interpol(i2, start.vect->size()).i;

			tri_2d<> tri(
				get_element(n1),
				get_element(n2),
				get_element(i.i)
			);

			mx.update(fabs(tri.area_x2()), i.i);
		}

		return mx.report_key();
	}

	int correct_vert2(int tent, int e1, int e2)
	{
		int i1 = interpolate(0.5, tent, round_robin_element<>(e1).fix_begin(tent).effective(start.vect->size()));
		int i2 = interpolate(0.5, tent, round_robin_element<>(e2).fix_end(tent).effective(start.vect->size()));

		max_value<double> mx;

		for (round_robin_element<> i = i1; i < round_robin_element<>(i2).fix_end(i1, start.vect->size()); i.inc(start.vect->size())) {
			tri_2d<> tri(
				get_element(i1),
				get_element(i2),
				get_element(i.i)
			);
			mx.update(fabs(tri.area_x2()), i.i);
			//show_point(get_element(i.i), RGB_ORANGE, 0);
			//mess(i.report(), "i");
			//show_windows();
		}

		//show_windows();

		//}

		return mx.report_key();
	}

	double calc_area_x2()
	{
		return acc.calc_tot_area_x2();
	}

	double calc_area()
	{
		return acc.calc_tot_area();
	}

	double calc_ave_height()
	{
		return acc.calc_ave_height();
	}

	unit_type get_start_sample()
	{
		return *start;
	}

	unit_type get_end_sample()
	{
		return *end;
	}

	unit_type get_vertex()
	{
		return *(end - 1);
	}

	double get_chord_len_sqrd()
	{
		return get_start_sample().dist_sqrd(get_end_sample());
	}

	double diff_ave_height(area_tracer<unit_type, tracer_type>& at)
	{
		return calc_ave_height() - at.calc_ave_height();
	}

	bool cont()
	{
		return end < round_robin_element<int>(0, 1);
	}

	bool cont_first()
	{
		//if (end > round_robin_element<>(0, 1))
		//	mess(end.report(), "cont first");

		return end < round_robin_element<int>(first, 1);
		//return end < round_robin_element<int>(0, 2);
	}

	bool cont_rev()
	{
		return  !(lap != 0 && end < first);
	}
};
*/

template <
	class vert_type = point_2d, 
	class tracer_type = std::vector<vert_type>, 
	class area_type = double
>
class area_tracer : public area_accum<vert_type, area_type> {
public:
	round_robin_iterator<vert_type, tracer_type> iter;
	
	/*
	void init_iter(tracer_type& p_iter)
	{
		iter.init(p_iter);
	}

	void init_iter(tracer_type& p_iter, round_robin_element<> p_start)
	{
		iter.init(p_iter, p_start);
	}
	*/

	/*
	void init_acc(round_robin_element<> p_start, area_type p_area = (area_type)0)
	{
		area_accum<vert_type, area_type>::init(iter.get_element(p_start.i), p_area);
	}

	void init_acc(round_robin_element<> p1, round_robin_element<> p2, area_type p_area = (area_type)0)
	{
		area_accum<vert_type, area_type>::init(iter.get_element(p1.i), iter.get_element(p2.i), p_area);
		//init_area(iter.get_element(p1.iteration()), iter.get_element(p2.iteration()), p_area);
	}
	*/

	void init(tracer_type& p_iter, area_type p_area = (area_type)0)
	{
		iter = round_robin_iterator<vert_type, tracer_type>(p_iter);
		area_accum<vert_type, area_type>::init(*iter, p_area);
	}
	
	void init(tracer_type& p_iter, round_robin_element<> p_start, area_type p_area = (area_type)0)
	{
		iter = round_robin_iterator<vert_type, tracer_type>(p_iter, p_start);
		area_accum<vert_type, area_type>::init(iter.get_element(p_start.i), p_area);
	}

	/*
	void init(tracer_type& p_iter, round_robin_element<> p1, round_robin_element<> p2, area_type p_area = (area_type)0)
	{
		iter = round_robin_iterator<vert_type, tracer_type>(p_iter, p2);
		area_accum<vert_type, area_type>::init(iter.get_element(p1.i), iter.get_element(p2.i), p_area);
	}

	void init(round_robin_element<> p1, round_robin_element<> p2, area_type p_area = (area_type)0)
	{
		area_accum<vert_type, area_type>::init(iter.get_element(p1.i), iter.get_element(p2.i), p_area);
	}
	*/

	/*
	void init(round_robin_element<> p_start)
	{
		area_accum<vert_type, area_type>::init(iter.get_element(p_start.i));
	}
	*/

	area_tracer ()
	{
	}

	area_tracer (tracer_type* p_iter,  round_robin_element<> p_start, area_type p_area = area_type(0))
	{
		init(p_iter, p_start, p_area);
	}

	/*
	area_tracer (tracer_type* p_iter,  round_robin_element<> p1, round_robin_element<> p2, area_type p_area = area_type(0))
	{
		init(p_iter, p1, p2, p_area);
	}
	*/

	area_tracer (tracer_type* p_iter, tent_vertex v)
	{
		init(*p_iter, v.index, v.area);
	}

	tent_vertex get_tent_vertex()
	{
		return tent_vertex(iter.e, area);
	}

	vert_type get_vert()
	{
		return *iter;
	}

	area_type calc_area(area_type tot_area)
	{
		return tot_area * area_type(iter.e.lap) + area;
	}

	area_type range_area_x2(area_tracer tail, area_type tot_area)
	{
		return calc_area(tot_area) - tail.calc_area(tot_area);
	}

	// why is actual area so small in rad_300?
	// what is with the extra vertex in circles.bmp?
	// why is circles.bmp hanging up close to the bot left?

	area_type range_area_x2B(area_tracer tail, area_type tot_area)
	{
		/*
		if (iter->x < tail.iter->x) {
			if (area < tail.area) {
				show_point(*tail.iter, RGB_RED, 3, false);
			}
		} else {
			if (area > tail.area) {
				show_point(*iter, RGB_GREEN, 3, false);
			}
		}
		*/

		// if it strattles the seam then add total area
		if (tail.iter.e.i > iter.e.i) {
		//if (tail.iter.e > iter.e) {

			if (true) { //tail.area > area) {
				//mess(area, "head area");
				//mess(tail.area, "mid area");

				return area - tail.area + tot_area;
			} else {
				return area - tail.area;
			}
		} else {
			return area - tail.area;
		}
	}

	area_type range_area(area_tracer tail, area_type tot_area)
	{
		return 0.5 * range_area_x2(tail, tot_area);
	}

	area_type trap_area_x2(area_tracer tail)
	{
		// area made negative by reversing points
		return trapizoid_area_x2(*iter, *tail.iter);
	}

	area_type trap_area(area_tracer tail)
	{
		return 0.5 * trap_area_x2(tail);
	}

	area_type seg_area_x2(area_tracer tail, area_type tot_area)
	{
		return range_area_x2(tail, tot_area) - trap_area_x2(tail);
	}

	area_type seg_area(area_tracer tail, area_type tot_area)
	{
		return 0.5 * seg_area_x2(tail, tot_area);
	}

	double chord_len_sqrd(area_tracer other)
	{
		return get_vert().dist_sqrd(other.get_vert());
	}

	/*
	double chord_len_sqrd(round_robin_element<> e)
	{
		return get_vert().dist_sqrd(iter->get_element(e.iteration()));
	}
	*/

	double chord_len(area_tracer tail)
	{
		return sqrt(chord_len_sqrd(tail));
	}

	double ave_height(area_tracer tail, area_type tot_area)
	{
		return seg_area(tail, tot_area) / chord_len(tail);
	}

	inline int ave_height_threshhold(area_tracer tail, area_type threshhold, area_type tot_area)
	{
		area_type a = seg_area_x2(tail, tot_area);
		
		if (a > (area_type)0) {
			if (a * a > 4.0 * threshhold * threshhold * curr.dist_sqrd(tail.curr)) {
				return 1;
			} else {
				return 0;
			}
		} else {
			if (a * a > 4.0 * threshhold * threshhold * curr.dist_sqrd(tail.curr)) {
				return -1;
			} else {
				return 0;
			}
		}
	}

	/*
	void move_mid(area_tracer<> p_head, area_tracer<> p_tail)
	{
		while (chord_len_sqrd(p_head) < chord_len_sqrd(p_tail)) {
			//show_point(*iter, RGB_PURPLE);
			//show_windows();
			--iter;
		} 

		while (chord_len_sqrd(p_head) > chord_len_sqrd(p_tail)) {
			//show_point(*iter, RGB_GREY);
			//show_windows();
			++iter;
		} 

		curr = *iter;
	}
	*/

	/*
	void init_mid_iter(tracer_type& p_t, area_tracer<> p_head, area_tracer<> p_tail)
	{
		iter = round_robin_iterator<vert_type, tracer_type>(p_t, p_head.iter.e.interpol(p_tail.iter.e, p_t.size(), 0.5));
		move_mid(p_head, p_tail);
	}
	*/

	void iterate()
	{
		++iter;
		add_col_x2(*iter);
	}

	void iterate(area_type tot_area)
	{
		if (iter.inc(tot_area)) {
			area -= tot_area;
		}
		add_col_x2(*iter);
	}

	void iterate_rev()
	{
		--iter;
		add_col_x2(*iter);
	}

	void iterate_rev(area_type tot_area)
	{
		if (iter.dec(tot_area)) {
			area += tot_area;
		}
		add_col_x2(*iter);
	}

	/*
	void iterate2()
	{
		add_col_x2(*iter);
		++iter;
	}

	void iterate_rev2()
	{
		--iter;
		sub_col_x2(*iter);
	}
	*/

	/*
	inline void copy_state(area_tracer a)
	{
		iter.copy_state(a.iter);
		area_accum<vert_type, area_type>::copy_state(a);
	}
	*/
};


template <class vert_type = point_2d, class tracer_type = std::vector<vert_type>, class area_type = double>
class corner_detector {
	double prev_u;
public:
	area_tracer<vert_type, tracer_type, area_type> head, tail;
	area_tracer<vert_type, tracer_type, area_type> last_vert, last_it;
	area_type tot_area;
	area_type last_area;

	/*
	void init(tracer_type& p_tracer, round_robin_element<> p_head, round_robin_element<> p_tail, area_type p_area = (area_type)0)
	{
		head.init(p_tracer, p_tail, p_head, p_area);
		tail.init(p_tracer, p_tail, p_area);
		prev_u = 0.0;
	}
	*/

	void init(
		tracer_type& p_tracer, area_type p_tot_area, round_robin_element<> p_start = round_robin_elemnt<>(0, 0), 
		area_type p_area = (area_type)0)
	{
		head.init(p_tracer, p_start, p_area);
		tail.init(p_tracer, p_start, p_area);
		prev_u = 0.0;
		tot_area = p_tot_area;
	}

	void init(tracer_type& p_tracer, area_type p_tot_area, tent_vertex p_head, tent_vertex p_tail)
	{
		// assuming that head is further along the tracer (greater) than tail
		// therefore head = v2, tail = v1
		head.init(p_tracer, p_head.index.fix_end(p_tail.index), p_head.area);
		tail.init(p_tracer, p_tail.index, p_tail.area);
		prev_u = 0.0;
		tot_area = p_tot_area;
	}

	corner_detector()
	{
	}

	/*
	corner_detector(
		tracer_type& p_tracer, 
		round_robin_element<> p_head, 
		round_robin_element<> p_tail, 
		area_type p_area = (area_type)0)
	{
		init(p_tracer, p_head, p_tail, p_area);
	}
	*/

	corner_detector(
		tracer_type& p_tracer, area_type p_tot_area = area_type(0),
		round_robin_element<> p_start = round_robin_element<>(0, 0), 
		area_type p_area = (area_type)0)
	{
		init(p_tracer, p_tot_area, p_start, p_area);
	}

	corner_detector(tracer_type& p_tracer, area_type p_tot_area, tent_vertex p_head, tent_vertex p_tail)
	{
		init(p_tracer, p_tot_area, p_head, p_tail);
	}

	vert_type get_vert()
	{
		//return *last_vert.iter;
		return *head.iter;
	}

	vert_type get_curr()
	{
		return *head.iter;
	}

	area_tracer<vert_type, tracer_type, area_type> get_vert_ind()
	{
		// return head;
		return last_vert;
	}

	tent_vertex get_tent_vert()
	{
		return last_vert.get_tent_vertex();
	}

	double ave_height()
	{
		if (head.iter.e.iteration() == tail.iter.e.iteration()) {
			return 0.0;
		} else {
			return head.ave_height(tail, tot_area);
		}
	}

	bool cont()
	{
		return tail.iter < round_robin_element<>(0, 1);
	}
	
	bool test(area_type threshhold = (area_type)DEFAULT_LINEAR_TOL)
	{
		double curr_u = head.secant_len_sqrd(tail);

		if (curr_u < prev_u) {
			return true;
		}
		prev_u = curr_u;

		head.ave_height_threshhold(tail, threshhold, tot_area) != 0;
	}

	bool iterate(area_type threshhold = (area_type)DEFAULT_LINEAR_TOL)
	{
		double curr_u = head.chord_len_sqrd(tail);
		bool ret;

		if (prev_u > curr_u) { // dist_diff_greater(prev_u, curr_u, 0.5)) {
			//prev_u = 0.0;
			//return true;
		}
		prev_u = curr_u;
		last_vert = head;

		head.iterate(tot_area);

		if (head.iter.e == round_robin_element<>(0, 1)) {
			last_area = head.area;
		}

		ret = head.ave_height_threshhold(tail, threshhold, tot_area) != 0;

		if (ret) {
			prev_u = 0.0;
		} 

		return ret;
	}

	bool iterate_rev(area_type threshhold = (area_type)DEFAULT_LINEAR_TOL)
	{
		double curr_u = head.chord_len_sqrd(tail);
		bool ret;

		if (prev_u > curr_u) { // dist_diff_greater(prev_u, curr_u, 0.5)) {
			//prev_u = 0.0;
			//return true;
		}
		prev_u = curr_u;
		last_vert = tail;

		tail.iterate_rev(tot_area);
		ret = head.ave_height_threshhold(tail, threshhold, tot_area) != 0;

		if (ret) {
			//tail = last_vert;//head;
			prev_u = 0.0;
		}

		return ret;
	}

	/*
	bool iterate2(area_type c = (area_type)DEFAULT_LINEAR_TOL)
	{
		bool ret;// = test(c);

		if (ret) {
			tail = head;
			last_vert = last_it;
			prev_u = 0.0;
		}

		last_it = head;
		last_vert = head;

		head.iterate();
		return ret;
	}
	*/

	void reset()
	{
		tail = head;
	}

	double set_last_area()
	{
		tot_area = last_vert.area;
	}

	double get_last_area()
	{
		return last_vert.area;
	}
};


template <class vert_type = point_2d, class tracer_type = std::vector<vert_type>, class area_type = double>
class arc_detector {

	public:

	area_tracer<vert_type, tracer_type, area_type> head, mid, tail;
	tent_vertex fwd_vert, fwd_mid;
	tent_vertex rev_vert, rev_mid;
	bool obtuse;
	area_type tot_area;

	inline void init(tracer_type& p_t, area_type p_tot_area, round_robin_element<> p_start = 0)
	{
		obtuse = false;
		tot_area = p_tot_area;
		head.init(p_t, p_start);
		mid.init(p_t, p_start);
		tail.init(p_t, p_start);
	}

	inline void init(
		tracer_type& p_t, area_type p_tot_area,
		round_robin_element<> p_head, area_type p_area_head, 
		round_robin_element<> p_mid, area_type p_area_mid,
		round_robin_element<> p_tail, area_type p_area_tail)
	{
		obtuse = false;
		tot_area = p_tot_area;
		head.init(p_t, p_head, p_area_head);
		mid.init(p_t, p_mid, p_area_mid);
		tail.init(p_t, p_tail, p_area_tail);
	}

	inline void init(tracer_type& p_t, area_type p_tot_area, tent_vertex p_head, tent_vertex p_mid, tent_vertex p_tail)
	{
		obtuse = false;
		tot_area = p_tot_area;
		head.init(p_t, p_head.index, p_head.area);
		mid.init(p_t, p_mid.index, p_mid.area);
		tail.init(p_t, p_tail.index, p_tail.area);
	}

	/*
	inline void init(tracer_type& p_t, round_robin_element<> p_head, round_robin_element<> p_tail, area_type p_init_area = area_type(0))
	{
		obtuse = false;

		head.init_iter(p_t, p_head);
		tail.init_iter(p_t, p_tail);
		mid.init_mid_iter(p_t, head, tail);

		head.init_acc(p_head, p_init_area);
		mid.init_acc(p_head, mid.iter.e, p_init_area);
		tail.init_acc(mid.iter.e, p_tail, mid.area);
	}
	*/

	arc_detector()
	{
	}

	arc_detector(tracer_type& p_t, area_type p_tot_area, round_robin_element<> p_start = 0) 
	{
		init(p_t, p_tot_area, p_start);
	}

	arc_detector(
		tracer_type& p_t, area_type p_tot_area,
		round_robin_element<> p_head, area_type p_area_head, 
		round_robin_element<> p_mid, area_type p_area_mid,
		round_robin_element<> p_tail, area_type p_area_tail)
	{
		init(p_t, p_tot_area,
			p_head, p_area_head,
			p_mid, p_area_mid,
			p_tail, p_area_tail
		);
	}

	arc_detector(tracer_type& p_t, area_type p_tot_area, tent_vertex p_head, tent_vertex p_mid, tent_vertex p_tail)
	{
		init(p_t, p_tot_area, p_head, p_mid, p_tail);
	}

	/*
	arc_detector(tracer_type& p_t, round_robin_element<> p_head, round_robin_element<> p_tail, area_type p_init_area = area_type(0))
	{
		init(p_t, p_head, p_tail, p_init_area);
	}
	*/

	bool iterate(double c = DEFAULT_ARC_TOL)
	{
		fwd_vert = head.get_tent_vertex();
		fwd_mid = mid.get_tent_vertex();

		if (!obtuse) {
			rev_vert = fwd_vert;
			rev_mid = fwd_mid;
		}

		head.iterate(tot_area);
		
		while (is_obtuse()) {
			//show_point(*tail.iter, RGB_GREY, 2, true);
			tail.iterate(tot_area);
			obtuse = true;
		}

		update_mid();

		/*
		if (test(c)) {
			mess(tail.area, "tail area");
			mess(mid.area, "mid area");
			mess(head.area, "head area");
		}
		*/

		/*
		if (mid.iter.e > head.iter.e) {
			mess("greater");
		}

		if (mid.iter.e < tail.iter.e) {
			mess("lesser");
		}
		*/

		return test(c);
	}

	bool iterate_rev(double c = DEFAULT_ARC_TOL)
	{
		rev_vert = tail.get_tent_vertex();
		rev_mid = mid.get_tent_vertex();

		if (!obtuse) {
			fwd_vert = rev_vert;
			fwd_mid = rev_mid;
		}

		//last_vert = tail;
		tail.iterate_rev(tot_area);

		while (is_obtuse()) {
			//show_point(*head.iter, RGB_GREY, 2, false); // this is screwed up
			//show_point(*tail.iter, RGB_BLUE, 2, true);

			//mess(head.area, "head");
			//mess(tail.area, "tail");

			head.iterate_rev(tot_area);
			obtuse = true;
		}
		update_mid();

		return test(c);
	}

	void update_mid()
	{
		static double prev_len = 0.0;
		double curr_len = head.chord_len_sqrd(tail);

		if (mid.chord_len_sqrd(head) < mid.chord_len_sqrd(tail)) {
			while (mid.chord_len_sqrd(head) < mid.chord_len_sqrd(tail)) {
				mid.iterate_rev(tot_area);
				//show_point(*mid.iter, RGB_GREY, 3, true);
			}
		} else {
			while (mid.chord_len_sqrd(head) > mid.chord_len_sqrd(tail)) {
				mid.iterate(tot_area);
				//show_point(*mid.iter, RGB_BLUE, 3, true);
			}
		}

		/*
		if (mid.iter.e < tail.iter.e) {
			mess("less than tail");
		}

		if (mid.iter.e > head.iter.e) {
			mess("greater than head");
		}
		*/

/*
		double v1 = 0.0;
		double v2 = 0.0;

		while (v1 <= v2) {
			point_2d h = *head.iter;
			point_2d t = *tail.iter;
			point_2d m = h.mean(t);
			point_2d m2 = *mid.iter;

			v1 = tail.calc_column_area(mid) - calc_column_area(m2, m) - calc_column_area(m, t);
			v2 = mid.calc_column_area(head) - calc_column_area(h, m) - calc_column_area(m, m2);
			show_text("diff area", v2 - v1, 30);
			show_text("dist m-m2", m.dist(t) - m.dist(h), 32);

			cull_geo();
			show_point(m, RGB_YELLOW, 3, false);
			show_point(t, RGB_ORANGE, 3, false);
			show_point(h, RGB_ORANGE, 3, false);
			show_point(*mid.iter, RGB_GREEN, 3, true);

			mid.iterate();
		}
		*/

		prev_len = curr_len;
	}
	
	bool is_obtuse()
	{
		// head and tail are swapped
		return fabs(head.seg_area_x2(tail, tot_area)) > (PI / 4.0) * head.chord_len_sqrd(tail);
	}

	/*
	void update_mid2()
	{
		if (false) {
			while (tail.calc_tot_area(mid) < mid.calc_tot_area(head)) {
				mid.iterate();
				//show_point(*mid.iter, RGB_YELLOW, 3, false);
			}
			while (tail.calc_tot_area(mid) > mid.calc_tot_area(head)) {
				mid.iterate_rev();
				//show_point(*mid.iter, RGB_YELLOW, 3, false);
			}
		} else {
			while (mid.chord_len_sqrd(head) < mid.chord_len_sqrd(tail)) {
				mid.iterate_rev();
				//show_point(*mid.iter, RGB_GREY, 3, false);
			}			
			while (mid.chord_len_sqrd(head) > mid.chord_len_sqrd(tail)) {
				mid.iterate();
				//show_point(*mid.iter, RGB_GREY, 3, false);
			}
		}
	}
	*/

	void update_tail()
	{
		//while (fabs(mid.seg_area_x2(head)) > (PI / 4.0 - 0.5) * mid.chord_len_sqrd(head)) {
		while (is_obtuse()) {
			tail.iterate(tot_area);
			obtuse = true;
		}

		update_mid();
	}

	void update_head()
	{
		//while (fabs(mid.seg_area_x2(head)) > (PI / 4.0 - 0.5) * mid.chord_len_sqrd(head)) {
		while (is_obtuse()) {
			head.iterate_rev(tot_area);
			obtuse = true;
		}

		update_mid();
	}

	double area_front_x2()
	{
		return head.seg_area_x2(mid, tot_area);
	}

	double area_back_x2()
	{
		return mid.seg_area_x2(tail, tot_area);
	}

	double h_front_x2()
	{
		return head.ave_height(mid, tot_area);
	}

	double h_back_x2()
	{
		return mid.ave_height(tail, tot_area);
	}

	bool test(double c)
	{
		double a1 = area_back_x2();
		double a2 = area_front_x2();
		double u1 = tail.chord_len_sqrd(mid);
		double u2 = mid.chord_len_sqrd(head);

		/*
		cull_geo();
		show_text("dx tail", a1 / ((*mid.iter).x - (*tail.iter).x), 30);
		show_text("dx head", a2 / ((*head.iter).x - (*mid.iter).x), 32);
		show_point(*head.iter, RGB_ORANGE, 3, false);
		show_point(*tail.iter, RGB_GREEN, 3, false);
		show_point(*mid.iter, RGB_YELLOW, 3, true);
		*/

		return
			u1 * u2 * sqrd_ks(-2.0 * a1 * a2) > sqrd_ks(4.0 * u1 * u2 * c * c - a1 * a1 * u2 - a2 * a2 * u1);
	}

	bool test_line()
	{
		return head.ave_height_threshhold(tail);
	}

	bool cont()
	{
		return head.iter.cont();
	}

	bool cont_tail()
	{
		return tail.iter.cont();
	}

	bool cont(round_robin_element<> e)
	{
		return head.iter.e <= e;
	}

	bool cont_rev(round_robin_element<> e)
	{
		return tail.iter.e >= e;
	}

	void reset()
	{
		tail = mid = head;
		obtuse = false;
	}
};


enum feature_type {feature_init, feature_edge, feature_arc};

/* tentative feature */
class tent_feature {
public:
	feature_type type;
	tent_vertex v1; // tentative arc start
	tent_vertex v2; // tentative arc end
	tent_vertex mid; // reverse mid
	tent_vertex vert; // reverse vert
	line_2d_dydx edge;
	circle_2d<> arc;

	// tent arc constructor
	tent_feature(tent_vertex p_v1, tent_vertex p_v2, tent_vertex p_mid, tent_vertex p_vert)
	{
		type = feature_arc;
		v1 = p_v1;
		v2 = p_v2;
		mid = p_mid;
		vert = p_vert;
	}

	// tent edge constructor
	tent_feature(vertex p_v1, vertex p_v2)
	{
		type = feature_edge;
		v1 = tent_vertex(p_v1);
		v2 = tent_vertex(p_v2);
	}
};

class feature {
public:
	feature_type type;
	double angle1; // edge angle or arc angle 1
	double angle2; // arc angle 2, if applicable
	double len; // edge length or radius

	// arc constructor
	feature(double p_angle1, double p_angle2, double p_radius)
	{
		type = feature_arc;
		angle1 = p_angle1;
		angle2 = p_angle2;
		len = p_radius;
	}

	// edge constructor
	feature(double p_angle, double p_length)
	{
		type = feature_edge;
		angle1 = p_angle;
		len = p_length;
	}
};

typedef std::vector<feature> feature_bank;
typedef std::vector<tracer_unit> circuit_type;
typedef std::vector<point_2d> mids_type;

class shape {
public:
	std::vector<vertex> verts;
	tracer_unit prev_el;
	circuit_type circuit;
	mids_type mids;
	round_robin_iterator<point_2d> mids_rr;
	double tot_area;

	void init()
	{
		prev_el = VECT_2D_INT_UNDEF_VALUE;
		mids_rr.init(mids);
	}

	void log_vertex(area_tracer<> a)
	{
		verts.push_back(vertex(a.get_vert(), a.iter.iteration(), a.area));

		//verts.push_back(vertex(t.get_vertex(), (t.end - 1).iteration()));
		//show_point(a.get_vert(), RGB_RED, 3, true);
		//mess(a.iter->report(), "point");
		//mess(a.area, "vert area");
		
		//show_windows();
	}
	
	void process_element(tracer_unit u)
	{
		//show_text("x", u.x);
		//show_text("y", u.y);
		
		//color_point(u.x, u.y, RGB_GREEN, true);
		//show_point(u);

		/*
		mess(u.x - 10, "x");
		mess(u.y - 10, "y");

		return;
		*/

		/*
		if (read_point_in(u.x, u.y) == RGB_WHITE) {
			color_point(u.x, u.y, RGB_RED);
		} else {
			color_point(u.x, u.y, RGB_BLUE);
		}
		*/

		circuit.push_back(u);
		//mids.push_back(u); // don't calc mid

		if (!prev_el.is_undef()) {
			//area += u.y * double(u.x - prev_el.x);
			mids.push_back(u.mean(prev_el)); // calc mid
			//show_text("curr", prev_el.report());
		}

		prev_el = u;
	}

	// this function is crap, apparently
	/*
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
				point_2d cn = norm.extend((float)ar.calc()).alias_pixel();
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
	*/

	void process_between(tracer_unit p1, tracer_unit p2)
	{
		if (p1.x < p2.x) {
			if (p1.y < p2.y) { // case 1 (changed)
				process_element(tracer_unit(p1.x, p1.y));
				for (int x = p1.x; x < p2.x; x++) {
					process_element(tracer_unit(x, p2.y));
				}
			} else if (p1.y > p2.y) { // case 2 (changed)
				for (int x = p1.x; x <= p2.x; x++) {
					process_element(tracer_unit(x, p1.y));
				}
			} else { // case 3 (not changed)
				for (int x = p1.x; x < p2.x; x++) {
					process_element(tracer_unit(x, p1.y));
				}
			}
		} else if (p1.x > p2.x) {
			if (p1.y < p2.y) { // case 4 (changed)
				process_element(tracer_unit(p1.x, p1.y));
				for (int x = p1.x; x > p2.x; x--) {
					process_element(tracer_unit(x, p2.y));
				}
			} else if (p1.y > p2.y) { // case 5 (changed)
				//process_element(tracer_unit(p1.x, p1.y));
				for (int x = p1.x; x >= p2.x; x--) {
					process_element(tracer_unit(x, p1.y));
				}
			} else { // case 6 (not changed)
				for (int x = p1.x; x > p2.x; x--) {
					process_element(tracer_unit(x, p1.y));
				}
			}
		} else { // case 7 (not changed)
			process_element(tracer_unit(p1.x, p1.y));
		}
	}

	void process_circuit(feature_bank* p_bank, tracer* p_circuit_start, rgb p_color)
	{
		static int ct = 0;
		tracer_unit first, last, p(VECT_2D_INT_UNDEF_VALUE);
		tracer_unit prev(VECT_2D_INT_UNDEF_VALUE);
		tracer* prev_tracer;
		tracer_type::iterator prev_corner;
		bool rep;
		prev_tracer = p_circuit_start;
		prev_corner = p_circuit_start->data->begin();

		ct++;
		show_text("begin", (double)ct);
		//show_windows();

		if (ct != 4) {
			//return;
		}

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

		//process_test4();
		//parse_corners(); // current corner parser 
		show_windows(0);
		init_background();
		parse_features();
		/*
		process_corners();
		correct_verts();
		*/

		//process_curves();
		//process_test3();
		//process_test2();
		//identify();
	}

	/*
	void parse_corners()
	{
		// 	best parse corner / edge - 1/19/2014
		test_current_fwd(); 
		get_background();
		correct_verts3();
		refine_verts();
	}
	*/

	void parse_arc(
		std::vector<tent_feature>& vect,
   		tent_vertex& v1 = TENT_VERTEX_NULL, 
		tent_vertex& v2 = TENT_VERTEX_NULL)
	{
		tent_vertex arc_start;
		round_robin_element<> arc_end;

		/*
		show_point(mids[v1.index.iteration()], RGB_GREY, 5, false);
		show_point(mids[v2.index.iteration()], RGB_BLACK, 5, true);
		*/

		if (v1.valid()) {
			if (v1.index == v2.index) {
				return;
			}
			arc_start = v1;
			arc_end = v2.index.fix_end(v1.index);
		} else {
			arc_start = verts[0];
			arc_end = round_robin_element<>(verts[0].index, 1);
		}

		//mess(tot_area, "total area");

		arc_detector<> ad(mids, tot_area, arc_start, arc_start, arc_start);

		//mess(tot_area, "total area");
		//show_point(mids[v1.index.iteration()], RGB_RED, 5);
		//show_point(mids[v2.index.iteration()], RGB_GREEN, 5);

		while (ad.cont(arc_end)) {
			//show_point(mids[ad.head.iter.e.iteration()], RGB_PURPLE, 3, false);

			if (ad.iterate()) {
				//show_point(mids[ad.fwd_vert.index.iteration()], RGB_AQUA, 5);
				log_arc(vect, arc_start, ad.fwd_vert, ad.rev_mid, ad.rev_vert);
				arc_start = ad.fwd_vert;
				ad.reset();
			}
		}

		if (arc_start.index < ad.fwd_vert.index) {
			log_arc(vect, arc_start, ad.fwd_vert, ad.rev_mid, ad.rev_vert);
		}
	}

	/*
void parse_arc(
	std::vector<tent_feature>& vect,
	tent_vertex& v1, 
	tent_vertex& v2)
{
	if (v1.index == v2.index) return;

	tent_vertex arc_start(v1);
	round_robin_element<> arc_end;

	//bool linear = true;
	//for (round_robin_element<> i = i1; i <= i2; i.inc(verts.size())) {
	//	if (round_robin_element<>::range(verts[i.i].index, s1, s2)) {
	//		if (tri_2d<>::ave_height_greater(mids[s1], mids[s2], verts[i.i].pixel, 0.5)) {
	//			linear = false;
	//			break;
	//		}
	//	}
	//}

	//if (linear) {
	//	store_edge(verts[i1.i], verts[i2.i]);
	//	return;
	//}

	arc_detector<> ad(mids, arc_start.index);

	arc_end = v2.index.fix_end(v1.index);

	while (ad.cont(arc_end)) {
		if (ad.iterate()) {
			log_arc(vect, arc_start, ad.get_head(), ad.get_mid());
			arc_start = ad.get_head();
			ad.reset();
		}
	}

	if (arc_start.index != ad.get_head().index) {
		log_arc(vect, arc_start, ad.get_head(), ad.get_mid());
	}
}
*/

	point_2d mean_point(round_robin_element<> e1, round_robin_element<> e2)
	{
		point_2dd sum = POINT_2DD_ORIG;

		for (	round_robin_element<> e = e1; e <= e2; e.inc(mids.size())) {
			//show_point(mids[e.i], RGB_ORANGE, 1);
			sum += mids[e.i];
		}

		return sum / double(e1.iterations(e2, mids.size()));
	}

	void store_edge(vertex v1, vertex v2)
	{
		for (round_robin_element<> e = v1.index; e <= round_robin_element<>::fix_end(v1.index, v2.index); e.inc(mids.size())) {
			show_point(mids[e.i], RGB_GREEN, 3, false);
		}

		//tent_features.push_back(tent_feature(v1, v2));

		/*
		round_robin_element<> start = round_robin_element<>(i1);
		round_robin_element<> end = round_robin_element<>::fix_end(i1, i2);
		round_robin_element<> e1 = start.interpol(end, mids.size(), 0.25);
		round_robin_element<> e2 = start.interpol(end, mids.size(), 0.75);
		point_2d delta = mids[e2.iteration()] - mids[e1.iteration()];
		*/

		/*
		mess(i1, "i1");
		mess(i2, "i2");
		mess(e1.report(), "e1");
		mess(e2.report(), "e2");
		*/

		/*
		line_2d_dydx line(delta.x, delta.y, mean_point(e1, e2));
		show_point(mean_point(e1, e2), RGB_RED);
		show_line(line, RGB_WHITE);

		show_windows();
		*/
	}

	void log_arc(std::vector<tent_feature>& vect, tent_vertex v1, tent_vertex v2, tent_vertex mid, tent_vertex vert)
	{
		//show_point(mids[v1.index.iteration()], RGB_GREEN, 5);
		//show_point(mids[v2.index.iteration()], RGB_RED, 5);

		//color_circuit(v1.index, v2.index, RGB_AQUA);

		//show_point(mids[v1.index.iteration()], RGB_ORANGE, 3, true);
		//show_point(mids[v2.index.iteration()], RGB_ORANGE, 3, true);

		/*
		mess(v1.area, "v1");
		mess(v2.area, "v2");
		mess(mid.area, "mid");
		mess(vert.area, "vert");
		*/

		vect.push_back(tent_feature(v1, v2, mid, vert));

		/*
		round_robin_element<> start = round_robin_element<>(i1);
		round_robin_element<> end = round_robin_element<>::fix_end(i1, i2);
		round_robin_element<> e1 = start.interpol(end, mids.size(), 3.0 / 12.0);
		round_robin_element<> e2 = start.interpol(end, mids.size(), 5.0 / 12.0);
		round_robin_element<> e3 = start.interpol(end, mids.size(), 7.0 / 12.0);
		round_robin_element<> e4 = start.interpol(end, mids.size(), 9.0 / 12.0);
		round_robin_element<> m = start.interpol(end, mids.size(), 0.5);

		point_2d p1 = mean_point(e1, e2); // mids[e1.iteration()];
		point_2d p2 = mean_point(e2, e3);//mids[e2.iteration()];
		point_2d p3 = mean_point(e3, e4);//mids[e3.iteration()];
		point_2d p4 = mids[e4.iteration()];

		//circle_2d<> circ(i1, i2, mids);
		circle_2d<> circ(p1, p2, p3);
		show_circle(circ, RGB_AQUA);
		mess(circ.center.dist(mids[e1.iteration()]), "rad mean pt 1");
		mess(circ.center.dist(mids[e2.iteration()]), "rad mean pt 2");
		mess(circ.center.dist(mids[e3.iteration()]), "rad mean pt 3");

		double r_squared = 0.0;
		for (round_robin_element<> i = e1; i <= e4; i.inc(mids.size())) {
			r_squared += circ.center.dist_sqrd(mids[i.iteration()]);
		}
	
		mess(sqrt(r_squared / double(e4.effective(mids.size()) - e1.effective(mids.size()) + 1)), "r now");

		show_windows();
		*/
	}

	line_2d_dydx calc_edge_line(round_robin_element<> r1, round_robin_element<> r2)
	{
		round_robin_element<> e1 = r1.add(0, mids.size());
		round_robin_element<> e2 = r2.sub(0, mids.size()).fix_end(e1);
		mean<point_2d> mean_b;

		point_2d delta = mids[e2.i] - mids[e1.i];
		
		for (round_robin_element<> e = e1; e <= e2; e.inc(mids.size())) {
			mean_b.update(mids[e.iteration()]);
		}

		return line_2d_dydx(delta.x, delta.y, mean_b.calc());
	}

	circle_2d<> calc_arc_circ(round_robin_element<> r1, round_robin_element<> r2)
	{
		//show_point(mids[r1.i], RGB_GREEN);
		//show_point(mids[r2.i], RGB_RED);
		//show_windows(0);

		line_2d_dydx cent(mids[r1.i], mids[r2.i], true);
		round_robin_element<> q1 = r1.interpol(r2, mids.size(), 0.25);
		round_robin_element<> q2 = r1.interpol(r2, mids.size(), 0.50);
		round_robin_element<> q3 = r1.interpol(r2, mids.size(), 0.75);
		point_2dd nsect;
		//int h = min(mid - r1, r2 - mid).effective(mids.size());

		//show_windows(0);

		nsect = POINT_2DD_ORIG;
		for (round_robin_element<> i = q1; i <= q3; i.inc(mids.size())) {
			line_2d_dydx l1(mids[r1.i], mids[i.i], true);
			line_2d_dydx l2(mids[i.i], mids[r2.i], true);
			nsect += l1.intersect(l2);
	//		show_line(orb, RGB_RED);
			//show_point(mids[i.i], RGB_YELLOW);
	//		show_point(mids[r2.i], RGB_ORANGE);
			//show_windows(0);
			//show_point(mids[i.i], RGB_GREY);
		}
		//show_point(nsect / (double)q1.iterations(q3, mids.size()), RGB_PURPLE);
		//show_point(point_2d(100, 100), RGB_RED);
		//show_windows();

		return circle_2d<>(point_2d(0.0), 1.0);
	}

	void log_edge_final(tent_vertex v1, tent_vertex v2)
	{
		line_2d_dydx l = calc_edge_line(v1.index, v2.index);
		
		//mess(l.m, "m");
		//mess(l.b, "b");

		show_line(mids[v1.index.iteration()], l, RGB_YELLOW, true);
		//color_circuit(v1.index, v2.index, RGB_AQUA);
		//show_point(mids[v1.index.iteration()], RGB_GREEN, 5, false);
		//show_point(mids[v2.index.iteration()], RGB_RED, 5, true);
	}

	void log_arc_final(tent_vertex v1, tent_vertex v2)
	{
		int mid = v1.index.mean(v2.index.fix_end(v1.index), mids.size(), 0.5).iteration();

		point_2d p1 = mids[v1.index.iteration()];
		point_2d p2 = mids[v2.index.iteration()];

		mess(v1.report(), "v1");
		mess(p1.report(), "p1");
		mess(v2.report(), "v2");
		mess(p2.report(), "p2");


		double r_cu = 0.0;
		double r_ct = 0.0;

		/*
		for (round_robin_element<> i = mid - 5; i <= mid + 5; i.inc(mids.size())) {
			point_2d p_mid = mids[i.iteration()];
			sector_2d<> sect(p1, p_mid, p2);
			r_cu += sect.radius;
			r_ct++;
		}
		*/

		point_2d p_mid = mids[mid];
		sector_2d<> sect(p1, p_mid, p2);
		newt_method<> newt(sect.theta);
		area_tracer<> t1(&mids, v1);
		area_tracer<> t2(&mids, v2);
		double seg_area_x2 = t2.seg_area_x2(t1, tot_area);

		/*
		mess(sect.segment_area_x2(), "seg area formula");
		mess(p1.dist(p2), "seg len formula");
		mess(sect.theta, "formula theta");
		*/

		//cull_geo();
		show_point(p1, RGB_GREEN);
		show_point(p2, RGB_RED);
		show_point(p_mid, RGB_RED);
		show_point(sect.center, RGB_YELLOW);

		//mess(sect.theta * (180.0 / PI), "guess");
		double a = t2.seg_area_x2(t1, tot_area);
		double s_sqrd = t2.iter->dist_sqrd(*t1.iter);

		mess(tot_area, "total area");
		mess(t2.area, "area head");
		mess(t1.area, "area tail");
		mess(t2.range_area_x2(t1, tot_area), "area diff");
		mess(t2.trap_area_x2(t1), "area trap"); // area made negative by reversing points

		mess(a, "A =");
		//mess(tot_area, "total area");

		//mess(r_cu / r_ct, "ave radius");

		while (newt.cont(100)) {
			double x = sect.theta;//newt.result();
			double sin_x = sin(x);
			double cos_x = cos(x);

			//mess(x, "curr x");

			newt.iterate(
				(2.0 * a * (1.0 - cos_x) - s_sqrd * (x - sin_x)) / 
				(2.0 * a * sin_x - s_sqrd * (1.0 - cos_x))
			);
		}

		double test_x = newt.result();
		double sin_x = sin(test_x);
		double cos_x = cos(test_x);
		//mess (2.0 * a * (1.0 - cos_x) - s_sqrd * (test_x - sin_x), "should be zero");
		mess(((test_x - sin_x) * s_sqrd) / (4.0 * (1.0 - cos_x)), "2A theory");
		mess(a, "2A actual");

		//mess(newt.result() * (180.0 / PI), "newt result");

		show_circle(sect, RGB_GREEN, true);

		mess(sqrt(s_sqrd) / (2.0 * sin(newt.result() * 0.5)), "newt radius");
		mess(sqrt(s_sqrd) / (2.0 * sin(sect.theta * 0.5)), "guess radius");

		color_circuit(v1.index, v2.index, RGB_PURPLE);
	}

	void define_feat_bound(tent_feature& f, tent_vertex& tent, round_robin_element<> limit)
	{
		bool init = false;
		bool linear;
		static bool tent_true = false;

		if (f.v1.index == f.v2.index) {
			return;
		}

		if (!tent.valid()) {
			round_robin_element<> mid_index = f.v1.index.interpol(f.v2.index.fix_end(f.v1.index), mids.size(), 0.5);
			area_tracer<> first(&mids, f.v1);

			while (first.iter.e <= mid_index) {
				first.iterate();
			}

			tent = first.get_tent_vertex();
			init = true;
		}
		
		/* test if current arc is actually linear */
		linear = false;
		if (f.type == feature_arc) {
			if (almost_edge(f.v1, tent)) {
				linear = true;
			}
		}

		if (f.type == feature_edge || linear) {
			corner_detector<> cd(mids, tot_area, tent, f.v1);

			if (linear) {
				cd.head.iterate_rev();
				cd.head.iterate_rev();
				tent = cd.head.get_tent_vertex();
			}

			while (!cd.iterate_rev() && cd.tail.iter.e.i != limit.i) {
				show_point(*cd.tail.iter, RGB_GREY, 2, false);
			}

			if (cd.tail.iter.e.i != limit.i) {
				if (!init) {
					log_edge_final(f.v1, tent);
				}
				tent = cd.get_tent_vert();
				tent_true = true;
			}
		} else if (f.type == feature_arc) {
			arc_detector<> ad;

			//show_point(mids[f.v1.index.iteration()], RGB_GREEN, 5);
			//show_point(mids[f.v2.index.iteration()], RGB_RED, 5);

			if (init) {
				ad.init(mids, tot_area, tent, f.v1, f.v1);
			} else {
				ad.init(mids, tot_area, tent, f.mid, f.v1);
			}

			ad.update_mid();

			while (!ad.iterate_rev() && ad.tail.iter.e.i != limit.i) {
				show_point(*ad.tail.iter, RGB_ORANGE, 2, true);
				mess(ad.area_back_x2(), "area back");
				mess(ad.area_front_x2(), "area front");
			};

			if (ad.tail.iter.e.i != limit.i) {
				if (!init) {
					log_arc_final(f.v1, tent);
				}
				tent = ad.rev_vert;
				tent_true = true;
			}
		} else {
			mess("not good");
			exit(77);
			tent.index = -2000;
		}
	}

	void define_feat_bounds(std::vector<tent_feature>& feat)
	{
		tent_vertex tent;
		int start = 0;

		//mess(round_robin_element<>(start - 1, 0).resolve_neg(feat.size()).report(), "end");

		tent.invalidate();
		for (round_robin_iterator<tent_feature> i(feat, start, true); i.e >= round_robin_element<>(feat.size() - start - 1, -1).resolve_neg(feat.size()); --i) {
			define_feat_bound(*i, tent, (i - 1)->v1.index);
			//mess(i.e.report(), "i.e");
		}
	}

	/*
	tent_vertex refine_arc_arc(tent_feature& f, tent_feature& prev_f)
	{
		round_robin_element<> s = round_robin_element<>(f.v1.index);
		round_robin_element<> m = round_robin_element<>::interpol(f.v1.index, f.v2.index, mids.size(), 0.5).fix_end(s);
		arc_detector<> ad(mids, m, s, f.v1.area);
		area_tracer<> prev_m(&mids, prev_f.mid);
		area_tracer<> prev_s(&mids, prev_f.v1);
	
		static int skip = 0;
		skip++;

		//show_point(mids[f.v1.index], RGB_RED, 1);
		//show_windows(0);

		mess(prev_f.v1.index, "v1 area");
		mess(prev_f.mid.index, "mid area");
		mess(prev_f.v2.index, "v2 area");

		show_point(mids[ad.get_head().index], RGB_GREEN);
		show_point(mids[ad.get_mid().index], RGB_YELLOW);
		show_point(mids[ad.get_tail().index], RGB_RED);
		show_windows();

		show_point(mids[f.v1.index], RGB_RED);
		show_point(mids[f.v2.index], RGB_BLUE);
		show_windows();

		show_point(mids[prev_f.v1.index], RGB_GREY);
		show_point(mids[prev_f.v2.index], RGB_GREY);
		show_point(mids[f.v1.index], RGB_GREY);
		show_point(mids[f.v2.index], RGB_GREY);

		min_value<double> min_area;

		//mess(prev_f.v1.area, "f.v1");
		//mess(prev_f.v2.area, "f.v2");

		do {
			//show_point(mids[ad.get_head().index], RGB_GREEN);
			//show_point(mids[ad.get_mid().index], RGB_YELLOW);
			//show_point(mids[ad.get_tail().index], RGB_RED);
		
			// mid.seg_area_x2(head);
			//mess(b, "b");

			prev_m.move_mid(ad.tail, prev_s);

			double f = ad.h_front_x2() - ad.h_back_x2();
			double b1 = prev_m.calc_ave_height_x2(prev_s);
			double b2 = ad.tail.calc_ave_height_x2(prev_m);
			double b =  b2 - b1;
			double sum = fabs(f) + fabs(b);

			min_area.update(sum, ad.tail.iter.e.iteration());

			if (skip == -1) {

				mess(f, "f");
				mess(b, "b");
				mess(sum, "sum hx2");

	//			show_point(prev_s.curr, RGB_RED);
	//			show_point(prev_m.curr, RGB_YELLOW);
	//			show_point(ad.tail.curr, RGB_GREEN);
	//			show_windows();

				mess(ad.tail.curr.report(), "h");
				mess(ad.tail.area, "h area");
				mess(prev_m.curr.report(), "m");
				mess(prev_m.area, "m area");
				mess(prev_s.curr.report(), "t");
				mess(prev_s.area, "t area");
			}

			//mess(ad.mid.seg_area_x2(ad.head), "area h");
			//mess(ad.tail.seg_area_x2(ad.mid), "area t");
			//mess(ad.head.curr.report(), "head curr");
			//ad.iterate_rev();
		} while (!ad.iterate_rev());

		//show_point(*ad.tail.iter, RGB_PURPLE, 1);
		//show_windows(0);
		//mess(min_area.report_key(), "key");

		if (min_area.report_key() >= 0) {
			show_point(mids[min_area.report_key()], RGB_YELLOW, 1);
			show_windows(0);
		} else {
			mess("oops");
		}

		return tent_vertex(0, 0.0);
	}
	*/

	/*
	tent_vertex refine_edge_edge(tent_feature& f, tent_feature& prev_f)
	{
		min_value<double> min;
		round_robin_element<> m = round_robin_element<>::interpol(f.v1.index, f.v2.index, mids.size(), 0.5);
		corner_detector<> cd(mids, m.fix_end(f.v1.index), f.v1.index, f.v1.area);

		double area = -1.0;
		int key = -1;

		//mess("start");
		do {
			//mess(cd.get_ave_height(), "h bar after");
			//show_point(cd.tail.get_vert(), RGB_YELLOW);
			//show_point(mids[prev_f.v1.index], RGB_PURPLE);
			double a1 = cd.head.calc_tot_area_x2(cd.tail);
			double a2 = cd.tail.calc_tot_area_x2();//area_accum<>(mids[prev_f.v1.index], prev_f.v1.area));
			double a = fabs(a1) + fabs(a2);

			if (key != -1 && a > area) {
				break;
			}

			key = cd.tail.iter.iteration();
			area = a;
			//mess(a1, "area 1");
			//mess(a2, "area 2");
			//mess(fabs(a1) + fabs(a2), "sum a");
			min.update(2.0 * fabs(a1) + fabs(a2), cd.tail.iter.iteration());
			//mess(prev_f.v1.area, "a2");
			//show_windows();
		} while (!cd.iterate_rev());

		key = min.report_key();

		if (key != -1) {
			//show_point(mids[key], RGB_RED, 0);
			show_windows();
		} else {
			mess("not init");
		}

		return tent_vertex(0, 0.0);
	}
	*/

	int show_case(int i1, int i2, int i3)
	{
		if (!tri_2d<>::ave_height_greater(mids_rr[i1], mids_rr[i2], mids_rr[i3], 0.5)) {
			show_point(mids_rr[i2], RGB_GREEN, 3, false);
			return 0;
		} else if (!hard_corner(mids[i1], mids[i2], mids[i3])) {
			show_point(mids_rr[i2], RGB_YELLOW, 3, false);
			return 1;
		} else {
			show_point(mids_rr[i2], RGB_RED, 3, false);
			return 2;
		}
	}

	void color_circuit(round_robin_element<> e1, round_robin_element<> e2, rgb color)
	{
		for (round_robin_element<> e = e1; e <= e2.fix_end(e1); e.inc(mids.size())) {
			color_point(mids[e.iteration()], color, 1.0, false); 
		}
	}

	void log_edge(std::vector<tent_feature>& vect, vertex v1, vertex v2)
	{
		//color_circuit(v1.index, v2.index, RGB_PURPLE);
		
		//show_point(mids[v1.index], RGB_GREEN);
		//show_point(mids[v2.index], RGB_GREEN);

		vect.push_back(tent_feature(v1, v2));
	}

	/*
void log_arc(vertex v1, vertex v2)
{
	arc_detector<> ad(mids);

	color_circuit(v1.index, v2.index, RGB_PURPLE);
	
	while (ad.cont()) {
	//	if (
		tent_features.push_back(tent_feature(v1, v2, vm));
	}
}
*/

	// shaves off the ends of a suspected segment and tests for linear
	// this is because when an edge is small, it gets confused
	bool almost_edge(tent_vertex v1, tent_vertex v2)
	{
		area_tracer<> t1(&mids, v1);
		area_tracer<> t2(&mids, v2);

		if (v1.index.iterations(v2.index, mids.size()) > 20) {
			for (int i = 0; i < 2; i++) {
				t1.iterate();
				t2.iterate_rev();
			}
		}

		return t2.ave_height_threshhold(t1, DEFAULT_LINEAR_TOL, tot_area) == 0;
	}

	void parse_features()
	{
		corner_detector<> cd(mids); 

		while (cd.cont()) {
			if (cd.iterate()) {
				log_vertex(cd.get_vert_ind());
				cd.reset();
			}
		}

		tot_area = cd.last_area;

		vertex prev_corner;
		vertex first_hard;

		prev_corner.invalidate();
		first_hard.invalidate();

		bool curr_corner = false;
		bool all_arcs = true;
		//round_robin_element<> arc_start = -1;
		round_robin_element<> arc_end = -1;
		round_robin_element<> arc_last = -1;
		round_robin_element<> ind_last = -1;
		std::vector<int> vert_hardness;
		std::vector<tent_feature> edges;

		for (round_robin_iterator<vertex> v(verts); v.cont(); ++v) {
			 if (!tri_2d<>::ave_height_greater(mids_rr[(v - 1)->index + 1], mids_rr[v->index], mids_rr[(v + 1)->index - 1], 0.5)) {
				//vert_hardness.push_back(1);
				 //mess("1");
			 } else if (hard_corner(mids_rr[(v - 1)->index + 1], mids_rr[v->index], mids_rr[(v + 1)->index - 1])) {
				//vert_hardness.push_back(3);
				//mess("3");
				if (prev_corner.valid()) {
					//mess("edge");
					//show_point(prev_corner.pixel, RGB_BLUE);
					//show_point(v->pixel, RGB_ORANGE);
					log_edge(edges, prev_corner, *v);
				}
				prev_corner = *v;
			} else {
				//mess("2");
				//vert_hardness.push_back(2);
				prev_corner.invalidate();
				if (!first_hard.valid()) {
					first_hard = -2;
				}
			}

			if (!first_hard.valid()) {
				first_hard = prev_corner;
			}

			//show_case((v - 1)->index + 1, v->index, (v + 1)->index - 1);
		}

		if (prev_corner.valid() && first_hard.index >= 0) {
			if (first_hard.index != prev_corner.index) {
				log_edge(edges, prev_corner, first_hard);
			}
		}

		//return;

		std::vector<tent_feature> feat;

		if (edges.size() > 0) {
			for (round_robin_iterator<tent_feature> f(edges); f.cont(); ++f) {
				feat.push_back(*f);
				parse_arc(feat, f->v2, (f + 1)->v1);
			}
		} else {
			// parse full arc
			parse_arc(feat);
		}

		//show_point(mids[0], RGB_PURPLE, 3, true);

		//return;
		mess("end");

		define_feat_bounds(feat);

		//return;

		/*
		for (std::vector<tent_feature>::iterator f = feat.begin(); f != feat.end(); ++f) {
			//mess(f->v1.index, "index 1");
			//mess(f->v2.index, "index 2");

			if (f->type == feature_edge) {
				color_circuit(f->v1.index, f->v2.index, RGB_GREEN);
				show_point(mids[f->v2.index], RGB_RED);
			} else if (f->type == feature_arc) {
				color_circuit(f->v1.index, f->v2.index, RGB_BLUE);
				show_point(mids[f->v2.index], RGB_ORANGE);
			}
		}
		*/

		//mess(first_hard, "first hard");
		//mess(prev_corner, "prev corner");

		//prev_corner = -1;

		/*
		value_hist_fpl<int, -2> corner_hist;
		int arc_start = -1;
		int first_edge = -1;
		*/

		//show_point(point_2d(0.0, 0.0));

		/*
		for (round_robin_iterator<int> i(vert_hardness); i.cont(); ++i) {
			if (*i == 1) {
				//show_point(verts[i.iteration()].pixel, RGB_GREEN, 3, true);
			} else if (*i == 2) {
				//show_point(verts[i.iteration()].pixel, RGB_YELLOW, 3, true);
				corner_hist = (prev_corner = -1);
			} else if (*i == 3) {
				//show_point(verts[i.iteration()].pixel, RGB_RED, 3, true);
			
				if (first_hard == -1) {
					first_hard = i.iteration();
				}

				if (arc_start != -1) {
					// parse arc
					mess("arc 1");
					parse_arc(arc_start, i.iteration());
					arc_start = -1;
				}

				if (prev_corner != -1) {
					// parse edge
					mess("edge 1");
					log_edge(verts[prev_corner], verts[i.iteration()]);
				
					arc_start = i.iteration();
					if (first_edge == -1) {
						first_edge = prev_corner;
					}
				}

				corner_hist = (prev_corner = i.iteration());
			}
		}

		if (corner_hist.first >= 0 && corner_hist.last >= 0) {
			// last edge
			mess("last edge");
			log_edge(verts[corner_hist.last], verts[corner_hist.first]);

			if (corner_hist.first != first_edge && first_edge >= 0) {
				// arc between last edge and first edge
				mess("arc 2");
				parse_arc(corner_hist.first, first_edge);
			}
		} else if (first_edge == -1) {
			// are all arcs so parse full arc
			mess("arc full");
			parse_arc_full();
		} else {
			if (arc_start != -1) {
				// last arc
				mess("arc 3");
				parse_arc(arc_start, first_hard);
			}
		}
		*/

		//return;

		//show_windows();
		//clear_windows();

		/*
		for (round_robin_iterator<vertex> v(verts); v.cont_overlap(3); ++v) {
			if (tri_2d<>::ave_height_greater((v - 1)->pixel, v->pixel, (v + 1)->pixel, 0.5)) {
				curr_corner = hard_corner((v - 1)->pixel, v->pixel, (v + 1)->pixel);


				if (curr_corner) {
					prev_corner = 1;
				} else {
					prev_corner = 0;
				}
			}
		}
		*/

		//mess(verts.size(), "verts size");

		/*
		all_arcs = true;
		prev_corner = -1;
		arc_start = -1;
		arc_end = -1;
		bool in_arc = false;

		for (round_robin_iterator<vertex> v(verts); v.cont_overlap(1); ++v) {
			curr_corner = hard_corner((v - 1)->pixel, v->pixel, (v + 1)->pixel);

			show_point(v->pixel, RGB_RED, 2);

			//mess(prev_corner, "prev");
			//mess(curr_corner, "curr");

			// prev, curr
			if (curr_corner) { // x, 1
				if (prev_corner == 1) { // 1, 1
					if (in_arc) {
						if (arc_start == -1) {
							mess("set arc end");
							arc_end = (v - 1).e;
						} else {
							mess("arc here");
							parse_arc(mids, arc_start, (v - 1).e.fix_end(arc_start));
						}
					}
	//				mess("edge");
					store_edge(*(v - 1), *v);
					all_arcs = false;
					in_arc = false;
					arc_start = v.e;
				} 
			} else { // x, 0
				if (prev_corner != -1) {
					mess("set in arc");
					in_arc = true;
				} else {
					mess("case crap");
				}
			}

			if (curr_corner) {
				prev_corner = 1;
			} else {
				prev_corner = 0;
			}
		}

		//if (in_arc) {
			if (all_arcs) {
	//			mess("parse arc full case");
				parse_arc_full(mids);
			} else if (arc_end != -1) {
				//mess(arc_start.report(), "here: arc start");
				//mess(arc_end.report(), "here: arc end");
	//			mess("parse arc case");
				parse_arc(mids, arc_start, arc_end.fix_end(arc_start));
			}
		//}
		*/

		/*
		for (round_robin_iterator<tent_feature> i(tent_features); i.cont(); ++i) {
			if (i->type == feature_edge) {
				if ((i - 1)->type == feature_edge) {
					//refine_edge_edge(*i, *(i - 1));
					mess("e-e");
				} else if ((i - 1)->type == feature_arc) {
					mess("a-e");
				}
			} else {
				if ((i - 1)->type == feature_edge) {
					mess("e-a");
				} else if ((i - 1)->type == feature_arc) {
					mess("a-a");
					//refine_arc_arc(*i, *(i - 1));
				}
			}

			show_windows();
		}
		*/
	}

	/*
void test_current_fwd()
{
	area_tracer<> t(mids);

	while (t.cont_first()) {
		//show_point(t.get_end_sample(), RGB_GREEN, 0);
		//show_windows();
		if (t.iterate_span()) {
			// changed this
			//log_vertex(t);
			
			if (t.end.e.lap == 0) {
				//show_point(t.get_vertex(), RGB_PURPLE);
			} else {
				//show_point(t.get_vertex(), RGB_ORANGE);
			}

			t.reset();
			show_windows();
		}
	}
}
*/

	/*
	void refine_verts()
	{
		for (round_robin_iterator<vertex> i(verts); i.cont(); ++i) {
			round_robin_element<> ea((i - 1)->index);
			round_robin_element<> eb((i + 1)->index);

			round_robin_element<> p1 = ea.fix_begin(i->index).mean(i->index, mids.size());
			round_robin_element<> p2 = eb.fix_end(i->index).mean(i->index, mids.size());

			max_value<double, round_robin_iterator<point_2d>> mx;
			for (round_robin_iterator<point_2d> n(p1, mids); n < p2; ++n) {
				tri_2d<> tri(
					mids[p1.i], *n, mids[p2.i]
				);
				mx.update(fabs(tri.area_x2()), n);
			}

			if (mx.report_key().e.i > 0) {
				//show_point(*mx.report_key(), RGB_GREEN);
				//show_windows(0);
			}
		}
	}
	*/

	/*
	void correct_verts3()
	{
		//	refine vertices
		//	this is the one that works best - 1/19/2014
		area_tracer<point_2d> t(mids);

		for (round_robin_iterator<vertex> i(verts); i < verts.size(); ++i) {
			int p = 
				t.correct_vert(
					verts[(i + 1).iteration()].index, 
					verts[(i + 0).iteration()].index, 
					verts[(i + 2).iteration()].index
				);

			if (p < 0) {
				//show_point(mids[verts[(i + 0).iteration()].index], RGB_ORANGE);
			} else {
				//show_point(t.get_element(p), RGB_YELLOW);	
			}

			//show_point( t.get_element(p), RGB_YELLOW);

			show_windows(0);
			get_background();
			//mess(p, "p");
		}
	}
	*/

	/*
		bool hard_corner2(point_2d p1, point_2d p2, point_2d p3, double thresh1 = 600.0, double thresh2 = 6.0)
		{
			tri_2d<> tri(p1, p2, p3);

			if (thresh1 < 0.0) {
				mess(tri.area_x2(), "ax2");
				mess(tri.ave_height(), "hbar");
			}

			return fabs(tri.area_x2()) > thresh1 || tri.ave_height_greater(thresh2);
		}
	*/

	bool hard_corner(point_2d p1, point_2d p2, point_2d p3, double thresh1 = 1100.0, double thresh2 = 20.0)
	{
		// thresh2 used to be 25.0
		// old defaults were 1100.0, 16.0

		angle_2d<> a(p1, p2, p3);
		double alpha = fabs(a.supplement().angle);
		double d1 = p2.dist(p1);
		double d2 = p2.dist(p3);
		double area = alpha * (d1 * d1 + d2 * d2);
		double arc = alpha * (d1 + d2);

		return (area > thresh1) || (arc > thresh2);
	}

public:

	feature_bank perimeter;
	std::vector<feature_bank*> voids;
	//std::vector<tent_feature> tent_features;
	std::vector<feature> features;

	shape()
	{
		init();
	}

	// shape is created when the perimeter is completed
	// and so the proto-shape is completed
	shape(tracer* p_outy)
	{
		init();
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
			rig_trace->store_rig(rig_x + 1, y);

			/*
			color_point_in(rig_x, y, RGB_RED);
			show_windows();
			*/
		} else { // new tracer
			lef_trace = rig_trace = new tracer();
			lef_trace->store_lef(lef_x, y);
			lef_trace->store_rig(rig_x + 1, y);
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
				tracer_unit ll = (*i)->lef_trace->get_last_lef();
				tracer_unit lr = (*i)->rig_trace->get_last_rig();
				(*i)->lef_trace->store_lef(ll.x, ll.y + 1);
				(*i)->rig_trace->store_rig(lr.x, ll.y + 1);

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

#endif // INC_COMPUTER_VISION