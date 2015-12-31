#include <cmath>
#include "profiler.h"
#include "util.h"

/****
 * SamplerValue
 */

template<typename T>
static const T getMean(std::vector<T> data) {
  T sum = 0;
  for (T i : data) sum += i;
  return (sum / (T) data.size());
}

template<typename T>
static const std::pair<T, T> getMinmax(std::vector<T> data) {
  std::pair<T, T> pair(data[0], data[0]);
  for (T i : data) {
    pair.first = std::min(i, pair.first);
    pair.second = std::max(i, pair.second);
  }
  return pair;
}

template<typename T>
SamplerValue<T>::SamplerValue(RollingSampler<T> sampler) :
    minmax(getMinmax(sampler.data)),
    min{minmax.first},
    max{minmax.second},
    mean{getMean(sampler.data)} { }

template<typename T>
std::string SamplerValue<T>::printWith(std::function<std::string(T)> show) {
  return show(min) + "->" + show(max) + "~~" + show(mean);
}

/****
 * Profiler
 */

Profiler::Profiler(int size) :
    cycleTime{size}, logicTime{size}, renderTime{size}, primCount{size},
    size{size} {
  cycleTime.push(0), cycleTime.push(0), renderTime.push(0), primCount.push(0);
  // for safety
}

static const std::string showTime(float fl) {
  return show((int) std::round(fl * 1000)) + "ms";
}

static const std::string showFps(float fl) {
  return show((int) std::round(fl)) + "fps";
}

static const std::string showInt(int i) {
  return show(i);
}

std::string Profiler::print() {
  SamplerValue<float> cycleValue{cycleTime}, logicValue{logicTime}, renderValue{
      renderTime};
  SamplerValue<int> primValue{primCount};
  return
      "cycle period: " + cycleValue.printWith(showTime) +
      " ; cycle frequency: " + showFps(1 / cycleValue.mean) +
      "\n" +
      "logic delta: " + logicValue.printWith(showTime) +
      " ; render delta " + renderValue.printWith(showTime) +
      "\n" +
      "prim count: " + primValue.printWith(showInt);
}

