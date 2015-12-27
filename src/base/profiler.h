/**
 * Manager for performance profiling.
 */
#ifndef SOLEMNSKY_BASE_PROFILE_H
#define SOLEMNSKY_BASE_PROFILE_H

#include <vector>
#include <string>
#include <tuple>
#include <functional>

template<typename T>
class RollingSampler {
private:
  const int size;

public:
  RollingSampler(int size) : size(size) { }

  std::vector<T> data{};

  void push(T value) {
    if (data.size() == size) data.erase(data.begin());
    data.push_back(value);
  }
};

template<typename T>
struct SamplerValue {
  const std::pair<T, T> minmax;
  const T max;
  const T min;
  const T mean;

  /**
   * Undefined for null sampler data.
   */
  SamplerValue(RollingSampler<T>);

  std::string printWith(std::function<std::string(T)> show);
};

/**
 * This assumes we're using a gameloop consisting of multiple logic steps
 * intercalated with renders, organised into regularly scheduled cycles.
 * All times are represented as float-precision seconds.
 */
class Profiler {
public:
  const int size;
  Profiler(int size);

  RollingSampler<float> cycleTime;
  RollingSampler<float> logicTime;
  RollingSampler<float> renderTime;
  RollingSampler<int> primCount;

  std::string print();
};

#endif //SOLEMNSKY_BASE_PROFILE_H
