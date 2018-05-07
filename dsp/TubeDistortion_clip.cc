// generated from file './/TubeDistortion_clip.dsp' by dsp2cc:
// Code generated with Faust 0.9.90 (http://faust.grame.fr)

#include "12au7_table.h"

namespace TubeDistortion_clip {
#define MAX_UPSAMPLE 8

class SimpleResampler
{
private:
  Resampler r_up, r_down;
  int32_t m_fact;
  int32_t ratio_a;
  int32_t ratio_b;
  static uint32_t gcd (int32_t a, int32_t b);
public:
  SimpleResampler(): r_up(), r_down(), m_fact() {}
  void setup(int32_t sampleRate, uint32_t fact);
  int32_t up(int32_t count, float *input, float *output);
  void down(int32_t count, float *input, float *output);
  int32_t get_max_out_size(int32_t i_size)
  {
    return (i_size * ratio_b) / ratio_a + 1;
  }
};

uint32_t SimpleResampler::gcd (int32_t a, int32_t b)
{
  if (a == 0) return b;
  if (b == 0) return a;
  while (1)
    {
      if (a > b)
        {
          a = a % b;
          if (a == 0) return b;
          if (a == 1) return 1;
        }
      else
        {
          b = b % a;
          if (b == 0) return a;
          if (b == 1) return 1;
        }
    }
  return 1;
}

void SimpleResampler::setup(int32_t sampleRate, uint32_t factum)
{
  int32_t fact = static_cast<int32_t>(factum);
  int32_t d = gcd(sampleRate, sampleRate*fact);
  ratio_a = sampleRate / d;
  ratio_b = (sampleRate*fact) / d;

  assert(fact <= MAX_UPSAMPLE);
  m_fact = fact;
  const int32_t qual = 16; // resulting in a total delay of 2*qual (0.7ms @44100)
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
  // std::cout<<"SimpleResampler::setup "<<sampleRate<<" "<<fact<<std::endl;
}

int32_t SimpleResampler::up(int32_t count, float *input, float *output)
{
  r_up.inp_count = count;
  r_up.inp_data = input;
  int m = get_max_out_size(count);
  r_up.out_count = m;
  r_up.out_data = output;
  r_up.process();
  assert(r_up.inp_count == 0);
  assert(r_up.out_count <= 1);
  r_down.inp_count = m - r_up.out_count;
  return r_down.inp_count;
}

void SimpleResampler::down(int32_t count, float *input, float *output)
{
  r_down.inp_count = count * m_fact;
  r_down.inp_data = input;
  r_down.out_count = count+1; // +1 == trick to drain input
  r_down.out_data = output;
  r_down.process();
  assert(r_down.inp_count == 0);
  assert(r_down.out_count == 1);
}

class Dsp: public PluginLV2 {
private:
	SimpleResampler res;
	uint32_t fSamplingFreq;

	void connect(uint32_t port,void* data);
	void clear_state_f();
	void init(uint32_t samplingFreq);
	void compute(int count, FAUSTFLOAT *input0, FAUSTFLOAT *output0);

	static void clear_state_f_static(PluginLV2*);
	static void init_static(uint32_t samplingFreq, PluginLV2*);
	static void compute_static(int count, FAUSTFLOAT *input0, FAUSTFLOAT *output0, PluginLV2*);
	static void del_instance(PluginLV2 *p);
	static void connect_static(uint32_t port,void* data, PluginLV2 *p);
public:
	Dsp();
	~Dsp();
};



Dsp::Dsp()
	: PluginLV2() {
	version = PLUGINLV2_VERSION;
	id = "TubeDistortion_clip";
	name = N_("TubeDistortion_clip");
	mono_audio = compute_static;
	stereo_audio = 0;
	set_samplerate = init_static;
	activate_plugin = 0;
	connect_ports = connect_static;
	clear_state = clear_state_f_static;
	delete_instance = del_instance;
}

Dsp::~Dsp() {
}

inline void Dsp::clear_state_f()
{

}

void Dsp::clear_state_f_static(PluginLV2 *p)
{
	static_cast<Dsp*>(p)->clear_state_f();
}

inline void Dsp::init(uint32_t samplingFreq)
{
	fSamplingFreq = samplingFreq;
    res.setup(samplingFreq,4);
}

void Dsp::init_static(uint32_t samplingFreq, PluginLV2 *p)
{
	static_cast<Dsp*>(p)->init(samplingFreq);
}

void always_inline Dsp::compute(int count, FAUSTFLOAT *input0, FAUSTFLOAT *output0)
{
    int rescount = res.get_max_out_size(count);
    float buf[rescount];
    res.up(count,input0,buf);
	for (int i=0; i<rescount; i++) {
        double fTemp0 = buf[i];
		buf[i] = (FAUSTFLOAT)tubeclip(fTemp0);
	}
    res.down(count,buf,output0);
}

void __rt_func Dsp::compute_static(int count, FAUSTFLOAT *input0, FAUSTFLOAT *output0, PluginLV2 *p)
{
	static_cast<Dsp*>(p)->compute(count, input0, output0);
}


void Dsp::connect(uint32_t port,void* data)
{
	switch ((PortIndex)port)
	{
	default:
		break;
	}
}

void Dsp::connect_static(uint32_t port,void* data, PluginLV2 *p)
{
	static_cast<Dsp*>(p)->connect(port, data);
}


PluginLV2 *plugin() {
	return new Dsp();
}

void Dsp::del_instance(PluginLV2 *p)
{
	delete static_cast<Dsp*>(p);
}

/*
typedef enum
{
} PortIndex;
*/

} // end namespace TubeDistortion_clip
