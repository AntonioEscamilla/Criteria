/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//Valiables globales del proyecto
static ScopedPointer<AudioDeviceManager>    deviceManagerCompartido;
juce::AudioDeviceManager::AudioDeviceSetup  newAudioSetup;
double                                      sampleRate;
AudioSampleBuffer                           *IRcompleta;
TextButton                                  *cancelMed;

//==============================================================================
MainContentComponent::MainContentComponent(): audioDeviceManager(MainContentComponent::getAudioDeviceManagerCompartido()),
readAheadThread("read Ahead thread"){
    
    addAndMakeVisible (menuBar = new MenuBarComponent (this));
#if JUCE_MAC
    menuBar->setModel (nullptr);
    MenuBarModel::setMacMainMenu (this);
#endif
    addAndMakeVisible(tabsComponent = new TabbedComponent(TabbedButtonBar::TabsAtTop));
    
    addChildComponent(ventanaConf);
    ventanaConf.addListener(this);
    
    addChildComponent(ventanaMedicion);
    ventanaMedicion.addChangeListener(this);

    addChildComponent(info= new GroupComponent ("info", CharPointer_UTF8 ("Informaci\xc3\xb3n del archivo")));
    info->setVisible(false);
    info->setAlpha(0.8);
    info->setColour(01, Colours::white);
    info->setBounds (320/2-250/2, 35, 250, 80);
    
    addChildComponent(cancelMed=new TextButton("Cancel"));
    cancelMed->addListener(this);
    cancelMed->addListener(&ventanaMedicion);
    
    addChildComponent(exportarIR=new TextButton("Exportar IR"));
    exportarIR->addListener(this);
    
    //Audio
    audioFormatManager.registerBasicFormats();
    audioDeviceManager.getAudioDeviceSetup(newAudioSetup);
    sampleRate=newAudioSetup.sampleRate;
    audioSourcePlayer.setSource(&audioTransportSource);
    audioDeviceManager.addAudioCallback(&audioSourcePlayer);
    readAheadThread.startThread(3);
    
    //Inicializar Variables
    IRcompleta=new AudioSampleBuffer();
    IRrecortada=new AudioSampleBuffer();
    bufferWaveform= new Buffer();
    tabsComponent->addTab("Criteria", Colour(0xff2f2f2f), nullptr, true);
    
    //Inicialización del OwnedArray para las respuestas filtradas
    for (int i=0;i<30;i++) {
        filteredAudioArray.add(new Buffer());//filteredAudioArray es un OwnedArray y debe ser declarado como una variable de la clase para que
    }
    
    //Inicialización de los OwnedArray para los parámetros temporales.
    for (int i=0; i<4; ++i) {
        Temporales.add(new Buffer(30));
        Xi.add(new Buffer(30));
    }
    
    //Inicialización de los OwnedArray para los parámetros energéticos.
    for (int i=0; i<7; ++i) {
        Energeticos.add(new Buffer(30));
    }
    
    filterRes=octavas;                  //por defecto filtros por octava, dependiendo de Fs se define NumneroBandas
    if(sampleRate==44100) numeroBandas=9;
    else if (sampleRate==48000) numeroBandas=10;
    IRchanged=true;
    validIR=false;
    
    setSize (1200, 520);
}

//==============================================================================
MainContentComponent::~MainContentComponent(){
    deleteAndZero(IRcompleta);
    deleteAndZero(cancelMed);
    deleteAndZero(exportarIR);
    
#if JUCE_MAC
    MenuBarModel::setMacMainMenu (nullptr);
#endif
    PopupMenu::dismissAllActiveMenus();

    audioTransportSource.stop();
    audioTransportSource.setSource(nullptr);
    audioFormatReaderSource=nullptr;
    deviceManagerCompartido=nullptr;
    readAheadThread.stopThread (500);
    
    aboutWindow.deleteAndZero();
}

