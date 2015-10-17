#include <numeric>
#include <random>
#include <algorithm>
#include <functional> // ususally used with algorithm
#include <glog/logging.h>

using std::minstd_rand;

using std::binary_search;
using std::reverse;
using std::remove_if;
using std::all_of;
using std::accumulate;
using std::inner_product;
using std::transform;
using std::copy;
using std::swap;

using std::divides;
using std::modulus;
using std::plus;
using std::equal;
using std::equal_to;
using std::greater;
using std::placeholders::_1;
using std::placeholders::_2;

struct IntToFloat {
	void Run(const int *int_array, float *float_array, const int n)
	{
		for (int i = 0; i < n; i++) {
			*float_array = *int_array;
			++int_array;
			++float_array;
		}
	}
	void Run_std(const int *int_array, float *float_array, const int n)
	{
    copy(int_array, int_array+n, float_array);
	}
};

int divi(int x, int y){return (x+y)/2;}

struct CalculateCarry {
	int Run(const int *a, const int *b, const int n)
	{
		int carry = 0;
		for (int i = 0; i < n; i++) {
			carry += *a + *b;
			carry /= 2;
			++a; ++b;
		}
		return carry;
	}
	int Run_std(const int *a, const int *b, const int n)
	{
		// Normally we use lambda if bind is dirty
		// But practice not to use it (yay)
    return inner_product(a, a+n, b, 0, divi, plus<int>());
	}
};

struct Reverse {
	void Run(int *array, const int n)
	{
		int *start = array;
		int *end = array + n - 1;
		while (start < end) {
			swap(*start, *end);
			--end;
			++start;
		}
	}
	void Run_std(int *array, const int n)
	{
    reverse(array, array+n);
	}
};

struct RemoveOdd {
	int Run(int *array, const int n)
	{
		int write_index = 0;
		for (int i = 0; i < n; i++) {
			int value = array[i];
			if (value%2 == 0) {
				array[write_index] = value;
				++write_index;
			}
		}
		return write_index;
	}
  struct is_odd{
    bool operator()(const int& value){return (value%2) == 1;}
  };
	int Run_std(int *array, const int n)
	{
		// Normally we use lambda if bind is dirty
		// But practice not to use it (yay)
	  int *result;
    result = remove_if(array, array+n, is_odd());
    return result - array;
  }
};

struct IsPrimeLessThan100 {
	static constexpr int table_size_ = 25;
	static const int table_[table_size_];

	bool Run(const int x)
	{
		for (int i = 0; i < table_size_; i++) {
			if (table_[i] == x) {
				return true;
			}
		}
		return false;
	}
	bool Run_std(const int x)
	{
    binary_search(table_, table_+table_size_, x);
	}
};

const int IsPrimeLessThan100::table_[IsPrimeLessThan100::table_size_] = {
	2, 3, 5, 7, 11, 13, 17, 19,
	23, 29, 31, 37, 41, 43, 47,
	53, 59, 61, 67, 71, 73, 79,
	83, 89, 97
};


struct VectorAdd {
	void Run(const int *a, const int *b, int *c, const int n)
	{
		for (int i = 0; i < n; i++) {
			*c = *a + *b;
			++a; ++b; ++c;
		}
	}
	void Run_std(const int *a, const int *b, int *c, const int n)
	{
    transform(a, a+n, b, c, plus<int>());
	}
};

struct VectorSum {
	int Run(const int *array, const int n)
	{
		int sum = 0;
		for (int i = 0; i < n; i++) {
			sum += *array;
			++array;
		}
		return sum;
	}
	int Run_std(const int *array, const int n)
	{
    accumulate(array+1, array+n, *array);
	}
};

struct AllNonzero {
	bool Run(const int *array, const int n)
	{
		for (int i = 0; i < n; i++) {
			if (array[i] == 0) {
				return false;
			}
		}
		return true;
	}
	bool Run_std(const int *array, const int n)
	{
    all_of(array, array+n, [](int i){return i!=0;});
	}
};

