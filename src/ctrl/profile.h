/**
 * Manager for performance profiling.
 */
#ifndef SOLEMNSKY_CTRL_PROFILE_H
#define SOLEMNSKY_CTRL_PROFILE_H

#include <vector>
#include <string>
#include <tuple>
#include <functional>

template<typename T>
class RollingSampler {
private:
    const int size;

public:
    RollingSampler(int size);

    std::vector<T> data;

    void clear();
    void push(T value);
};

template<typename T>
struct SamplerValue {
    const std::tuple<T, T> minmax;
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
 * This assumes we're using a gameloop consisting of multiple logic steps intercalated with renders, organised into
 * regularly scheduled cycles.
 * All times are represented as float-precision seconds.
 */
class Profiler {
public:
    Profiler(int size);

    RollingSampler<float> cycleTime;
    RollingSampler<float> logicTime;
    RollingSampler<float> renderTime;
    RollingSampler<int> renderPrims;

    std::string print();
};

#endif //SOLEMNSKY_CTRL_PROFILE_H
