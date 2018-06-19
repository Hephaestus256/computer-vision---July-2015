#ifndef INC_MATH_3D
#define INC_MATH_3D

template <class gen = float>
class vect_3d {
public:

	gen x, y, z;

	vect_3d ()
	{
	}

	vect_3d (gen dim)
	{
		x = y = z = dim;
	}

	vect_3d (gen p_x, gen p_y, gen p_z = gen(0))
	{
		x = p_x;
		y = p_y;
		z = p_z;
	}
};

// 3d cube square with xyz axes
template <class gen = vect_3d<>>
class cube_3d_sqr {
public:

	gen min_value, max_value;

	cube_3d_sqr ()
	{
	}

	cube_3d_sqr (gen p_min, gen p_max)
	{
		min_value = p_min;
		max_value = p_max;
	}
};

#endif // INC_MATH_3D