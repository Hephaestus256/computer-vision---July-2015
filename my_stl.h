#ifndef INC_MY_STL
#define INC_MY_STL

#define MY_STL_NOT_INIT (int)(-4)
#define MY_STL_ERROR (int)(-3)
#define MY_STL_NO_SOLUTION (int)(-2)
#define MY_STL_MULTIPLE_VALUES (int)(-1)
#define MY_STL_VALID (int)(0)
#define MY_STL_IMAGINARY (int)(1)

#define ROUND_ROBIN_ELEMENT_LOOP round_robin_element<int>(0, 1)
#define ROUND_ROBIN_ELEMENT_LOOP_REV round_robin_element<int>(0, -1)
#define ROUND_ROBIN_ELEMENT_NULL round_robin_element<int>(-10000, -10000)

#define CHUNK_LIST_SIZE_DEFAULT 32

#include "C:\Users\Nathan\Documents\Visual Studio 2012\Libraries\alt_null.h"
#include <vector>
#include <list>


// base class to let a value become several different values of NULL (statuses)
template <
	class stat_type = int,
	int stat_not_init = MY_STL_NOT_INIT, 
	int stat_valid = MY_STL_VALID>
class null_value_base {
public:
	
	stat_type status;

	inline null_value_base()
	{
		status = stat_not_init;
	}

	inline bool is_valid()
	{
		return status >= stat_valid;
	}

	inline bool is_init()
	{
		return status != stat_not_init;
	}
 
	inline bool not_init()
	{
		return status == stat_not_init;
	}
};

template <
	class data_type = int, 
	int stat_not_init = MY_STL_NOT_INIT, 
	int stat_valid = MY_STL_VALID>
class init_value : public null_value_base<data_type, stat_not_init, stat_valid> {
public:

	inline init_value() : null_value_base<data_type, stat_not_init, stat_valid>
	{
	}

	inline data_type operator = (data_type p_data)
	{
		return (status = p_data);
	}
};

template <class data_type = int, data_type stat_not_init = MY_STL_NOT_INIT>
class value_hist_fpl {
public:
	data_type first, prev, last;

	inline value_hist_fpl()
	{
		first = prev = last = stat_not_init;
	}

	inline value_hist_fpl(data_type d)
	{
		first = prev = last = d;
	}

	inline bool first_is_init()
	{
		return first != stat_not_init;
	}

	inline bool prev_is_init()
	{
		return prev != stat_not_init;
	}

	inline data_type operator = (data_type d)
	{
		if (!first_is_init()) {
			first = d;
		}
		prev = last;
		last = d;

		return d;
	}
};

// lets a value take on several versions of NULL (statuses)
template <class data_type>
class null_value : public null_value_base<> {
public:
	data_type data;

	inline null_value() : null_value_base()
	{
	}

	inline null_value(data_type p_data, int p_status = MY_STL_VALID)
	{
		data = p_data;
		status = p_status;
	}

	inline data_type operator = (data_type p_data)
	{
		status = MY_STL_VALID;
		return (data = p_data);
	}
};

// stores null or first value in a sequence
template <class data_type>
class value_hist_nf : public null_value<data_type> {
public:
	inline value_hist_nf() : null_value()
	{
	}

	inline data_type operator = (data_type p_data)
	{
		if (not_init()) {
			status = MY_STL_VALID;
			data = p_data;
		}

		return data;
	}
};

// stores null or first and last value in a sequence
template <class data_type>
class value_hist_nfl : public null_value_base<> {
public:
	data_type first, last;

	inline value_hist_nfl() : null_value_base()
	{
	}

	inline data_type operator = (data_type p_data)
	{
		if (not_init()) {
			status = MY_STL_VALID;
			first = p_data;
		}

		last = p_data;
		return p_data;
	}
};

// value history null first previous last
template <class data_type>
class value_hist_nfpl : public value_hist_nfl<data_type> {
public:
	data_type prev;

	inline value_hist_nfpl() : value_hist_nfl()
	{
	}

	inline data_type operator = (data_type p_data)
	{
		if (not_init()) {
			status = MY_STL_VALID;
			first = p_data;
		}

		prev = last;
		last = p_data;
		return p_data;
	}
};



/*
template <class gen1>
class my_stl {
	int status;
public:
	gen1 iter;

	my_stl(int p_status = MY_STL_NOT_INIT)
	{
		status = p_status;
	}

	my_stl(gen1 p_iterator)
	{
		iter = p_iterator;
	}

	inline gen1 operator = (gen1 p_iterator)
	{
		return iter = p_iterator;
	}

	inline bool operator != (gen1 p_iterator)
	{
		return iter != p_iterator;
	}

	inline bool operator == (gen1 p_iterator)
	{
		return iter == p_iterator;
	}

	inline gen1 operator -> ()
	{
		return iter;
	}

	inline gen1 operator ++ ()
	{
		return ++iter;
	}

	inline gen1 operator -- ()
	{
		return --iter;
	}

	inline void set_status(int p_status)
	{
		status = p_status;
	}

	inline int get_status()
	{
		return status;
	}
};
*/

template <class gen1>
class my_list {
public:
	class element {
		element* next;
		gen1 data;
	public:
		element(element* p_next = (element*)_alt_null.get_null(MY_STL_NOT_INIT))
		{
			next = p_next;
		}

		element(gen1 p_data, element* p_next = (element*)_alt_null.get_null(MY_STL_NOT_INIT))
		{
			data = p_data;
			next = p_next;
		}

		element* get_next()
		{
			return next;
		}

		void set_next(element* p_next = _alt_null.get_null(MY_STL_NOT_INIT))
		{
			next = p_next;
		}

		gen1 get_data()
		{
			return data;
		}

		gen1* get_data_ptr()
		{
			return &data;
		}
	};

	class iterator {
		element* el;

		void set_null(int null_n = MY_STL_NOT_INIT)
		{
			el = (element*)_alt_null.get_null(null_n);
		}

	public:
		iterator ()
		{
			set_null();
		}

		iterator (element* p_el)
		{
			el = p_el;
		}

		bool is_valid()
		{
			return el != (element*)_alt_null.get_null(MY_STL_NOT_INIT);
		}

		inline void invalidate()
		{
			set_null();
		}

		void operator ++ ()
		{
			el = el->get_next();
		}

		inline gen1 operator * ()
		{
			return el->get_data();
		}

		inline gen1* operator -> ()
		{
			return &el->get_data();
		}

		inline bool operator == (iterator p_it)
		{
			return p_it.el == el;
		}

		inline bool operator != (iterator p_it)
		{
			return p_it.el != el;
		}
	};

private:
	element* start;
	element* finish;
	int el_ct;

