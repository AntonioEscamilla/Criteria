/*
  ==============================================================================

    parameterComponent.h
    Created: 17 Aug 2015 4:44:36pm
    Author:  Tony Ducks

  ==============================================================================
*/

#ifndef parameterComponent_H_INCLUDED
#define parameterComponent_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "matPlotsHeader.h"
#include "PlotComponent.h"
#include "CustomButtonComponent.h"

//==============================================================================
/*
*/

static const uint32 coloresActivacion4[]={0xffaf40ff,0xffff7840,0xffffff40,0xff40ffff};
static const uint32 coloresActivacion7[]={0xfffffc64,0xff64ff67,0xff64d7ff,0xff8164ff,0xffff64ed,0xffff7a64,0xffffc564};

class parameterComponent    : public Component,
                              public CustomButtonComponent::Listener
{
public:
    parameterComponent(OwnedArray<Buffer>* ,const char* botonText[],int& numBandas);//class constructor using pointer to an
    ~parameterComponent();                                              // "OwnedArray<Buffer>" donde estan parametros de tiempo
                                                                        //o energeticos
    void paint (Graphics&);
    void resized();
    
    void buttonClicked (CustomButtonComponent*);

private:
    ScopedPointer<OctaveBandPlot>           octaveBandComponent;
    ScopedPointer<ThirdBandPlot>            thirdBandComponent;
    OwnedArray<CustomButtonComponent>       botones;
    OwnedArray<Buffer>*                     parametersBufferArray;
    bool                                    octaveResolution;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (parameterComponent)
};


#endif  // parameterComponent_H_INCLUDED
