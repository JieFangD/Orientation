#include <vector>
#include <random>
#include <cstdio>
#include <chrono>
#include <glog/logging.h>
using std::vector;
using std::minstd_rand;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;

struct Finder {
  int FindPeak(const vector<int> &values){
    int tot = 0, left = 0, right = values.size()-1, num;
    while(right-left >= 2){
      num = (left + right)/2;
      if(values[num-1]<values[num] && values[num]<values[num+1])
        left = num;
      else if(values[num-1]>values[num] && values[num]>values[num+1])
        right = num; 
      else
        return num;
    }
    num = right - left;
    if(num == 0)
      return 0;
    else if(num == 1){
      if(values[left]<values[right])
        return right;
      else 
        return left;
    }
  }
};

int main(int argc, char const* argv[])
{
	google::InitGoogleLogging(argv[0]);
	FLAGS_logtostderr = true;

	minstd_rand random_generator(0x1234567);
	int test_sizes[] = {1, 2, 3, 4, 5, 100, 20000, 1000000, 10000000};
	constexpr int num_test_size = sizeof(test_sizes)/sizeof(decltype(*test_sizes));
	constexpr int test_times = 20;
	constexpr int test_iteration_count = 1000;
	vector<vector<int>> testcases;
	vector<int> answers;

	for (int i = 0; i < num_test_size; ++i) {
		int test_size = test_sizes[i];
		LOG(INFO) << "Generate: test_size = " << test_size;
		for (int j = 0; j < test_times; ++j) {
			int answer = random_generator() % test_size;
			LOG(INFO) << "    answer = "<< answer;
			testcases.emplace_back(test_sizes[i]);
			answers.push_back(answer);

			vector<int> &testcase = testcases.back();
			testcase[0] = random_generator() % 16;
			for (int k = 1; k <= answer; ++k) {
				testcase[k] = testcase[k-1] + random_generator()%16 + 1;
			}
			for (int k = answer+1; k < test_size; ++k) {
				testcase[k] = testcase[k-1] - random_generator()%16 - 1;
			}
		}
	}

	LOG(INFO) << "==================== start testing ====================";
	auto tic = high_resolution_clock::now();

	Finder f;
	for (int i = 0; i < testcases.size(); ++i) {
		for (int j = 0; j < test_iteration_count; ++j) {
			int your_answer = f.FindPeak(testcases[i]);
			int golden_answer = answers[i];
			CHECK_EQ(your_answer, golden_answer) << "Wrong answer at testcase " << i;
		}
	}

	auto toc = high_resolution_clock::now();
	LOG(INFO) << "==================== passed ====================";
	LOG(INFO) << duration_cast<microseconds>(toc-tic).count() << " us used";

	return 0;
}
