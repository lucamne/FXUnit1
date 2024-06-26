#pragma once

#include "Effect.h"

#include <stdint.h>

/**  
       @brief Distortion / Overdrive Module
       @author Ported by Ben Sergentanis 
       @date Jan 2021 
       Ported from pichenettes/eurorack/plaits/dsp/fx/overdrive.h \n
       to an independent module. \n
       Original code written by Emilie Gillet in 2014. \n
*/

class Basic_Overdrive : public Effect
{
  public:
    Basic_Overdrive();
    ~Basic_Overdrive() {}

    /** Initializes the module with 0 gain */
    void Init(float sample_rate);

    /** Get the next sample
      \param in Input to be overdriven
    */
    float Process(float in);

    void SetParam(float val);

  private:
    float drive_;
    float pre_gain_;
    float post_gain_;

    /** Set the amount of drive
      \param drive Works from 0-1
    */
    void SetDrive(float drive);
};