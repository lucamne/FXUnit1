#pragma once

#include "Effect.h"
#include "daisysp.h"

template<size_t max_size>
class Basic_Delay : public Effect
{
public:
    Basic_Delay(): Effect("Delay",3),m_lvl{0.5f}, m_feedback{0.5f}, m_max_samples{max_size}, m_delay_time{static_cast<float>(m_max_samples)/2.0f} {}
    ~Basic_Delay() {}

    void Init(float sample_rate) {m_sample_rate = sample_rate; m_delay_line.Init(); CycleParam(0);}
    float Process(float val)
    {
        const float next_samp{m_delay_line.Read()};
        m_delay_line.Write(val + next_samp*m_feedback);
        return val + next_samp*m_lvl;
    }

    void CycleParam(int param)
    {
        switch (param)
        {
            case 0:
                m_current_param_name = "Level";
                break;
            case 1:
                m_current_param_name = "DelayTime";
                break;
            case 2:
                m_current_param_name = "Feedback";
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
                m_lvl = val;
                return val;
            case 1:
                m_delay_time = {(static_cast<float>(m_max_samples) * val)};
                m_delay_line.SetDelay(m_delay_time);
                return val;
            case 2:
                m_feedback = val;
                return val;
            // return negative if error occurs and no valid param is selected
            default:
                return -1.0f;
        }
    }

private:
    daisysp::DelayLine<float,max_size> m_delay_line{};
    float m_lvl{};
    float m_feedback{};
    int m_max_samples{};
    float m_delay_time{};
};