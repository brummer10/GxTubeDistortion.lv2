// generated from file './/TubeDistortion_in.dsp' by dsp2cc:
// Code generated with Faust 0.9.90 (http://faust.grame.fr)


namespace TubeDistortion_in {

class Dsp: public PluginLV2 {
private:
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
	double 	fRec1[5];
	double 	fConst19;
	double 	fConst20;
	double 	fConst21;
	double 	fConst22;
	double 	fConst23;
	double 	fConst24;
	double 	fConst25;
	double 	fConst26;
	double 	fConst27;

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
	for (int i=0; i<5; i++) fRec1[i] = 0;
}

void Dsp::clear_state_f_static(PluginLV2 *p)
{
	static_cast<Dsp*>(p)->clear_state_f();
}

inline void Dsp::init(uint32_t samplingFreq)
{
	fSamplingFreq = samplingFreq;
	fConst0 = double(min(1.92e+05, max(1.0, (double)fSamplingFreq)));
	fConst1 = (6.97215184175589e-20 * fConst0);
	fConst2 = (1.04891720676574e-16 + (fConst0 * (1.06140218972973e-17 + fConst1)));
	fConst3 = (1.86046677485506e-19 * fConst0);
	fConst4 = (2.41088750774286e-13 + (fConst0 * (9.84807267763385e-15 + (fConst0 * (1.11764606899178e-16 + fConst3)))));
	fConst5 = (3.57343297243374e-18 * fConst0);
	fConst6 = ((fConst0 * (9.54671774827767e-18 + (fConst0 * (fConst5 - 3.82800272757065e-18)))) - 9.19872311650736e-17);
	fConst7 = (9.20403656138857e-21 * fConst0);
	fConst8 = ((fConst0 * (8.65280408509776e-19 - fConst7)) - 2.02446447202099e-17);
	fConst9 = faustpower<2>(fConst0);
	fConst10 = (1.39443036835118e-19 * fConst9);
	fConst11 = (fConst10 - 2.09783441353147e-16);
	fConst12 = (7.44186709942025e-19 * fConst0);
	fConst13 = ((fConst9 * (2.23529213798356e-16 - fConst12)) - 4.82177501548572e-13);
	fConst14 = (2.09783441353147e-16 - fConst10);
	fConst15 = (4.82177501548572e-13 + (fConst9 * (0 - (2.23529213798356e-16 + fConst12))));
	fConst16 = ((fConst0 * (1.06140218972973e-17 - fConst1)) - 1.04891720676574e-16);
	fConst17 = ((fConst0 * (9.84807267763385e-15 + (fConst0 * (fConst3 - 1.11764606899178e-16)))) - 2.41088750774286e-13);
	fConst18 = ((1.11628006491304e-18 * fConst9) - 1.96961453552677e-14);
	fConst19 = (1.4293731889735e-17 * fConst0);
	fConst20 = ((fConst9 * (7.6560054551413e-18 - fConst19)) - 1.83974462330147e-16);
	fConst21 = (1.84080731227771e-20 * fConst9);
	fConst22 = (fConst21 - 4.04892894404199e-17);
	fConst23 = ((2.14405978346025e-17 * fConst9) - 1.90934354965553e-17);
	fConst24 = (1.83974462330147e-16 + (fConst9 * (0 - (7.6560054551413e-18 + fConst19))));
	fConst25 = (4.04892894404199e-17 - fConst21);
	fConst26 = (9.19872311650736e-17 + (fConst0 * (9.54671774827767e-18 + (fConst0 * (3.82800272757065e-18 + fConst5)))));
	fConst27 = (2.02446447202099e-17 + (fConst0 * (8.65280408509776e-19 + fConst7)));
	clear_state_f();
}

void Dsp::init_static(uint32_t samplingFreq, PluginLV2 *p)
{
	static_cast<Dsp*>(p)->init(samplingFreq);
}

void always_inline Dsp::compute(int count, FAUSTFLOAT *input0, FAUSTFLOAT *output0)
{
#define fslider0 (*fslider0_)
	double 	fSlow0 = (0.007000000000000006 * double(fslider0));
	for (int i=0; i<count; i++) {
		fRec0[0] = (fSlow0 + (0.993 * fRec0[1]));
		double fTemp0 = (2.42094669442371e-13 + (fConst0 * (fConst4 + (fConst2 * fRec0[0]))));
		fRec1[0] = ((double)input0[i] - (((fRec1[2] * (1.45256801665422e-12 + (fConst9 * (fConst18 - (2.12280437945946e-17 * fRec0[0]))))) + ((fRec1[4] * (2.42094669442371e-13 + (fConst0 * (fConst17 + (fConst16 * fRec0[0]))))) + ((fRec1[1] * (9.68378677769483e-13 + (fConst0 * (fConst15 + (fConst14 * fRec0[0]))))) + (fRec1[3] * (9.68378677769483e-13 + (fConst0 * (fConst13 + (fConst11 * fRec0[0])))))))) / fTemp0));
		output0[i] = (FAUSTFLOAT)(fConst0 * ((((((fRec1[0] * (fConst27 + (fConst26 * fRec0[0]))) + (fRec1[1] * (fConst25 + (fConst24 * fRec0[0])))) + (fConst0 * (fRec1[2] * ((fConst23 * fRec0[0]) - 1.73056081701955e-18)))) + (fRec1[3] * (fConst22 + (fConst20 * fRec0[0])))) + (fRec1[4] * (fConst8 + (fConst6 * fRec0[0])))) / fTemp0));
		// post processing
		for (int i=4; i>0; i--) fRec1[i] = fRec1[i-1];
		fRec0[1] = fRec0[0];
	}
#undef fslider0
}

void __rt_func Dsp::compute_static(int count, FAUSTFLOAT *input0, FAUSTFLOAT *output0, PluginLV2 *p)
{
	static_cast<Dsp*>(p)->compute(count, input0, output0);
}


void Dsp::connect(uint32_t port,void* data)
{
	switch ((PortIndex)port)
	{
	case INPUT_: 
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
} PortIndex;
*/

} // end namespace TubeDistortion_in