	void init()
	{
		start = finish = (element*)_alt_null.get_null(MY_STL_AT_END);
		el_ct = 0;
	}

public:
	void clear()
	{
		element* curr = start;
		while(curr != (element*)_alt_null.get_null(MY_STL_AT_END)) {
			element* next = curr->get_next();
			delete curr;
			curr = next;
		};
		init();
	}

	my_list()
	{
		init();
	}

	~my_list()
	{
		clear();
	}

	gen1* push_back(gen1 p_data)
	{
		element* curr = new element(p_data, (element*)_alt_null.get_null(MY_STL_AT_END));

		if (finish == (element*)_alt_null.get_null(MY_STL_AT_END)) {
			start = curr;
		} else {
			finish->set_next(curr);
		}

		el_ct++;
		finish = curr;
		return (*curr).get_data_ptr();
	}

	iterator begin()
	{
		return start;
	}

	iterator end()
	{
		return iterator((element*)_alt_null.get_null(MY_STL_AT_END));
	}

	int size()
	{
		return el_ct;
	}
};

/*
Class "appendable_element"
Primises of this class:
- Has no central object, but are all joined elements
- Optimized to add elements to the beginning or end, but not insert elements.
- Order of all methods should be constant and independent of the number of elements. O(c) not O(n).
*/
template <class gen1>
class appendable_element {
public:
	appendable_element* next;
	appendable_element* other_end;
	gen1 data;
public:
	class iterator {
		appendable_element* ptr;
	public:
		void operator ++ ()
		{
			ptr = ptr->next;
		}

		gen1 operator * ()
		{
			return data;
		}

		gen1* operator -> ()
		{
			return &data;
		}
	};

	appendable_element ()
	{
		other_end = this;
		next = at_end();
	}

	appendable_element (gen1 p_data)
	{
		data = p_data;
		other_end = this;
		next = at_end();
	}

	inline appendable_element* get_next ()
	{
		return next;
	}

	inline bool append (appendable_element* p_tail)
	{
		next = p_tail;
		
		if (is_connected(p_tail)) { // joining to itself, round robin
			next = (appendable_element*)_alt_null.get_null(ALT_NULL_NA);
			return true;
		} else { // not round robin
			appendable_element* other_end_bak = other_end;
			other_end->other_end = p_tail->other_end;
			p_tail->other_end->other_end = other_end_bak;
			return false;
		}
	}

	inline bool prepend (appendable_element* p_begin)
	{
		p_begin->next = begin;

		if (is_connected(p_begin)) { // joining to itself, round robin
			return true;
		} else { // not round robin
			begin = p_begin->begin;
			return false;
		}
	}

	// if is connected or is same list, return true, otherwise false
	inline bool is_connected (appendable_element* p_ae)
	{
		return other_end == p_ae;
	}

	inline static appendable_element* at_end()
	{
		return (appendable_element*)_alt_null.get_null(ALT_NULL_NOT_INIT);
	}

	inline gen1* data_ptr()
	{
		return &data;
	}
};


template <class iter_type = int>
class round_robin_element {

	static char outp[50];

public:

	int lap;
	iter_type i;

	inline void init(iter_type p_i = 0, int p_lap = 0)
	{
		lap = p_lap;
		i = p_i;
	}

	inline round_robin_element()
	{
	}

	inline round_robin_element(iter_type p_i, int p_lap = 0)
	{
		init(p_i, p_lap);
	}

	inline round_robin_element(iter_type p_i, int p_lap, int p_size)
	{
		init(p_i, p_lap);
		resolve(p_size);
	}

	// constructor for end-point of for loop
	inline round_robin_element(iter_type p_begin, iter_type p_i, bool forward)
	{
		if (forward) { // forward
			if (p_begin <= p_i) {
				init(p_i, 0);
			} else {
				init(p_i, 1);
			}
		} else { // reverse
			if (p_begin >= p_i) {
				init(p_i, 0);
			} else {
				init(p_i, -1);
			}
		}
	}

	inline round_robin_element(round_robin_element p_i, round_robin_element p_lap)
	{
		init(p_i.i, p_lap.lap);
	}

	inline round_robin_element(iter_type p_i, round_robin_element p_lap)
	{
		init(p_i, p_lap.lap);
	}

	inline round_robin_element combine_i(round_robin_element p_i, int p_size)
	{
		return round_robin_element(p_i.i, lap).resolve(p_size);
	}

	inline round_robin_element combine_lap(round_robin_element p_lap, int p_size)
	{
		return round_robin_element(i, p_lap.lap).resolve(p_size);
	}

	inline round_robin_element fix_begin(round_robin_element p_end)
	{
		if (*this > p_end) {
			return *this - ROUND_ROBIN_ELEMENT_LOOP;
		} else {
			return *this;
		}
	}

	inline static round_robin_element fix_begin(iter_type p_begin, iter_type p_end)
	{
		if (p_begin > p_end) {
			return round_robin_element(p_begin, -1);
		} else {
			return round_robin_element(p_begin);
		}
	}

	inline static round_robin_element fix_end(iter_type p_begin, iter_type p_end)
	{
		if (p_begin > p_end) {
			return round_robin_element(p_end, 1);
		} else {
			return round_robin_element(p_end);
		}
	}

	inline round_robin_element fix_end(round_robin_element p_begin)
	{
		if (*this < p_begin) {
			return *this + ROUND_ROBIN_ELEMENT_LOOP;
		} else {
			return *this;
		}
	}

	inline bool test_end(round_robin_element<> fin)
	{
		return *this <= fix_end(fin);
	}

	/*
	inline round_robin_element fix_end(round_robin_element p_begin, int p_size)
	{
		return fix_end(p_begin).resolve_pos(p_size);
	}
	*/

	inline iter_type iteration()
	{
		return i;
	}

	inline iter_type effective(int p_size)
	{
		return i + (lap * p_size);
	}

	inline void resolve_long(int p_size)
	{
		if (p_size > 0) {
			int ef = effective(p_size);
			i = ef % p_size;
			lap = ef / p_size;
		}
	}

	inline round_robin_element resolve(int p_size)
	{
		if (p_size > 0) {
			if (i < 0) {
				i += p_size;
				lap--;
			} else if (i >= p_size) {
				i -= p_size;
				lap++;
			}
		}

		return *this;
	}

	inline bool resolve_this_pos(int p_size)
	{
		if (p_size > 0) {
			if (i >= p_size) {
				i -= p_size;
				lap++;
				return true;
			}
		}

		return false;
	}

