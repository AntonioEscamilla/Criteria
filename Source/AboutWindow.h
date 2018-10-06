/*
  ==============================================================================

    AboutWindow.h
    Created: 14 Dec 2015 11:53:01am
    Author:  Tony Ducks

  ==============================================================================
*/

#ifndef ABOUTWINDOW_H_INCLUDED
#define ABOUTWINDOW_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "EmbeddedImage.h"

//==============================================================================
class AboutWindowContentComp :  public Component
{
public:
    
    AboutWindowContentComp(){
        addAndMakeVisible(&embeddedImg);
        setSize(333,480);
    }
    
    ~AboutWindowContentComp(){
        
    }
    
    void paint (Graphics& g){
        g.setColour(Colours::whitesmoke);
        int margen = 10;
        int offset = 20;
        
        g.setFont (juce::Font (16.0f));
        g.drawText (CharPointer_UTF8 ("Universidad de San Buenaventura Medell\xc3\xadn"), margen, 15, getWidth()-2*margen, offset, Justification::centred, true);
        g.setFont (juce::Font (12.0f));
        g.drawText (CharPointer_UTF8 ("Grupo de Modelamiento y Simulaci\xc3\xb3n Computacional (GIMSC)"), margen, 15+offset, getWidth()-2*margen, offset, Justification::centred, true);
        g.drawText (CharPointer_UTF8 ("L\xc3\xadnea de Investigaci\xc3\xb3n en Ac\xc3\xbastica y Procesamiento de Se\xc3\xb1""al"), margen, 15+2*offset, getWidth()-2*margen, offset, Justification::centred, true);
        
        g.setFont (juce::Font (16.0f));
        g.drawText ("Grupo de Desarrolladores", margen, 15+4*offset, getWidth()-2*margen, offset, Justification::centred, true);
        g.setFont (juce::Font (12.0f));
        g.drawText (CharPointer_UTF8 ("Antonio Escamilla"), margen, 15+5*offset, getWidth()-2*margen, offset, Justification::centred, true);
        g.drawText (CharPointer_UTF8 ("Juan Fernando Londoño"), margen, 15+6*offset, getWidth()-2*margen, offset, Justification::centred, true);
        g.drawText (CharPointer_UTF8 ("Yonny Alexander Quiceno"), margen, 15+7*offset, getWidth()-2*margen, offset, Justification::centred, true);
        g.drawText (CharPointer_UTF8 ("Alejandro Mejia"), margen, 15+8*offset, getWidth()-2*margen, offset, Justification::centred, true);
    }
    
    void resized(){
        embeddedImg.setBounds(getWidth()/2-embeddedImg.getWidth()/2,220,embeddedImg.getWidth(),embeddedImg.getHeight());
    }
    
private:
    EmbeddedImage embeddedImg;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AboutWindowContentComp)
};

//==============================================================================
class AboutWindow   : public DocumentWindow
{
public:
    AboutWindow (const String& name, Colour backgroundColour, int buttonsNeeded)
    : DocumentWindow (name, backgroundColour, buttonsNeeded)
    {
        setContentComponentSize(333, 480);
        setContentOwned(new AboutWindowContentComp(),false);
        
        Rectangle<int> area (0, 0, 333, 480);
        const RectanglePlacement placement (RectanglePlacement::xLeft+ RectanglePlacement::yTop + RectanglePlacement::doNotResize);
        Rectangle<int> result (placement.appliedTo (area, Desktop::getInstance().getDisplays().getMainDisplay().userArea.reduced (100)));
        setBounds (result);
        
        setResizable (false, false);
        setUsingNativeTitleBar (true);
        setVisible (true);
    }
    
    void closeButtonPressed(){
        delete this;
    }
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AboutWindow)
};

#endif  // ABOUTWINDOW_H_INCLUDED
