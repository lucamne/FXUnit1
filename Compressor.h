#pragma once

#include "Effect.h"

#include "daisysp.h"

#include <cmath>

// const positions of param values in m_param_arry

class Compressor : public Effect
{
public:
    Compressor(): 
    Compressor("Compressor",3)
    {}

    Compressor(const std::string& name, int param_count)
    :Effect(name,param_count),
    m_attack{0.99f},
    m_release{0.999f},
    m_knee_width{5.0f},
    m_threshold{-20.0f},
    m_makeup_gain{0.0f},
    m_ratio{3.0f},
    m_limit_mode{false},
    m_dbs_compressed{0.0f} 
    {}


    ~Compressor() {}

    void Init(float sample_rate)
    {
        m_sample_rate = sample_rate;
        m_param_array[0] = Parameter{"Threshold",&m_threshold,60.0f,-60.0f};
        m_param_array[1] = Parameter{"Ratio",&m_ratio,8.0f,0.0f};
        m_param_array[2] = Parameter{"Makeup",&m_makeup_gain,60.0f,0.0f};
    }

    float Process(float input)
    {
        float sidechain{input * 3.6f};//>  current value of side chain, multiply by 3.6to convert float signal to volts

        sidechain = {std::abs(sidechain)};//> get sidechain absolute val

        sidechain = ToDB(sidechain);//> convert to log domain

        sidechain = sidechain - GainComputer(sidechain);//> calculate difference between signal and compressed signal using signal from GetLevel
        m_dbs_compressed = sidechain;

        sidechain = GetLevel(sidechain);//> calculate level and subtract from makeup gain
        sidechain = m_makeup_gain - sidechain;

        sidechain = ToLinear(sidechain);//> convert to linear domain

        return input * sidechain / 3.6f;//> apply gain, convert volts back to float
    }

    //void SetMakeup(float makeup) {m_makeup_gain = makeup;}
    //void SetThreshold(float threshold) {m_threshold = threshold;}
    float GetDbsCompressed() const {return m_dbs_compressed;}//< returns current compression activity of compressor

protected:
    float m_attack{};//< attack in alpha value
    float m_release{};//< release in alpha value
    float m_knee_width{};
    float m_threshold{};//< threshold in decibels
    float m_makeup_gain{};//< makeup gain in db
    float m_ratio{};//< defined as the reciprocal slope of the line segment above the compression theshold

    bool m_limit_mode{}; //< sets ratio to infinite

    float m_dbs_compressed{};//< tracks level of compression for readouts and debugging

    float GainComputer(float x)
    {
        const float T{m_threshold};//> get threshold
        const float W{m_knee_width};//> get knee wisth
        const float R{m_ratio};//> get ratio
        float out{};
        // calculate compressed level based on level relative to threshold and knee width
        if (!m_limit_mode)
        {
            if (2 * (x - T) < -1.0f * W) {out = x;}
            else if (2 * std::abs(x - T) <= W) {out = x + ((1.0f/R)-1.0f) * (x - T + (W/2.0f)) * (x - T + (W/2.0f))/(2.0f*W);}
            else {out = T + (x - T)/R;}
        }
        else
        {
            if (2 * (x - T) < -1.0f * W) {out = x;}
            else if (2 * std::abs(x - T) <= W) {out = x + (-1.0f) * (x - T + (W/2.0f)) * (x - T + (W/2.0f))/(2.0f*W);}
            else {out = T;} 
        }

        return out;
    }

    float ToDB(float x) {return 20.0f * std::log10(x);}

    float ToLinear(float x) {return std::pow(10.0f,x/20.0f);}

    float GetLevel(float x)
    {
        static float m_prev_level{0.0f};
        
        if (x > m_prev_level) {x = m_attack * m_prev_level + (1.0f - m_attack) * x;}
        else {x = m_release * m_prev_level;}
        m_prev_level = x;

        return x;
    }
};