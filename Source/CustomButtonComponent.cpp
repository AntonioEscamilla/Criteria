/*
  ==============================================================================

    CustomButtonComponent.cpp
    Created: 17 Aug 2015 6:04:31pm
    Author:  Tony Ducks

  ==============================================================================
*/

#include "CustomButtonComponent.h"

/*************************************************************************/
CustomButtonComponent::CustomButtonComponent(Colour c,juce::String s):isActive(false),isMouseIn(false){
    activeColor = c;
    botonText = s;
}

/*************************************************************************/
CustomButtonComponent::~CustomButtonComponent(){
}

/*************************************************************************/
void CustomButtonComponent::paint (Graphics& g){
    g.fillAll (Colour(0xff333333));   // background igual que el ParentComponent
    
    if (isMouseIn || isActive) {
        g.fillAll (Colour(0xff3f3f3f));
        g.setColour (Colours::white);
    }else{
        g.setColour(Colour::greyLevel (0.5f));
    }
    g.setFont (14.0f);
    g.drawText (botonText, getLocalBounds(),Justification::centred, true);
    
    if (isActive) {
        g.setColour (Colour(activeColor));
        g.fillRect(0, 0, 5, getHeight());
    }
}

/*************************************************************************/
void CustomButtonComponent::resized(){

}

/*************************************************************************/
void CustomButtonComponent::setActive(bool decision){
    isActive = decision;
}

/*************************************************************************/
void CustomButtonComponent::mouseEnter(const MouseEvent& event){
    isMouseIn=true;
    repaint();
}

/*************************************************************************/
void CustomButtonComponent::mouseExit(const MouseEvent& event){
    isMouseIn=false;
    repaint();
}

/*************************************************************************/
void CustomButtonComponent::mouseUp(const MouseEvent& event){
    Component::BailOutChecker checker (this);
    
    if (! checker.shouldBailOut()){
        buttonListeners.callChecked (checker, &CustomButtonComponent::Listener::buttonClicked, this);
    }
}

/*************************************************************************/
void CustomButtonComponent::addListener (Listener* const newListener){
    buttonListeners.add (newListener);
}