	inline round_robin_element resolve_pos(int p_size)
	{
		round_robin_element ret = *this;
		ret.resolve_this_pos(p_size);
		return ret;
	}

	inline round_robin_element resolve_neg(int p_size)
	{
		round_robin_element ret = *this;
		ret.resolve_this_neg(p_size);
		return ret;
	}

	inline bool resolve_this_neg(int p_size)
	{
		if (p_size > 0) {
			if (i < 0) {
				i += p_size;
				lap--;
				return true;
			}
		}

		return false;
	}

	// within range inclusive - assumes r1 comes before r2
	inline static bool range(iter_type i, iter_type r1, iter_type r2)
	{
		if (r1 < r2) {
			return i >= r1 && i <= r2;
		} else {
			return i >= r1 || i <= r2;
		}
	}

	inline static iter_type iteration_pos(iter_type i, int p_size)
	{
		if (i >= p_size) {
			return i - p_size;
		} else {
			return i;
		}
	}

	inline static iter_type iteration_neg(iter_type i, int p_size)
	{
		if (i < 0) {
			return i + p_size;
		} else {
			return i;
		}
	}

	inline static iter_type iteration(iter_type i, int p_size)
	{
		if (i < 0) {
			return i + p_size;
		} else if (i >= p_size) {
			return i - p_size;
		} else {
			return i;
		}
	}

	inline round_robin_element mean(round_robin_element p_e, int p_size)
	{
		return round_robin_element(((*this + p_e).effective(p_size)) >> 1).resolve(p_size);
	}

	inline round_robin_element mean(round_robin_element p_e, int p_size, double weight)
	{
		iter_type a = effective(p_size);
		iter_type b = p_e.effective(p_size);
		return round_robin_element(a + round(double(b - a) * weight)).resolve(p_size);
	}

	inline round_robin_element interpol(round_robin_element p_e, int p_size, double alpha = 0.5)
	{
		return round_robin_element(interpolate(alpha, effective(p_size), p_e.effective(p_size))).resolve(p_size);
	}

	inline static round_robin_element interpol(int i1, int i2, int size, double alpha = 0.5)
	{
		return round_robin_element<>(i1).interpol(fix_end(i1, i2), size, alpha);
	}

	inline bool operator == (round_robin_element rr)
	{
		return (rr.lap == lap) && (rr.i == i);
	}

	inline bool operator != (round_robin_element rr)
	{
		return (rr.lap != lap) || (rr.i != i);
	}

	inline bool operator < (round_robin_element rr)
	{
		if (lap == rr.lap) {
			return i < rr.i;
		} else  {
			return lap < rr.lap;
		}
	}

	inline bool operator <= (round_robin_element rr)
	{
		if (lap == rr.lap) {
			return i <= rr.i;
		} else  {
			return lap < rr.lap;
		}
	}

	inline bool operator > (round_robin_element rr)
	{
		if (lap == rr.lap) {
			return i > rr.i;
		} else {
			return lap > rr.lap;
		}
	}

	inline bool operator >= (round_robin_element rr)
	{
		if (lap == rr.lap) {
			return i >= rr.i;
		} else {
			return lap > rr.lap;
		}
	}

	inline round_robin_element inc(int p_size)
	{
		i++;
		resolve_this_pos(p_size);
		return *this;
	}

	template <class gen>
	inline round_robin_element inc(int p_size, gen p_inc)
	{
		i += p_inc;
		resolve_this_pos(p_size);
		return *this;
	}

	inline bool inc_carry(int p_size)
	{
		i++;
		return resolve_this_pos(p_size);
	}

	inline round_robin_element dec(int p_size)
	{
		i--;
		resolve_this_neg(p_size);
		return *this;
	}

	template <class gen>
	inline round_robin_element dec(int p_size, gen p_dec)
	{
		i -= p_dec;
		resolve_this_neg(p_size);
		return *this;
	}

	inline bool dec_carry(int p_size)
	{
		i--;
		return resolve_this_neg(p_size);
	}

	inline round_robin_element operator + (round_robin_element e)
	{
		return round_robin_element(i + e.i, lap + e.lap);
	}

	inline round_robin_element operator - (round_robin_element e)
	{
		return round_robin_element(i - e.i, lap - e.lap);
	}

	inline round_robin_element add(iter_type p_n, int p_size = -1)
	{
		return round_robin_element(i + p_n, lap).resolve(p_size);
	}

	inline round_robin_element add(round_robin_element e, int p_size = -1)
	{
		return (*this + e).resolve(p_size);
	}

	inline round_robin_element sub(iter_type p_n, int p_size = -1)
	{
		return round_robin_element(i - p_n, lap).resolve(p_size);
	}

	inline round_robin_element sub(round_robin_element e, int p_size = -1)
	{
		return (*this - e).resolve(p_size);
	}

	inline round_robin_element operator * (iter_type n)
	{
		return round_robin_element(i * n, lap * n);
	}

	template <class gen>
	inline round_robin_element mult(gen n, int p_size = -1)
	{
		return round_robin_element(int((gen)effective(p_size) * n)).resolve(p_size);
	}

	inline int iterations(round_robin_element<> to, int p_size)
	{
		return 1 + (to.i - i) + (to.lap - lap) * p_size;
	}

	template <class gen>
	inline gen lookup(std::vector<gen> arr)
	{
		return arr[i];
	}

	inline void invalidate()
	{
		*this = ROUND_ROBIN_ELEMENT_NULL;
	}

	inline bool valid()
	{
		return *this != ROUND_ROBIN_ELEMENT_NULL;
	}

	inline char* report()
	{
		sprintf(outp, "lap=%i, i=%i", lap, i);
		return outp;
	}
};


template <class iter_type>
char round_robin_element<iter_type>::outp[50];

template <class data_type, class container_type = std::vector<data_type>, class iter_type = int>
class round_robin_iterator {
	
	static char outp[70];

public:
	round_robin_element<iter_type> e;
	container_type* vect;

	void init(container_type& p_vect)
	{
		vect = &p_vect;
	}

	void init(container_type& p_vect, round_robin_element<iter_type> p_e)
	{
		vect = &p_vect;
		e = p_e;
	}

	void init(container_type& p_vect, bool reverse)
	{
		vect = &p_vect;
		e = p_vect.size() - 1;
	}

	round_robin_iterator()
	{
	}

	round_robin_iterator(iter_type p_i)
	{
		e = round_robin_element<iter_type>(p_i, 0);
	}

	round_robin_iterator(round_robin_element<iter_type> p_e)
	{
		e = p_e;
	}

	round_robin_iterator(round_robin_element<iter_type> p_e, int p_size)
	{
		e = p_e;
	}

