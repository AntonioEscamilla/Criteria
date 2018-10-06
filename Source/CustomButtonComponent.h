/*
  ==============================================================================

    CustomButtonComponent.h
    Created: 17 Aug 2015 6:04:31pm
    Author:  Tony Ducks

  ==============================================================================
*/

#ifndef CUSTOMBUTTONCOMPONENT_H_INCLUDED
#define CUSTOMBUTTONCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class CustomButtonComponent    : public Component
{
public:
    CustomButtonComponent(Colour c,juce::String s);
    ~CustomButtonComponent();

    void paint (Graphics&);
    void resized();
    void mouseEnter(const MouseEvent& event);
    void mouseUp(const MouseEvent& event);
    void mouseExit(const MouseEvent& event);
    void setActive(bool);
    
    class JUCE_API  Listener{
    public:
        /** Destructor. */
        virtual ~Listener()  {}
        
        /** Called when the button is clicked. */
        virtual void buttonClicked (CustomButtonComponent*) = 0;
    };
    
    void addListener (Listener* newListener);
    
private:
    bool                    isActive;
    bool                    isMouseIn;
    juce::String            botonText;
    Colour                  activeColor;
    ListenerList<Listener>  buttonListeners;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomButtonComponent)
};


#endif  // CUSTOMBUTTONCOMPONENT_H_INCLUDED
