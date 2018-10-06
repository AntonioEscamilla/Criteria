/*
  ==============================================================================

    Signal.h
    Created: 1 Dec 2015 4:26:25pm
    Author:  Juan Londoño

  ==============================================================================
*/

#ifndef SIGNAL_H_INCLUDED
#define SIGNAL_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <math.h>

class Signal {
public:
    Signal();
    ~Signal();
    
    /**seno=1, cuadrada=2, diente=3, triangular=4, LinSweep=5, LogSweep=6, white=7**/
    void setTipo(int tipo);
    
    /**Duracion del Sweep en segundos. La duración por default es de 1s**/
    void setDuracionSweep(float NuevaDuracion);
    
    /**'startSample' es la variable que controla la fase (). Freq2 es solo para las señales tipo 'Sweep'**/
    const float *getReadSamples(float freq1, float freq2, int startSample, int numSamplesExpected, double SampleRate);
    
    
private:
    double      sRate;
    //float desfase;//Para guardar el desfase que tenia la señal
    int         tipoS;
    float       duracionSweepS;
    float f1,f2; //f1
    double t;
    Random rand;
    AudioSampleBuffer *signalBuffer;
};



#endif  // SIGNAL_H_INCLUDED