	round_robin_iterator(container_type& p_vect, int p_ind, bool reverse)
	{
		if (reverse) {
			e = p_vect.size() - 1 - p_ind;
		} else {
			e = p_ind;
		}

		vect = &p_vect;
	}

	round_robin_iterator(container_type& p_vect, round_robin_element<iter_type> p_e = 0)
	{
		e = p_e;
		vect = &p_vect;
	}
	
	round_robin_iterator(container_type& p_vect, bool reverse)
	{
		init(p_vect, true);
	}

	round_robin_iterator(round_robin_iterator p_i, round_robin_iterator p_lap)
	{
		e = round_robin_element<>(p_i.e, p_lap.e);
		vect = p_i.vect;
	}

	unsigned size()
	{
		return vect->size();
	}

	inline iter_type iteration()
	{
		return e.i;
	}

	inline iter_type iteration(int offset)
	{
		return e.add(offset, vect->size()).i;
	}

	inline bool cont()
	{
		return e < ROUND_ROBIN_ELEMENT_LOOP;
	}

	inline bool cont_rev()
	{
		return e >= 0;
	}

	// assumes this and i both point to same container
	inline void copy_state(round_robin_iterator i)
	{
		e = i.e;
	}

	inline bool cont_overlap(int overlap_i = 1, int overlap_lap = 0)
	{
		return e < round_robin_element<iter_type>(overlap_i, overlap_lap + 1);
	}

	template <class gen>
	inline round_robin_iterator operator = (gen p_i)
	{
		e = p_i;
		return e;
	}

	inline round_robin_iterator operator ++ ()
	{
		return e.inc(vect->size());
	}

	inline round_robin_iterator operator -- ()
	{
		return e.dec(vect->size());
	}

	inline bool inc(double area)
	{
		return e.inc_carry(vect->size());
	}

	inline bool dec(double area)
	{
		return e.dec_carry(vect->size());
	}

	inline round_robin_iterator operator += (iter_type p_n)
	{
		return round_robin_iterator(*vect, e.inc(vect->size(), p_n));
	}

	inline round_robin_iterator operator -= (iter_type p_n)
	{
		return round_robin_iterator(*vect, e.dec(vect->size(), p_n));
	}

	inline round_robin_iterator operator + (round_robin_iterator p_i)
	{
		return round_robin_iterator(*vect, e.add(p_i.e, vect->size()));
	}

	inline round_robin_iterator operator + (iter_type p_n)
	{
		return round_robin_iterator(*vect, e.add(p_n, vect->size()));
	}

	inline round_robin_iterator operator - (round_robin_iterator p_i)
	{
		return round_robin_iterator(*vect, e.sub(p_i.e, vect->size()));
	}

	inline round_robin_iterator operator - (iter_type p_n)
	{
		return round_robin_iterator(*vect, e.sub(p_n, vect->size()));
	}

	template <class gen>
	inline round_robin_iterator operator * (gen p_n)
	{
		return round_robin_iterator(*vect, e.mult(p_n, vect->size()));
	}

	inline bool& operator == (round_robin_iterator p_i)
	{
		return e == p_i.e;
	}

	inline bool operator != (round_robin_iterator p_i)
	{
		return e != p_i.e;
	}

	inline bool operator < (round_robin_iterator p_i)
	{
		return e < p_i.e;
	}

	inline bool operator <= (round_robin_iterator p_i)
	{
		return e <= p_i.e;
	}

	inline bool operator > (round_robin_iterator p_i)
	{
		return e > p_i.e;
	}

	inline bool operator >= (round_robin_iterator p_i)
	{
		return e >= p_i.e;
	}

	inline round_robin_iterator mean(round_robin_iterator p_i)
	{
		return round_robin_iterator(e.mean(p_i.e, vect->size()), vect->size());
	}

	inline round_robin_iterator mean(round_robin_iterator p_i, double weight)
	{
		return round_robin_iterator(e.mean(p_i.e, vect->size(), weight), vect->size());
	}

	inline data_type get_element(iter_type p_n)
	{
		round_robin_element<iter_type> e(p_n);
		return (*vect)[e.resolve(vect->size()).i];
	}

	inline data_type operator * ()
	{
		return (*vect)[e.i];
	}

	inline data_type operator [] (int p_n)
	{
		round_robin_element<> el(p_n, (int)0, (int)vect->size());
		return (*vect)[el.resolve_pos(vect->size()).i];
	}

	inline data_type* operator -> ()
	{
		return &(*vect)[e.i];
	}

	inline char* report()
	{
		sprintf(outp, "lap=%i, i=%i, size=%i", e.lap, e.i, vect->size());
		return outp;
	}
};
template <class data_type, class container_type, class iter_type>
char round_robin_iterator<data_type, container_type, iter_type>::outp[70];

/* When updated by a sequence of values, if value_a and value_b (A & B) are both in the set,
     it will report a mixed state.  If value_a is in the set, but value_b is not (A & !B) then status
	 is A.  If value_b is in the set, but value_a is not (!A & B) then status is B.  If neither A nor
	 B are in the set (!A & !B) then status is null. */
template <class type = int, type value_a = -1, type value_b = 1>
class mixed_value {
	enum status_values {status_null, status_a, status_b, status_mixed};
	status_values status;

	inline void init()
	{
		status = status_null;
	}

public:

	inline mixed_value()
	{
		init();
	}

	bool update(type value)
	{
		if (status == status_mixed) {
			return false;
		} else if (value == value_a) {
			if (status == status_b) {
				status = status_mixed;
				return false;
			}
			status = status_a;
		} else if (value == value_b) {
			if (status == status_a) {
				status = status_mixed;
				return false;
			}
			status = status_b;
		}

		return true;
	}

	inline bool test()
	{
		return status <> status_mixed;
	}

	inline bool test_a()
	{
		return status == status_a;
	}

	inline bool test_b()
	{
		return status == status_b;
	}

	inline bool test_null()
	{
		return status == status_null;
	}
};


template <class data_type, int chunk_size = 512 / sizeof(data_type)>
class chunk_list_forward {

	class chunk {
	public:
		data_type data[chunk_size];
		chunk* next;
		chunk* prev;
	};

	chunk* first_chunk;
	chunk* last_chunk;
	int first_chunk_pos;
	int last_chunk_pos;
	int size_base;

public:

	class iterator {
	public:
		chunk* chk;
		int index;

		iterator()
		{
		}

		iterator(chunk* p_chunk, int p_index)
		{
			chk = p_chunk;
			index = p_index;
		}