//==============================================================================
void MainContentComponent::paint (Graphics& g){
    float gap = 5.0;
    GuiHelpers::drawBevel (g, tabsComponent->getBounds().toFloat(), gap, Colours::darkgrey);
    info->setVisible(false);
    if (IRrecortada->getNumSamples()>1&&!ventanaConf.isVisible()&&!ventanaMedicion.isVisible()) {
        //g.setFont(juce::Font (12.0f));
        
        info->setVisible(true);
        g.setOrigin(info->getBounds().getX()+15,info->getBounds().getY()+20);
        g.setColour(Colours::whitesmoke);
        if (!soundfile.hasFileExtension(".wav")) {
            exportarIR->setVisible(true);
        }
        g.drawFittedText("Nombre: " + soundfile.getFileName(), getLocalBounds(), Justification::topLeft, 2);
        g.setOrigin(0, 15);
        g.drawFittedText(String(CharPointer_UTF8 ("Duraci\xc3\xb3n: "))+String(std::ceil(IRcompleta->getNumSamples()*100.0/sampleRate)/100.0)+" [s]", getLocalBounds(), Justification::topLeft, 2);
        g.setOrigin(0, 15);
        g.drawFittedText("Frecuencia de muestreo: "+String(fsMostrar)+" [Hz]", getLocalBounds(), Justification::topLeft, 2);
        
        g.resetToDefaultState();
    }
}

//==============================================================================
void MainContentComponent::resized(){
    const int w = getWidth();
    const int h = getHeight();
    const int gap = 320;
    
    tabsComponent->setBounds(gap, 1, w-gap-5, h-5);
    
    ventanaConf.setBounds(0, 30, gap-5, h-5);
    ventanaMedicion.setBounds(30, 30, gap-5, h-5);
    cancelMed->setBounds(320/2-80/2,getHeight()*85/100+30,80,25);
    exportarIR->setBounds(320/2-120/2,info->getY()+info->getHeight()+5,100,22);
}

//==============================================================================
StringArray MainContentComponent:: getMenuBarNames(){
    const char* const MenuNames[] = {"Archivo", "Bandas", "Ayuda", nullptr};
    return StringArray (MenuNames);
}

//==============================================================================
PopupMenu MainContentComponent::getMenuForIndex(int menuIndex, const String& name) {
    PopupMenu menu;
    if (name == "Archivo"){
        menu.addItem(medicion, CharPointer_UTF8 ("Nueva medici\xc3\xb3n"));
        menu.addItem(importar, "Importar respuesta al impulso");
        menu.addItem(exParametros, CharPointer_UTF8 ("Exportar par\xc3\xa1metros ac\xc3\xbasticos"));
    }else if (name=="Bandas"){
        menu.addItem(octava, "Por octava", validIR, filterRes==octavas);
        menu.addItem(tercio, "Por tercio", validIR, filterRes==tercios);
    }else if (name=="Ayuda"){
        menu.addItem(about, "Acerca de Criteria");
    }
    return menu;
}

