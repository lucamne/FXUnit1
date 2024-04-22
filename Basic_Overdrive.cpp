#include "daisysp.h"
#include "Basic_Overdrive.h"
#include <algorithm>

Basic_Overdrive::Basic_Overdrive()
:Effect("Overdrive",2)
{
    m_param_array[0] = Parameter{"Mix",0.5f};
    m_param_array[1] = Parameter{"Drive",0.5f};
}

void Basic_Overdrive::Init(float sample_rate)
{
    m_sample_rate = sample_rate;
    SetDrive(m_param_array[1].value);
}

float Basic_Overdrive::Process(float in)
{
    float pre = pre_gain_ * in;
    const float mix{m_param_array[0].value};
    return (daisysp::SoftClip(pre) * post_gain_)*mix + in*(1-mix);
}

void Basic_Overdrive::SetDrive(float drive)
{
    drive  = daisysp::fclamp(drive, 0.f, 1.f);
    drive_ = 2.f * drive;

    const float drive_2    = drive_ * drive_;
    const float pre_gain_a = drive_ * 0.5f;
    const float pre_gain_b = drive_2 * drive_2 * drive_ * 24.0f;
    pre_gain_              = pre_gain_a + (pre_gain_b - pre_gain_a) * drive_2;

    const float drive_squashed = drive_ * (2.0f - drive_);
    post_gain_ = 1.0f / daisysp::SoftClip(0.33f + drive_squashed * (pre_gain_ - 0.33f));
}

void Basic_Overdrive::SetParam(float val)
{
    m_param_array[m_current_param].value = val;
    if (m_current_param == 1) {SetDrive(val);}
}