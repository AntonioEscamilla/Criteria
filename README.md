# Criteria
Open source software for the measurement of impulse responses and acoustic room parameters in concert halls and ordinary enclousures, based on the ISO 3382-1:2009 and ISO 18233: 2006 standards and programmed in C++ using the JUCE framework.

The software development, began with an analysis of different functionalities of available commercial software and the review of the state of the art on the subject. The research and study of theoretical referents allowed us to identify the different processing stages that make up the software as suggested by the ISO 18233: 2006 and ISO 3382-1: 2009 standards, to obtain the impulse response and the acoustic parameters, respectively. The software was conceived in a modular way, and implements algorithms for the generation of the excitation signal, the impulse response measurement, the filtering by octave and third octave bands, the calculation of the decay curve and the calculation of acoustic parameters.

## ISO Standards
1. The ISO 3382-1: 2009 standard specifies different methods for measuring the reverberation time in enclosures. It is a standard that applies to auditoriums, concert halls, venues for words and music. It also describes the measurement procedure, the necessary equipment and the method to evaluate the data. In addition, information is given on obtaining the acoustic parameters of the room that are derived from the impulse response.

2. The ISO 18233: 2006 standard describes the methodology of considered new measurement techniques in buildings through digital signal processing. Depending on the type of excitation signal, MLS or Sine Sweep, the processing technique that relates the excitation signal and the response of the room, differs. These methods are alternatives to those described in the ISO 3382-1: 2009 standard, and raise indirect processes to obtain the impulse response through operations such as the Hadamard transform, in the case of MLS signals; and deconvolution, if Sine Sweep test signals are used.

## Overview
The software was designed to be multiplatform and was codified using the C ++ programming language and the JUCE framework, which is a broad, extensive and powerful set of tools for the creation of audio applications, published under the GNU Public License. The classes that make up this framework include models for the integration of processes that go from the construction of graphic interfaces to the detection of hardware for audio applications.

For the calculation of acoustic parameters, the App allows to import a previously recorded impulse response or to perform an impulse response measurement by configuring the audio capture and playback hardware first and then the parameters of the test signal, As shown in Fig. 1. 

![Criteria_1](https://github.com/AntonioEscamilla/images-in-readMe/blob/master/Criteria/Criteria_1.png)

At any time, the user can choose between octave or third octave band filters for frequency based analysis to calculate the temporal parameters: EDT, T10, T20, T30 and the energy parameters: C50, C80, D50, STearly, STlate, Ts and SNR, depending on how they are defined in the ISO 3382-1: 2009 standard, see Fig. 2. In addition, the calculated acoustic parameters data can be exported as a text file to facilitate its analysis outside the application.

![Criteria_2](https://github.com/AntonioEscamilla/images-in-readMe/blob/master/Criteria/Criteria_2.png)

## Measurement with Sinusoidal Sweep Signals
Modern systems for measuring impulse responses in acoustic enclosures are based on using, as a system input, a known test signal; and with this, measure the response of the room. The conditions for the choice of the test signal, should be: The perfect reproducibility of the signal, the ability to maximize the signal-to-noise ratio in the impulse response, and allow for the elimination of non-linear artifacts that may appear in the impulse response. Traditionally, 2 types of signals have been used: Pseudorandom periodicals with practically the same stochastic properties of white noise, known as MLS signals and/or sinusoidal sweep signals of changing frequency over time (Sine Sweep).

As a result of the previous analysis, it was decided to use Sine Sweep signals to excite the enclosure. The user interface allows to choose between linear or logarithmic sinusoidal signals, define their duration and enter an estimated reverberation time in the room to optimize the capture time of the system response. The sweep in frequency, which should cover the entire audible range (20Hz-20KHz), extends at both ends to minimize the influence of the transients introduced, both at the beginning and at the end, by the mathematical function for the calculation of the sinusoidal signal.

## Results and Comparison
For the analysis of results, measurements were taken in several acoustics rooms of the University of San Buenaventura Medellín, which present different characteristics in terms of surfaces, volume, noise level, use, among others. In each of these enclosures, measurement processes were carried out using the Criteria, Dirac® and Aurora® software, taking into account that the configuration parameters of the measurement with each software were the same. In addition, the entire electro-acoustic chain for the measurements was the same, in order to guarantee a well-established point of comparison. Below are some of the most representative results.

* C80 Energy Parameter

![Criteria_4](https://github.com/AntonioEscamilla/images-in-readMe/blob/master/Criteria/criteria_4.png)

* D50 Energy Parameter

![Criteria_8](https://github.com/AntonioEscamilla/images-in-readMe/blob/master/Criteria/criteria_8.png)

* RT30 Time Parameter

![Criteria_5](https://github.com/AntonioEscamilla/images-in-readMe/blob/master/Criteria/criteria_5.png)

* RT20 Time Parameter

![Criteria_6](https://github.com/AntonioEscamilla/images-in-readMe/blob/master/Criteria/criteria_6.png)

* EDT Time Parameter

![Criteria_7](https://github.com/AntonioEscamilla/images-in-readMe/blob/master/Criteria/criteria_7.png)

## About this Software
Criteria was developed by  **Yonny Quiceno, Alejandro Mejia, Juan F. Londoño** and **Antonio Escamilla Pinilla**, in the context of a research project entitled **Desarrollo de un Software de Medición para la Obtención de Parámetros Acústicos en Recintos Cerrados Soportado en la Norma ISO-3382**.

The full design, implementation and analisys process was presented in the AES 2016 Latin American Convention, Academic Event with an international peer review committee.