//==============================================================================
void MainContentComponent::menuItemSelected(int menuID,  int index){
    if (menuID==medicion){
        ventanaMedicion.setVisible(false);
        ventanaConf.setVisible(true);
        cancelMed->setVisible(true);
        exportarIR->setVisible(false);
        if (tabsComponent->getNumTabs()>1) {
            tabsComponent->getCurrentContentComponent()->setVisible(false);
            //tabsComponent->getCurrentContentComponent()->getChildComponent(0)->setVisible(false);
        }
        tabsComponent->setEnabled(false);
        tabsComponent->setAlpha(0.2);
        repaint();
        
    }
    else if (menuID==importar){
        importarIR();
        if (IRchanged) {
            ventanaConf.setVisible(false);
            ventanaMedicion.setVisible(false);
            cancelMed->setVisible(false);
            tabsComponent->setEnabled(true);
            tabsComponent->setAlpha(1);
            menuItemSelected(4, 1);                 //Llama al item para calcular por octava. (NumeroBandas=10;encontrarParametros();)
        }
    }
    else if (menuID==exParametros){
        exportarParametros();
    }
    else if (menuID==octava){
        filterRes=octavas;
        if(sampleRate==44100) numeroBandas=9;
        else if (sampleRate==48000) numeroBandas=10;
        encontrarParametros();
    }
    else if (menuID==tercio){
        filterRes=tercios;
        if(sampleRate==44100) numeroBandas=29;
        else if (sampleRate==48000) numeroBandas=30;
        encontrarParametros();
    }
    else if (menuID==about){
        aboutWindow = new AboutWindow ("Acerca de este Software",Colour(0xff3f3f3f),DocumentWindow::allButtons);
    }
#if JUCE_MAC
    menuBar->setModel (nullptr);
    MenuBarModel::setMacMainMenu (this);
#endif
}

//==============================================================================
//Listeners
void MainContentComponent::buttonClicked (ventanaConfiguracionesAudio*){
    ventanaConf.setVisible(false);
    ventanaMedicion.setVisible(true);
    cancelMed->setVisible(true);
}

//==============================================================================
void MainContentComponent::buttonClicked (Button* buttonThatWasClicked){
    if(buttonThatWasClicked==cancelMed){
        ventanaConf.setVisible(false);
        ventanaMedicion.setVisible(false);
        cancelMed->setVisible(false);
        //exportarIR->setVisible(false);
        if (tabsComponent->getNumTabs()>1) {
            tabsComponent->getCurrentContentComponent()->setVisible(true);
        }
        tabsComponent->setEnabled(true);
        tabsComponent->setAlpha(1);
        repaint();
    }else if(buttonThatWasClicked==exportarIR){
        exportarWav();
        exportarIR->setVisible(false);
        repaint();
    }
}

//==============================================================================
void MainContentComponent::changeListenerCallback (ChangeBroadcaster* source){
    if (source == &ventanaMedicion){
        ventanaMedicion.setVisible(false);
        cancelMed->setVisible(false);
        tabsComponent->setEnabled(true);
        tabsComponent->setAlpha(1);
        
        soundfile=File();
        fsMostrar=sampleRate;
        
        exportarIR->setVisible(true);
        validIR=true;                           //variable para activar el menu->Bandas para escoger octava o tercio

        menuItemSelected(4, 1);                 //Llama al item para calcular por octava. (NumeroBandas=10;encontrarParametros();)
    }
}

//==============================================================================
AudioDeviceManager& MainContentComponent::getAudioDeviceManagerCompartido(){
    if (deviceManagerCompartido==nullptr) {
        deviceManagerCompartido=new AudioDeviceManager();
        deviceManagerCompartido->initialise(1, 1, 0, true);
    }
    return *deviceManagerCompartido;
}

//==============================================================================
void MainContentComponent::importarIR(){
    FileChooser chooser ("Seleccione una respuesta al impulso",File::nonexistent, "*.wav");
    if (chooser.browseForFileToOpen()){
        audioTransportSource.stop();
        audioTransportSource.setSource (nullptr);
        audioFormatReaderSource = nullptr;
        
        soundfile=chooser.getResult();
        audioFormatReader = audioFormatManager.createReaderFor(soundfile);
        
        IRcompleta->clear();
        IRcompleta->setSize(1, audioFormatReader->lengthInSamples);
        audioFormatReader->read(IRcompleta, 0, audioFormatReader->lengthInSamples, 0, true, false);
        sampleRate=audioFormatReader->sampleRate;
        fsMostrar=sampleRate;
        
        //Aqui se cambia la frecuencia de muestreo de la sesión a la del .wav
        audioDeviceManager.getAudioDeviceSetup(newAudioSetup);
        newAudioSetup.sampleRate=sampleRate;
        audioDeviceManager.setAudioDeviceSetup(newAudioSetup, true);
        
        //Playback preparation
        audioFormatReaderSource = new AudioFormatReaderSource(audioFormatReader, true);
        audioTransportSource.setSource(audioFormatReaderSource, 32768, &readAheadThread, 0, 2);   //el 0 es para que no haga resamplig....
        audioSourcePlayer.setSource(&audioTransportSource);                                       //..depende de haber cambiado Fs en
        audioDeviceManager.addAudioCallback(&audioSourcePlayer);                                  //..AudioDevice
        
        IRchanged=true;
        validIR=true;                                                   //variable para activar el menu->Bandas para escoger octava o tercio
        
    }else{
        IRchanged=false;
    }
    
}

