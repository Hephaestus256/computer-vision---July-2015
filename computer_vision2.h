#ifndef INC_COMPUTER_VISION2
#define INC_COMPUTER_VISION2

#include <math.h>
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\math_1d.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\math_2d.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\math_3d.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\my_stl.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\graphics_debug.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\graphics_2d.h"
#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\color.h"
#include <vector>

template <class sample_type = rgb>
class sample_reader {
	
	sample_type* source;
	quantum_2d source_dims;
	quantum_2d bot_lef, top_rig;
	unsigned int row_start;
	quantum_2d pos;

	inline unsigned int calc_source_index(int x, int y)
	{
		return x + y * source_dims.x;
	}

public:

	sample_reader (
		sample_type* p_source, quantum_2d p_source_dims,
		quantum_2d p_bot_lef = VECT_2D_INT_ORIG, quantum_2d p_top_rig = p_source_dims - quantum_2d(1, 1))
	{
		source = p_source;
		source_dims = p_source_dims;
		bot_lef = p_bot_lef;
		top_rig = p_top_rig;
	}

	~sample_reader ()
	{
	}

	quantum_2d get_source_pos()
	{
		return pos;
	}

	bool x_cont()
	{
		return pos.x <= top_rig.x;
	}

	bool y_cont()
	{
		return pos.y <= top_rig.y;
	}

	inline sample_type get_source_sample()
	{
		return source[row_start + pos.x];
	}

	void init_x()
	{		
		pos.x = bot_lef.x + 1;
	}

	void init_y()
	{
		pos.y = bot_lef.y + 1;
		row_start = calc_source_index(0, bot_lef.y) + source_dims.x;
	}

	void advance_x()
	{
		pos.x++;
	}

	void advance_y()
	{
		pos.y++;
		row_start += source_dims.x;
	}

	/*
	bool test_x()
	{
		return cannon.cv_float().color_percept().gamma(0.333).dist_euc_thresh(
			last_sample.cv_float().color_percept().gamma(0.333),
			(float)threshhold
		);
	}
	*/

	point_2d div_x()
	{
		return get_source_pos().cv_float() + point_2d(0.0f, -0.5f);
	}

	point_2d div_y()
	{
		return get_source_pos().cv_float() + point_2d(0.5f, -1.0f);
	}
};

