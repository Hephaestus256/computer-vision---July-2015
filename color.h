#ifndef INC_COLOR
#define INC_COLOR

char color_h_report_text[50];

#define RGB_BLACK rgb(0, 0, 0)
#define RGB_WHITE rgb(255, 255, 255)
#define RGB_GREY rgb(127, 127, 127)
#define RGB_RED rgb(255, 0, 0)
#define RGB_GREEN rgb(0, 255, 0)
#define RGB_BLUE rgb(0, 0, 255)
#define RGB_YELLOW rgb(255, 255, 0)
#define RGB_AQUA rgb(0, 255, 255)
#define RGB_PURPLE rgb(255, 0, 255)
#define RGB_ORANGE rgb(255, 165, 0)
#define COLOR_PERCEPT pixel<float>(0.2989, 0.5870, 0.1140)

template <class data_type = int, int red_max = 255, int green_max = 255, int blue_max = 255>
class pixel {
public:
	union {
		struct {
			data_type r, g, b;
		};
		data_type primary[3];
	};

	pixel ()
	{
	}

	template <class gen>
	pixel (gen lightness)
	{
		r = g = b = (data_type)lightness;
	}

	template <class gen>
	pixel (pixel<gen> p)
	{
		r = (data_type)p.r;
		g = (data_type)p.g;
		b = (data_type)p.b;
	}

	template <class gen>
	pixel (gen p_r, gen p_g, gen p_b)
	{
		r = (data_type)p_r;
		g = (data_type)p_g;
		b = (data_type)p_b;
	}

	bool operator == (pixel c)
	{
		return r == c.r && g == c.g && b == c.b;
	}

	bool operator != (pixel c)
	{
		return r != c.r || g != c.g || b != c.b;
	}

	bool operator <= (pixel c)
	{
		return r <= c.r && g <= c.g && b <= c.b;
	}

	bool operator < (pixel c)
	{
		return r < c.r && g < c.g && b < c.b;
	}

	bool operator >= (pixel c)
	{
		return r >= c.r && g >= c.g && b >= c.b;
	}

	bool operator > (pixel c)
	{
		return r > c.r && g > c.g && b > c.b;
	}

	pixel<int> cv_int()
	{
		return pixel<int>(int(r), int(g), int(b));
	}

	pixel<BYTE> cv_byte()
	{
		return pixel<BYTE>(BYTE(r), BYTE(g), BYTE(b));
	}

	pixel<float> cv_float()
	{
		return pixel<float>(float(r), float(g), float(b));
	}

	pixel operator * (pixel<float> p)
	{
		return pixel(data_type(r * p.r), data_type(g * p.g), data_type(b * p.b));
	}

	template <class gen>
	pixel operator * (gen n)
	{
		return pixel(data_type(r * n), data_type(g * n), data_type(b * n));
	}

	pixel reverse()
	{
		return pixel(b, g, r);
	}

	pixel<float> normalize()
	{
		return pixel<float>(
			float(r) * float(1.0 / (double)red_max), 
			float(g) * float(1.0 / (double)green_max), 
			float(b) * float(1.0 / (double)blue_max)
		); 
	}

	pixel denormalize()
	{
		return pixel(
			data_type(r * (float)red_max),
			data_type(g * (float)green_max),
			data_type(b * (float)blue_max)
		);
	}

	template <class gen>
	pixel gamma(gen gamma)
	{
		pixel<float> norm = normalize();
		pixel<float> p(
			(float)pow((float)norm.r, (float)gamma),
			(float)pow((float)norm.g, (float)gamma),
			(float)pow((float)norm.b, (float)gamma)
		);
		
		return pixel(p.denormalize());
	}

	// euclidean distance from color space origin
	float dist_euc()
	{
		return
			sqrt(
				float(r) * float(r) 
				+ float(g) * float(g) 
				+ float(b) * float(b)
			);
	}

	float dist_euc_sqrd()
	{
		return
			float(r) * float(r) 
			+ float(g) * float(g) 
			+ float(b) * float(b)
		;
	}

	// euclidean distance from 
	float dist_euc(pixel c)
	{
		return (cv_int() - c.cv_int()).dist_euc();
	}

	float dist_euc_sqrd(pixel c)
	{
		return (*this - c).dist_euc_sqrd();
	}

	bool euc_dist_thresh(pixel c, float thresh)
	{
		return dist_euc_sqrd(c) <= thresh * thresh;
	}

	// color perception 1: rgb weighted
	pixel color_percept1()
	{
		return pixel(*this * COLOR_PERCEPT);
	}

	// color perception 2: rgb weighted and gamma adjusted
	pixel color_percept2()
	{
		return color_percept1().gamma(1.0 / 2.4);
	}

	pixel operator + (pixel c)
	{
		return pixel(r + c.r, g + c.g, b + c.b);
	}

	pixel operator - (pixel c)
	{
		return pixel(r - c.r, g - c.g, b - c.b);
	}

	pixel operator += (pixel c)
	{
		return pixel(r += c.r, g += c.g, b += c.b);
	}

	pixel operator -= (pixel c)
	{
		return pixel(r -= c.r, g -= c.g, b -= c.b);
	}

	pixel min_value(pixel c)
	{
		return pixel(min(r, c.r), min(g, c.g), min(b, c.b));
	}

	pixel max_value(pixel c)
	{
		return pixel(max(r, c.r), max(g, c.g), max(b, c.b));
	}

	pixel debug_byte()
	{
		if (r < 0 || g < 0 || b < 0) {
			return pixel(RGB_GREY.r, RGB_GREY.g, RGB_GREY.b);
		} else if (r > 255 || g > 255 || b > 255) {
			return pixel(RGB_ORANGE.r, RGB_ORANGE.g, RGB_ORANGE.b);
		} else {
			return *this;
		}
	}

	inline char* report()
	{
		sprintf_s(color_h_report_text, "(%f, %f, %f)", (float)r, (float)g, (float)b);
		return color_h_report_text;
	}

	inline char* report_int()
	{
		sprintf_s(color_h_report_text, "(%i, %i, %i)", (int)r, (int)g, (int)b);
		return color_h_report_text;
	}
};

typedef pixel<BYTE> rgb;

#endif // INC_COLOR