//==============================================================================
void MainContentComponent::exportarWav(){
    int LongitudBufferIR=IRcompleta->getNumSamples();
//    float maximoIR=IRcompleta->getMagnitude(0, 0, LongitudBufferIR); //Normalización
//    IRcompleta->applyGain(0,0,LongitudBufferIR,1.0/maximoIR);
    
    FileChooser chooser ("Guardar respuesta al impulso (IR)",File::nonexistent, "*.wav");
    if (chooser.browseForFileToSave(true)) {
        soundfile=chooser.getResult();
        //file.deleteFile();
        ScopedPointer<FileOutputStream> fileStream (soundfile.createOutputStream());
        
        WavAudioFormat wavFormat;
        ScopedPointer<AudioFormatWriter> writer;
        writer = wavFormat.createWriterFor (fileStream, sampleRate, 1, 16, StringPairArray(), 0);
        
        fileStream.release(); // Se pasa la responsabilidad de borrar el stream al writerObject que ahora lo esta usando
        writer->writeFromAudioSampleBuffer(*IRcompleta, 0, LongitudBufferIR);
        
        //        delete writer;
        //        writer=nullptr;
        
    }else{
        exportarWav();
    }
}

//==============================================================================
void MainContentComponent::audioDownSamplig(AudioSampleBuffer* input, Buffer* output,int downSampligFactor, int audioFileProportion){
    int sampleCounter=0;
    float* dataWaveform = output->getData();
    ScopedPointer<Buffer>  auxBuf = new Buffer(N);
    float* auxBufData = auxBuf->getData();
    int sampleIndexModulo;
    int maxLoc;
    float maxVal;
    
    for(int j = 0; j < input->getNumSamples(); j++){
        float sample = *input->getReadPointer(0, j);
        sampleIndexModulo = j%N;
        auxBufData[sampleIndexModulo] = sample;
        if(sampleIndexModulo==0){
            sampleCounter++;
            if (sampleCounter < output->getSize()) {
                if (sample > 0.0f) findMax(auxBufData, auxBuf->getSize(), maxLoc, maxVal);
                else findMin(auxBufData, auxBuf->getSize(), maxLoc, maxVal);
                dataWaveform[sampleCounter] = maxVal;
            }
        }
    }
}

//==============================================================================
void MainContentComponent::filtrarIR(){
    //Inicialización del OwnedArray para las respuestas filtradas
    for (int i=0;i<30;i++) {
        filteredAudioArray.getUnchecked(i)->reset();
        filteredAudioArray.getUnchecked(i)->setSize(LongitudIRrecortada);
    }
    
    ScopedPointer<FilterBank> filterBank = new FilterBank(filterRes,sampleRate);
    filterBank->setCoeficientes();
    filterBank->processSamples(IRrecortada->getWritePointer(0), &filteredAudioArray, LongitudIRrecortada);
}

