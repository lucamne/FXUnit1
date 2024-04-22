#include "Effect.h"

Effect::Effect(std::string effect_name, int param_count)
:m_effect_name{effect_name},
m_sample_rate{48000.0f},
m_param_count{param_count},
m_current_param{0}
{
    if (param_count > 0) {m_param_array = new Parameter[param_count];}
    else {m_param_array = nullptr;}
}

void Effect::CycleParam()
{
    m_current_param = (m_current_param + 1) % m_param_count;    //> increment current param
    if (m_current_param >= m_param_count) m_current_param = 0;  //> bounds checking
}

void Effect::SetParam(float val) 
{ 
    if(m_param_array != nullptr) {m_param_array[m_current_param].value = val;}
}

void Effect::SetParam(int param_num, float val)
{
    assert(param_num < m_param_count);
    if(m_param_array != nullptr) {m_param_array[param_num].value = val;}
}

const std::string& Effect::GetCurrentParamName() const 
{
    if(m_param_array == nullptr) return m_effect_name; 
    else return m_param_array[m_current_param].name;
}

float Effect::GetCurrentParamValue() 
{
    if(m_param_array == nullptr) return 0.0f;
    else return m_param_array[m_current_param].value;
}

float Effect::GetCurrentParamValue()
{
    if(m_param_array == nullptr) return 0.0f;
    else return m_param_array[m_current_param].value;
}