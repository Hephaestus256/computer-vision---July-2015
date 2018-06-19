#ifndef INC_MY_STL_BAK
#define INC_MY_STL_BAK

#define MY_STL_NOT_INIT 0
#define MY_STL_ERROR 1
#define MY_STL_VALID 2
#define MY_STL_AT_END 3

#include "C:\Users\Owner\Documents\Visual Studio 2010\Library\alt_null.h"

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
Primises of this class:
- Has no central object, but are all joined elements
- Optimized to add elements to the beginning or end, but not insert elements.
- Order of all methods should be constant and independent of the number of elements. O(c) not O(n).
*/
template <class gen1>
class appendable_element {
public:
	appendable_element* begin;
	appendable_element* end;
	appendable_element* next;
	//appendable_element* other_end;
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
		end = begin = this;
		// other_end = this;
		next = at_end();
	}

	appendable_element (gen1 p_data)
	{
		data = p_data;
		end = begin = this;
		//other_end = this;
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
			p_tail->end->end = (appendable_element*)_alt_null.get_null(ALT_NULL_NA);
			return true;
		} else { // not round robin
			p_tail->end->begin = begin;
			begin->end = p_tail->end;
			return false;
		}
	}

	inline bool prepend (appendable_element* p_begin)
	{
		p_begin->next = begin;

		if (is_connected(p_begin)) { // joining to itself, round robin
			//p_begin->end = end = (appendable_element*)_alt_null.get_null(ALT_NULL_NA);
			return true;
		} else { // not round robin
			begin = p_begin->begin;
			//p_begin->end = end;
			return false;
		}
	}

	// if is connected or is same list, return true, otherwise false
	inline bool is_connected (appendable_element* p_ae)
	{
		return p_ae->begin == begin;
	}

	//inline bool is_round_robin ()
	//{
	//	return end == (appendable_element*)_alt_null.get_null(ALT_NULL_NA);
	//}

	inline static appendable_element* at_end()
	{
		return (appendable_element*)_alt_null.get_null(ALT_NULL_NOT_INIT);
	}

	inline gen1* data_ptr()
	{
		return &data;
	}
};

#endif // !INC_MY_STL_BAK