/*
  ==============================================================================

    ParametroEnergetico.h
    Created: 26 Oct 2015 11:13:39am
    Author:  Juan Londoño

  ==============================================================================
*/

#ifndef PARAMETROENERGETICO_H_INCLUDED
#define PARAMETROENERGETICO_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h" //JUCE
using namespace drow;
using namespace std;

/*Clase que calcula relaciones energéticas early-to-late, para encontrar parámetros acústicos. Relacion entre (Energía entre los limites Lim1t y Lim2t) y (Energía entre los limites Lim3t y Lim4t)*/

//NOTAS:
//Es mejor corregir el groupdelay desde afuera, justo despues de los filtros.

class ParametroEnergetico {
    
public:
    ParametroEnergetico(double FrecuenciaMuestreo, bool dB);
    ~ParametroEnergetico();
    
    /** Se guarda en 'salidai' la relación energética utilizando la señal de 'entrada'. 
     El 'groupdelayi' es el retraso en muestras debido a los filtros para la 'entradai'*/
    void relacionEnergetica(Buffer *entradai, float &salidai/*, int Longitud*/, int groupdelayi);
    
    /**La relacion energética cambia cuando el parámetro es 'Center Time' Ts*/
    void relacionEnergeticaTS(Buffer *entradai, float &salidai/*, int Longitud, int groupdelayi*/);
    
    void getSNR(Buffer *entradai,float &salidai, int LengthSignal, int LengthCola);
    
    /**Limites de integracion (en tiempo) para la relacion energética. 
    La integral del numerador va de Lim1t a Lim2t. 
    La integral del denominador va de Lim3t a Lim4t */
    void CambiarLimites(float Lim1t, float Lim2t, float Lim3t, float Lim4t);
    
    /**True si la relacion energetica debe ser en decibeles*/
    void setdB(bool dB);
    
    void CambiarFS(double nuevaFrecuenciaMuestreo);
    
private:
    double SampleRate;
    int Lim1muestras, Lim2muestras, Lim3muestras, Lim4muestras;
    bool ParametrodB;
    
};



#endif  // PARAMETROENERGETICO_H_INCLUDED