//==============================================================================
void MainContentComponent::encontrarParametros(){
    repaint();
    
    encontrart0Posi(IRcompleta, t0, posi);
    
    //RecortarAudio y guardarlo en IRrecortada!!!
    int seguridad=10000;
    LongitudIRcola=0.1*(IRcompleta->getNumSamples());
    LongitudIRrecortada=posi-t0+seguridad+LongitudIRcola;
    IRrecortada->setSize(1, LongitudIRrecortada);
    IRrecortada->copyFrom(0, 0, *IRcompleta, 0, t0, posi-t0);
    IRrecortada->copyFrom(0, posi-t0, *IRcompleta, 0, IRcompleta->getNumSamples()-LongitudIRcola-seguridad, LongitudIRcola+seguridad);
    
    IRchanged=true;
    
    bufferWaveform->setSize(IRcompleta->getNumSamples()/(M*N));
    audioDownSamplig(IRcompleta,bufferWaveform,N,M);
    
    if (tabsComponent->getNumTabs()>0) {
        tabsComponent->clearTabs();
    }
    
    tabsComponent->addTab("Respuesta al Impulso", Colour(0xff2f2f2f), new AudioWaveForm(bufferWaveform,true), true);
    
    if ((sampleRate==48000)||(sampleRate==44100)) {
        filtrarIR();
        
        encontrarParametrosTemporales();
        tabsComponent->addTab(CharPointer_UTF8 ("Par\xc3\xa1meros Temporales"), Colour(0xff2f2f2f),new parameterComponent(&Temporales,botonTextTime,numeroBandas), true);
        
        encontrarParametrosEnergeticos();
        tabsComponent->addTab(CharPointer_UTF8 ("Par\xc3\xa1meros Energ\xc3\xa9ticos"), Colour(0xff2f2f2f),new parameterComponent(&Energeticos,botonTextEnergy,numeroBandas), true);
    }else{
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Advertencia", CharPointer_UTF8 ("El c\xc3\xa1lculo de los par\xc3\xa1metros ac\xc3\xbasticos solo es v\xc3\xa1lido para respuestas al impulso con frecuencia de m"
                                         "uestreo igual a 44100 o 48000 [Hz]."));
    }
}

//==============================================================================
void MainContentComponent::encontrarParametrosTemporales(){
    for (int i=0; i<4; ++i) {
        Temporales.getUnchecked(i)->reset();
        Xi.getUnchecked(i)->reset();
    }

    ScopedPointer<ParametroTemporal> ObjetoTemporal=new ParametroTemporal(sampleRate);
    for (int i=0; i<numeroBandas; ++i) {
        //OJO!!!
        //Corregir el retraso del filtro y corregir el ruido!!!!
        //******************************************************
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //EDT
        ObjetoTemporal->almacenarParametroTemporal(filteredAudioArray.getUnchecked(i), Temporales.getUnchecked(0)->getData()[i], 0, -10, Xi.getUnchecked(0)->getData()[i]);
        
        //RT10
        ObjetoTemporal->almacenarParametroTemporal(filteredAudioArray.getUnchecked(i), Temporales.getUnchecked(1)->getData()[i], -5, -15, Xi.getUnchecked(1)->getData()[i]);
        
        //RT20
        ObjetoTemporal->almacenarParametroTemporal(filteredAudioArray.getUnchecked(i), Temporales.getUnchecked(2)->getData()[i], -5, -25, Xi.getUnchecked(2)->getData()[i]);
        
        //RT30
        ObjetoTemporal->almacenarParametroTemporal(filteredAudioArray.getUnchecked(i), Temporales.getUnchecked(3)->getData()[i], -5, -35, Xi.getUnchecked(3)->getData()[i]);
    }
}