void group_filter(
	draw_surface* in,
	quantum_2d bot_lef = VECT_2D_INT_ORIG, quantum_2d top_rig = quantum_2d(-1, -1), 
	float threshhold = 255.0 * 3.0 * 0.01 * 1.5) 
{
	typedef segment_2d<int> seg_type;
	typedef pixel<float> wrk_sample_type;
	typedef rgb inp_sample_type;

	class group_seg {
	public:
		seg_type seg;
		wrk_sample_type cannon;
		inp_sample_type raw_cannon;
		pixel<int> sum;
		bool locked;

		group_seg()
		{
		}

		group_seg(int p_x, wrk_sample_type p_cannon, inp_sample_type p_raw_cannon)
		{
			seg.a = p_x;
			cannon = p_cannon;
			raw_cannon = p_raw_cannon;
			sum = 0;
			locked = false;
		}

		group_seg(seg_type p_seg, wrk_sample_type p_cannon, inp_sample_type p_raw_cannon, pixel<int> p_sum)
		{
			seg = p_seg;
			cannon = p_cannon;
			raw_cannon = p_raw_cannon;
			sum = p_sum;
			locked = false;
		}

		bool test(wrk_sample_type p_sample, float p_threshhold = 5.0)
		{
			return cannon.euc_dist_thresh(p_sample, p_threshhold);
		}
		
		wrk_sample_type mean_sample()
		{
			return sum * (1.0 / double(seg.b - seg.a));
		}
	};

	typedef std::list<group_seg> seg_list;

	class group_seg_list {

		void init_seg(int p_x)
		{
			wrk_seg = group_seg(p_x, sample, raw_sample);
		}

		wrk_sample_type conv_inp_sample(inp_sample_type p_sample)
		{
			return p_sample.color_percept2();
		}

	public:

		seg_list* working;
		seg_list* reference;
		group_seg wrk_seg;
		seg_list::iterator ref_iter;
		wrk_sample_type sample;
		inp_sample_type raw_sample;
		int start_x;
		bool conx;

		group_seg_list(int p_start_x)
		{
			start_x = p_start_x;
			working = new seg_list;
			reference = new seg_list;
		}

		~group_seg_list()
		{
			delete working;
			delete reference;
		}

		void set_sample(inp_sample_type p_sample)
		{
			sample = conv_inp_sample(p_sample);
			raw_sample = p_sample.reverse();
			wrk_seg.sum += p_sample;
		}
		
		void init_row()
		{
			init_seg(start_x);

			// swap working and reference lists
			seg_list* temp = working;
			working = reference;
			reference = temp;
			conx = false;

			// clear working list about to be used
			working->clear();
			ref_iter = reference->begin();
		}

		void cut_seg(int p_x)
		{
			wrk_seg.seg.b = p_x - 1;
			working->push_back(wrk_seg);
			init_seg(p_x);
		}

		void copy_cannon()
		{
			if (!wrk_seg.locked) {
				wrk_seg.cannon = ref_iter->cannon;
				wrk_seg.raw_cannon = ref_iter->raw_cannon;
				wrk_seg.locked = true;
			} else if (!ref_iter->locked) {
				ref_iter->cannon  = wrk_seg.cannon;
				ref_iter->raw_cannon  = wrk_seg.raw_cannon;
				ref_iter->locked = true;
			}
		}
		
		void end_row(int p_x)
		{
			wrk_seg.seg.b = p_x;
			working->push_back(wrk_seg);
		}

		bool test_working(float p_threshhold)
		{
			return wrk_seg.test(sample, p_threshhold);
		}

		bool update_ref(quantum_2d p_p)
		{
			bool moved = false;

			if (!reference->empty()) {
				while (ref_iter->seg.b < p_p.x) {
					++ref_iter;
					moved = true;
				}
			}

			return moved;
		}

		bool test_reference(quantum_2d p_p, float p_threshhold)
		{
			return !reference->empty() && ref_iter->test(sample, p_threshhold);
		}

		bool test_ref_cannon(quantum_2d p_p, float p_threshhold)
		{
			return !reference->empty() && ref_iter->test(wrk_seg.cannon, p_threshhold);
		}

		void parse0(sample_reader<rgb>& samp, float threshhold)
		{
			if (!test_working(threshhold)) { // test horizontally
				cut_seg(samp.get_source_pos().x);
			}

			update_ref(samp.get_source_pos());
	
			if (test_reference(samp.get_source_pos(), threshhold)) { // test vertically
				copy_cannon();
			}
		}

		void parse1(sample_reader<rgb>& samp, float threshhold)
		{
			update_ref(samp.get_source_pos());
	
			if (test_reference(samp.get_source_pos(), threshhold)) { // test vertically
				if (!conx) {
					cut_seg(samp.get_source_pos().x);
				}
				conx = true;
				copy_cannon();
			}  else {
				if (conx) {
					//cut_seg(samp.get_source_pos().x);
				}
				conx = false;
			}

			if (!test_working(threshhold)) { // test horizontally
				cut_seg(samp.get_source_pos().x);
				//conx = false;
			}


		}

		void parse2(sample_reader<rgb>& samp, float threshhold)
		{
			if (!test_working(threshhold)) { // test horizontally
				//color_seg_horz(wrk_seg.seg.a, samp.get_source_pos().x, samp.get_source_pos().y, wrk_seg.raw_cannon);
				conx = false;
				cut_seg(samp.get_source_pos().x);
				show_point(samp.get_source_pos().left(), RGB_ORANGE, 2, false);

			} else {
				//conx = true;

				update_ref(samp.get_source_pos());

				if (test_reference(samp.get_source_pos(), threshhold)) { // test vertically
					conx = true;
					copy_cannon();
				} else { // if (!test_ref_cannon(samp.get_source_pos(), threshhold)) {
					show_point(samp.get_source_pos().top(), RGB_BLUE, 2, false);
					if (conx) {
						cut_seg(samp.get_source_pos().x);
						show_point(samp.get_source_pos().left(), RGB_RED, 2, false);
					}

					conx = false;
				}			
			}
		}

		void parse3(sample_reader<rgb>& samp, float threshhold)
		{
			update_ref(samp.get_source_pos());

			if (false) { // test_reference(samp.get_source_pos(), threshhold)) { // vertically connected
				//copy_cannon();
				//conx = true;
			} else {
				//show_point(samp.get_source_pos().top(), RGB_BLUE, 2, false);

				if (test_working(threshhold)) { // horizontally connected
					conx = true;
				} else { // disconnected
					if (conx) { // previously connected
						//show_point(samp.get_source_pos().left(), RGB_ORANGE, 2, false);
						cut_seg(samp.get_source_pos().x);
					}

					conx = false;
				}
			}
		}
	};

	if (top_rig.x < 0) {
		top_rig = in->get_top_rig();
	}

	sample_reader<rgb> samp((rgb*)in->get_direct_ptr(), in->get_dims(), bot_lef, top_rig);
	group_seg_list segs(bot_lef.x);

	for (samp.init_y(); samp.y_cont(); samp.advance_y()) {
		bool conx = false;
		segs.init_row();

		for (samp.init_x(); samp.x_cont(); samp.advance_x()) {
			segs.set_sample(samp.get_source_sample());
			segs.parse0(samp, threshhold);
		}

		for (seg_list::iterator i = segs.reference->begin(); i != segs.reference->end(); ++i) {
			color_seg_horz(i->seg.a, i->seg.b, samp.get_source_pos().y, i->raw_cannon);
			//show_point(point_2d(float(i->seg.a), samp.get_source_pos().y - 0.5f), RGB_YELLOW, 2, false);
		}

		segs.end_row(samp.get_source_pos().x);
	}
}

#endif // INC_COMPUTER_VISION2