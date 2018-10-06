/*
  ==============================================================================

    ventanaMedicion.cpp
    Created: 25 Nov 2015 8:14:37pm
    Author:  Juan Londoño

  ==============================================================================
*/

#include "ventanaMedicion.h"

//==============================================================================
//Ventana Medición
ventanaMedicion::ventanaMedicion() : deviceManager(MainContentComponent::getAudioDeviceManagerCompartido()), meterThread ("Meter Thread"), f1(10),f2(22000)
{
    //Medición
    int offsetX=0;
    GroupComponent *groupfuente = addToList (new GroupComponent ("group", "Fuente"));
    groupfuente->setBounds (5+offsetX, 5, 250, 300);
    groupfuente->setColour(01, Colours::white);
    GroupComponent *groupsenal = addToList (new GroupComponent ("group", "Barrido/Sweep"));
    groupsenal->setBounds (15+offsetX, 20, 230, 125);
    GroupComponent *groupgain = addToList (new GroupComponent ("group", "Nivel"));
    groupgain->setBounds (15+offsetX, 150, 230, 145);
    
    addAndMakeVisible(testbtn=new TextButton("Test"));
    testbtn->setBounds(100+50+offsetX,210,70,25);
    testbtn->addListener(this);
    
    addAndMakeVisible(startbtn=new TextButton(medText));
    startbtn->setBounds(320/2-140/2-30+offsetX,420*75/100,140,30);
    startbtn->addListener(this);
    
    deconvbtn=new TextButton();
    deconvbtn->addListener(this);
    
    addAndMakeVisible(gainsweep);
    gainsweep.setBounds(100+offsetX,165,60,108);
    gainsweep.setSliderStyle(Slider::LinearVertical);
    gainsweep.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    gainsweep.setRange(0.0,1.0,0.001);
    gainsweep.setDoubleClickReturnValue (true, 0.5);
    gainsweep.setValue (0.5);
    
    
    addAndMakeVisible(LinLog=new ComboBox("combobox"));
    LinLog->setBounds(115+offsetX,48,100,18);
    LinLog->setEditableText (false);
    LinLog->setJustificationType (Justification::centredLeft);
    LinLog->setTextWhenNothingSelected (String::empty);
    LinLog->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    LinLog->addItem (TRANS("Lineal"), 1);
    LinLog->addItem (TRANS(CharPointer_UTF8 ("Logar\xc3\xadtmico ")), 2);
    LinLog->setText(CharPointer_UTF8 ("Logar\xc3\xadtmico "));
    
    addAndMakeVisible(duracionsweep = new TextEditor("Tiempo T"));
    duracionsweep->setBounds(115+offsetX,77,100,18);
    duracionsweep->setMultiLine (false);
    duracionsweep->setReturnKeyStartsNewLine (false);
    duracionsweep->setReadOnly (false);
    duracionsweep->setScrollbarsShown (true);
    duracionsweep->setCaretVisible (true);
    duracionsweep->setPopupMenuEnabled (true);
    duracionsweep->setText (TRANS("1"));
    
    addAndMakeVisible(duracionRTestimado = new TextEditor("RT estimado"));
    duracionRTestimado->setBounds(115+offsetX,106,100,18);
    duracionRTestimado->setMultiLine (false);
    duracionRTestimado->setReturnKeyStartsNewLine (false);
    duracionRTestimado->setReadOnly (false);
    duracionRTestimado->setText ("2");
    duracionRTestimado->setEnabled(true);
    
    amplitude=0.0;
    phase=0.0;
    scount=0;
    T=duracionsweep->getText().getFloatValue();//Duración del sweep
    valorRTestimado=duracionRTestimado->getText().getDoubleValue();
    deconvolucion=false;
    outputEnabled=false;
    mySweeps=new Signal();
    mySweeps->setDuracionSweep(T);
    mySweeps->setTipo(6);
    sweepSizeSamples=ceil(T*sampleRate);
    
    definirFFT(1);
    
    addAndMakeVisible (&meterL);
    meterThread.addTimeSliceClient (&meterL);
    meterThread.startThread(1);
    
}