		void operator ++ ()
		{
			if (index >= chunk_size - 1) {
				chk = chk->next;
				index = 0;
			} else {
				++index;
			}
		}

		bool operator == (iterator it)
		{
			return (chk == it.chk) && (index == it.index);
		}

		bool operator != (iterator it)
		{
			return (chk != it.chk) || (index != it.index);
		}

		data_type operator * ()
		{
			return chk->data[index];
		}
	};

	chunk_list_forward()
	{
		first_chunk_pos = 0;
		last_chunk_pos = 0;
		first_chunk = last_chunk = new chunk;
		last_chunk->next = NULL;
		size_base = 1 - chunk_size;
	}

	~chunk_list_forward() 
	{
		// free all blocks allocated
		while (first_chunk != NULL) {
			chunk* ch = first_chunk->next;
			delete first_chunk;
			first_chunk = ch;
		}
	}

	iterator begin()
	{
		return iterator(first_chunk, first_chunk_pos);
	}

	iterator rbegin()
	{
		return iterator(last_chunk, last_chunk_pos);
	}

	iterator end()
	{
		if (last_chunk_pos >= chunk_size - 1) {
			return iterator(NULL, 0);
		} else {
			return iterator(last_chunk, last_chunk_pos + 1);
		}
	}

	iterator rend()
	{
		if (first_chunk_pos <= 0) {
			return iterator(NULL, chunk_size - 1);
		} else {
			return iterator(first_chunk, first_chunk_pos);
		}
	}

	inline data_type front()
	{
		return first_chunk->data[first_chunk_pos];
	}

	inline data_type back()
	{
		return last_chunk->data[last_chunk_pos];
	}

	void push_front(data_type data)
	{
		if (first_chunk_pos <= 0) {
			chunk* n = new chunk;

			if (last_chunk == NULL) {
				last_chunk = n;
				n->next = NULL;
			} else {
				n->next = first_chunk;
			}

			first_chunk = n;
			first_chunk_pos = chunk_size - 1;
		} else {
			--first_chunk_pos;
		}

		first_chunk->data[first_chunk_pos] = data;
	}

	inline void push_back(data_type& data)
	{
		if (++last_chunk_pos >= chunk_size) {
			chunk* n = new chunk;
			n->next = NULL;
			last_chunk->next = n;
			last_chunk = n;
			last_chunk_pos = 0;
			size_base += chunk_size;
		}

		last_chunk->data[last_chunk_pos] = data; // write data to chunk
	}

	inline void pop_front()
	{
		if (first_chunk_pos >= chunk_size - 1) {
			chunk* ch = first_chunk->next;
			delete first_chunk;
			
			if ((first_chunk = ch) == NULL) {
				last_chunk = NULL;
			}

			first_chunk_pos = 0;
			size_base -= chunk_size;
		} else {
			++first_chunk_pos;
		}
	}

	// pop front no delete 
	inline void pop_front_no_del()
	{
		if (first_chunk_pos >= chunk_size - 1) {
			chunk* ch = first_chunk->next;
			
			if ((first_chunk = ch) == NULL) {
				last_chunk = NULL;
			}

			first_chunk_pos = 0;
			size_base -= chunk_size;
		} else {
			++first_chunk_pos;
		}
	}

	int size()
	{
		return size_base - first_chunk_pos + last_chunk_pos;
	}

	bool empty()
	{
		return size_base != first_chunk_pos - last_chunk_pos;
	}
};


template <class data_type, int chunk_size = CHUNK_LIST_SIZE_DEFAULT>
class chunk_list_reverse {
};


// todo: be able to specify your own allocator methods and default to
// internal allocator.  then you can specify mem_pool allocators
template <class data_type, unsigned int chunk_size = 512 / sizeof(data_type)>
class chunk_list {

	class chunk {
	public:
		data_type data[chunk_size];
		chunk* next;
		chunk* prev;
	};

	chunk* first_chunk;
	chunk* last_chunk;
	unsigned int first_chunk_pos;
	unsigned int last_chunk_pos;
	unsigned int size_base;

public:

	class iterator {
	public:
		chunk* chk;
		unsigned int index;

		iterator()
		{
		}

		iterator(chunk* p_chunk, unsigned int p_index = 0)
		{
			chk = p_chunk;
			index = p_index;
		}

		data_type* ptr()
		{
			return &chk[index];
		}

		bool inc()
		{
			if (index >= chunk_size - 1) {
				chk = chk->next;
				index = 0;
				return true;
			} else {
				++index;
				return false;
			}
		}

		bool dec()
		{
			if (index <= 0) {
				chk = chk->prev;
				index = chunk_size - 1;
				return true;
			} else {
				--index;
				return false;
			}
		}

		iterator operator ++ ()
		{
			iterator ret = *this;
			inc();
			return ret;
		}

		void operator -- ()
		{
			iterator ret = *this;
			dec();
			return ret;
		}

		bool operator == (iterator it)
		{
			return (chk == it.chk) && (index == it.index);
		}

		bool operator != (iterator it)
		{
			return (chk != it.chk) || (index != it.index);
		}

		data_type operator * ()
		{
			return chk->data[index];
		}

		bool end_of_chunk()
		{
			return index >= chunk_size - 1;
		}
	};

	chunk_list()
	{
		first_chunk_pos = 0;
		last_chunk_pos = 0;
		size_base = 0;
		first_chunk = last_chunk = new chunk;
		last_chunk->prev = last_chunk->next = NULL;
	}

	~chunk_list() 
	{
		// free all blocks allocated
		while (!empty()) {
			chunk* ch = first_chunk->next;
			delete first_chunk;
			first_chunk = ch;
		}
	}

	iterator alloc_front()
	{
		if (first_chunk->prev == NULL) {
			chunk* n = new chunk;
			first_chunk->prev = n;
			n->next = first_chunk;
			first_chunk = n;
			n->prev = NULL;
		} else {
			first_chunk = first_chunk->prev;
		}
		
		size_base += chunk_size;
		first_chunk_pos = chunk_size - 1;

		return iterator(first_chunk, chunk_size - 1);
	}

	iterator alloc_back()
	{
		if (last_chunk->next == NULL) {
			chunk* n = new chunk;
			n->prev = last_chunk;
			n->next = NULL;
			last_chunk->next = n;
			last_chunk = n;
		} else {
			last_chunk = last_chunk->next;
		}
			
		size_base += chunk_size;
		last_chunk_pos = 0;

		return iterator(last_chunk, 0);
	}

	bool first_chunk_full()
	{
		return first_chunk_pos >= chunk_size - 1; 
	}

	bool last_chunk_full()
	{
		return last_chunk_pos >= chunk_size - 1;
	}

