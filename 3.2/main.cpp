#include <algorithm>
#include <functional>
#include <array>
#include <cmath>
#include <glog/logging.h>
using std::abs;
using std::array;
using std::bind;
using std::minus;
using std::multiplies;
using std::placeholders::_1;
using std::placeholders::_2;

struct MyFunctionClass {
	// f(x, y) = x^2+2xy+2y^2+4y+10
	typedef array<float, 2> FVec2;
	typedef FVec2 VectorType;

	FVec2 operator()(const FVec2 &v)
	{
		float value;
		FVec2 gradient;
		const float x = v[0];
		const float y = v[1];

    gradient = {2*x + 2*y, 2*x + 4*y + 4};
		return gradient;
	}
};

float calc(float& a, float& b, float& c){
  return a-b*c;
}
    

template <class FunctionType>
class OptimizationDriver {
	typedef typename FunctionType::VectorType VectorType;
	typedef typename FunctionType::VectorType::value_type ValueType;

	VectorType v_;
	FunctionType f_;

public:
	ValueType mu_;
	OptimizationDriver(ValueType mu=1): mu_(mu){}

	void Init(const VectorType &v0)
	{
		v_ = v0;
	}

	void RunIterate()
	{
		auto gradient = f_(v_);
    std::transform (v_.begin(), v_.end(), gradient.begin(), v_.begin(), bind(calc, _1, _2, mu_));
    // TODO: v -= ug
		// use std::transform, bind, minus, _1, _2, multiplies
		// You can do it in one function call!
	}

	VectorType get_v()
	{
		return v_;
	}
};

int main(int argc, char const* argv[])
{
	google::InitGoogleLogging(argv[0]);
	FLAGS_logtostderr = true;

	OptimizationDriver<MyFunctionClass> d;
	d.mu_ = 0.1f;

	d.Init({0.0f, 0.0f});
	auto v = d.get_v();
	LOG(INFO) << "After initialization x, y is " << v[0] << ", " << v[1];
	for (int i = 0; i < 100; i++) {
		d.RunIterate();
		auto v = d.get_v();
		LOG(INFO) << "After iteration " << i << ", x, y is " << v[0] << ", " << v[1];
	}

	return 0;
}