ventanaMedicion::~ventanaMedicion(){
    deleteAndZero(testbtn);
    deleteAndZero(startbtn);
    deleteAndZero(deconvbtn);
    
    meterThread.stopThread (500);
    
    deviceManager.removeAudioCallback(this);
    
    fftw_destroy_plan(transformadaX);
    fftw_destroy_plan(transformadaY);
    fftw_destroy_plan(transformadah);
    fftw_free(h);
    fftw_free(H);
    fftw_free(x);
    fftw_free(X);
    fftw_free(y);
    fftw_free(Y);
}

void ventanaMedicion::paint (Graphics& g){
    meterL.setBounds(55,174,15,90);
    
    int offsetX=0;
    
    g.setFont (juce::Font (12.0f));
    g.setColour(Colours::white);
    g.drawText ("Tipo", 30+offsetX, 46, 30, 25, Justification::centredLeft, true);
    g.drawText (CharPointer_UTF8 ("Duraci\xc3\xb3n[s]:"), 30+offsetX, 75, 80, 25, Justification::centredLeft, true);
    g.drawText ("RT estimado[s]:", 30+offsetX, 102, 80, 25, Justification::centredLeft, true);
    g.drawText ("Mic IN", 47, 263, 80, 25, Justification::centredLeft, true);
    g.drawText ("Sweep Gain", 94, 263, 80, 25, Justification::centredLeft, true);
    
    g.setFont (juce::Font (8.0f));
    g.setColour(Colours::lightgrey);
    g.drawText ("1", 106, 163, 80, 25, Justification::centredLeft, true);
    g.drawText ("0.5", 102, 206, 80, 25, Justification::centredLeft, true);
    g.drawText ("0", 106, 251, 80, 25, Justification::centredLeft, true);

    g.drawVerticalLine(117, 174, 264);
    g.drawHorizontalLine(174, 114, 121);
    g.drawHorizontalLine(218, 116, 119);
    g.drawHorizontalLine(264, 114, 121);

    //        g.setColour(Colours::lightgrey);
    //        g.drawText ("1", 110+offsetX, 166, 80, 25, Justification::centredLeft, true);
    //        g.drawText ("0", 110+offsetX, 249, 80, 25, Justification::centredLeft, true);
}