int main(int argc, char const* argv[])
{
	google::InitGoogleLogging(argv[0]);
	FLAGS_logtostderr = true;

	AllNonzero all_nonzero;
	IntToFloat int_to_float;
	CalculateCarry calculate_carry;
	VectorSum vector_sum;
	VectorAdd vector_add;
	RemoveOdd remove_odd;
	Reverse reverse_array;
	IsPrimeLessThan100 is_prime_lt_100;
	minstd_rand random_generator(0x1234567);

	constexpr int test_size = 100;
	constexpr int test_count = 100;
	int a_1[test_size];
	int a_2[test_size];
	int b_1[test_size];
	int b_2[test_size];
	int c_1[test_size];
	int c_2[test_size];

	LOG(INFO) << "=== Start test AllNonzero ===";
	for (int i = 0; i < test_count; i++) {
		for (int j = 0; j < test_size; j++) {
			a_1[j] = random_generator() % test_size; // prob of all nonzero ~ 1/e
		}
		bool gold = all_nonzero.Run(a_1, test_size);
		bool yours = all_nonzero.Run_std(a_1, test_size);
		CHECK_EQ(gold, yours) << "Error at testcase " << i;
	}

	LOG(INFO) << "=== Start test int_to_float===";
	for (int i = 0; i < test_count; i++) {
		float f_yours[100];
		float f_gold[100];
		for (int j = 0; j < test_size; j++) {
			a_1[j] = random_generator();
		}
		int_to_float.Run(a_1, f_gold, test_size);
		int_to_float.Run_std(a_1, f_yours, test_size);
		LOG_IF(ERROR, !equal(f_gold, f_gold+test_size, f_yours)) << "Error at testcase " << i;
	}

	LOG(INFO) << "=== Start test CalculateCarry ===";
	for (int i = 0; i < test_count; i++) {
		for (int j = 0; j < test_size; j++) {
			a_1[j] = (random_generator() >> 10) & 1;
			b_1[j] = (random_generator() >> 10) & 1;
		}
		int gold = calculate_carry.Run(a_1, b_1, test_size);
		int yours = calculate_carry.Run_std(a_1, b_1, test_size);
		CHECK_EQ(gold, yours) << "Error at testcase " << i;
	}

	LOG(INFO) << "=== Start test VectorSum ===";
	for (int i = 0; i < test_count; i++) {
		for (int j = 0; j < test_size; j++) {
			a_1[j] = random_generator();
		}
		int gold = vector_sum.Run(a_1, test_size);
		int yours = vector_sum.Run_std(a_1, test_size);
		CHECK_EQ(gold, yours) << "Error at testcase " << i;
	}

	LOG(INFO) << "=== Start test VectorAdd ===";
	for (int i = 0; i < test_count; i++) {
		for (int j = 0; j < test_size; j++) {
			a_1[j] = random_generator();
			b_1[j] = random_generator();
		}
		vector_add.Run(a_1, b_1, c_1, test_size);
		vector_add.Run_std(a_1, b_1, c_2, test_size);
		LOG_IF(ERROR, !equal(c_1, c_1+test_size, c_2)) << "Error at testcase " << i;
	}

	LOG(INFO) << "=== Start test RemoveOdd ===";
	for (int i = 0; i < test_count; i++) {
		for (int j = 0; j < test_size; j++) {
			a_1[j] = random_generator();
		}
		copy(a_1, a_1+test_size, a_2);
		int position_gold = remove_odd.Run(a_1, test_size);
		int position_yours = remove_odd.Run_std(a_2, test_size);
		CHECK_EQ(position_gold, position_yours) << "Error index at testcase " << i;
		CHECK_LE(position_yours, test_size) << "Error index at testcase " << i;
		LOG_IF(ERROR, !equal(a_1, a_1+position_gold, a_2)) << "Error result at testcase " << i;
	}

	LOG(INFO) << "=== Start test Reverse ===";
	for (int i = 0; i < test_count; i++) {
		for (int j = 0; j < test_size; j++) {
			a_1[j] = random_generator();
		}
		copy(a_1, a_1+test_size, a_2);
		reverse_array.Run(a_1, test_size);
		reverse_array.Run_std(a_2, test_size);
		LOG_IF(ERROR, !equal(a_1, a_1+test_size, a_2)) << "Error at testcase " << i;
	}

	LOG(INFO) << "=== Start test IsPrimeLessThan100 ===";
	for (int i = 0; i < 1000; i++) {
		bool gold = is_prime_lt_100.Run(i);
		bool yours = is_prime_lt_100.Run_std(i);
		CHECK_EQ(gold, yours) << "Error at testcase " << i;
	}

	return 0;
}
