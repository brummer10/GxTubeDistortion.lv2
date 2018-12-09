/*
 * Copyright (C) 2014 Guitarix project MOD project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */


#include <cstdlib>
#include <cmath>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <assert.h>

///////////////////////// MACRO SUPPORT ////////////////////////////////

#define __rt_func __attribute__((section(".rt.text")))
#define __rt_data __attribute__((section(".rt.data")))

///////////////////////// FAUST SUPPORT ////////////////////////////////

#define FAUSTFLOAT float
#ifndef N_
#define N_(String) (String)
#endif
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))

#define always_inline inline __attribute__((always_inline))

template <int32_t N> inline float faustpower(float x)
{
  return powf(x, N);
}
template <int32_t N> inline double faustpower(double x)
{
  return pow(x, N);
}
template <int32_t N> inline int32_t faustpower(int32_t x)
{
  return faustpower<N/2>(x) * faustpower<N-N/2>(x);
}
template <>      inline int32_t faustpower<0>(int32_t x)
{
  return 1;
}
template <>      inline int32_t faustpower<1>(int32_t x)
{
  return x;
}

////////////////////////////// LOCAL INCLUDES //////////////////////////

#include "gx_TubeDistortion.h"        // define struct PortIndex
#include "gx_pluginlv2.h"   // define struct PluginLV2
#include "resampler.cc"   // 
#include "resampler-table.cc"   // 
#include "zita-resampler/resampler.h"   //
#include "TubeDistortion_in.cc"    // dsp class generated by faust -> dsp2cc
#include "TubeDistortion_clip.cc"    // dsp class generated by faust -> dsp2cc
#include "TubeDistortion_out.cc"    // dsp class generated by faust -> dsp2cc

////////////////////////////// PLUG-IN CLASS ///////////////////////////

namespace TubeDistortion {

class SimpleResampler {
 private:
    Resampler r_up, r_down;
    int m_fact;
 public:
    SimpleResampler(): r_up(), r_down(), m_fact() {}
    void setup(int sampleRate, unsigned int fact);
    void up(int count, float *input, float *output);
    void down(int count, float *input, float *output);
};

void SimpleResampler::setup(int sampleRate, unsigned int fact)
{
	m_fact = fact;
	const int qual = 16; // resulting in a total delay of 2*qual (0.7ms @44100)
	// upsampler
	r_up.setup(sampleRate, sampleRate*fact, 1, qual);
	// k == inpsize() == 2 * qual
	// pre-fill with k-1 zeros
	r_up.inp_count = r_up.inpsize() - 1;
	r_up.out_count = 1;
	r_up.inp_data = r_up.out_data = 0;
	r_up.process();
	// downsampler
	r_down.setup(sampleRate*fact, sampleRate, 1, qual);
	// k == inpsize() == 2 * qual * fact
	// pre-fill with k-1 zeros
	r_down.inp_count = r_down.inpsize() - 1;
	r_down.out_count = 1;
	r_down.inp_data = r_down.out_data = 0;
	r_down.process();
}

void SimpleResampler::up(int count, float *input, float *output)
{
	r_up.inp_count = count;
	r_up.inp_data = input;
	r_up.out_count = count * m_fact;
	r_up.out_data = output;
	r_up.process();
	assert(r_up.inp_count == 0);
	assert(r_up.out_count == 0);
}

void SimpleResampler::down(int count, float *input, float *output)
{
	r_down.inp_count = count * m_fact;
	r_down.inp_data = input;
	r_down.out_count = count+1; // +1 == trick to drain input
	r_down.out_data = output;
	r_down.process();
	assert(r_down.inp_count == 0);
	assert(r_down.out_count == 1);
}

class Gx_TubeDistortion_
{
private:
  // pointer to buffer
  float*          output;
  float*          input;
  // pointer to dsp class
  PluginLV2*      TubeDistortion_in;
  PluginLV2*      TubeDistortion_clip;
  PluginLV2*      TubeDistortion_out;

  // bypass ramping
  float*          bypass;
  uint32_t        bypass_;
 
  bool            needs_ramp_down;
  bool            needs_ramp_up;
  float           ramp_down;
  float           ramp_up;
  float           ramp_up_step;
  float           ramp_down_step;
  bool            bypassed;

  uint32_t fSamplingFreq;
  SimpleResampler smp;
  unsigned int sru;
  unsigned int fact;