void ventanaMedicion::buttonClicked(Button* buttonThatWasClicked){
    if (buttonThatWasClicked==testbtn){
        if (testbtn->getButtonText()=="Test") {
            testbtn->setButtonText("Parar");
            LinLog->setEnabled(false);
            startbtn->setEnabled(false);
            duracionsweep->setEnabled(false);
            duracionRTestimado->setEnabled(false);
            
            amplitude=gainsweep.getValue();
            T=duracionsweep->getText().getFloatValue();
            mySweeps->setDuracionSweep(T);
            sweepSizeSamples=ceil(T*sampleRate);
            if (LinLog->getText() == "Lineal") {
                mySweeps->setTipo(5);
            }else{
                mySweeps->setTipo(6);
            }
            scount=0;
            outputEnabled=true;
            
            deviceManager.addAudioCallback(this);
            
        }else if (testbtn->getButtonText()=="Parar"){
            deviceManager.removeAudioCallback(this);
            outputEnabled=false;
            testbtn->setButtonText("Test");
            amplitude=0.0;
            
            LinLog->setEnabled(true);
            startbtn->setEnabled(true);
            duracionsweep->setEnabled(true);
            duracionRTestimado->setEnabled(true);
        }
    }else if(buttonThatWasClicked==startbtn){
        if (startbtn->getButtonText()==CharPointer_UTF8 ("Iniciar medici\xc3\xb3n")) {
            
            startbtn->setButtonText("Parar");
           
            T=duracionsweep->getText().getFloatValue();
            mySweeps->setDuracionSweep(T);
            sweepSizeSamples=ceil(T*sampleRate);
            if (LinLog->getText() == "Lineal") {
                mySweeps->setTipo(5);
            }else{
                mySweeps->setTipo(6);
            }
            fftSize=1.11*sweepSizeSamples+1.11*ceil(duracionRTestimado->getText().getFloatValue()*sampleRate);
            definirFFT(fftSize);
            
            amplitude=gainsweep.getValue();
            scount=0;
            
            deconvolucion=true;
            outputEnabled=true;
            
            LinLog->setEnabled(false);
            testbtn->setEnabled(false);
            duracionsweep->setEnabled(false);
            duracionRTestimado->setEnabled(false);
            gainsweep.setEnabled(false);
            
            deviceManager.addAudioCallback(this);
            
        }else {
            deviceManager.removeAudioCallback(this);
            outputEnabled=false;
            startbtn->setButtonText(medText);
            amplitude=0.0;
            
            LinLog->setEnabled(true);
            testbtn->setEnabled(true);
            duracionsweep->setEnabled(true);
            duracionRTestimado->setEnabled(true);
            gainsweep.setEnabled(true);
        }
    }else if(buttonThatWasClicked==deconvbtn){
        deviceManager.removeAudioCallback(this);
        startbtn->setButtonText(medText);
        
        LinLog->setEnabled(true);
        testbtn->setEnabled(true);
        duracionsweep->setEnabled(true);
        duracionRTestimado->setEnabled(true);
        gainsweep.setEnabled(true);
        
        //Deconvolucion
        fftw_execute(transformadaX);
        fftw_execute(transformadaY);
        
        for (int k=0; k<fftSize; ++k) {
            float a=X[k][0]*X[k][0]+X[k][1]*X[k][1];
            H[k][0]=(1.0/a)*(Y[k][0]*X[k][0]+Y[k][1]*X[k][1]);
            H[k][1]=(1.0/a)*(Y[k][1]*X[k][0]-Y[k][0]*X[k][1]);
        }
        
        fftw_execute(transformadah); //Respuesta al impulso queda en la variable 'h'
        int Length=(fftSize-sweepSizeSamples);
        
        IRcompleta->clear();
        IRcompleta->setSize(1,Length);
        for (int i=0; i<Length ; i++) {
            //IRcompleta->addSample(0, i, h[i][0]/fftSize);
            IRcompleta->getWritePointer(0)[i]=h[i][0]/fftSize;
        }
        
        //Normalización
        float maxIRc=IRcompleta->getMagnitude(0, 0, Length);
        IRcompleta->applyGain(0,0,Length,1.0/maxIRc);
        
        sendChangeMessage();
        
    }else if(buttonThatWasClicked==cancelMed){
        if (testbtn->getButtonText()=="Parar"){
            testbtn->triggerClick();
        }else if(startbtn->getButtonText()=="Parar"){
            startbtn->triggerClick();
        }
    }
}


