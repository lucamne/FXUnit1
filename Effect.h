#pragma once

#include <string>
#include <cassert>

struct Parameter
{
    std::string name{};
    float value{0.0f};
    float range{1.0f};//< factor used to scale input
    float floor{0.0f};//< set min value of input

    void SetValue(float val) {value = val * range + floor;}
};

class Effect
{
public:
    Effect(std::string effect_name, int param_count);
    ~Effect() {delete[] m_param_array;}

    virtual void Init(float sample_rate) {m_sample_rate = sample_rate;} //> general init function that should be overridden if neccassary
    virtual float Process(float val) = 0;   //> processes sample, to be implemented in each effect
    void CycleParam();  //> cycles to next parameter
    void SelectCurrentParam(int param) {m_current_param = param;}   //> manually select next parameter
    
    void SetSampleRate(float rate) {m_sample_rate = rate;}
    virtual void SetParam(float val);   //> Set value of currently selected parameter
    virtual void SetParam(int param_num, float val);    //> set value of provided parameter number

    const std::string& GetEffectName() const {return m_effect_name;}
    const std::string& GetCurrentParamName() const;
    float GetSampleRate() const {return m_sample_rate;}
    float GetCurrentParamValue();


protected:
    std::string m_effect_name{};
    float m_sample_rate{};
    const int m_param_count{};
    int m_current_param{};

    Parameter* m_param_array{}; //< Holds parameters for each effect
};