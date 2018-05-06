// generated from file './/TubeDistortion.dsp' by dsp2cc:
// Code generated with Faust 0.9.90 (http://faust.grame.fr)

#include "12au7_table.h"
#include "resampler.cc"   // define struct PluginLV2
#include "resampler-table.cc"   // define struct PluginLV2
#include "zita-resampler/resampler.h"
#include <assert.h>

namespace TubeDistortion {
#define MAX_UPSAMPLE 8

class SimpleResampler
{
private:
  Resampler r_up, r_down;
  int32_t m_fact;
  int32_t ratio_a;
  int32_t ratio_b;
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
	FAUSTFLOAT 	fslider0;
	FAUSTFLOAT	*fslider0_;
	double 	fRec0[2];
	double 	fConst0;
	double 	fConst1;
	double 	fConst2;
	double 	fConst3;
	double 	fConst4;
	double 	fConst5;
	double 	fConst6;
	double 	fConst7;
	FAUSTFLOAT 	fslider1;
	FAUSTFLOAT	*fslider1_;
	double 	fRec2[2];
	double 	fConst8;
	double 	fConst9;
	double 	fConst10;
	double 	fConst11;
	double 	fConst12;
	double 	fConst13;
	double 	fConst14;
	double 	fConst15;
	double 	fConst16;
	double 	fConst17;
	double 	fConst18;
	double 	fConst19;
	double 	fConst20;
	double 	fConst21;
	double 	fConst22;
	double 	fConst23;
	double 	fConst24;
	double 	fRec3[5];
	double 	fConst25;
	double 	fConst26;
	double 	fConst27;
	double 	fConst28;
	double 	fConst29;
	double 	fConst30;
	double 	fConst31;
	double 	fConst32;
	double 	fConst33;
	double 	fRec1[3];
	double 	fConst34;
	double 	fConst35;

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
	id = "TubeDistortion_in";
	name = N_("TubeDistortion_in");
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
	for (int i=0; i<2; i++) fRec0[i] = 0;
	for (int i=0; i<2; i++) fRec2[i] = 0;
	for (int i=0; i<5; i++) fRec3[i] = 0;
	for (int i=0; i<3; i++) fRec1[i] = 0;
}

void Dsp::clear_state_f_static(PluginLV2 *p)
{
	static_cast<Dsp*>(p)->clear_state_f();
}

inline void Dsp::init(uint32_t samplingFreq)
{
	fSamplingFreq = samplingFreq*2;
    res.setup(samplingFreq,2);
	fConst0 = double(min(1.92e+05, max(1.0, (double)fSamplingFreq)));
	fConst1 = ((4.33109552574987e-10 * fConst0) - 4.33109552574987e-10);
	fConst2 = (4.33116319823797e-10 * fConst0);
	fConst3 = (4.33116319823797e-08 + (fConst0 * (fConst2 - 4.37490794654017e-08)));
	fConst4 = faustpower<2>(fConst0);
	fConst5 = (8.66232639647594e-08 - (8.66232639647594e-10 * fConst4));
	fConst6 = (4.33116319823797e-08 + (fConst0 * (4.37490794654017e-08 + fConst2)));
	fConst7 = (1.0 / fConst6);
	fConst8 = (6.97215184175589e-20 * fConst0);
	fConst9 = (1.04891720676574e-16 + (fConst0 * (1.06140218972973e-17 + fConst8)));
	fConst10 = (1.86046677485506e-19 * fConst0);
	fConst11 = (2.41088750774286e-13 + (fConst0 * (9.84807267763385e-15 + (fConst0 * (1.11764606899178e-16 + fConst10)))));
	fConst12 = (3.57343297243374e-18 * fConst0);
	fConst13 = ((fConst0 * (9.54671774827767e-18 + (fConst0 * (fConst12 - 3.82800272757065e-18)))) - 9.19872311650736e-17);
	fConst14 = (9.20403656138857e-21 * fConst0);
	fConst15 = ((fConst0 * (8.65280408509776e-19 - fConst14)) - 2.02446447202099e-17);
	fConst16 = (1.39443036835118e-19 * fConst4);
	fConst17 = (fConst16 - 2.09783441353147e-16);
	fConst18 = (7.44186709942025e-19 * fConst0);
	fConst19 = ((fConst4 * (2.23529213798356e-16 - fConst18)) - 4.82177501548572e-13);
	fConst20 = (2.09783441353147e-16 - fConst16);
	fConst21 = (4.82177501548572e-13 + (fConst4 * (0 - (2.23529213798356e-16 + fConst18))));
	fConst22 = ((fConst0 * (1.06140218972973e-17 - fConst8)) - 1.04891720676574e-16);
	fConst23 = ((fConst0 * (9.84807267763385e-15 + (fConst0 * (fConst10 - 1.11764606899178e-16)))) - 2.41088750774286e-13);
	fConst24 = ((1.11628006491304e-18 * fConst4) - 1.96961453552677e-14);
	fConst25 = (1.4293731889735e-17 * fConst0);
	fConst26 = ((fConst4 * (7.6560054551413e-18 - fConst25)) - 1.83974462330147e-16);
	fConst27 = (1.84080731227771e-20 * fConst4);
	fConst28 = (fConst27 - 4.04892894404199e-17);
	fConst29 = ((2.14405978346025e-17 * fConst4) - 1.90934354965553e-17);
	fConst30 = (1.83974462330147e-16 + (fConst4 * (0 - (7.6560054551413e-18 + fConst25))));
	fConst31 = (4.04892894404199e-17 - fConst27);
	fConst32 = (9.19872311650736e-17 + (fConst0 * (9.54671774827767e-18 + (fConst0 * (3.82800272757065e-18 + fConst12)))));
	fConst33 = (2.02446447202099e-17 + (fConst0 * (8.65280408509776e-19 + fConst14)));
	fConst34 = (4.33109552574987e-10 * (1.0 + fConst0));
	fConst35 = (fConst0 / fConst6);
	clear_state_f();
}

void Dsp::init_static(uint32_t samplingFreq, PluginLV2 *p)
{
	static_cast<Dsp*>(p)->init(samplingFreq);
}

void always_inline Dsp::compute(int count, FAUSTFLOAT *input0, FAUSTFLOAT *output0)
{
#define fslider0 (*fslider0_)
#define fslider1 (*fslider1_)
	double 	fSlow0 = (0.007000000000000006 * double(fslider0));
	double 	fSlow1 = (0.007000000000000006 * double(fslider1));
    int rescount = res.get_max_out_size(count);
    float buf[rescount];
    res.up(count,input0,buf);
	for (int i=0; i<rescount; i++) {
		fRec0[0] = (fSlow0 + (0.993 * fRec0[1]));
		fRec2[0] = (fSlow1 + (0.993 * fRec2[1]));
		double fTemp0 = (2.42094669442371e-13 + (fConst0 * (fConst11 + (fConst9 * fRec2[0]))));
		fRec3[0] = ((double)buf[i] - (((fRec3[2] * (1.45256801665422e-12 + (fConst4 * (fConst24 - (2.12280437945946e-17 * fRec2[0]))))) + ((fRec3[4] * (2.42094669442371e-13 + (fConst0 * (fConst23 + (fConst22 * fRec2[0]))))) + ((fRec3[1] * (9.68378677769483e-13 + (fConst0 * (fConst21 + (fConst20 * fRec2[0]))))) + (fRec3[3] * (9.68378677769483e-13 + (fConst0 * (fConst19 + (fConst17 * fRec2[0])))))))) / fTemp0));
		fRec1[0] = (tubeclip((fConst0 * ((((((fRec3[0] * (fConst33 + (fConst32 * fRec2[0]))) + (fRec3[1] * (fConst31 + (fConst30 * fRec2[0])))) + (fConst0 * (fRec3[2] * ((fConst29 * fRec2[0]) - 1.73056081701955e-18)))) + (fRec3[3] * (fConst28 + (fConst26 * fRec2[0])))) + (fRec3[4] * (fConst15 + (fConst13 * fRec2[0])))) / fTemp0))) - (fConst7 * ((fConst5 * fRec1[1]) + (fConst3 * fRec1[2]))));
		//output0[i] = fTemp1;
		//for (int i=4; i>0; i--) fRec3[i] = fRec3[i-1];
		//fRec2[1] = fRec2[0];
		//fRec0[1] = fRec0[0];
   // }
   // res.up(count,output0,buf);
   // for (int i=0; i<rescount; i++) {
   //     double fTemp3 = buf[i];
    //    double fTemp4 = tubeclip(fTemp3);
   //     buf[i] = fTemp4;
   // }
   // res.down(count,buf,output0);
    //for (int i=0; i<count; i++) {
	//	fRec0[0] = (fSlow0 + (0.993 * fRec0[1]));
	//	fRec1[0] = output0[i];
		buf[i] = (FAUSTFLOAT)(fConst35 * (((fRec1[0] * (4.33109552574987e-12 + (fConst34 * fRec0[0]))) + (fConst0 * (fRec1[1] * (0 - (8.66219105149974e-10 * fRec0[0]))))) + (fRec1[2] * ((fConst1 * fRec0[0]) - 4.33109552574987e-12))));
		// post processing
		for (int i=4; i>0; i--) fRec3[i] = fRec3[i-1];
		fRec2[1] = fRec2[0];
		fRec0[1] = fRec0[0];
		fRec1[2] = fRec1[1]; fRec1[1] = fRec1[0];
		fRec0[1] = fRec0[0];
	}
    res.down(count,buf,output0);
#undef fslider0
#undef fslider1
}

void __rt_func Dsp::compute_static(int count, FAUSTFLOAT *input0, FAUSTFLOAT *output0, PluginLV2 *p)
{
	static_cast<Dsp*>(p)->compute(count, input0, output0);
}


void Dsp::connect(uint32_t port,void* data)
{
	switch ((PortIndex)port)
	{
	case INPUT: 
		fslider1_ = (float*)data; // , 0.5, 0.0, 1.0, 0.01 
		break;
	case OUTPUT: 
		fslider0_ = (float*)data; // , 0.5, 0.0, 1.0, 0.01 
		break;
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
   INPUT, 
   OUTPUT, 
} PortIndex;
*/

} // end namespace TubeDistortion
