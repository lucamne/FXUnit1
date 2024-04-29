#pragma once

#include "Effect.h"

#include <cmath>

class LevelDetector : public Effect
{
public:
    LevelDetector()
    :Effect("Level Detector", 0),
    m_prev_peak_level{-100.0f},
    m_attack{1.0f},
    m_release{300.0f} {}
    ~LevelDetector() {}

    float Process(float in)
    {
        GetLevel(ToDB(std::abs(in)));
        return in;
    }

    float GetPeakLevel() const {return m_prev_peak_level;}

private:
    float m_prev_peak_level{};
    float m_attack{};
    float m_release{};

    constexpr float ToDB(float x) {return 20.0f * std::log10(x);}

    constexpr float MsToAlpha(float x) {return std::exp(-1.0f / (x / 1000.0f * m_sample_rate));}

    void GetLevel(float x)
    {
        if (x < -999.9f) {return;} //> if sample is too small it can be interpreted as negative infinity which will cause level to always read -inf from that point on
        const float y1 {std::max(x,MsToAlpha(m_release) * m_prev_peak_level + (1 - MsToAlpha(m_release)) * x)};
        const float yL {MsToAlpha(m_attack) * m_prev_peak_level + (1 - MsToAlpha(m_attack)) * y1};
        m_prev_peak_level = yL;
    }

};