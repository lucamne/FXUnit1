#pragma once

#include "Effect.h"

class Bypass : public Effect
{
public:
    Bypass()
    :Effect("Bypass",0) {};
    ~Bypass() {};

    float Process(float val) {return val;}
};