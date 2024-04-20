#pragma once

#include "Effect.h"

class Bypass : public Effect
{
public:
    Bypass()
    :Effect("Bypass",1) {};
    ~Bypass() {};

    float Process(float val) {return val;}

    void CycleParam() {m_current_param = 0;}

    float SetParam(float val) {return val;}
};