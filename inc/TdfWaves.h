#ifndef TDFWAVES_H_
#define TDFWAVES_H_

#include "systemc.h"
#include "systemc-ams.h"

SCA_TDF_MODULE (Sine) 
{
public:
	
    sca_tdf::sca_de::sca_out<double> out; // output port
    
    Sine( sc_core::sc_module_name nm, double ampl_= 1.0, double freq_ = 1.0e3, sca_core::sca_time Tm_ = sca_core::sca_time(0.125, sc_core::SC_MS)) : out("out"), ampl(ampl_), freq(freq_), Tm(Tm_) 
    {

    }

    void set_attributes() 
    {
        set_timestep(Tm);
    }

    void processing() 
    {
        double t = get_time().to_seconds(); // actual time
        out.write( ampl * std::sin( 2.0 * M_PI * freq * t ) );
    }

private:
    double ampl; // amplitude
    double freq; // frequency
    sca_core::sca_time Tm; // module time step
};

#define WAVE_SIN1_AMPL                   3.0     
#define WAVE_SIN1_FREQ                   1000000 // 1MHz

#define WAVE_SIN2_AMPL                   3.0
#define WAVE_SIN2_FREQ                   3000000 // 1MHz

SCA_TDF_MODULE (TdfComplexWavGen)
{
public:
    sca_tdf::sca_de::sca_out<double> out; // output port

    sca_core::sca_time Tm; // module time step
    sc_signal<double> m_Sin1Out;
    sc_signal<double> m_Sin2Out;
    Sine SinWave1;
    Sine SinWave2;

    TdfComplexWavGen(sc_core::sc_module_name nm, sca_core::sca_time Tm_ = sca_core::sca_time(0.125, sc_core::SC_MS)) : out("out"), Tm(Tm_), 
        SinWave1("Sine1", WAVE_SIN1_AMPL, WAVE_SIN1_FREQ, Tm_),
        SinWave2("Sine2", WAVE_SIN2_AMPL, WAVE_SIN2_FREQ, Tm_)
    {
        SinWave1.out(m_Sin1Out);
        SinWave2.out(m_Sin2Out);
    }
    
    void set_attributes() 
    {
        set_timestep(Tm);
    }

    void processing() 
    {
        out.write( m_Sin1Out.read() + m_Sin2Out.read() );
    }
};

#endif