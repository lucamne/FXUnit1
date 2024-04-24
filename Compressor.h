#pragma once

#include "Effect.h"

#include "daisysp.h"

#include <cmath>

// const positions of param values in m_param_arry
constexpr size_t THRESHOLD {0};
constexpr size_t RATIO {1};
constexpr size_t MAKEUP {2};

class Compressor : public Effect
{
public:
    Compressor(): 
    Effect{"Compressor",3},
    m_attack{0.98f},
    m_release{0.99f}
    {}

    ~Compressor() {}

    void Init(float sample_rate)
    {
        m_sample_rate = sample_rate;
        m_param_array[0] = Parameter{"Threshold",-30.0f,60.0f,-60.0f};
        m_param_array[1] = Parameter{"Ratio",3.0f,8.0f,0.0f};
        m_param_array[2] = Parameter{"Makeup",0.0f,30.0f,0.0f};
    }

    float Process(float input)
    {
        ///*** Sidechain ***///
        float sidechain_signal {std::abs(input)};//< get signal amplitude
        
        /// peak level detection
        sidechain_signal -= std::pow(10.0f,m_param_array[THRESHOLD].value / 20.0f);//< bias input to level detector by converting threshold to linear domain and subtracting
        static float prev_sig{-1.0f * std::pow(10.0f,m_param_array[THRESHOLD].value / 20.0f)};
        // calculate peak level value
        if (sidechain_signal > prev_sig) {sidechain_signal = m_attack * (prev_sig) + (1.0f - m_attack) * sidechain_signal;}
        else {sidechain_signal = m_release * (prev_sig) + (1.0f - m_release) * sidechain_signal;}
        prev_sig = sidechain_signal;

        sidechain_signal += std::pow(10.0f,m_param_array[THRESHOLD].value / 20.0f);//< reapply threshold removed for biasing

        /// Conversion to decimal
        sidechain_signal = 20.0f * std::log10(sidechain_signal);

        /// Calculate static compression characteristic with hard knee
        float compressed_sig{};
        if (sidechain_signal <= m_param_array[THRESHOLD].value) compressed_sig = sidechain_signal;
        else compressed_sig = m_param_array[THRESHOLD].value + (sidechain_signal - m_param_array[THRESHOLD].value) / m_param_array[RATIO].value;

        /// Gain calculation in db
        float gain = compressed_sig - sidechain_signal + m_param_array[MAKEUP].value;

        /// Linearize
        gain = std::pow(10,gain/20.0f);

        return input * gain;
    }

private:
    float m_attack;//< attack in alpha value
    float m_release;//< release in alpha value
    //float m_threshold;//< threshold in decibels
    //float m_makeup_gain;//< makeup gain in db
    //float m_ratio;//< defined as the reciprocal slope of the line segment above the compression theshold
};