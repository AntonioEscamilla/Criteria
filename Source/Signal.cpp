/*
  ==============================================================================

    Signal.cpp
    Created: 1 Dec 2015 4:26:26pm
    Author:  Juan Londoño

  ==============================================================================
*/

#include "Signal.h"

Signal::Signal(){
    tipoS=1;
    sRate=0.0;
    signalBuffer=new AudioSampleBuffer(1,1);
    duracionSweepS=1;
    f1=1000;
    f2=21000;
    t=0.0l;
}
Signal::~Signal(){
    deleteAndZero(signalBuffer);
}
void Signal::setTipo(int tipo){
    tipoS=tipo;
}

void Signal::setDuracionSweep(float duracion){
    if (duracion>0){
        duracionSweepS=duracion;
    }
}


const float *Signal::getReadSamples(float freq1, float freq2, int startSample, int numSamplesExpected, double SampleRate){
    signalBuffer->clear();
    signalBuffer->setSize(1, numSamplesExpected);
    t=0.0l;
    f1=freq1;
    f2=freq2;
    switch (tipoS) {
        case 1://Seno
            for (int i=0; i<numSamplesExpected; ++i) {
                t=fmod(1.0/SampleRate*(i+startSample), 1);
                //t=1.0/SampleRate*(i+startSample);
                signalBuffer->getWritePointer(0)[i]=sin(2*float_Pi*f1*t);
            }
            break;
        case 2://Cuadrada
            for (int i=0; i<numSamplesExpected; ++i) {
                t=fmod(1.0/SampleRate*(i+startSample), 1);
                signalBuffer->getWritePointer(0)[i]=powf(-1, floorf(2*f1*t));
            }
            break;
        case 3://Diente de sierra
            for (int i=0; i<numSamplesExpected; ++i) {
                t=fmod(1.0/SampleRate*(i+startSample), 1);
                signalBuffer->getWritePointer(0)[i]=2*(freq1*t-floorf(f1*t-0.5)-1);
            }
            break;
        case 4://Triangular
            for (int i=0; i<numSamplesExpected; ++i) {
                t=fmod(1.0/SampleRate*(i+startSample), 1);
                signalBuffer->getWritePointer(0)[i]=2*std::abs(2*floorf(f1*t - 0.25) - 2*f1*t + 1.5) - 1.0;
            }
            break;
        case 5://LinSweep
            for (int i=0; i<numSamplesExpected; ++i) {
                t=fmod(1.0/SampleRate*(i+startSample), duracionSweepS);
                signalBuffer->getWritePointer(0)[i]=sin(fmod((2.0f*float_Pi*f1*t) + (((2.0f*float_Pi*(f2-f1))*(1/((duracionSweepS)*1)))*(t*t)/2), 2.0f*float_Pi));
            }
            break;
        case 6://LogSweep
            for (int i=0; i<numSamplesExpected; ++i) {
                t=fmod(1.0/SampleRate*(i+startSample), duracionSweepS);
                signalBuffer->getWritePointer(0)[i]=sin(fmod(((2 * float_Pi*f1*((duracionSweepS)*1))/(log(f2/f1)))*((exp((t/((duracionSweepS)*1))*(log(f2/f1))))-1), 2.0f * float_Pi));
                
            }
            break;
        case 7://White noise
            for (int i=0; i<numSamplesExpected; ++i) {
                signalBuffer->getWritePointer(0)[i]=(-1+2*rand.nextFloat());
            }
            break;
            
        default:
            break;
    }

    return signalBuffer->getReadPointer(0);
}