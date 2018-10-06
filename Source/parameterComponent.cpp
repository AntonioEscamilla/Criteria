/*
  ==============================================================================

    parameterComponent.cpp
    Created: 17 Aug 2015 4:44:36pm
    Author:  Tony Ducks

  ==============================================================================
*/

#include "parameterComponent.h"

//==============================================================================
parameterComponent::parameterComponent(OwnedArray<Buffer>* _parametersBufferArray,const char* botonTexto[],int& numBandas){
    parametersBufferArray = _parametersBufferArray;
    if(numBandas<=10){
        octaveResolution = true;
        octaveBandComponent = new OctaveBandPlot(parametersBufferArray,false,numBandas);
        addAndMakeVisible(octaveBandComponent);
    }else{
        octaveResolution = false;
        thirdBandComponent = new ThirdBandPlot(parametersBufferArray,false,numBandas);
        addAndMakeVisible(thirdBandComponent);
    }
    
    for(int i=0;i<parametersBufferArray->size();i++){
        CustomButtonComponent* boton;
        if (parametersBufferArray->size()==4)
            boton = new CustomButtonComponent(Colour(coloresActivacion4[i]),botonTexto[i]);
        else if (parametersBufferArray->size()==7)
            boton = new CustomButtonComponent(Colour(coloresActivacion7[i]),botonTexto[i]);
        boton->addListener(this);
        botones.add(boton);
        addAndMakeVisible(boton);
    }
}

//==============================================================================
parameterComponent::~parameterComponent(){
}

//==============================================================================
void parameterComponent::paint (Graphics& g){
}

//==============================================================================
void parameterComponent::resized(){
    for(int i=0;i<botones.size();i++){
        botones[i]->setBounds(0,33*i, 120, 30);
    }
    if(octaveResolution){
        octaveBandComponent->setBounds(140, 0, getWidth()-140, getHeight()-30);
    }else{
        thirdBandComponent->setBounds(140, 0, getWidth()-140, getHeight()-30);
    }
    
}

//==============================================================================
void parameterComponent::buttonClicked(CustomButtonComponent* boton){
    
    int botonIndex = botones.indexOf(boton);
    for(int i=0;i<botones.size();i++){
        if (i==botonIndex) {
            botones[i]->setActive(true);
            if(octaveResolution){
                octaveBandComponent->setActive(true);
                if (parametersBufferArray->size()==4)
                    octaveBandComponent->setPlotColor(Colour(coloresActivacion4[i]));
                else if (parametersBufferArray->size()==7)
                    octaveBandComponent->setPlotColor(Colour(coloresActivacion7[i]));
                octaveBandComponent->changeBuffer(parametersBufferArray->getUnchecked(i));
                octaveBandComponent->setYlabelOffset(i);
                octaveBandComponent->repaint();
            }else{
                thirdBandComponent->setActive(true);
                if (parametersBufferArray->size()==4)
                    thirdBandComponent->setPlotColor(Colour(coloresActivacion4[i]));
                else if (parametersBufferArray->size()==7)
                    thirdBandComponent->setPlotColor(Colour(coloresActivacion7[i]));
                thirdBandComponent->changeBuffer(parametersBufferArray->getUnchecked(i));
                thirdBandComponent->setYlabelOffset(i);
                thirdBandComponent->repaint();
            }
            
        }else{
            botones[i]->setActive(false);
        }
        botones[i]->repaint();
    }
}