//==============================================================================
void MainContentComponent::encontrarParametrosEnergeticos(){
    for (int i=0; i<7; ++i) {
        Energeticos.getUnchecked(i)->reset();
    }
    
    ScopedPointer<ParametroEnergetico> ObjetoEnergetico=new ParametroEnergetico(sampleRate, true);
    int groupdelay;
    for (int i=0; i<numeroBandas; ++i) {
        
        if (numeroBandas==9) {
            groupdelay=groupdelay9[i];
        }
        else if(numeroBandas==10){
            groupdelay=groupdelay10[i];
        }
        else if (numeroBandas==29){
            groupdelay=groupdelay29[i];
        }
        else if (numeroBandas==30){
            groupdelay=groupdelay30[i];
        }
        
        //C50
        ObjetoEnergetico->setdB(true);
        ObjetoEnergetico->CambiarLimites(0, 50e-3, 50e-3, (LongitudIRrecortada-groupdelay)/sampleRate);
        ObjetoEnergetico->relacionEnergetica(filteredAudioArray.getUnchecked(i), Energeticos.getUnchecked(0)->getData()[i], groupdelay);
        
        //C80
        ObjetoEnergetico->CambiarLimites(0, 80e-3, 80e-3, (LongitudIRrecortada-groupdelay)/sampleRate);
        ObjetoEnergetico->relacionEnergetica(filteredAudioArray.getUnchecked(i), Energeticos.getUnchecked(1)->getData()[i], groupdelay);
        
        //STe
        ObjetoEnergetico->CambiarLimites(20e-3, 100e-3, 0, 10e-3);
        ObjetoEnergetico->relacionEnergetica(filteredAudioArray.getUnchecked(i), Energeticos.getUnchecked(2)->getData()[i], groupdelay);
        
        //STl
        ObjetoEnergetico->CambiarLimites(100e-3, 1, 0, 10e-3);
        ObjetoEnergetico->relacionEnergetica(filteredAudioArray.getUnchecked(i), Energeticos.getUnchecked(3)->getData()[i], groupdelay);
        
        //D50
        ObjetoEnergetico->setdB(false); //También Funciona para el Ts
        ObjetoEnergetico->CambiarLimites(0, 50e-3, 0, (LongitudIRrecortada-groupdelay)/sampleRate);
        ObjetoEnergetico->relacionEnergetica(filteredAudioArray.getUnchecked(i), Energeticos.getUnchecked(4)->getData()[i], groupdelay);
        
        //Ts
        ObjetoEnergetico->CambiarLimites(0, (LongitudIRrecortada-groupdelay)/sampleRate, 0, (LongitudIRrecortada-groupdelay)/sampleRate);
        ObjetoEnergetico->relacionEnergeticaTS(filteredAudioArray.getUnchecked(i), Energeticos.getUnchecked(5)->getData()[i]/*, groupdelay[i]*/);
        
        //SNR //Falta corregir el retraso del filtro
        //******************************************************
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        ObjetoEnergetico->getSNR(filteredAudioArray.getUnchecked(i), Energeticos.getUnchecked(6)->getData()[i], posi-t0, LongitudIRcola);
    }
    
}

