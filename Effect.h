#pragma once

#include <string>
#include <cassert>

struct Parameter
{
    std::string name{};
    float value{0.0f};
};

class Effect
{
public:
    Effect(std::string effect_name, int param_count)
    :m_effect_name{effect_name}, m_sample_rate{48000.0f}, m_param_count{param_count}, m_current_param{0}
    {
        if (param_count > 0) {m_param_array = new Parameter[param_count];}
        else {m_param_array = nullptr;}
    }
    ~Effect() {delete[] m_param_array;}
    // general init function
    virtual void Init(float sample_rate) {m_sample_rate = sample_rate;}
    // processes sample 
    virtual float Process(float val) = 0;
    // cycles to next param
    void CycleParam()
    {
        m_current_param = (m_current_param + 1) % m_param_count; 
        if (m_current_param >= m_param_count) m_current_param = 0;
    }
    // choose next param
    void SelectCurrentParam(int param) {m_current_param = param;}
    
    void SetSampleRate(float rate) {m_sample_rate = rate;}
    void SetParam(float val) { if(m_param_array != nullptr) {m_param_array[m_current_param].value = val;}}
    virtual void SetParam(int param_num, float val) {assert(param_num < m_param_count);if(m_param_array != nullptr) {m_param_array[param_num].value = val;}}

    const std::string& GetEffectName() const {return m_effect_name;}
    float GetSampleRate() const {return m_sample_rate;}
    const std::string& GetCurrentParamName() const {if(m_param_array == nullptr) return m_effect_name; else return m_param_array[m_current_param].name;}
    float GetCurrentParamValue() {if(m_param_array == nullptr) return 0.0f;else return m_param_array[m_current_param].value;}

protected:
    std::string m_effect_name{};
    float m_sample_rate{};
    const int m_param_count{};
    int m_current_param{};

    Parameter* m_param_array{}; //< Holds parameters for each effect
};