  // private functions
  inline void run_dsp_(uint32_t n_samples);
  inline void connect_(uint32_t port,void* data);
  inline void init_dsp_(uint32_t rate);
  inline void connect_all__ports(uint32_t port, void* data);
  inline void activate_f();
  inline void clean_up();
  inline void deactivate_f();

public:
  // LV2 Descriptor
  static const LV2_Descriptor descriptor;
  // static wrapper to private functions
  static void deactivate(LV2_Handle instance);
  static void cleanup(LV2_Handle instance);
  static void run(LV2_Handle instance, uint32_t n_samples);
  static void activate(LV2_Handle instance);
  static void connect_port(LV2_Handle instance, uint32_t port, void* data);
  static LV2_Handle instantiate(const LV2_Descriptor* descriptor,
                                double rate, const char* bundle_path,
                                const LV2_Feature* const* features);
  Gx_TubeDistortion_();
  ~Gx_TubeDistortion_();
};

// constructor
Gx_TubeDistortion_::Gx_TubeDistortion_() :
  output(NULL),
  input(NULL),
  TubeDistortion_in(TubeDistortion_in::plugin()),
  TubeDistortion_clip(TubeDistortion_clip::plugin()),
  TubeDistortion_out(TubeDistortion_out::plugin()),
  bypass(0),
  bypass_(2),
  needs_ramp_down(false),
  needs_ramp_up(false),
  bypassed(false) {};

// destructor
Gx_TubeDistortion_::~Gx_TubeDistortion_()
{
  // just to be sure the plug have given free the allocated mem
  // it didn't hurd if the mem is already given free by clean_up()
  if (TubeDistortion_in->activate_plugin !=0)
    TubeDistortion_in->activate_plugin(false, TubeDistortion_in);
  // delete DSP class
  TubeDistortion_in->delete_instance(TubeDistortion_in);
  if (TubeDistortion_clip->activate_plugin !=0)
    TubeDistortion_clip->activate_plugin(false, TubeDistortion_clip);
  // delete DSP class
  TubeDistortion_clip->delete_instance(TubeDistortion_clip);
  if (TubeDistortion_out->activate_plugin !=0)
    TubeDistortion_out->activate_plugin(false, TubeDistortion_out);
  // delete DSP class
  TubeDistortion_out->delete_instance(TubeDistortion_out);
};

///////////////////////// PRIVATE CLASS  FUNCTIONS /////////////////////

void Gx_TubeDistortion_::init_dsp_(uint32_t rate)
{
  fSamplingFreq = rate;
  // samplerate check
  fact = fSamplingFreq/48000;
  if (fact>1) {
    smp.setup(fSamplingFreq, fact);
    fSamplingFreq = 48000;
  }
  // set values for internal ramping
  ramp_down_step = 32 * (256 * rate) / 48000; 
  ramp_up_step = ramp_down_step;
  ramp_down = ramp_down_step;
  ramp_up = 0.0;

  TubeDistortion_in->set_samplerate(fSamplingFreq, TubeDistortion_in); // init the DSP class
  TubeDistortion_clip->set_samplerate(fSamplingFreq, TubeDistortion_clip); // init the DSP class
  TubeDistortion_out->set_samplerate(fSamplingFreq, TubeDistortion_out); // init the DSP class
}

// connect the Ports used by the plug-in class
void Gx_TubeDistortion_::connect_(uint32_t port,void* data)
{
  switch ((PortIndex)port)
    {
    case EFFECTS_OUTPUT:
      output = static_cast<float*>(data);
      break;
    case EFFECTS_INPUT:
      input = static_cast<float*>(data);
      break;
    case BYPASS: 
      bypass = static_cast<float*>(data); // , 0.0, 0.0, 1.0, 1.0 
      break;
    default:
      break;
    }
}

void Gx_TubeDistortion_::activate_f()
{
  // allocate the internal DSP mem
  if (TubeDistortion_in->activate_plugin !=0)
    TubeDistortion_in->activate_plugin(true, TubeDistortion_in);
  if (TubeDistortion_clip->activate_plugin !=0)
    TubeDistortion_clip->activate_plugin(true, TubeDistortion_clip);
  if (TubeDistortion_out->activate_plugin !=0)
    TubeDistortion_out->activate_plugin(true, TubeDistortion_out);
}

void Gx_TubeDistortion_::clean_up()
{
  // delete the internal DSP mem
  if (TubeDistortion_in->activate_plugin !=0)
    TubeDistortion_in->activate_plugin(false, TubeDistortion_in);
  if (TubeDistortion_clip->activate_plugin !=0)
    TubeDistortion_clip->activate_plugin(false, TubeDistortion_clip);
  if (TubeDistortion_out->activate_plugin !=0)
    TubeDistortion_out->activate_plugin(false, TubeDistortion_out);
}

void Gx_TubeDistortion_::deactivate_f()
{
  // delete the internal DSP mem
  if (TubeDistortion_in->activate_plugin !=0)
    TubeDistortion_in->activate_plugin(false, TubeDistortion_in);
  if (TubeDistortion_clip->activate_plugin !=0)
    TubeDistortion_clip->activate_plugin(false, TubeDistortion_clip);
  if (TubeDistortion_out->activate_plugin !=0)
    TubeDistortion_out->activate_plugin(false, TubeDistortion_out);
}

void Gx_TubeDistortion_::run_dsp_(uint32_t n_samples)
{
  uint32_t ReCount = n_samples;
  if (fact>1) {
    ReCount = n_samples/fact ;
  }
  FAUSTFLOAT buf[ReCount];
  if (fact>1) {
    smp.down(ReCount, input, buf);
  } else {
    memcpy(buf, input, n_samples*sizeof(float));
  }
  // do inplace processing at default
  //memcpy(output, input, n_samples*sizeof(float));
  // check if bypass is pressed
  if (bypass_ != static_cast<uint32_t>(*(bypass))) {
    bypass_ = static_cast<uint32_t>(*(bypass));
    ramp_down = ramp_down_step;
    ramp_up = 0.0;    
    if (!bypass_) needs_ramp_down = true;
    else needs_ramp_up = true;
  }
  // check if raming is needed
  if (needs_ramp_down) {
    for (uint32_t i=0; i<ReCount; i++) {
      if (ramp_down >= 0.0) {
        --ramp_down;
      }
      //fprintf(stderr, "ramp_down = %f\n",(ramp_down /ramp_down_step));
      buf[i] *=  (ramp_down /ramp_down_step) ;
    }

    if (ramp_down <= 0.0) {
      // when ramped down, clear buffer from TubeDistortion class
      TubeDistortion_in->clear_state(TubeDistortion_in);
     //  TubeDistortion_clip->clear_state(TubeDistortion_clip);
      TubeDistortion_out->clear_state(TubeDistortion_out);
      needs_ramp_down = false;
      bypassed = true;
      //needs_ramp_up = true;
      //ramp_down = ramp_down_step;
    }
  } else if (needs_ramp_up) {
    bypassed = false;
    for (uint32_t i=0; i<ReCount; i++) {
      if (ramp_up <= ramp_up_step) {
        ++ramp_up;
      }
      //fprintf(stderr, "ramp_up = %f\n",(ramp_up /ramp_up_step));
      buf[i] *= (ramp_up /ramp_up_step);
    }
    if (ramp_up >= ramp_up_step) {
      needs_ramp_up = false;
     //ramp_up = 0.0;
    }
  }
  if (!bypassed) {
    TubeDistortion_in->mono_audio(static_cast<int>(ReCount), buf, buf, TubeDistortion_in);
    TubeDistortion_clip->mono_audio(static_cast<int>(ReCount), buf, buf, TubeDistortion_clip);
    TubeDistortion_out->mono_audio(static_cast<int>(ReCount), buf, buf, TubeDistortion_out);
  }
  if (fact>1) {
    smp.up(ReCount, buf, output);
  } else {
    memcpy(output, buf, n_samples*sizeof(float));
  }
}

void Gx_TubeDistortion_::connect_all__ports(uint32_t port, void* data)
{
  // connect the Ports used by the plug-in class
  connect_(port,data); 
  // connect the Ports used by the DSP class
  TubeDistortion_in->connect_ports(port,  data, TubeDistortion_in);
  TubeDistortion_clip->connect_ports(port,  data, TubeDistortion_clip);
  TubeDistortion_out->connect_ports(port,  data, TubeDistortion_out);
}

////////////////////// STATIC CLASS  FUNCTIONS  ////////////////////////

LV2_Handle 
Gx_TubeDistortion_::instantiate(const LV2_Descriptor* descriptor,
                            double rate, const char* bundle_path,
                            const LV2_Feature* const* features)
{
  // init the plug-in class
  Gx_TubeDistortion_ *self = new Gx_TubeDistortion_();
  if (!self) {
    return NULL;
  }

  self->init_dsp_((uint32_t)rate);

  return (LV2_Handle)self;
}

void Gx_TubeDistortion_::connect_port(LV2_Handle instance, 
                                   uint32_t port, void* data)
{
  // connect all ports
  static_cast<Gx_TubeDistortion_*>(instance)->connect_all__ports(port, data);
}

void Gx_TubeDistortion_::activate(LV2_Handle instance)
{
  // allocate needed mem
  static_cast<Gx_TubeDistortion_*>(instance)->activate_f();
}

void Gx_TubeDistortion_::run(LV2_Handle instance, uint32_t n_samples)
{
  // run dsp
  static_cast<Gx_TubeDistortion_*>(instance)->run_dsp_(n_samples);
}

void Gx_TubeDistortion_::deactivate(LV2_Handle instance)
{
  // free allocated mem
  static_cast<Gx_TubeDistortion_*>(instance)->deactivate_f();
}

void Gx_TubeDistortion_::cleanup(LV2_Handle instance)
{
  // well, clean up after us
  Gx_TubeDistortion_* self = static_cast<Gx_TubeDistortion_*>(instance);
  self->clean_up();
  delete self;
}

const LV2_Descriptor Gx_TubeDistortion_::descriptor =
{
  GXPLUGIN_URI "#_TubeDistortion_",
  Gx_TubeDistortion_::instantiate,
  Gx_TubeDistortion_::connect_port,
  Gx_TubeDistortion_::activate,
  Gx_TubeDistortion_::run,
  Gx_TubeDistortion_::deactivate,
  Gx_TubeDistortion_::cleanup,
  NULL
};


} // end namespace TubeDistortion

////////////////////////// LV2 SYMBOL EXPORT ///////////////////////////

extern "C"
LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
  switch (index)
    {
    case 0:
      return &TubeDistortion::Gx_TubeDistortion_::descriptor;
    default:
      return NULL;
    }
}

///////////////////////////// FIN //////////////////////////////////////
