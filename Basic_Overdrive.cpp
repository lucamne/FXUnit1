#include "daisysp.h"
#include "Basic_Overdrive.h"
#include <algorithm>

void Basic_Overdrive::Init(float sample_rate)
{
    m_sample_rate = sample_rate;
    SetDrive(.5f);
    CycleParam(0);
}

float Basic_Overdrive::Process(float in)
{
    float pre = pre_gain_ * in;
    return (daisysp::SoftClip(pre) * post_gain_)*m_mix + in*(1-m_mix);
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

void Basic_Overdrive::CycleParam(int param) 
{
    switch (param)
    {
        case 0:
            m_current_param_name = "Mix";
            break;
        case 1:
            m_current_param_name = "Drive";
            break;
        default:
            m_current_param_name = "?";
    }
}

float Basic_Overdrive::SetParam(float val)
{
    switch (m_current_param)
    {
        case 0:
            m_mix = val;
            return val;
        case 1:
            SetDrive(val);
            return val;
        // return negative if error occurs and no valid param is selected
        default:
            return -1.0f;
    }
}