#include "Basic_Overdrive.h"
#include "Basic_Delay.h"
#include "Bypass.h"

#include "daisy_seed.h"
//#include "daisysp.h"

constexpr int SAMPLE_RATE{48000};

// define hardware
daisy::DaisySeed hw{};

// init effects
Bypass bypass{};
Basic_Overdrive overdrive{};
Basic_Delay<SAMPLE_RATE> delay{};
// create array to hold all effects
constexpr int EFFECT_COUNT{3};
Effect* effect_array[EFFECT_COUNT]{&bypass,&overdrive,&delay};
Effect* current_effect{effect_array[0]};

void AudioCallback(daisy::AudioHandle::InterleavingInputBuffer in, daisy::AudioHandle::InterleavingOutputBuffer out, size_t size)
{
    // currently only in mono
    for (size_t i = 0; i < size; i+=2)
    {
        float sig_out{current_effect->Process(in[i])};
        out[i] = sig_out;
        out[i+1] = sig_out;
    }
}

int main(void)
{
	// init hardware
	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(daisy::SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);
	hw.StartLog();
	const float hw_sample_rate {hw.AudioSampleRate()};

	// init overdrive
	overdrive.Init(hw_sample_rate);
	overdrive.SetDrive(0.5f);
	// init bypass
	bypass.Init(hw_sample_rate);
	// init delay
	delay.Init(hw_sample_rate);

	// init encoder for switching modes
	daisy::GPIO encoder_out_a{};
	daisy::GPIO encoder_out_b{};
	encoder_out_a.Init(daisy::seed::D0, daisy::GPIO::Mode::INPUT, daisy::GPIO::Pull::PULLDOWN);
	encoder_out_b.Init(daisy::seed::D1, daisy::GPIO::Mode::INPUT, daisy::GPIO::Pull::PULLDOWN);
	// bool values used to track encoder movement
	bool previous_a_state{encoder_out_a.Read()};
	bool curr_a_state{previous_a_state};

	// init button for switching param target
	daisy::GPIO param_button{};
	param_button.Init(daisy::seed::D2, daisy::GPIO::Mode::INPUT, daisy::GPIO::Pull::PULLUP);
	// bool value to track button state
	bool prev_button_state {false};

	// Configure ADC input
	daisy::AdcChannelConfig adc_channel_config{};
	// set pin to ADC
	adc_channel_config.InitSingle(daisy::seed::A0);
	// give handle to adc_cahnnel_config array and length
	hw.adc.Init(&adc_channel_config,1);
	// start adc
	hw.adc.Start();

	// tracks current effect mode
	int mode {0};
	constexpr int MODE_COUNT {EFFECT_COUNT};

	while(1)
	{
		/* Handles encoder movement */
		curr_a_state = encoder_out_a.Read();
		/* current encoder output a read is different then previous, a pulse has occured and the encoder has been moved 
		check that curr_a_state is true to prevent double reading*/
		if (curr_a_state != previous_a_state && curr_a_state)
		{

			// if encoder output b state is different then a state, the encoder is rotating clockwise
			if (curr_a_state != encoder_out_b.Read())
			{
				mode = (mode + 1) % MODE_COUNT;
			}
			// otherwise encoder is rotating counter clockwise
			else
			{
				mode = (mode  + MODE_COUNT - 1) % MODE_COUNT;
			}
			// if mode is within array bounds
			if (mode >= 0 && mode < EFFECT_COUNT)
			{
				current_effect = effect_array[mode];
			}
			// otherwise set to bypass
			else
			{
				current_effect = effect_array[0];
			}
		}
		previous_a_state = curr_a_state;

		/* Handle button state */
		bool button_state {!param_button.Read()};
		// if button went from unpressed to pressed
		if (button_state && !prev_button_state)
		{
			current_effect->CycleParam();
		}
		prev_button_state = button_state;

		float param_value{};

		/* read current adc mix pot value */
		const float main_pot_val {hw.adc.GetFloat(0)};
		// snap to 0
		if (main_pot_val < 0.005f ) {param_value = current_effect->SetParam(0.0f); }
		// snap to 100
		else if (main_pot_val > 0.995f ) {param_value = current_effect->SetParam(1.0f); }
		// if difference bewteen current mix value and previous is large enough then change it
		else {param_value = current_effect->SetParam(main_pot_val);}

		hw.PrintLine("Mode:%s Param:%s Amt:%f" ,current_effect->GetEffectName().c_str(),current_effect->GetParamName().c_str(),param_value);

		hw.DelayMs(1);
	}
}