//==============================================================================
void MainContentComponent::encontrart0Posi(AudioSampleBuffer *entrada, int &t0, int &posi){
    //Detección de picos en la magnitud de la señal
    int j=0;
    const int Length=entrada->getNumSamples();
    ScopedPointer<AudioSampleBuffer> pos=new AudioSampleBuffer(1,Length);
    ScopedPointer<AudioSampleBuffer> picos=new AudioSampleBuffer(1,Length);
    float sus1;
    float sus2;
    for (int i=1; i<(Length-1); ++i) {
        sus1=(std::abs(entrada->getSample(0, i-1)))-(std::abs(entrada->getSample(0, i)));
        sus2=(std::abs(entrada->getSample(0, i)))-(std::abs(entrada->getSample(0, i+1)));
        if ((sus1<0) && ((sus1*sus2)<0)){
            j=j+1;
            pos->setSample(0, j-1, i);
            picos->setSample(0, j-1, std::abs(entrada->getSample(0, i)));
        }
    }
    
    int Mb=0;
    int Msua=10;
    int Mu2=0;
    
    ScopedPointer<Buffer> Mpicossua=new Buffer(j);
    ScopedPointer<Buffer> Mpossua=new Buffer(j);
    ScopedPointer<Buffer> Mpicos=new Buffer();
    ScopedPointer<Buffer> MabsPicos=new Buffer();
    ScopedPointer<Buffer> Mlolo=new Buffer();
    for (int Mu1=0;Mu1<(j-Msua-1);Mu1=Mu1+Msua){
        Mu2=Mu1+Msua;

        Mpicos->reset();
        Mpicos->setSize(Mu2-Mu1);
        
        MabsPicos->reset();
        MabsPicos->setSize(Mu2-Mu1);
        
        Mlolo->reset();
        Mlolo->setSize(Mu2-Mu1);
        
        for (int i=0; i<(Mu2-Mu1);++i){
            Mpicos->getData()[i]=picos->getSample(0, Mu1+i);
        }
        float nada;
        findMinMax(Mpicos->getData(), Mu2-Mu1, nada, Mpicossua->getData()[Mb]);
        for (int i=0; i<(Mu2-Mu1);++i) {
            MabsPicos->getData()[i]=std::abs(picos->getReadPointer(0)[Mu1+i]-Mpicossua->getData()[Mb]);
        }
        int MinimoSuaPos;
        findMin(MabsPicos->getData(), Mu2-Mu1, MinimoSuaPos, nada);
        Mpossua->getData()[Mb]=MinimoSuaPos;
        
        for (int i=0; i<(Mu2-Mu1);++i) {
            Mlolo->getData()[i]=pos->getWritePointer(0)[Mu1+i];
        }
        int indiceSua=Mpossua->getData()[Mb];
        Mpossua->getData()[Mb]=Mlolo->getData()[indiceSua];
        Mb=Mb+1;
    }
    
    int envLength=0;
    //Construcción continua de la seudoenvolvente
    ScopedPointer<Buffer> PrevEnv=new Buffer(Length);
    PrevEnv->reset();
    for (int i=0; i<(Mb-1) ; ++i) {
        float m=0.0;
        float b=0.0;
        if ((Mpossua->getData()[i+i])-(Mpossua->getData()[i])!=0) {
            m=((Mpicossua->getData()[i+1])-(Mpicossua->getData()[i]))/((Mpossua->getData()[i+i])-(Mpossua->getData()[i]));
            b=Mpicossua->getData()[i]-m*(Mpossua->getData()[i]+1);
        }
        for (int k=Mpossua->getData()[i]; k<=Mpossua->getData()[i+1]; ++k) {
            PrevEnv->getData()[k-1]=m*k+b;
            ++envLength;
        }
    }

//    ScopedPointer<Buffer> env=new Buffer(envLength);
//    for (int i=0; i<envLength; ++i) {
//        env->getData()[i]=PrevEnv->getData()[i];
//    }

    //Calcula rms del ruido de fondo (10%final de la se?al)
    int lcenv=envLength*0.1;
    ScopedPointer<Buffer> tailenv=new Buffer(lcenv);
    for (int i=0; i<lcenv; ++i) {
        tailenv->getData()[i]=PrevEnv->getData()[Length-lcenv+i];
    }
    float Nlvl=findMean(tailenv->getData(), lcenv);
    
    int lc=Length*0.1;
    ScopedPointer<Buffer> taili=new Buffer(lc);
    for (int i=0; i<lc; ++i) {
        taili->getData()[i]=entrada->getReadPointer(0)[Length-lc+i];//(IR[Length-lc+i]);//*(IR[Length-lc+i]);
    }
    float rmsnoise=findRMS(taili->getData(), lc);

    int Minn;
    float MaximoIR;
    findMax(entrada->getReadPointer(0), Length, Minn, MaximoIR);
    for (int i=0; i<Length; ++i) {
        if ((PrevEnv->getData()[i]-rmsnoise)>0.2*MaximoIR) {
            t0=i;
            break;
        }
    }
    for (int f=t0; f<envLength; ++f) {
        if (PrevEnv->getData()[f]<Nlvl){
            posi=f+1;
            break;
        }
    }
}