	iterator begin()
	{
		return iterator(first_chunk, first_chunk_pos);
	}

	iterator rbegin()
	{
		return iterator(last_chunk, last_chunk_pos);
	}

	iterator end()
	{
		if (last_chunk_pos >= chunk_size - 1) {
			return iterator(NULL, 0);
		} else {
			return iterator(last_chunk, last_chunk_pos + 1);
		}
	}

	iterator rend()
	{
		if (first_chunk_pos <= 0) {
			return iterator(NULL, chunk_size - 1);
		} else {
			return iterator(first_chunk, first_chunk_pos);
		}
	}

	inline data_type front()
	{
		return first_chunk->data[first_chunk_pos];
	}

	inline data_type back()
	{
		return last_chunk->data[last_chunk_pos];
	}

	inline void push_front(data_type& data)
	{
		if (first_chunk_pos <= 0) {
			alloc_front();
		} else {
			--first_chunk_pos;
		}

		first_chunk->data[first_chunk_pos] = data;
	}

	inline void push_back(data_type& data)
	{
		if (last_chunk_pos >= chunk_size - 1) {
			alloc_back();
		} else {
			++last_chunk_pos;
		}

		last_chunk->data[last_chunk_pos] = data;
	}

	inline void pop_front()
	{
		if (first_chunk_pos >= chunk_size - 1) {
			chunk* ch = first_chunk->next;
			delete first_chunk;
			
			if ((first_chunk = ch) == NULL) {
				last_chunk = NULL;
			}

			first_chunk_pos = 0;
			size_base -= chunk_size;
		} else {
			++first_chunk_pos;
		}
	}

	inline void pop_back()
	{
		if (last_chunk_pos <= 0) {
			chunk* ch = last_chunk->prev;
			delete last_chunk;
			
			if ((last_chunk = ch) == NULL) {
				first_chunk = NULL;
			}

			last_chunk_pos = chunk_size - 1;
			size_base -= chunk_size;
		} else {
			--last_chunk_pos;
		}
	}

	// pop front no delete
	inline data_type pop_front_no_del()
	{
		data_type data = first_chunk->data[first_chunk_pos];

		if (first_chunk_pos >= chunk_size - 1) {
			chunk* ch = first_chunk->next;
			
			if ((first_chunk = ch) == NULL) {
				last_chunk = NULL;
			}

			first_chunk_pos = 0;
			size_base -= chunk_size;
		} else {
			++first_chunk_pos;
		}

		return data;
	}

	inline void pop_back_no_del()
	{
		if (last_chunk_pos <= 0) {
			if ((last_chunk = last_chunk->prev) == NULL) {
				first_chunk = NULL;
			}
			last_chunk_pos = chunk_size;
			size_base -= chunk_size;
		}
		
		--last_chunk_pos;
	}

	inline int size()
	{
		return size_base - first_chunk_pos + last_chunk_pos;
	}

	inline bool empty()
	{
		return size_base <= first_chunk_pos - last_chunk_pos;
	}
};


template <class data_type, unsigned int chunk_size = max(1, 512 / sizeof(data_type))>
class my_vector {

	class chunk {
	public:
		data_type data[chunk_size];
	};

	std::vector<chunk*> map;
	chunk* last_chunk;
	unsigned int last_chunk_pos;

public:

	class iterator {
		typename std::vector<chunk*>::iterator chk;
		unsigned int index;

	public:

		iterator () {}

		iterator (typename std::vector<chunk*>::iterator p_chunk, unsigned int p_index)
		{
			chk = p_chunk;
			index = p_index;
		}

		void operator ++ ()
		{
			if (index >= chunk_size - 1) {
				index = -1;
				chk++;
			}

			index++;
		}

		data_type operator * ()
		{
			return (*chk)->data[index];
		}

		bool operator == (iterator i)
		{
			return (i.chk == chk) && (i.index == index);
		}

		bool operator != (iterator i)
		{
			return (i.chk != chk) || (i.index != index);
		}
	};

	my_vector ()
	{
		last_chunk_pos = chunk_size - 1;
	}

	~my_vector ()
	{
		// release all data chunks
		for (std::vector<chunk*>::iterator i = map.begin(); i != map.end(); ++i) {
			delete *i;
		}
	}

	iterator begin()
	{
		return iterator(map.begin(), 0);
	}

	iterator end()
	{
		if (last_chunk_pos >= chunk_size - 1) {
			return iterator(map.end(), 0);
		} else {
			return iterator(map.end() - 1, last_chunk_pos + 1);
		}
	}

	iterator rbegin()
	{
		return iterator(map.end() - 1, last_chunk_pos);
	}

	iterator rend()
	{
		return iterator(map.begin - 1, 0);
	}

	data_type front()
	{
		return *begin();
	}

	data_type back()
	{
		return *rbegin();
	}

	void push_back(data_type& p_data)
	{
		if (last_chunk_pos >= chunk_size - 1) {
			last_chunk = new chunk;
			map.push_back(last_chunk);
			last_chunk_pos = 0;
		} else {
			++last_chunk_pos;
		}

		last_chunk->data[last_chunk_pos] = p_data;
	}

	void pop_back()
	{
		if (last_chunk_pos <= 0) {
			map.pop_back();
			last_chunk = map.back();
			last_chunk_pos = chunk_size;
		} 
		
		--last_chunk_pos;
	}

	unsigned int size()
	{
		return map.size() * chunk_size + last_chunk_pos - (chunk_size - 1);
	}
};


template <class data_type, int chunk_size = max(1, 512 / sizeof(data_type))>
class my_vector_b {

	class chunk {
	public:
		data_type data[chunk_size];
	};

public:

	class iterator {
		std::vector<chunk*>* map;
	public:
		unsigned int index;

		iterator () {}

		iterator (std::vector<chunk*>* p_map, int p_index)
		{
			map = p_map;
			index = (unsigned int)p_index;
		}

		iterator (std::vector<chunk*>* p_map, unsigned int p_index)
		{
			map = p_map;
			index = p_index;
		}

		void operator ++ ()
		{
			index++;
		}

		void operator -- ()
		{
			index--;
		}

		iterator operator + (unsigned int i)
		{
			return iterator(map, index + i);
		}

		iterator operator - (unsigned int i)
		{
			return iterator(map, index - i);
		}

		data_type operator * ()
		{
			unsigned int c = index / chunk_size;
			unsigned int i = index % chunk_size;
			return (*map)[c]->data[i];
		}

		bool operator == (iterator i)
		{
			return i.index == index;
		}

		bool operator != (iterator i)
		{
			return i.index != index;
		}
	};

private:

	std::vector<chunk*> map;
	chunk* last_chunk;
	iterator s;

public:

	my_vector_b ()
	{
		s = iterator(&map, 0);

		last_chunk = new chunk;
		map.push_back(last_chunk);
	}

	~my_vector_b ()
	{
		for (std::vector<chunk*>::iterator i = map.begin(); i != map.end(); ++i) {
			delete *i;
		}
	}

	iterator begin()
	{
		return iterator(&map, 0);
	}

	iterator end()
	{
		return s;
	}

	iterator rbegin()
	{
		return s - 1;
	}

	iterator rend()
	{
		return begin() - 1;
	}

	data_type front()
	{
		return *begin();
	}

	data_type back()
	{
		return *rbegin();
	}

/*
	this push_back:

	push	ebp
	mov	ebp, esp
	push	esi
	mov	esi, ecx
	push	edi
	mov	ecx, DWORD PTR [esi+20]
	mov	edi, ecx
	and	edi, 31					; 0000001fH
	jne	SHORT $LN1@
		mov	edx, DWORD PTR [esi]
		mov	eax, DWORD PTR [esi+4]
		sub	eax, edx
		shr	ecx, 5
		sar	eax, 2
		cmp	eax, ecx
		ja	SHORT $LN2@
			push	512					; 00000200H
			call	DWORD PTR __imp_??2@YAPAXI@Z
			lea	ecx, DWORD PTR [esi+12]
			add	esp, 4
			mov	DWORD PTR [ecx], eax
			push	ecx
			mov	ecx, esi
			call	?push_back@?$vector@PAVchunk@?$my_vector_b@Va@?6?wWinMain@16@$0CA@@@V?$allocator@PAVchunk@?$my_vector_b@Va@?6?wWinMain@16@$0CA@@@@std@@@std@@QAEXABQAVchunk@?$my_vector_b@Va@?6?wWinMain@16@$0CA@@@@Z ; ?push_back@?$vector@PAVchunk@?$my_vector_b@Va@?6?wWinMain@16@$0CA@@@V?$allocator@PAVchunk@?$my_vector_b@Va@?6?wWinMain@16@$0CA@@@@std@@@std@@QAEXABQAVchunk@?$my_vector_b@Va@?6?wWinMain@16@$0CA@@@@Z
		jmp	SHORT $LN1@
			$LN2@:
			mov	eax, DWORD PTR [edx+ecx*4]
			mov	DWORD PTR [esi+12], eax
	$LN1@:
	movq	xmm0, QWORD PTR _p_data$[ebp]
	shl	edi, 4
	add	edi, DWORD PTR [esi+12]
	movq	QWORD PTR [edi], xmm0
	movq	xmm0, QWORD PTR _p_data$[ebp+8]
	movq	QWORD PTR [edi+8], xmm0
	inc	DWORD PTR [esi+20]
	pop	edi
	pop	esi
	pop	ebp
	ret	16					; 00000010H

	std:my_deque push_back

	push	ebp
	mov	ebp, esp
	push	esi
	mov	esi, ecx
	push	edi
	
	mov	ecx, DWORD PTR [esi+16]
	mov	eax, DWORD PTR [esi+12]
	add	eax, ecx
	test	eax, 496				; 000001f0H
	jne	SHORT $LN2@
		lea	eax, DWORD PTR [ecx+32]
		shr	eax, 13					; 0000000dH
		cmp	DWORD PTR [esi+8], eax
		ja	SHORT $LN2@
			push	ecx
			mov	ecx, esi
		call	?_Growmap@?$deque@Va@?6?wWinMain@16@V?$allocator@Va@?6?wWinMain@16@@std@@@std@@IAEXI@Z ; ?_Growmap@?$deque@Va@?6?wWinMain@16@V?$allocator@Va@?6?wWinMain@16@@std@@@std@@IAEXI@Z
	$LN2@:
	mov	ecx, DWORD PTR [esi+8]
	mov	edi, DWORD PTR [esi+16]
	mov	eax, ecx
	shl	eax, 9
	shr	eax, 4
	dec	eax
	and	DWORD PTR [esi+12], eax
	add	edi, DWORD PTR [esi+12]
; Line 783
	dec	ecx
	mov	eax, edi
	shr	eax, 13					; 0000000dH
	and	ecx, eax
; Line 1150
	mov	eax, DWORD PTR [esi+4]
	push	ebx
	lea	ebx, DWORD PTR [ecx*4]
	cmp	DWORD PTR [ebx+eax], 0
	jne	SHORT $LN1@
		push	512					; 00000200H
		call	DWORD PTR __imp_??2@YAPAXI@Z
		mov	ecx, eax
		add	esp, 4
		test	ecx, ecx
		jne	SHORT $LN16@
			call	DWORD PTR __imp_?_Xbad_alloc@std@@YAXXZ
			$LN46@:
		$LN16@:
		mov	eax, DWORD PTR [esi+4]
		mov	DWORD PTR [ebx+eax], ecx
	$LN1@:
	mov	eax, DWORD PTR [esi+4]
	shr	edi, 4
	and	edi, 31					; 0000001fH
	shl	edi, 4
	add	edi, DWORD PTR [ebx+eax]
	pop	ebx
	je	SHORT $LN38@

	    theirs:
		mov	eax, DWORD PTR __Val$[ebp]
		movq	xmm0, QWORD PTR [eax]
		movq	QWORD PTR [edi], xmm0
		movq	xmm0, QWORD PTR [eax+8]
		movq	QWORD PTR [edi+8], xmm0

		mine:
		movq	xmm0, QWORD PTR _p_data$[ebp]
		shl	edi, 4
		add	edi, DWORD PTR [esi+12]
		movq	QWORD PTR [edi], xmm0
		movq	xmm0, QWORD PTR _p_data$[ebp+8]
		movq	QWORD PTR [edi+8], xmm0


	$LN38@:
	inc	DWORD PTR [esi+16]
	pop	edi
	pop	esi
	pop	ebp
	ret	4

*/

void push_back(data_type& p_data)
	{
		unsigned int last_chunk_pos = s.index % chunk_size;

		if (last_chunk_pos == 0) {
			unsigned int c = s.index / chunk_size;

			if (map.size() <= c) {
				last_chunk = new chunk; // 625
				map.push_back(last_chunk); // 100
			} else {
				last_chunk = map[c]; // 0
			}
		}

		last_chunk->data[last_chunk_pos] = p_data; // 1000
		++s;
	}

	inline void pop_back()
	{
		last_chunk = map[s.index / chunk_size];
		--s;
	}

	unsigned int size()
	{
		return s.index;
	}
};


