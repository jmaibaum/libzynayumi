/****************************************************************************
    
    Parameters for Zynayumi

    parameters.cpp

    Copyleft (c) 2016-2019 Nil Geisweiller <ngeiswei@gmail.com>
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

****************************************************************************/

#include "parameters.hpp"

#include <cmath>
#include <sstream>

#include "patch.hpp"
#include "zynayumi.hpp"

namespace zynayumi {

Parameter::Parameter(const std::string& n)
	: name(n)
{
}

std::string Parameter::to_string(std::string indent) const
{
	return name + ": " + value_to_string();
}

BoolParameter::BoolParameter(const std::string& n, bool* v_ptr, bool v_dflt)
	: Parameter(n), value_ptr(v_ptr)
{
	*value_ptr = v_dflt;
}

std::string BoolParameter::value_to_string() const
{
	return std::to_string(*value_ptr);
}

float BoolParameter::float_value() const
{
	return (float)*value_ptr;
}

void BoolParameter::set_value(float f)
{
	*value_ptr = (bool)std::round(f);
}

float BoolParameter::norm_float_value() const
{
	return float_value();
}

void BoolParameter::set_norm_value(float nf)
{
	set_value(nf);
}

std::string IntParameter::value_to_string() const
{
	return std::to_string(*value_ptr);
}

IntParameter::IntParameter(const std::string& n, int* v_ptr, int v_dflt, int l, int u)
	: Parameter(n), value_ptr(v_ptr), low(l), up(u)
{
	*value_ptr = v_dflt;
}

float IntParameter::float_value() const
{
	return (float)*value_ptr;
}

void IntParameter::set_value(float f)
{
	*value_ptr = (int)std::round(f);
}

float IntParameter::norm_float_value() const
{
	return affine(low, up, 0.0f, 1.0f, (float)*value_ptr);
}

void IntParameter::set_norm_value(float nf)
{
	*value_ptr = (int)std::round(affine(0.0f, 1.0f, low, up, nf));
}

FloatParameter::FloatParameter(const std::string& n, float* v_ptr, float v_dflt, float l, float u)
	: Parameter(n), value_ptr(v_ptr), low(l), up(u)
{
	*value_ptr = v_dflt;
}

float FloatParameter::float_value() const
{
	return *value_ptr;
}

void FloatParameter::set_value(float f)
{
	*value_ptr = f;
}

std::string FloatParameter::value_to_string() const
{
	return std::to_string(*value_ptr);
}

LinearFloatParameter::LinearFloatParameter(const std::string& n, float* v_ptr,
                                           float v_dflt, float l, float u)
	: FloatParameter(n, v_ptr, v_dflt, l, u)
{
}

float LinearFloatParameter::norm_float_value() const
{
	return affine(low, up, 0.0f, 1.0f, *value_ptr);
}

void LinearFloatParameter::set_norm_value(float nf)
{
	*value_ptr = affine(0.0f, 1.0f, low, up, nf);
}

TanFloatParameter::TanFloatParameter(const std::string& n, float* v_ptr,
                                     float v_dflt, float l, float u)
	: FloatParameter(n, v_ptr, v_dflt, l, u)
	, atan_low(atanf(l))
	, atan_up(atanf(u))
{
}

float TanFloatParameter::norm_float_value() const
{
	if (*value_ptr == low)
		return 0.0;
	if (*value_ptr == up)
		return 1.0;
	return affine(atan_low, atan_up, 0.0f, 1.0f, atanf(*value_ptr));
}

void TanFloatParameter::set_norm_value(float nf)
{
	if (nf == 0.0)
		*value_ptr = low;
	else if (nf == 1.0)
		*value_ptr = up;
	else
		*value_ptr = tanf(affine(0.0f, 1.0f, atan_low, atan_up, nf));
}

Parameters::Parameters(Zynayumi& zyn)
	: zynayumi(zyn), parameters(PARAMETERS_COUNT)
{
	// Emulation mode (YM2149 vs AY-3-8910)
	parameters[EMUL_MODE] = new EnumParameter<EmulMode>(EMUL_MODE_STR,
	                                                    &zynayumi.patch.emulmode,
	                                                    EMUL_MODE_DFLT);

	// Play mode
	parameters[PLAY_MODE] = new EnumParameter<PlayMode>(PLAY_MODE_STR,
	                                                    &zynayumi.patch.playmode,
	                                                    PLAY_MODE_DFLT);

	// Tone
	parameters[TONE_TIME] = new TanFloatParameter(TONE_TIME_STR,
	                                              &zynayumi.patch.tone.time,
	                                              TONE_TIME_DFLT,
	                                              TONE_TIME_L,
	                                              TONE_TIME_U);

	parameters[TONE_DETUNE] = new TanFloatParameter(TONE_DETUNE_STR,
	                                                &tone_detune,
	                                                TONE_DETUNE_DFLT,
	                                                TONE_DETUNE_L,
	                                                TONE_DETUNE_U);

	parameters[TONE_TRANSPOSE] = new IntParameter(TONE_TRANSPOSE_STR,
	                                              &tone_transpose,
	                                              TONE_TRANSPOSE_DFLT,
	                                              TONE_TRANSPOSE_L,
	                                              TONE_TRANSPOSE_U);

	parameters[TONE_SPREAD] = new LinearFloatParameter(TONE_SPREAD_STR,
	                                                   &zynayumi.patch.tone.spread,
	                                                   TONE_SPREAD_DFLT,
	                                                   TONE_SPREAD_L,
	                                                   TONE_SPREAD_U);

	// Noise
	parameters[NOISE_TIME] = new TanFloatParameter(NOISE_TIME_STR,
	                                               &zynayumi.patch.noise.time,
	                                               NOISE_TIME_DFLT,
	                                               NOISE_TIME_L,
	                                               NOISE_TIME_U);

	parameters[NOISE_PERIOD] = new IntParameter(NOISE_PERIOD_STR,
	                                            &zynayumi.patch.noise.period,
	                                            NOISE_PERIOD_DFLT,
	                                            NOISE_PERIOD_L,
	                                            NOISE_PERIOD_U);

	// Noise Period Envelope
	parameters[NOISE_PERIOD_ENV_ATTACK] = new IntParameter(NOISE_PERIOD_ENV_ATTACK_STR,
	                                                       &zynayumi.patch.noise_period_env.attack,
	                                                       NOISE_PERIOD_ENV_ATTACK_DFLT,
	                                                       NOISE_PERIOD_ENV_ATTACK_L,
	                                                       NOISE_PERIOD_ENV_ATTACK_U);

	parameters[NOISE_PERIOD_ENV_TIME] = new TanFloatParameter(NOISE_PERIOD_ENV_TIME_STR,
	                                                          &zynayumi.patch.noise_period_env.time,
	                                                          NOISE_PERIOD_ENV_TIME_DFLT,
	                                                          NOISE_PERIOD_ENV_TIME_L,
	                                                          NOISE_PERIOD_ENV_TIME_U);

	// Amplitude envelope
	parameters[AMP_ENV_ATTACK_TIME] = new TanFloatParameter(AMP_ENV_ATTACK_TIME_STR,
	                                                        &zynayumi.patch.ampenv.attack_time,
	                                                        AMP_ENV_ATTACK_TIME_DFLT,
	                                                        AMP_ENV_ATTACK_TIME_L,
	                                                        AMP_ENV_ATTACK_TIME_U);

	parameters[AMP_ENV_HOLD1_LEVEL] = new LinearFloatParameter(AMP_ENV_HOLD1_LEVEL_STR,
	                                                           &zynayumi.patch.ampenv.hold1_level,
	                                                           AMP_ENV_HOLD1_LEVEL_DFLT,
	                                                           AMP_ENV_HOLD1_LEVEL_L,
	                                                           AMP_ENV_HOLD1_LEVEL_U);

	parameters[AMP_ENV_INTER1_TIME] = new TanFloatParameter(AMP_ENV_INTER1_TIME_STR,
	                                                        &zynayumi.patch.ampenv.inter1_time,
	                                                        AMP_ENV_INTER1_TIME_DFLT,
	                                                        AMP_ENV_INTER1_TIME_L,
	                                                        AMP_ENV_INTER1_TIME_U);

	parameters[AMP_ENV_HOLD2_LEVEL] = new LinearFloatParameter(AMP_ENV_HOLD2_LEVEL_STR,
	                                                           &zynayumi.patch.ampenv.hold2_level,
	                                                           AMP_ENV_HOLD2_LEVEL_DFLT,
	                                                           AMP_ENV_HOLD2_LEVEL_L,
	                                                           AMP_ENV_HOLD2_LEVEL_U);

	parameters[AMP_ENV_INTER2_TIME] = new TanFloatParameter(AMP_ENV_INTER2_TIME_STR,
	                                                        &zynayumi.patch.ampenv.inter2_time,
	                                                        AMP_ENV_INTER2_TIME_DFLT,
	                                                        AMP_ENV_INTER2_TIME_L,
	                                                        AMP_ENV_INTER2_TIME_U);

	parameters[AMP_ENV_HOLD3_LEVEL] = new LinearFloatParameter(AMP_ENV_HOLD3_LEVEL_STR,
	                                                           &zynayumi.patch.ampenv.hold3_level,
	                                                           AMP_ENV_HOLD3_LEVEL_DFLT,
	                                                           AMP_ENV_HOLD3_LEVEL_L,
	                                                           AMP_ENV_HOLD3_LEVEL_U);

	parameters[AMP_ENV_DECAY_TIME] = new TanFloatParameter(AMP_ENV_DECAY_TIME_STR,
	                                                       &zynayumi.patch.ampenv.decay_time,
	                                                       AMP_ENV_DECAY_TIME_DFLT,
	                                                       AMP_ENV_DECAY_TIME_L,
	                                                       AMP_ENV_DECAY_TIME_U);

	parameters[AMP_ENV_SUSTAIN_LEVEL] = new LinearFloatParameter(AMP_ENV_SUSTAIN_LEVEL_STR,
	                                                             &zynayumi.patch.ampenv.sustain_level,
	                                                             AMP_ENV_SUSTAIN_LEVEL_DFLT,
	                                                             AMP_ENV_SUSTAIN_LEVEL_L,
	                                                             AMP_ENV_SUSTAIN_LEVEL_U);

	parameters[AMP_ENV_RELEASE] = new TanFloatParameter(AMP_ENV_RELEASE_STR,
	                                                    &zynayumi.patch.ampenv.release,
	                                                    AMP_ENV_RELEASE_DFLT,
	                                                    AMP_ENV_RELEASE_L,
	                                                    AMP_ENV_RELEASE_U);

	// Pitch envelope
	parameters[PITCH_ENV_ATTACK_PITCH] = new LinearFloatParameter(PITCH_ENV_ATTACK_PITCH_STR,
	                                                              &zynayumi.patch.pitchenv.attack_pitch,
	                                                              PITCH_ENV_ATTACK_PITCH_DFLT,
	                                                              PITCH_ENV_ATTACK_PITCH_L,
	                                                              PITCH_ENV_ATTACK_PITCH_U);

	parameters[PITCH_ENV_TIME] = new TanFloatParameter(PITCH_ENV_TIME_STR,
	                                                   &zynayumi.patch.pitchenv.time,
	                                                   PITCH_ENV_TIME_DFLT,
	                                                   PITCH_ENV_TIME_L,
	                                                   PITCH_ENV_TIME_U);

	// Arpegio
	parameters[ARP_PITCH1] = new IntParameter(ARP_PITCH1_STR,
	                                          &zynayumi.patch.arp.pitch1,
	                                          ARP_PITCH1_DFLT,
	                                          ARP_PITCH1_L,
	                                          ARP_PITCH1_U);

	parameters[ARP_PITCH2] = new IntParameter(ARP_PITCH2_STR,
	                                          &zynayumi.patch.arp.pitch2,
	                                          ARP_PITCH2_DFLT,
	                                          ARP_PITCH2_L,
	                                          ARP_PITCH2_U);

	parameters[ARP_PITCH3] = new IntParameter(ARP_PITCH3_STR,
	                                          &zynayumi.patch.arp.pitch3,
	                                          ARP_PITCH3_DFLT,
	                                          ARP_PITCH3_L,
	                                          ARP_PITCH3_U);

	parameters[ARP_FREQ] = new LinearFloatParameter(ARP_FREQ_STR,
	                                                &zynayumi.patch.arp.freq,
	                                                ARP_FREQ_DFLT,
	                                                ARP_FREQ_L,
	                                                ARP_FREQ_U);

	parameters[ARP_REPEAT] = new IntParameter(ARP_REPEAT_STR,
	                                          &zynayumi.patch.arp.repeat,
	                                          ARP_REPEAT_DFLT,
	                                          ARP_REPEAT_L,
	                                          ARP_REPEAT_U);

	// Ring modulation
	parameters[RING_MOD_WAVEFORM_LEVEL1] = new LinearFloatParameter(RING_MOD_WAVEFORM_LEVEL1_STR,
	                                                                &zynayumi.patch.ringmod.waveform[0],
	                                                                RING_MOD_WAVEFORM_LEVEL1_DFLT,
	                                                                RING_MOD_WAVEFORM_LEVEL1_L,
	                                                                RING_MOD_WAVEFORM_LEVEL1_U);

	parameters[RING_MOD_WAVEFORM_LEVEL2] = new LinearFloatParameter(RING_MOD_WAVEFORM_LEVEL2_STR,
	                                                                &zynayumi.patch.ringmod.waveform[1],
	                                                                RING_MOD_WAVEFORM_LEVEL2_DFLT,
	                                                                RING_MOD_WAVEFORM_LEVEL2_L,
	                                                                RING_MOD_WAVEFORM_LEVEL2_U);

	parameters[RING_MOD_WAVEFORM_LEVEL3] = new LinearFloatParameter(RING_MOD_WAVEFORM_LEVEL3_STR,
	                                                                &zynayumi.patch.ringmod.waveform[2],
	                                                                RING_MOD_WAVEFORM_LEVEL3_DFLT,
	                                                                RING_MOD_WAVEFORM_LEVEL3_L,
	                                                                RING_MOD_WAVEFORM_LEVEL3_U);

	parameters[RING_MOD_WAVEFORM_LEVEL4] = new LinearFloatParameter(RING_MOD_WAVEFORM_LEVEL4_STR,
	                                                                &zynayumi.patch.ringmod.waveform[3],
	                                                                RING_MOD_WAVEFORM_LEVEL4_DFLT,
	                                                                RING_MOD_WAVEFORM_LEVEL4_L,
	                                                                RING_MOD_WAVEFORM_LEVEL4_U);

	parameters[RING_MOD_WAVEFORM_LEVEL5] = new LinearFloatParameter(RING_MOD_WAVEFORM_LEVEL5_STR,
	                                                                &zynayumi.patch.ringmod.waveform[4],
	                                                                RING_MOD_WAVEFORM_LEVEL5_DFLT,
	                                                                RING_MOD_WAVEFORM_LEVEL5_L,
	                                                                RING_MOD_WAVEFORM_LEVEL5_U);

	parameters[RING_MOD_WAVEFORM_LEVEL6] = new LinearFloatParameter(RING_MOD_WAVEFORM_LEVEL6_STR,
	                                                                &zynayumi.patch.ringmod.waveform[5],
	                                                                RING_MOD_WAVEFORM_LEVEL6_DFLT,
	                                                                RING_MOD_WAVEFORM_LEVEL6_L,
	                                                                RING_MOD_WAVEFORM_LEVEL6_U);

	parameters[RING_MOD_WAVEFORM_LEVEL7] = new LinearFloatParameter(RING_MOD_WAVEFORM_LEVEL7_STR,
	                                                                &zynayumi.patch.ringmod.waveform[6],
	                                                                RING_MOD_WAVEFORM_LEVEL7_DFLT,
	                                                                RING_MOD_WAVEFORM_LEVEL7_L,
	                                                                RING_MOD_WAVEFORM_LEVEL7_U);

	parameters[RING_MOD_WAVEFORM_LEVEL8] = new LinearFloatParameter(RING_MOD_WAVEFORM_LEVEL8_STR,
	                                                                &zynayumi.patch.ringmod.waveform[7],
	                                                                RING_MOD_WAVEFORM_LEVEL8_DFLT,
	                                                                RING_MOD_WAVEFORM_LEVEL8_L,
	                                                                RING_MOD_WAVEFORM_LEVEL8_U);

	parameters[RING_MOD_MIRROR] = new BoolParameter(RING_MOD_MIRROR_STR,
	                                                &zynayumi.patch.ringmod.mirror,
	                                                RING_MOD_MIRROR_DFLT);

	parameters[RING_MOD_SYNC] = new BoolParameter(RING_MOD_SYNC_STR,
	                                              &zynayumi.patch.ringmod.sync,
	                                              RING_MOD_SYNC_DFLT);

	parameters[RING_MOD_DETUNE] = new TanFloatParameter(RING_MOD_DETUNE_STR,
	                                                    &ringmod_detune,
	                                                    RING_MOD_DETUNE_DFLT,
	                                                    RING_MOD_DETUNE_L,
	                                                    RING_MOD_DETUNE_U);

	parameters[RING_MOD_TRANSPOSE] = new IntParameter(RING_MOD_TRANSPOSE_STR,
	                                                  &ringmod_transpose,
	                                                  RING_MOD_TRANSPOSE_DFLT,
	                                                  RING_MOD_TRANSPOSE_L,
	                                                  RING_MOD_TRANSPOSE_U);

	parameters[RING_MOD_FIXED_FREQUENCY] = new LinearFloatParameter(RING_MOD_FIXED_FREQUENCY_STR,
	                                                                &zynayumi.patch.ringmod.fixed_freq,
	                                                                RING_MOD_FIXED_FREQUENCY_DFLT,
	                                                                RING_MOD_FIXED_FREQUENCY_L,
	                                                                RING_MOD_FIXED_FREQUENCY_U);

	parameters[RING_MOD_FIXED_VS_RELATIVE] = new TanFloatParameter(RING_MOD_FIXED_VS_RELATIVE_STR,
	                                                               &zynayumi.patch.ringmod.fixed_vs_relative,
	                                                               RING_MOD_FIXED_VS_RELATIVE_DFLT,
	                                                               RING_MOD_FIXED_VS_RELATIVE_L,
	                                                               RING_MOD_FIXED_VS_RELATIVE_U);

	// Pitch LFO
	parameters[LFO_FREQ] = new LinearFloatParameter(LFO_FREQ_STR,
	                                                &zynayumi.patch.lfo.freq,
	                                                LFO_FREQ_DFLT,
	                                                LFO_FREQ_L,
	                                                LFO_FREQ_U);

	parameters[LFO_DELAY] = new TanFloatParameter(LFO_DELAY_STR,
	                                              &zynayumi.patch.lfo.delay,
	                                              LFO_DELAY_DFLT,
	                                              LFO_DELAY_L,
	                                              LFO_DELAY_U);

	parameters[LFO_DEPTH] = new TanFloatParameter(LFO_DEPTH_STR,
	                                              &zynayumi.patch.lfo.depth,
	                                              LFO_DEPTH_DFLT,
	                                              LFO_DEPTH_L,
	                                              LFO_DEPTH_U);

	// Portamento
	parameters[PORTAMENTO_TIME] = new TanFloatParameter(PORTAMENTO_TIME_STR,
	                                                    &zynayumi.patch.port,
	                                                    PORTAMENTO_TIME_DFLT,
	                                                    PORTAMENTO_TIME_L,
	                                                    PORTAMENTO_TIME_U);

	// Gain
	parameters[GAIN] = new LinearFloatParameter(GAIN_STR,
	                                            &zynayumi.patch.gain,
	                                            GAIN_DFLT,
	                                            GAIN_L,
	                                            GAIN_U);

	// Pan
	parameters[PAN0] = new LinearFloatParameter(PAN0_STR,
	                                            &zynayumi.patch.pan.ym_channel[0],
	                                            PAN0_DFLT,
	                                            PAN0_L,
	                                            PAN0_U);

	parameters[PAN1] = new LinearFloatParameter(PAN1_STR,
	                                            &zynayumi.patch.pan.ym_channel[1],
	                                            PAN1_DFLT,
	                                            PAN1_L,
	                                            PAN1_U);

	parameters[PAN2] = new LinearFloatParameter(PAN2_STR,
	                                            &zynayumi.patch.pan.ym_channel[2],
	                                            PAN2_DFLT,
	                                            PAN2_L,
	                                            PAN2_U);

	// Control
	parameters[PITCH_WHEEL] = new IntParameter(PITCH_WHEEL_STR,
	                                           &zynayumi.patch.control.pitchwheel,
	                                           PITCH_WHEEL_DFLT,
	                                           PITCH_WHEEL_L,
	                                           PITCH_WHEEL_U);

	parameters[VELOCITY_SENSITIVITY] = new LinearFloatParameter(VELOCITY_SENSITIVITY_STR,
	                                                            &zynayumi.patch.control.velocity_sensitivity,
	                                                            VELOCITY_SENSITIVITY_DFLT,
	                                                            VELOCITY_SENSITIVITY_L,
	                                                            VELOCITY_SENSITIVITY_U);

	parameters[MODULATION_SENSITIVITY] = new TanFloatParameter(MODULATION_SENSITIVITY_STR,
	                                                           &zynayumi.patch.control.modulation_sensitivity,
	                                                           MODULATION_SENSITIVITY_DFLT,
	                                                           MODULATION_SENSITIVITY_L,
	                                                           MODULATION_SENSITIVITY_U);
}

Parameters::~Parameters()
{
	for (auto& param : parameters)
		delete param;
}

std::string Parameters::get_name(ParameterIndex pi) const
{
	if (pi < parameters.size())
		return parameters[pi]->name;
	return "";
}

std::string Parameters::get_value_str(ParameterIndex pi) const
{
	if (pi < parameters.size())
		return parameters[pi]->value_to_string();
	return "";
}

float Parameters::float_value(ParameterIndex pi) const
{
	if (pi < parameters.size())
		return parameters[pi]->float_value();
	return 0.0f;
}

void Parameters::set_value(ParameterIndex pi, float f)
{
	parameters[pi]->set_value(f);
	switch (pi) {
	case TONE_DETUNE:
	case TONE_TRANSPOSE:
		zynayumi.patch.tone.detune = tone_detune + tone_transpose;
		break;
	case RING_MOD_DETUNE:
	case RING_MOD_TRANSPOSE:
		zynayumi.patch.ringmod.detune = ringmod_detune + ringmod_transpose;
		break;
	default:
		break;
	}
}

float Parameters::norm_float_value(ParameterIndex pi) const
{
	if (pi < parameters.size())
		return parameters[pi]->norm_float_value();
	return 0.0f;
}

void Parameters::set_norm_value(ParameterIndex pi, float nf)
{
	parameters[pi]->set_norm_value(nf);
	switch (pi) {
	case TONE_DETUNE:
	case TONE_TRANSPOSE:
		zynayumi.patch.tone.detune = tone_detune + tone_transpose;
		break;
	case RING_MOD_DETUNE:
	case RING_MOD_TRANSPOSE:
		zynayumi.patch.ringmod.detune = ringmod_detune + ringmod_transpose;
		break;
	default:
		break;
	}
}

std::string Parameters::to_string(std::string indent) const
{
	std::stringstream ss;
	for (std::size_t i = 0; i < parameters.size(); i++) {
		if (i != 0)
			ss << std::endl;
		ss << parameters[i]->to_string(indent);
	}
	return ss.str();
}

} // ~namespace zynayumi