//==============================================================================
void MainContentComponent::exportarParametros(){
    FileChooser chooser (CharPointer_UTF8 ("Guardar par\xc3\xa1metros ac\xc3\xbasticos temporales y energ\xc3\xa9ticos"),File::nonexistent);
    if (chooser.browseForFileToSave(true)) {
        std::ofstream outTxt;
        
        String direccionT=chooser.getResult().getFullPathName();
        String direccionE=chooser.getResult().getFullPathName();
        direccionT.append("-Temporales.txt", 50);
        direccionE.append(CharPointer_UTF8 ("-Energ\xc3\xa9ticos.txt"), 50);
        
        outTxt.open(direccionT.toRawUTF8());
        outTxt << "Frecuencias[Hz]" << std::endl;
        
        for (int i=0; i<numeroBandas; ++i) {
            if (numeroBandas==10||numeroBandas==9) {
                outTxt<< bandasOctava[i] << std::endl;
            }else{
                outTxt<< bandasTercioOut[i] << std::endl;
            }
        }
        
        outTxt << std::endl << "EDT[s]" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Temporales.getUnchecked(0)->getData()[i] << std::endl;
        }
        outTxt << std::endl << "T10[s]" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Temporales.getUnchecked(1)->getData()[i] << std::endl;
        }
        outTxt << std::endl << "T20[s]" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Temporales.getUnchecked(2)->getData()[i] << std::endl;
        }
        outTxt << std::endl << "T30[s]" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Temporales.getUnchecked(3)->getData()[i] << std::endl;
        }
        outTxt << std::endl << "Parametro de no Linealidad (Xi) - EDT" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Xi.getUnchecked(0)->getData()[i] << std::endl;
        }
        
        outTxt << std::endl << "Parametro de no Linealidad (Xi) - T10" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Xi.getUnchecked(1)->getData()[i] << std::endl;
        }
        
        outTxt << std::endl << "Parametro de no Linealidad (Xi) - T20" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Xi.getUnchecked(2)->getData()[i] << std::endl;
        }
        
        outTxt << std::endl << "Parametro de no Linealidad (Xi) - T30" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Xi.getUnchecked(3)->getData()[i] << std::endl;
        }
        outTxt.close();
        
        outTxt.open(direccionE.toRawUTF8());
        outTxt << "Frecuencias[Hz]" << std::endl;
        
        for (int i=0; i<numeroBandas; ++i) {
            if (numeroBandas==10||numeroBandas==9) {
                outTxt<< bandasOctava[i] << std::endl;
            }else{
                outTxt<< bandasTercioOut[i] << std::endl;
            }
        }
       
        outTxt << std::endl <<  "C50[dB]" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Energeticos.getUnchecked(0)->getData()[i] << std::endl;
        }
        outTxt << std::endl <<  "C80[dB]" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Energeticos.getUnchecked(1)->getData()[i] << std::endl;
        }
        outTxt << std::endl << "D50[-]" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Energeticos.getUnchecked(4)->getData()[i] << std::endl;
        }
        outTxt << std::endl << "Ts[ms]" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Energeticos.getUnchecked(5)->getData()[i] << std::endl;
        }
        outTxt << std::endl << "STe[dB]" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Energeticos.getUnchecked(2)->getData()[i] << std::endl;
        }
        outTxt << std::endl << "STl[dB]" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Energeticos.getUnchecked(3)->getData()[i] << std::endl;
        }
        outTxt << std::endl << "SNR[dB]" << std::endl;
        for (int i=0; i<numeroBandas; ++i) {
            outTxt << Energeticos.getUnchecked(6)->getData()[i] << std::endl;
        }
        
        outTxt.close();
    }
    
}