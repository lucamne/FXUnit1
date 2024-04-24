#include "Compressor.h"
#include "Basic_Delay.h"
#include "Bypass.h"

#include "daisy_seed.h"

///*** Global Values ***///
constexpr int SAMPLE_RATE{48000};

daisy::DaisySeed hw{}; //> Daisy seed hardware object

// init effects
Bypass bypass{};
Basic_Delay<SAMPLE_RATE> delay{};
Compressor comp{};
// Store effects for easy access
constexpr int EFFECT_COUNT{3};
Effect* effect_array[EFFECT_COUNT]{&bypass,&delay,&comp};
Effect* current_effect{effect_array[0]};

void AudioCallback(daisy::AudioHandle::InterleavingInputBuffer in, daisy::AudioHandle::InterleavingOutputBuffer out, size_t size)
{
    for (size_t i = 0; i < size; i+=2)
    {
        float sig_out{current_effect->Process(in[i])};
        out[i] = sig_out;
        out[i+1] = sig_out;
    }
}

int main(void)
{
	///*** Init section ***///
	// init hardware
	hw.Init();
	hw.SetAudioBlockSize(4); //> number of samples handled per callback
	hw.SetAudioSampleRate(daisy::SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);
	hw.StartLog();
	hw.PrintLine("Hardware Initialized");
	const float hw_sample_rate {hw.AudioSampleRate()};
	
	// init effects
	bypass.Init(hw_sample_rate);
	delay.Init(hw_sample_rate);
	comp.Init(hw_sample_rate);
	hw.PrintLine("Effects Initialized");

	// init encoders for switching modes
	daisy::GPIO encoder_out_a{};
	daisy::GPIO encoder_out_b{};
	encoder_out_a.Init(daisy::seed::D0, daisy::GPIO::Mode::INPUT, daisy::GPIO::Pull::PULLDOWN);
	encoder_out_b.Init(daisy::seed::D1, daisy::GPIO::Mode::INPUT, daisy::GPIO::Pull::PULLDOWN);
	
	// init button for switching param target
	daisy::GPIO param_button{};
	param_button.Init(daisy::seed::D2, daisy::GPIO::Mode::INPUT, daisy::GPIO::Pull::PULLUP);
	// Configure ADC input
	daisy::AdcChannelConfig adc_channel_config{};
	adc_channel_config.InitSingle(daisy::seed::A0);	//> set pin to ADC
	hw.adc.Init(&adc_channel_config,1);	//> give handle to adc_cahnnel_config array and length
	hw.adc.Start(); //> start adc
	hw.PrintLine("External Hardware Initialized");

	///*** State Variables ***///
	int mode {0};	//> tracks current effect mode
	constexpr int MODE_COUNT {EFFECT_COUNT};
	// bool values used to track encoder movement
	bool previous_a_state{encoder_out_a.Read()};
	bool curr_a_state{previous_a_state};
	bool prev_button_state {false};	//> bool value to track button state=
	float main_pot_val{hw.adc.GetFloat(0)};	//> track main pot value

	///*** Pogram Loop ***///
	while(1)
	{
		// Handles encoder movement
		curr_a_state = encoder_out_a.Read();
		/* current encoder output a read is different then previous, a pulse has occured and the encoder has been moved 
		check that curr_a_state is true to prevent double reading*/
		if (curr_a_state != previous_a_state && curr_a_state)
		{
			// if encoder output b state is different then a state, the encoder is rotating clockwise
			if (curr_a_state != encoder_out_b.Read()) {mode = (mode + 1) % MODE_COUNT;}
			// otherwise encoder is rotating counter clockwise
			else {mode = (mode  + MODE_COUNT - 1) % MODE_COUNT;}
			// if mode is within array bounds
			if (mode >= 0 && mode < EFFECT_COUNT) {current_effect = effect_array[mode];}
			// otherwise set to bypass
			else {current_effect = effect_array[0];}
		}
		previous_a_state = curr_a_state;

		// Handles button state
		bool button_state {!param_button.Read()};
		if (button_state && !prev_button_state) {current_effect->CycleParam();}	//> if button went from unpressed to pressed
		prev_button_state = button_state;

		// Handles main potentiometer state
		if (std::abs(hw.adc.GetFloat(0) - main_pot_val) >= 0.005f)	//> if difference between new and previous value is large enough then update
		{
			main_pot_val = hw.adc.GetFloat(0);
			if (main_pot_val < 0.005f) {main_pot_val = 0.0f;} //< snap to 0
			else if (main_pot_val > 0.995f) {main_pot_val = 1.0f;} //< snap to 1
			current_effect->SetParam(main_pot_val);
		}

		// Print to serial monitor
		hw.PrintLine("Mode:%s Param:%s Amt:%f",
			current_effect->GetEffectName().c_str(),
			current_effect->GetCurrentParamName().c_str(),
			current_effect->GetCurrentParamValue());
	}
}
