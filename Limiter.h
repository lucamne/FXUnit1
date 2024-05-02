#pragma once

#include "Compressor.h"

#include <cmath>

class Limiter : public Compressor
{
public:
    Limiter(): Compressor("Limiter", 0) {}
    ~Limiter() {}

    void Init(float sample_rate)
    {
        m_sample_rate = sample_rate;
        m_threshold = -0.3f;
        m_limit_mode = true;
    }

    bool IsLimiting() const {return m_dbs_compressed >= 0.3f;} //> returns true if limiter is active
};