void ventanaMedicion::audioDeviceIOCallback(const float** inputData,int InputChannels,float** outputData,int OutputChannels,int numSamples){
    meterL.copySamples (inputData[0], numSamples);
    const AudioSampleBuffer RespBuff (const_cast<float**> (inputData), 1, numSamples);
    const AudioSampleBuffer SweepBuff (const_cast<float**> (outputData), 1, numSamples);
    amplitudEnvelope=new AudioSampleBuffer(1,numSamples);
    
    if (outputEnabled) {
        for (int i=0; i<OutputChannels; ++i) {
            amplitudEnvelope->clear();
            
            for (int j=0; j<numSamples; j++){
                if (scount>sweepSizeSamples) {
                    amplitudEnvelope->getWritePointer(0)[j]=0.0f;
                }
                else if(scount+j>(sweepSizeSamples/*-1000*/)){
                    amplitudEnvelope->getWritePointer(0)[j]=gainsweep.getValue()*0.5f*(1.0+cos(float_Pi*(scount+j-kp)/Nt));
                }else{
                    amplitudEnvelope->getWritePointer(0)[j]=gainsweep.getValue();
                    if (scount+j==(sweepSizeSamples/*-1000*/)) {
                        kp=scount+j+1;
                        Nt= numSamples-(j+2);
                        //Nt= /*-1000*/;
                    }
                }
            }
            FloatVectorOperations::multiply(outputData[i],mySweeps->getReadSamples(f1,f2,scount,numSamples,sampleRate),amplitudEnvelope->getReadPointer(0), numSamples);
            
            for (int j=0; j<numSamples; ++j) {
                if (startbtn->getButtonText()=="Parar") {
                    if ((scount+j)<fftSize) {
                        x[scount+j][0]=SweepBuff.getSample(0, j);
                        y[scount+j][0]=RespBuff.getSample(0, j);
                    }else{
                        outputEnabled=false;
                        if(deconvolucion){
                            deconvbtn->triggerClick();
                        }
                        deconvolucion=false;
                    }
                }
            }
            

        }
        if (scount>sweepSizeSamples && testbtn->getButtonText()=="Parar") {
            scount=0;
        }else{
            scount+=numSamples;
        }
    }else{
        // JUCE: "We need to clear the output buffers, in case they're full of junk.."
        for (int i = 0; i < OutputChannels; ++i)
            if (outputData[i] != nullptr)
                FloatVectorOperations::clear (outputData[i], numSamples);
    }
    
}

void ventanaMedicion::definirFFT(int fftSize){
    x  = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * fftSize );
    X  = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * fftSize );
    y  = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * fftSize );
    Y  = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * fftSize );
    h  = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * fftSize );
    H  = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * fftSize );
    
    transformadaX  = fftw_plan_dft_1d( fftSize , x, X, FFTW_FORWARD, FFTW_ESTIMATE );
    transformadaY  = fftw_plan_dft_1d( fftSize , y, Y, FFTW_FORWARD, FFTW_ESTIMATE );
    transformadah  = fftw_plan_dft_1d( fftSize , H, h, FFTW_BACKWARD, FFTW_ESTIMATE );
}

//========================================================================================
ventanaConfiguracionesAudio::ventanaConfiguracionesAudio() : deviceManager(MainContentComponent::getAudioDeviceManagerCompartido()), settingsComp(deviceManager, 1, 1, 1, 1, false, false, false, false)
{
    //Configuraciones
    addAndMakeVisible(settingsComp);
    //settingsComp.setSize(480, 280);
    settingsComp.setSize(320, 420*75/100);//320 es el gap de la ventana principal
    
    addAndMakeVisible(botonSiguiente=new TextButton("Siguiente"));
    botonSiguiente->setBounds(320/2-120/2,420*75/100,120,25);
    botonSiguiente->addListener(this);
    
    MainContentComponent::getAudioDeviceManagerCompartido().addChangeListener(this);
    
}

ventanaConfiguracionesAudio::~ventanaConfiguracionesAudio()
{
    deleteAndZero(botonSiguiente);
}
void ventanaConfiguracionesAudio::buttonClicked(Button* buttonThatWasClicked){
    if (buttonThatWasClicked==botonSiguiente){
        deviceManager.getAudioDeviceSetup(newAudioSetup);
        sampleRate=newAudioSetup.sampleRate;
        
        Component::BailOutChecker checker (this);
        if (!checker.shouldBailOut()){
            buttonExtListeners.callChecked (checker, &ventanaConfiguracionesAudio::Listener::buttonClicked, this);
        }
    }
    
}

void ventanaConfiguracionesAudio::changeListenerCallback (ChangeBroadcaster*){
    deviceManager.getAudioDeviceSetup(newAudioSetup);
    
    if (!((newAudioSetup.sampleRate==48000)||(newAudioSetup.sampleRate==44100))) {
        botonSiguiente->setEnabled(false);
        if (isVisible()) {
            AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Advertencia", CharPointer_UTF8 ("Por favor seleccione una frecuencia de muestreo v\xc3\xa1lida: 44100 o 48000 [Hz]."));
        }
    }else{
        botonSiguiente->setEnabled(true);
    }

}
