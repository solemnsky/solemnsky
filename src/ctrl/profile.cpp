#include "profile.h"
#include <numeric>
#include <algorithm>


/**********************************************************************************************************************/
/* RollingSampler
/**********************************************************************************************************************/

template<typename T>
RollingSampler<T>::RollingSampler(int size) : size(size) { }

template<typename T>
void RollingSampler<T>::clear() {
    data.clear();
}

template<typename T>
void RollingSampler<T>::push(T value) {
    if (data.size() == size) data.erase(data.end());
    data.push_back(value);
}

/**********************************************************************************************************************/
/* SamplerValue
/**********************************************************************************************************************/

template<typename T>
static const T mean(std::vector<T> data) {
    float sum = std::accumulate(data.begin(), data.end(), 0, [](T x, T y) { return x + y; });
    return (sum / data.size());
}

template<typename T>
static const std::pair<T, T> minmax(std::vector<T> data) {
    std::pair<T, T> pair(data[0], data[0]);
    for (T i : data) {
        pair.first = std::min(i, pair.first);
        pair.second = std::max(i, pair.second);
    }
    return pair;
}

template<typename T>
SamplerValue<T>::SamplerValue(RollingSampler<T> sampler) :
        minmax(minmax(sampler.data)),
        min{minmax.first},
        max{minmax.second},
        mean{mean(sampler.data)} { }

template<typename T>
std::string SamplerValue<T>::printWith(std::function<std::string(T)> show) {
    return show(min) + "->" + show(max) + "~~" + show(mean);
}

/**********************************************************************************************************************/
/* Profiler
/**********************************************************************************************************************/

Profiler::Profiler(int size) : cycleTime{size}, logicTime{size}, renderTime{size}, renderPrims{size} {
    cycleTime.push(0), cycleTime.push(0), renderTime.push(0), renderPrims.push(0);
    // for safety
}


static const std::string showFloat(float fl) {
    return "<float>";
}

static const std::string showInt(float fl) {
    return "<int>";
}

std::string Profiler::print() {
    SamplerValue<float> cycleValue{cycleTime}, logicValue{logicTime}, renderValue{renderTime};
    SamplerValue<int> primCount{renderPrims};
    return
            "cycle period: " + cycleValue.printWith(showFloat) +
            " -> cycle frequency: " + showFloat(1 / cycleValue.mean) +
            "\n" +
            "logic delta: " + logicValue.printWith(showFloat) +
            " ; render delta " + renderValue.printWith(showFloat) +
            "\n" +
            "prim count: " + primCount.printWith(showInt);
}

