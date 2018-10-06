/*
  ==============================================================================

    ParametroTemporal.cpp
    Created: 27 Oct 2015 8:42:02pm
    Author:  Juan Londoño

  ==============================================================================
*/

#include "ParametroTemporal.h"

//==============================================================================
ParametroTemporal::ParametroTemporal(double FrecuenciaMuestreo){
    SampleRate=FrecuenciaMuestreo;
}

ParametroTemporal::~ParametroTemporal(){
    
}

//==============================================================================
void ParametroTemporal::almacenarParametroTemporal(Buffer *entradai, float &salida, float EdB1, float EdB2, float &Xi){
    int LongitudIR=entradai->getSize();
    
    ///*float */IRi=entradai->getData();
    IRi=new float[LongitudIR];
    FloatVectorOperations::copy(IRi, entradai->getData(), LongitudIR);
    
    
    //Curva de decaimiento i en dB EdB
    /*float */EdBi=new float[LongitudIR-1];
    
    integralSchroeder(IRi, EdBi, LongitudIR);
    
    //Encontrar las muestras para poder hacer la regresion sobre un tramo de EdB
    int muestraEdB1, muestraEdB2;
    muestraEdB1=encontrarMuestraAprox(EdBi, EdB1, LongitudIR-1);
    muestraEdB2=encontrarMuestraAprox(EdBi, EdB2, LongitudIR-1);
    //Selecciona el tramo de EdB que decae de EdB1 a EdB2
    
    /*float */EdBcortado=new float[muestraEdB2-muestraEdB1+1];
    /*float */EdBregresion=new float[muestraEdB2-muestraEdB1+1 ];

    float aISO;
    float bISO;//Pendiente de la regresion. RT=-60/bISO.
    float Rcuad;
    cortarVector(EdBi, EdBcortado, muestraEdB1, muestraEdB2);
    
    regresionLineal(EdBcortado, EdBregresion, aISO, bISO, muestraEdB2-muestraEdB1+1, Rcuad);
    
    Xi=1000*(1-Rcuad);
    salida=-60/(bISO*SampleRate);

}

//==============================================================================

void ParametroTemporal::integralSchroeder(float *IR, float *EdB, int Longitud){
    /*float */IRcuad=new float[Longitud];
    //multiplicarVectoresPuntoPunto(IR, IR, IRcuad, Longitud);
    FloatVectorOperations::multiply(IRcuad, IR, IR, Longitud);
    float EnergiaTotal=sumar(IRcuad, Longitud);
    
    /*float */E=new float[Longitud];
    
    float h1=0;
    for (int i=0; i<Longitud; ++i) {
        h1+=IRcuad[i];
        E[i]=EnergiaTotal-h1;
    }
    float maxE=encontrarMaximo(E, Longitud);
    
    for (int i=0; i<Longitud-1; ++i) {
        EdB[i]=10*std::log10(E[i]/maxE);
    }
}

//==============================================================================

void ParametroTemporal::regresionLineal(float *vector, float *regresion, float &a, float&b, int Longitud, float &Rcuad){
    /*float */tiempo=new float[Longitud];
    
    for (int i=0; i<Longitud; ++i) {
        tiempo[i]=i+1;
    }
    
    float Lprom=sumar(vector, Longitud)/Longitud;
    float tprom=sumar(tiempo, Longitud)/Longitud;
    
    /*float */txL=new float[Longitud];
    /*float */tCuad=new float[Longitud]; 
    
    //multiplicarVectoresPuntoPunto(tiempo, vector, txL, Longitud);
    FloatVectorOperations::multiply(txL, tiempo, vector, Longitud);
    //multiplicarVectoresPuntoPunto(tiempo, tiempo, tCuad, Longitud);
    FloatVectorOperations::multiply(tCuad, tiempo, tiempo, Longitud);
    
    b=(sumar(txL, Longitud)-Longitud*tprom*Lprom)/(sumar(tCuad, Longitud)-Longitud*tprom*tprom);
    a=Lprom-b*tprom;
    
    /*float */vVector=new float[Longitud];
    /*float */vRegresion=new float[Longitud];
    for (int i=0; i<Longitud; ++i) {
        regresion[i]=a+b*tiempo[i];
        
        vVector[i]=(vector[i]-Lprom)*(vector[i]-Lprom);
        vRegresion[i]=(regresion[i]-Lprom)*(regresion[i]-Lprom);
    }
    
    Rcuad=sumar(vRegresion, Longitud)/sumar(vVector, Longitud);
    
}

//==============================================================================

//void ParametroTemporal::multiplicarVectoresPuntoPunto(float *vector1, float *vector2, float *Multiplicado, int Longitud){
//    for (int i=0; i<Longitud; ++i) {
//        Multiplicado[i]=vector1[i]*vector2[i];
//    }
//}

//==============================================================================

float ParametroTemporal::sumar(float *vector, int Longitud){
    float suma=0.0;
    for (int i=0; i<Longitud; ++i) {
        suma+=vector[i];
    }
    return suma;
}

//==============================================================================

float ParametroTemporal::encontrarMaximo(float *vector, int Longitud){
    float max=0.0; //Cambiar por un mejor valor que encuentre el maximo incluyendo valores negativos.
    for (int i=0; i<Longitud; ++i) {
        if (vector[i]>max) { 
            max=vector[i];
        }
    }
    return max;
}

//==============================================================================

float ParametroTemporal::encontrarMinimo(float *vector, int Longitud){
    float min=Longitud; //Cambiar por un mejor valor que encuentre el minimo, no solo los que pueden ser mayores a Longitud
    for (int i=0; i<Longitud; ++i) {
        if (vector[i]<min) {
            min=vector[i];
        }
    }
    return min;
}

//==============================================================================

int ParametroTemporal::encontrarMuestraAprox(float *vector, float valor, int Longitud){
    /*float */vectorAbs=new float[Longitud]; 
    for (int i=0; i<Longitud; ++i) {
        vectorAbs[i]=std::abs(vector[i]-valor);
    }
    float min=encontrarMinimo(vectorAbs, Longitud);
    int muestra=0;
    for (int i=0; i<Longitud; ++i) {
        if (min==vectorAbs[i]) {
            muestra=i;
            break;
        }
    }
    return muestra;
}

//==============================================================================

void ParametroTemporal::cortarVector(float *vector, float *vectorC, int m1, int m2){
    if (m1<m2) {
        for (int i=0; i<=(m2-m1); ++i) {
            vectorC[i]=vector[m1+i];
        }
    }
}
