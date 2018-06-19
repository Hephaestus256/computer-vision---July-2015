#ifndef INC_MATH_1D
#define INC_MATH_1D

#include <math.h>
#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <limits>

float _fzero = (float)0.0f;
double _dzero = (double)0.0;

#define FNAN ((float)0.0f / _fzero)
#define DNAN ((double)0.0 / _dzero)
#define FINF ((float)1.0f / _fzero)
#define DINF ((double)1.0f / _dzero)
#define INT_UNDEF_VALUE INT_MAX
#define FLOAT_UNDEF_VALUE FNAN
#define DOUBLE_UNDEF_VALUE DNAN

#define FLOAT_PRECISION 0.001f
#define DOUBLE_PRECISION 0.00001

#define PI M_PI
#define PI_2 M_PI * 0.5
#define PI_4 M_PI * 0.25

template <class gen1>
inline void swap(gen1 &a, gen1 &b)
{
	gen1 temp = a;
	a = b;
	b = temp;
}

template <class gen1>
inline bool is_nan(gen1 num)
{
	return num != num;
}

template <class gen1>
inline bool is_inf(gen1 num)
{
	return num == (gen1)DINF;
}

template <class gen1>
inline bool is_undef_value(gen1 num)
{
	return is_nan(num);
}

inline bool is_undef_value(int num)
{
	return num == INT_UNDEF_VALUE;
}

/* for overloading with templates */
inline int floor(int i)
{
	return i;
}

template <class gen>
inline gen frac(gen n)
{
	return n - floor(n);
}

inline double frac(double n)
{
	return n - floor(n);
}

template <class gen1>
inline int round(gen1 n)
{
	if (n > 0) {
		return int(n + 0.5);
	} else {
		return int(n - 0.5);
	}
}

template <class gen1>
inline int trunc(gen1 n)
{
	return int(n);
}

/* for overloading with templates */
inline int round(int n)
{
	return n;
}

inline float precision(float v)
{
	return FLOAT_PRECISION;
}

inline double precision(double v)
{
	return DOUBLE_PRECISION;
}

template <class gen1>
inline bool is_between(gen1 x, gen1 x1, gen1 x2)
{
	return x >= x1 && x <= x2;
}

inline bool approx_equal(int x1, int x2)
{
	return x1 == x2;
}

inline bool approx_equal(float x1, float x2)
{
	return is_between(x1, x2 - FLOAT_PRECISION, x2 + FLOAT_PRECISION);
}

inline bool approx_equal(double x1, double x2)
{
	return is_between(x1, x2 - DOUBLE_PRECISION, x2 + DOUBLE_PRECISION);
}

inline bool approx_zero (int x)
{
	return x == 0;
}

inline bool approx_zero (float x)
{
	return is_between(x, -FLOAT_PRECISION, FLOAT_PRECISION);
}

inline bool approx_zero (double x)
{
	return is_between(x, -DOUBLE_PRECISION, DOUBLE_PRECISION);
}

template <class gen1>
inline gen1 interpolate (double alpha, gen1 x1, gen1 x2)
{
	return gen1((double)x1 + double(x2 - x1) * alpha);
}

/* returns the sign of the value
negative=-1, positive=1, zero=0 */
template <class gen>
inline int sign(gen x)
{
	if (x < (gen)0) {
		return -1;
	} else if (x > (gen)0) {
		return 1;
	} else {
		return 0;
	}
}

/* returns the sign of the distance between values 
negative=-1, positive=1, zero=0 */
template <class gen1, class gen2>
inline int sign(gen1 x1, gen2 x2)
{
	if (x1 < x2) {
		return -1;
	} else if (x1 > x2) {
		return 1;
	} else {
		return 0;
	}
}

template <class value_type>
value_type tolerance(value_type standard, value_type value)
{
	return (value - standard) / standard;
}

template <class value_type, class tolerance_type>
int tolerance_case(value_type standard, value_type value, tolerance_type tolerance)
{
	if (value - standard > tolerance * standard) {
		return 1;
	} else if (value - standard < -tolerance * standard) {
		return -1;
	} else {
		return 0;
	}
}

/* finds the minimum value of values */
template <class min_type = int, class key_type = int>
class min_value {
	
	min_type value;
	key_type key;

public:

	min_value(min_type p_init = 10000, key_type p_key = -1)
	{
		value = p_init;
		key = p_key;
	}

	inline bool update(min_type p_value, key_type p_key)
	{
		if (p_value < value) {
			value = p_value;
			key = p_key;
			return true;
		}

		return false;
	}

	inline min_type report_value()
	{
		return value;
	}

	inline key_type report_key()
	{
		return key;
	}
};

/* finds the maximum value */
template <class max_type = int, class key_type = int>
class max_value {
	
	max_type value;
	key_type key;

public:

	max_value(max_type p_init = -10000, key_type p_key = -1)
	{
		init(p_init, p_key);
	}

	void init(max_type p_init = -10000, key_type p_key = -1)
	{
		value = p_init;
		key = p_key;
	}

	// change value if new max value and return if updated or not
	inline bool update(max_type p_value, key_type p_key)
	{
		if (p_value > value) {
			value = p_value;
			key = p_key;
			return true;
		}

		return false;
	}

	inline bool update(max_type p_value)
	{
		if (p_value > value) {
			value = p_value;
			return true;
		}

		return false;
	}

	inline max_type report_value()
	{
		return value;
	}

	inline key_type report_key()
	{
		return key;
	}
};

/* finds the average value */
template <class mean_type>
class mean {
public:

	mean_type acc;
	double ct;

	inline mean(mean_type p_init = 0, double p_ct = 0.0)
	{
		ct = p_ct;
		acc = p_init;
	}

	inline void update(mean_type p_next)
	{
		ct += 1.0;
		acc += p_next;
	}

	inline void cull(mean_type p_value)
	{
		ct -= 1.0;
		acc -= p_value;
	}

	inline mean_type calc()
	{
		return acc / (double)ct;
	}

	inline mean_type sum()
	{
		return acc;
	}

	inline double count()
	{
		return ct;
	}
};

class stats {
public:
	double min;
	double max;
	double range;

	stats ()
	{
		min = DINF;
		max = -DINF;
	}

	void update(double n)
	{
		if (n < min) {
			min = n;
		} else if (n > max) {
			max = n;
		}
	}

	double get_range()
	{
		return max - min;
	}
};

// square but keep the sign
template <class gen>
inline gen sqrd_ks(gen n)
{
	return n * fabs(n);
}

template <class gen>
bool quadratic(gen a, gen b, gen c, gen& x1, gen& x2)
{
	gen under = b * b - (gen)4 * a * c;

	if (under < (gen)0) {
		// imaginary results
		return false;
	} else {
		// real results
		// (-b +- sqrt(b^2 - 4ac)) / 2a
		gen den = (gen)0.5 / a;
		gen rad = sqrt(under);
		x1 = (rad - b) * den;
		x2 = (-rad - b) * den;

		return true;
	}
}

#endif