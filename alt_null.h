#ifndef ALT_NULL
#define ALT_NULL

#define ALT_NULL_SLOTS 16
#define ALT_NULL_SEED_INC 0xcf1a87b3

#define ALT_NULL_NOT_INIT 0
#define ALT_NULL_ERROR 1
#define ALT_NULL_NA 2

typedef int alt_null_slot_type;

template <class gen1 = alt_null_slot_type>
class alt_null {
	gen1* dummy; // the dummy values that are pointed to
	gen1* cannon; // a backup of the dummy values so that 
	int slots;
	static int seed;
public:
	//template <class gen2>
	gen1* get_null(int n = 0)
	{
		return dummy + n;
	}

	alt_null(int p_slots = ALT_NULL_SLOTS)
	{
		slots = p_slots;

		dummy = new gen1[p_slots];
		cannon = new gen1[p_slots];

		BYTE* b_dummy = (BYTE*)dummy;
		BYTE* b_cannon = (BYTE*)cannon;

		srand(seed);

		for (unsigned int i = 0; i < p_slots * sizeof(gen1); i++) {
			int r = rand();
			b_dummy[i] = b_cannon[i] = r % 256;
		}

		seed += ALT_NULL_SEED_INC;
	}

	~alt_null()
	{
		delete [] dummy;
		delete [] cannon;
	}

	inline bool is_null(gen1* p_ptr)
	{
		return (p_ptr >= dummy) && (p_ptr <= dummy + slots - 1);
	}
};
template <class gen1>
int alt_null<gen1>::seed = ALT_NULL_SEED_INC;

alt_null<int> _alt_null;

#endif //!ALT_NULL