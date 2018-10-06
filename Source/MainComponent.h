/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/


#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

//Incluir un header general
#include "../JuceLibraryCode/JuceHeader.h"
#include <fstream>
#include "matPlotsHeader.h"
#include "PlotComponent.h"
#include "parameterComponent.h"
#include "FilterBank.h"
#include "ParametroTemporal.h"
#include "ParametroEnergetico.h"
#include "ventanaMedicion.h"
#include "AboutWindow.h"
using namespace drow;

//Variables Globales
extern juce::AudioDeviceManager::AudioDeviceSetup newAudioSetup;
extern double sampleRate;
extern AudioSampleBuffer *IRcompleta;
extern TextButton *cancelMed;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
 
*/

static const char* botonTextTime[6] = {"EDT[s]","RT10[s]", "RT20[s]", "RT30[s]"};
static const char* botonTextEnergy[7] = {"C50[dB]","C80[dB]","STe[dB]", "STl[dB]", "D50[-]", "Ts[ms]", "SNR[dB]"};
static const char* bandasTercioOut[30] = {"25","31.5","40","50","63","80","100","125","160","200","250","315","400","500","630","800","1k","1.25k","1.6k","2k","2.5k","3.15k","4k","5k","6k","8k","10k","12.5k","16k","20k"};
static int groupdelay9[9]={2000, 1000, 521, 257, 128, 64, 32, 16, 8};
static int groupdelay10[10]={2180, 1090, 557, 277, 140, 69, 35, 17, 8, 5};
static int groupdelay29[29]={8463, 6176, 4903, 3896, 3098, 2457, 1977, 1557, 1239, 988, 786, 625, 490, 393, 312, 247, 196, 156, 124, 99, 78, 62, 49, 39, 31, 24, 19, 15, 11};
static int groupdelay30[30]={9900, 6720, 5367, 4306, 3369, 2710, 2151, 1700, 1354, 1075, 850, 667, 537, 427, 340, 270, 214, 170, 135, 107, 85, 67, 53, 42, 33, 26, 21, 16, 13, 9};


class MainContentComponent   : public Component, public MenuBarModel, public ventanaConfiguracionesAudio::Listener, public ButtonListener, private ChangeListener//, public botonTabCerrar::Listener
{
public:
    //==============================================================================
    //Básicos
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();
    
    //==============================================================================
    static AudioDeviceManager& getAudioDeviceManagerCompartido();
    
    void importarIR();
    void filtrarIR();
    void encontrarParametrosTemporales();
    void encontrarParametrosEnergeticos();
    void encontrarParametros();
    void encontrart0Posi(AudioSampleBuffer *entrada, int &t0, int &posi);
    void exportarParametros();
    void exportarWav();
    void audioDownSamplig(AudioSampleBuffer* input, Buffer* output,int downSampligFactor, int audioFileProportion);
    
    void buttonClicked (ventanaConfiguracionesAudio*);
    void buttonClicked (Button* buttonThatWasClicked);
    void changeListenerCallback (ChangeBroadcaster* source);
    //void medicionFinalizada (ventanaMedicion*);
    
    //==============================================================================
    //Menu Bar
    StringArray getMenuBarNames();
    PopupMenu getMenuForIndex(int index,const String& name);
    enum MenuIDs{medicion=1,importar,exParametros,octava,tercio, about};
    void menuItemSelected(int menuID, int index);
    //==============================================================================
   
    
    
private:
    
    //==============================================================================
    //Variables MenuBar
    ScopedPointer<MenuBarComponent> menuBar;
    
    //==============================================================================
    ScopedPointer<AudioSampleBuffer>        IRrecortada;
    OwnedArray<Buffer>                      filteredAudioArray;
    OwnedArray<Buffer>                      Temporales;
    OwnedArray<Buffer>                      Xi; //Relacionado al Rcuadrado de los temporales Xi=1000(1-Rcuadrado)
    OwnedArray<Buffer>                      Energeticos;
    
    int                     numeroBandas;
    int                     LongitudIRrecortada;
    int                     t0,posi;
    int                     LongitudIRcola;
    bool                    IRchanged;
    bool                    validIR;
    enum FilterRes{
        octavas=1,
        tercios
    }filterRes;
    
    //Para Pintar
    ScopedPointer<Buffer>                   bufferWaveform;
    ScopedPointer<TabbedComponent>          tabsComponent;
    ScopedPointer<GroupComponent>           info;
    double                                  fsMostrar;
    
    //Para leer wav
    AudioFormatReader                       *audioFormatReader;
    File                                    soundfile;
    AudioFormatManager                      audioFormatManager;
    ScopedPointer<AudioFormatReaderSource>  audioFormatReaderSource;
    AudioDeviceManager                      &audioDeviceManager;
    AudioTransportSource                    audioTransportSource;
    AudioSourcePlayer                       audioSourcePlayer;
    TimeSliceThread                         readAheadThread;
    
    ventanaConfiguracionesAudio             ventanaConf;
    ventanaMedicion                         ventanaMedicion;
    Component::SafePointer<AboutWindow>     aboutWindow;
    TextButton                              *exportarIR;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
#endif  // MAINCOMPONENT_H_INCLUDED

