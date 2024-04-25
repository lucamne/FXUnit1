#pragma once

#include "Effect.h"

#include "daisysp.h"

template<size_t max_size>
class Basic_Delay : public Effect
{
public:
    Basic_Delay(): Effect("Delay",3),m_level{0.5f},m_delay_time{0.5f},m_feedback{0.5f} {}
    ~Basic_Delay() {}

    void Init(float sample_rate)
    {
        m_sample_rate = sample_rate; 
        m_delay_line.Init();
        m_param_array[0] = Parameter{"Level",&m_level};
        m_param_array[1] = Parameter{"Feedback",&m_feedback};
        m_param_array[2] = Parameter{"Delay Time",&m_delay_time};

        m_delay_line.SetDelay(static_cast<float>(max_size) * m_delay_time);
    }

    float Process(float val)
    {
        const float next_samp{m_delay_line.Read()};
        m_delay_line.Write(val + next_samp * m_feedback);
        return val + next_samp * m_level;
    }    


    void SetParam(float val)
    {
        m_param_array[m_current_param].SetValue(val);
        if (m_current_param == 2) {m_delay_line.SetDelay(static_cast<float>(max_size) * m_delay_time);}
    }

    void SetParam(int param_num, float val)
    {
        m_param_array[param_num].SetValue(val);
        if (param_num == 2) {m_delay_line.SetDelay(static_cast<float>(max_size) * m_delay_time);}
    }

private:
    daisysp::DelayLine<float,max_size> m_delay_line{};
    int m_max_samples{};
    float m_level{};
    float m_delay_time{};
    float m_feedback{};
};