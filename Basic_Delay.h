#pragma once

#include "Effect.h"
#include "daisysp.h"

template<size_t max_size>
class Basic_Delay : public Effect
{
public:
    Basic_Delay(): Effect("Delay",2), m_feedback{0.5f}, m_max_samples{max_size}, m_delay_time{static_cast<float>(m_max_samples)/2.0f} {}
    ~Basic_Delay() {}

    void Init(float sample_rate) {m_sample_rate = sample_rate; m_delay_line.Init();}
    float Process(float val)
    {
        const float next_samp{m_delay_line.Read()};
        m_delay_line.Write(val + next_samp*m_feedback);
        return val*(1-m_mix) + next_samp*m_mix;
    }

    void CycleParam()
    {
        m_current_param = (m_current_param + 1) % m_param_count; if (m_current_param >= m_param_count) m_current_param = 0;

        switch (m_current_param)
        {
            case 0:
                m_current_param_name = "Mix";
                break;
            case 1:
                m_current_param_name = "DelayTime";
                break;
            default:
                m_current_param_name = "?";
        }
    }      


    float SetParam(float val)
    {
        switch (m_current_param)
        {
            case 0:
                m_mix = val;
                return val;
            case 1:
                m_delay_time = {(static_cast<float>(m_max_samples) * val)};
                m_delay_line.SetDelay(m_delay_time);
                return val;
            // return negative if error occurs and no valid param is selected
            default:
                return -1.0f;
        }
    }

private:
    daisysp::DelayLine<float,max_size> m_delay_line{};
    float m_feedback{};
    int m_max_samples{};
    float m_delay_time{};
};