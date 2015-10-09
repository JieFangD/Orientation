#include <cstdint>
#include <glog/logging.h>
#include <cmath>
using std::sqrt;

class RsqrtSolver {
	float a_half_;
	float x_;
public:
	inline float Init(const float a)
  {
    a_half_ = a / 2;
    if(a > 1)
      x_ = 2 / a;
    else
      x_ = 0.5 / a;
  }
	inline float get_x() {return x_;}

	inline void RunIterate()
	{
    x_ = x_ * (1.5 - x_*x_*a_half_);
	}
};

inline float eval_error(const float a, const float b)
{
	return (a*a+b*b)/(a*b)-2.0f;
}

int main(int argc, char const* argv[])
{
	google::InitGoogleLogging(argv[0]);
	FLAGS_logtostderr = true;
	RsqrtSolver s;

	for (float f = 0.1f; f < 10.0f; f *= 1.7f) {
		const float gold = 1.0 / sqrt(f);
		s.Init(f);
		LOG(INFO) << "Calculating number " << f;
		LOG(INFO) << "    After initialization, error is " << eval_error(s.get_x(), gold);
		for (int i = 0; i < 3; i++) {
			s.RunIterate();
			LOG(INFO) << "    After " << i << " iterations, error is " << eval_error(s.get_x(), gold);
		}
	}
	return 0;
}
