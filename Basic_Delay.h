#pragma once

#include "Effect.h"
#include "daisysp.h"

template<size_t max_size>
class Basic_Delay : public Effect
{
public:
    Basic_Delay(): Effect("Delay",3) {}
    ~Basic_Delay() {}

    void Init(float sample_rate)
    {
        m_sample_rate = sample_rate; 
        m_delay_line.Init();
        m_param_array[0] = Parameter{"Level",0.5f};
        m_param_array[1] = Parameter{"Feedback",0.5f};
        m_param_array[2] = Parameter{"Delay Time",0.5f};

        m_delay_line.SetDelay(static_cast<float>(max_size) * m_param_array[2].value);
    }

    float Process(float val)
    {
        const float next_samp{m_delay_line.Read()};
        m_delay_line.Write(val + next_samp*m_param_array[1].value);
        return val + next_samp*m_param_array[0].value;
    }    


    void SetParam(float val)
    {
        m_param_array[m_current_param].value = val;
        if (m_current_param == 2)
        {
            
            m_delay_line.SetDelay(static_cast<float>(max_size) * val);
        }
    }

private:
    daisysp::DelayLine<float,max_size> m_delay_line{};
    int m_max_samples{};
};