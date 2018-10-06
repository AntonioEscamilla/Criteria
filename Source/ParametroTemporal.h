/*
  ==============================================================================

    ParametroTemporal.h
    Created: 27 Oct 2015 8:42:02pm
    Author:  Juan Londoño

  ==============================================================================
*/

#ifndef PARAMETROTEMPORAL_H_INCLUDED
#define PARAMETROTEMPORAL_H_INCLUDED
#include "../JuceLibraryCode/JuceHeader.h"
using namespace drow;

class ParametroTemporal {
public:
    ParametroTemporal(double FrecuenciaMuestreo);
    ~ParametroTemporal();
    
    /**Se pasa la respuesta al impulso IR y guarda el decaimiento en dB en la variable EdB
     El decaimiento energético en dB va de 0 a menos(-)infinito*/
    void integralSchroeder(float *IR, float *EdB, int LongitudIR);
    
    /**Almacena en 'regresion' la regresión lineal sobre los datos en 'vector'. 'Longitud' es la longitud de 'vector'. 
     Regresion=a+bt. También almacena en 'Rcuad' el coeficiente de correlacion r^2*/
    void regresionLineal(float *vector, float *regresion, float &a, float&b, int Longitud, float &Rcuad);//NO se necesita float *regresion
    
    /**Toma la IR de entrada y guarda en salida el parametro temporal tomando como base el decaimiento energético ente EdB1 y EdB2 [dB] (Hay que tener en cuenta que el decaimiento va de 0 a menos(-)infinito)
    Ej: Para hallar RT30 se deben poner los limites del decaimiento energético entre EdB1=-5 y EdB2=-35 [dB] */
    void almacenarParametroTemporal(Buffer *entradai, float &salida, float EdB1, float EdB2, float &Xi);
    
protected:
    /**Corta el vector entre las muestra m1 y m2 y lo almacena en vectorC que tiene Longitud m2-m1*/
    void cortarVector(float *vector, float *vectorC, int m1, int m2);//No se revisa que los limites no excedan la longitud de vector
    
    float sumar(float *vector, int Longitud);
    float encontrarMaximo(float *vector, int Longitud); //Mirar si se puede utilizar un objeto diferente de float (ej. Buffer) que haga estas operaciones.
    float encontrarMinimo(float *vector, int Longitud);
    //void multiplicarVectoresPuntoPunto(float *vector1, float *vector2, float *Multiplicado, int Longitud);
    /**Devuelve el indice i en donde el 'valor' se aproxima mas al valor de vector[i]*/
    int encontrarMuestraAprox(float *vector, float valor, int Longitud);
    
private:
    double SampleRate;
    
    //Variables 
    ScopedPointer<float> IRi;
    ScopedPointer<float> EdBi;
    ScopedPointer<float> EdBcortado;
    ScopedPointer<float> EdBregresion;
    ScopedPointer<float> IRcuad;
    ScopedPointer<float> E;
    ScopedPointer<float> tiempo;
    ScopedPointer<float> txL;
    ScopedPointer<float> tCuad;
    ScopedPointer<float> vVector;
    ScopedPointer<float> vRegresion;
    ScopedPointer<float> vectorAbs;
};



#endif  // PARAMETROTEMPORAL_H_INCLUDED