template <class data_type, int chunk_size = max(1, 512 / sizeof(data_type))>
class my_deque {

	class chunk {
	public:
		data_type data[chunk_size];
	};

	std::vector<chunk*> first_index, last_index;
	chunk* first_chunk, last_chunk;
	unsigned int first_chunk_pos, last_chunk_pos;

public:

	my_deque ()
	{
		first_chunk_pos = 0;
		last_chunk_pos = chunk_size - 1;
	}

	~my_deque ()
	{
		// release all data chunks

		for (std::vector<chunk*>::iterator i = first_index.begin(); i != first_index.end(); ++i) {
			delete *i;
		}

		for (std::vector<chunk*>::iterator i = last_index.begin(); i != last_index.end(); ++i) {
			delete *i;
		}
	}

	void push_back(data_type& p_data)
	{
		if (last_chunk_pos >= chunk_size - 1) {
			last_chunk = new chunk;
			last_index.push_back(last_chunk);
			last_chunk_pos = 0;
		} else {
			++last_chunk_pos;
		}

		last_chunk->data[last_chunk_pos] = p_data;
	}

	void pop_back()
	{
		if (last_chunk_pos <= 0) {
			last_index.pop_back();
			last_chunk = index.back();
			last_chunk_pos = chunk_size - 1;
		} else {
			--last_chunk_pos;
		}
	}
};


template <class data_type>
class static_array {
public:
	data_type* data;
	int max;
	int first;
	int last;

	typedef data_type* iterator;

	static_array()
	{
		data = NULL;
		max = first = last = 0;
	}

	static_array(int p_max)
	{
		reserve(p_max);
	}

	template <class cl_data_type, int cl_chunk_size>
	static_array(chunk_list<cl_data_type, cl_chunk_size>& p_chunk_list, bool p_reverse = false)
	{
		if (p_reverse) {
		} else {
			for (chunk_list<cl_data_type, cl_chunk_size>::chunk* chk = p_chunk_list.begin()->next; chk != p_chunk_list.end().chk; chk = chk->next) {
				for (int i = 0; i < cl_chunk_size; i++) {
					push_back(chk->data[i]);
				}
			}
		}
	}

	template <class cl_data_type, int cl_chunk_size>
	static_array(chunk_list_forward<cl_data_type, cl_chunk_size>& p_chunk_list, bool p_reverse = false)
	{
	}

	template <class cl_data_type, int cl_chunk_size>
	static_array(std::vector<cl_data_type>& p_vect, bool p_reverse = false)
	{
	}

	~static_array()
	{
		if (data != NULL) {
			delete data;
		}
	}

	void reserve(unsigned int n)
	{
		data = new data_type[n];
		max = n;
		first = n;
		last = 0;
	}

	void clear()
	{
		delete data;
	}

	bool push_front(data_type p_data)
	{
		--first;
		data[first] = p_data;

		return first > 0;
	}

	bool push_back(data_type p_data)
	{
		data[last] = p_data;
		++last;

		return last < max;
	}

	data_type pop_front()
	{
		return data[first++];
	}

	data_type pop_back()
	{
		return data[last--];
	}

	data_type operator [] (unsigned int n)
	{
		return data[n];
	}

	data_type* operator & ()
	{
		return &data[first];
	}

	data_type* begin()
	{
		return &data[first];
	}

	data_type* end()
	{
		return &data[last];
	}

	int size()
	{
		return last - first + 1;
	}
	
	int capacity()
	{
		return max;
	}

	bool full_end()
	{
		return last == max;
	}

	bool full_start()
	{
		return first == 0;
	}

	bool empty()
	{
		return first == last;
	}
};


template <class obj_type>
inline void obj_copy2(obj_type& source, obj_type& dest)
{
	(double&)dest = (double&)source;
}

const int max_unwrap = 32;
typedef double qw_type;
typedef unsigned int dw_type;
typedef unsigned short sw_type;
typedef unsigned char sb_type;
	
__declspec( align( 16 ) ) class mov_type {
public:
	qw_type qw[max_unwrap / sizeof(qw_type)]; // 8 bytes
	dw_type dw[max_unwrap / sizeof(dw_type)]; // 4 bytes
	sw_type sw[max_unwrap / sizeof(sw_type)]; // 2 bytes
	sb_type sb[max_unwrap / sizeof(sb_type)]; // 1 byte
};

template <class obj_type>
inline void obj_copy(obj_type& source, obj_type& dest, unsigned int pos, unsigned int len)
{
	if (len >= sizeof(qw_type)) {
		((mov_type&)dest).qw[pos / len] = ((mov_type&)source).qw[pos / len];
		pos += sizeof(qw_type);
		len -= sizeof(qw_type);
	}

	if (len >= sizeof(qw_type)) {
		((mov_type&)dest).qw[pos / len] = ((mov_type&)source).qw[pos / len];
		pos += sizeof(qw_type);
		len -= sizeof(qw_type);
	}

	if (len == sizeof(dw_type)) {
		((mov_type&)dest).dw[pos / len] = ((mov_type&)source).dw[pos / len];
		pos += len;
	}
	if (len == sizeof(sw_type)) {
		((mov_type&)dest).sw[pos / len] = ((mov_type&)source).sw[pos / len];
		pos += len;
	}
	if (len == sizeof(sb_type)) {
		((mov_type&)dest).sb[pos / len] = ((mov_type&)source).sb[pos / len];
		pos += len;
	}
}

template <class obj_type>
inline void obj_copy(obj_type& source, obj_type& dest)
{
	obj_copy(source, dest, 0, sizeof(obj_type));	
};


template <class data_type, unsigned int page_size = (1024 * 64) / sizeof(data_type)>
class mem_pool {
	typedef chunk_list<data_type, page_size> pool_type;
	typedef chunk_list<data_type*, 64> avail_type;

	pool_type pages;
	avail_type avail;
	typename pool_type::iterator next;

	mem_pool()
	{
		next = pages.begin();
	}

	data_type* alloc()
	{
		if (next.end_of_chunk()) { // chunk is full
			if (avail.empty()) { // chunk is full and avail (deleted) is empty
				// allocate new chunk
				next = pages.alloc_back();
				return *next;
			} else { // chunk is full and avail (deleted) has at least one slot
				// return deleted slot
				data_type* n = avail.back();
				avail.pop_back();
				return n;
			}
		} else { // not at end of chunk
			// return next slot and inc
			data_type* n = next.ptr();
			next++;
			return n;
		}
	 }

	void free(data_type* p_data)
	{
		avail.push_back(p_data);
	}
};


#endif // !INC_MY_STL