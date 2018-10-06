/*
  ==============================================================================

    ventanaMedicion.h
    Created: 25 Nov 2015 8:14:37pm
    Author:  Juan Londoño

  ==============================================================================
*/

#include "MainComponent.h"

#ifndef VENTANAMEDICION_H_INCLUDED
#define VENTANAMEDICION_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
#include "Signal.h"
#include "fftw3.h" //Librería FFTW
using namespace drow;
static const String medText=CharPointer_UTF8 ("Iniciar medici\xc3\xb3n");

class ventanaMedicion : public Component, public AudioIODeviceCallback, public ButtonListener, public ChangeBroadcaster
{
    
public:
    ventanaMedicion();
    ~ventanaMedicion();
    
    void paint (Graphics& g);
    
    void buttonClicked(Button* buttonThatWasClicked);
    void audioDeviceIOCallback(const float** inputData,int InputChannels,float** outputData,int OutputChannels,int numSamples);
    void audioDeviceAboutToStart (AudioIODevice* device){
        //transportSource.prepareToPlay(device->getCurrentBufferSizeSamples(),device->getCurrentSampleRate());
    }
    void audioDeviceStopped(){
    };
    
    void definirFFT(int fftSize);
    
private:
    AudioDeviceManager&                 deviceManager;
    
    OwnedArray<Component>               components;
    
    template <typename ComponentType>
    ComponentType* addToList (ComponentType* newComp)
    {
        components.add (newComp);
        addAndMakeVisible (newComp);
        return newComp;
    }
    
    Slider                              gainsweep;
    TextButton                          *testbtn;
    TextButton                          *startbtn;
    TextButton                          *deconvbtn;
    ScopedPointer<TextEditor>           duracionsweep;
    ScopedPointer<TextEditor>           duracionRTestimado;
    ScopedPointer<ComboBox>             LinLog;
    
    //Meter
    TimeSliceThread                     meterThread;
    SegmentedMeter                      meterL;
    
    float                               amplitude;
    float                               T; //Duración del sweep
    float                               phase;
    float                               valorRTestimado;
    int                                 sweepSizeSamples;
    int                                 fftSize;
    unsigned int                        scount;
    bool                                deconvolucion;
    bool                                outputEnabled;
    ScopedPointer<Signal>               mySweeps;
    ScopedPointer<AudioSampleBuffer>    amplitudEnvelope;
    int                                 kp,Nt;
        
    const float                         f1;//Frecuencia de Inicio de Sweep (inicializadas en la llamada al constructor)
    const float                         f2;//Frecuencia Final del Sweep
    
    //FFT
    fftw_complex                        *x, *X, *y, *Y, *h, *H;
    fftw_plan                           transformadaX;
    fftw_plan                           transformadaY;
    fftw_plan                           transformadah;
    
};

class ventanaConfiguracionesAudio : public Component, public ButtonListener, public ChangeListener
{
public:
    ventanaConfiguracionesAudio();
    ~ventanaConfiguracionesAudio();
    
    class JUCE_API  Listener{
    public:
        /** Destructor. */
        virtual ~Listener()  {}
        
        /** Called when the button is clicked. */
        virtual void buttonClicked (ventanaConfiguracionesAudio*) = 0;
    };
    
    void addListener (Listener* newListener){
        buttonExtListeners.add (newListener);
    }
    
    void paint (Graphics& g){}
    void buttonClicked(Button* buttonThatWasClicked);
    
    void changeListenerCallback (ChangeBroadcaster*);
    
private:
    AudioDeviceManager& deviceManager;
    AudioDeviceSelectorComponent settingsComp;

    TextButton *botonSiguiente;
    
    ListenerList<Listener>  buttonExtListeners;
};

#endif  // VENTANAMEDICION_H_INCLUDED
