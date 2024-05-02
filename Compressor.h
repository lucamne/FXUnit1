#pragma once

#include "Effect.h"

#include "daisysp.h"

#include <cmath>



/** 
    @brief Compressor model with limit mode
    @author Luca Negris
    @date April 2024 
    
    Compressor designed following reccomendations of "Digital Dynamic Range Compressor Design—
    A Tutorial and Analysis" by GIANNOULIS, MASSBERG, and REISS 
    (https://www.eecs.qmul.ac.uk/~josh/documents/2012/GiannoulisMassbergReiss-dynamicrangecompression-JAES2012.pdf). 
*/

class Compressor : public Effect
{
public:
    Compressor(): 
    Compressor("Compressor",5)
    {}

    Compressor(const std::string& name, int param_count)
    :Effect(name,param_count),
    m_attack{1.0f},
    m_release{40.0f},
    m_knee_width{1.0f},
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
        m_param_array[2] = Parameter{"Attack(ms)", &m_attack, 99.0f, 1.0f};
        m_param_array[3] = Parameter{"Release(ms)", &m_release, 1960.0f, 40.0f};
        m_param_array[4] = Parameter{"Makeup",&m_makeup_gain,60.0f,0.0f};
    }

    float Process(float input)
    {
        float sidechain{input};//>  current value of side chain
        // get absolute peak of signal
        sidechain = {std::abs(sidechain)};
        // convert to log domain
        sidechain = ToDB(sidechain);
        // calculate difference between signal and signal with gain applied
        sidechain = sidechain - GainComputer(sidechain);
        // Record compression level for readouts
        m_dbs_compressed = sidechain;
        // calculate level according to envelope follower and subtract from makeup gain
        sidechain = GetLevel(sidechain);
        sidechain = m_makeup_gain - sidechain;
        // convert to linear domain
        sidechain = ToLinear(sidechain);
        // apply gain
        return input * sidechain;
    }
    // returns current compression activity of compressor
    float GetDbsCompressed() const {return m_dbs_compressed;}

protected:
    float m_attack{};//< attack in ms
    float m_release{};//< release in ms
    float m_knee_width{};//< width of compression characteristic curve in db
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
        
        // apply compression according to compression characteristic curve defined by T, W, and R
        if (!m_limit_mode) //> If in limit mode then ratio is infinity
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
    // convert linear signal to log scale (db)
    constexpr float ToDB(float x) {return 20.0f * std::log10(x);}
    // convert to linear from log (db) domain
    constexpr float ToLinear(float x) {return std::pow(10.0f,x/20.0f);}
    // convert from milliseconds to time constant values used in peak level envelope follower
    constexpr float MsToAlpha(float x) {return std::exp(-1.0f / (x / 1000.0f * m_sample_rate));}
    // to be deleted once other GetLevel is working
    float _GetLevel(float x)
    {
        static float prev_level{0.0f};

        if (x > prev_level) {x = MsToAlpha(m_attack) * prev_level + (1.0f - MsToAlpha(m_attack)) * x;}
        else {x = MsToAlpha(m_release) * prev_level;}

        if (std::isinf(x)) {return prev_level;}

        prev_level = x;

        return x;
    }

    // Generate envelope based on peak level
    float GetLevel(float x)
    {
        static float prev_level{0.0f};

        const float y1 {std::max(x,MsToAlpha(m_release) * prev_level + (1 - MsToAlpha(m_release)) * x)};
        const float yL {MsToAlpha(m_attack) * prev_level + (1 - MsToAlpha(m_attack)) * y1};

        if (std::isinf(yL) || std::isnan(yL)) {return prev_level;} //> prevent issues caused by rounded float values

        prev_level = yL;
        return yL;
    }
};