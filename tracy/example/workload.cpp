#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "tracy/Tracy.hpp"

static const int kFrames = 32;
static const int kSamples = 4096;

static double accumulate(double *scratch, int seed)
{
    ZoneScopedN("accumulate");
    double acc = 0.0;
    for (int i = 0; i < kSamples; ++i)
    {
        scratch[i] = std::sin((double)(seed + i) * 0.0009765625);
        acc += scratch[i] * scratch[i];
    }
    return acc;
}

static double reduce(const double *scratch)
{
    ZoneScopedN("reduce");
    double peak = 0.0;
    for (int i = 0; i < kSamples; ++i)
    {
        const double v = std::fabs(scratch[i]);
        if (v > peak) peak = v;
    }
    return peak;
}

int main(void)
{
    static const char kAppInfo[] = "spn tracy example";
    TracyAppInfo(kAppInfo, sizeof(kAppInfo) - 1);
    TracySetProgramName("workload");

    const size_t bytes = sizeof(double) * (size_t)kSamples;
    double *scratch = (double *)std::malloc(bytes);
    if (!scratch) return 1;
    TracyAlloc(scratch, bytes);

    double total = 0.0;
    double peak = 0.0;
    for (int frame = 0; frame < kFrames; ++frame)
    {
        ZoneScopedN("frame");
        total += accumulate(scratch, frame * kSamples);
        const double p = reduce(scratch);
        if (p > peak) peak = p;

        TracyPlot("energy", total);
        TracyPlot("peak", peak);

        char msg[64];
        const int len = std::snprintf(msg, sizeof(msg), "frame %d energy %.6f", frame, total);
        TracyMessage(msg, (size_t)len);

        FrameMark;
    }

    TracyFree(scratch);
    std::free(scratch);

    std::printf("frames=%d samples=%d energy=%.6f peak=%.6f connected=%d\n",
                kFrames, kSamples, total, peak, TracyIsConnected ? 1 : 0);
    return 0;
}
