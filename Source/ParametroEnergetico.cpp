/*
  ==============================================================================

    ParametroEnergetico.cpp
    Created: 26 Oct 2015 11:13:39am
    Author:  Juan Londoño

  ==============================================================================
*/

#include "ParametroEnergetico.h"


ParametroEnergetico::ParametroEnergetico(double FrecuenciaMuestreo, /*float Lim1t, float Lim2t, float Lim3t, float Lim4t, */bool dB){
    SampleRate=FrecuenciaMuestreo;
//    Lim1muestras=SampleRate*Lim1t;
//    Lim2muestras=SampleRate*Lim2t;
//    Lim3muestras=SampleRate*Lim3t;
//    Lim4muestras=SampleRate*Lim4t;
    ParametrodB=dB;
}

ParametroEnergetico::~ParametroEnergetico(){
    
}

void ParametroEnergetico::CambiarLimites(float Lim1t, float Lim2t, float Lim3t, float Lim4t){
    Lim1muestras=SampleRate*Lim1t; //Cambia los limites de tiempo (como se ingresan) a muestras.
    Lim2muestras=SampleRate*Lim2t;
    Lim3muestras=SampleRate*Lim3t;
    Lim4muestras=SampleRate*Lim4t;
}

void ParametroEnergetico::relacionEnergetica(Buffer *entradai, float &salidai /*, int Longitud*/, int groupdelayi){
    float sumaNum, sumaDen;
    sumaNum=0;
    sumaDen=0;
    float *Signal=entradai->getData(); //Mirar si es mejor definir esta variable en el .h y destruirla.
    for (int i=Lim1muestras+groupdelayi; i<=Lim2muestras+groupdelayi; ++i) {
        if (i<entradai->getSize()){ //Verifica que no se sobrepasen los limites de la señal
            sumaNum+=(Signal[i]*Signal[i]);
        }else{
            break;
        }
        //sumaNum+=std::pow(Signal[i], 2);
    }
    for (int i=Lim3muestras+groupdelayi; i<Lim4muestras+groupdelayi; ++i){
        if (i<entradai->getSize()){
            sumaDen+=(Signal[i]*Signal[i]);
        }else{
            break;
        }
        
    }
    if(sumaDen==0){
        salidai=0.0; //Este es por seguridad en caso de que el buffer de entrada no tenga nada y el denominador quede en cero.
    }else{
        if (ParametrodB) {
            salidai=10*log10(sumaNum/sumaDen);
        }else{
            salidai=sumaNum/sumaDen;
        }
    }
}

void ParametroEnergetico::relacionEnergeticaTS(Buffer *entradai, float &salidai/*, int Longitud, int groupdelayi*/){
    //En el TS no se corrigen las muestras porque igual se toma toda en cuenta toda la señal
    float sumaNum, sumaDen;
    sumaNum=0;
    sumaDen=0;
    float *Signal=entradai->getData();
    for (int i=Lim1muestras; i<=Lim2muestras; ++i) {
        if (i<entradai->getSize()){ //Verifica que no se sobrepasen los limites de la señal
            sumaNum+=(i/SampleRate)*(Signal[i]*Signal[i]);
        }else{
            break;
        }
    }
    for (int i=Lim3muestras; i<Lim4muestras; ++i){
        if (i<entradai->getSize()){
            sumaDen+=(Signal[i]*Signal[i]);
        }else{
            break;
        }
    }
    
    if(sumaDen==0){
        salidai=0.0; //Este es por seguridad en caso de que el buffer de entrada no tenga nada y el denominador quede en cero.
    }else{
        salidai=1000*sumaNum/sumaDen;
    }
}


void ParametroEnergetico::setdB(bool dB){
    ParametrodB=dB;
}

void ParametroEnergetico::CambiarFS(double nuevaFrecuenciaMuestreo){
    int oldSampleRate=SampleRate;
    SampleRate=nuevaFrecuenciaMuestreo;
    
    Lim1muestras=SampleRate*(Lim1muestras/oldSampleRate);
    Lim2muestras=SampleRate*(Lim2muestras/oldSampleRate);
    Lim3muestras=SampleRate*(Lim3muestras/oldSampleRate);
    Lim4muestras=SampleRate*(Lim4muestras/oldSampleRate);
    
}

void ParametroEnergetico::getSNR(Buffer *entradai, float &salidai, int LengthSignal, int LengthCola){
    float *Scompleta=entradai->getData();
    int Lcompleta=entradai->getSize();
    ScopedPointer<Buffer> signal=new Buffer(LengthSignal);
    ScopedPointer<Buffer> cola=new Buffer(LengthCola);
    for (int i=0; i<LengthSignal; ++i) {
        signal->getData()[i]=Scompleta[i];
    }
    for (int i=0; i<LengthCola; ++i) {
        cola->getData()[i]=Scompleta[Lcompleta-LengthCola+i];
    }
    float signalRMS=findRMS(signal->getData(), LengthSignal);
    float colaRMS=findRMS(cola->getData(), LengthCola);
    salidai=20*std::log10(signalRMS)-20*std::log10(colaRMS);
}