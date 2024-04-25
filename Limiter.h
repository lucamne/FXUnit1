#pragma once

#include "Effect.h"

#include <cmath>

class Limiter : public Effect
{
public:
    Limiter(): Effect("Limiter", 0), m_led_state{false}, m_attack{0.99f}, m_release{0.999f} {}
    ~Limiter() {}

    void Init(float sample_rate)
    {
        m_sample_rate = sample_rate;
    }

    float Process(float input)
    {
        float sidechain{input * 5};//>  current value of side chain

        sidechain = {std::abs(sidechain)};//> get sidechain absolute val

        sidechain = ToDB(sidechain);//> convert to log domain

        sidechain = sidechain - GainComputer(sidechain);//> calculate difference between signal and compressed signal using signal from GetLevel

        sidechain = GetLevel(sidechain);//> calculate level

        sidechain = ToLinear(sidechain);//> convert to linear domain

        float out {input * sidechain};//> apply gain


        if (ToDB(std::abs(out)) - ToDB(std::abs(input)) >= 1.0f) m_led_state = true;
        else m_led_state = false;

        return out;
    }
    bool GetLedState() const {return m_led_state;}

private:
    bool m_led_state{};
    float m_attack{};
    float m_release{};
    
    float GainComputer(float x)
    {
        const float T{-0.3f};//> get threshold
        const float W{2.0f};//> get knee wisth
        float out{};
        // calculate compressed level based on level relative to threshold and knee width
        if (2 * (x - T) < -1.0f * W) {out = x;}
        else if (2 * std::abs(x - T) <= W) {out = x + (-1.0f) * (x - T + (W/2.0f)) * (x - T + (W/2.0f))/(2.0f*W);}
        else {out = T;}

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