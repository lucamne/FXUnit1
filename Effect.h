#pragma once

#include <string>

class Effect
{
public:
    Effect(std::string effect_name, int param_count)
    :m_effect_name{effect_name}, m_sample_rate{48000.0f}, m_param_count{param_count}, m_current_param{0}, m_current_param_name{"Mix"}, m_mix{1.0f} {}
    ~Effect() {}

    virtual void Init(float sample_rate) {m_sample_rate = sample_rate;}
    virtual float Process(float val) = 0;
    virtual void CycleParam() = 0;
    
    void SetSampleRate(float rate) {m_sample_rate = rate;}
    virtual float SetParam(float val) = 0;

    const std::string& GetEffectName() const {return m_effect_name;}
    float GetSampleRate() const {return m_sample_rate;}
    const std::string& GetParamName() const {return m_current_param_name;}

protected:
    std::string m_effect_name{};
    float m_sample_rate{};
    int m_param_count{};
    int m_current_param{};
    std::string m_current_param_name{};
    float m_mix{}; //< m_mix should be between 0.0f and 1.0f
};