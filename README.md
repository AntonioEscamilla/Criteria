# Criteria
Open source software for the measurement of impulse responses and acoustic room parameters in concert halls and ordinary enclousures, based on the ISO 3382-1:2009 and ISO 18233: 2006 standards and programmed in C++ using the JUCE framework.

The software development, began with an analysis of different functionalities of available commercial software and the state of the art on the subject. The research and the study of theoretical referents allowed us to identify the different processing stages that make up the software as suggested by the ISO 18233: 2006 and ISO 3382-1: 2009 standards, to obtain the impulse response and the acoustic parameters, respectively. The software was conceived in a modular way, and implements algorithms for the generation of the excitation signal, the impulse response measurement, the filtering by octave and third octave bands, the calculation of the decay curve and the calculation of acoustic parameters.

## ISO Standards
1. The ISO 3382-1: 2009 standard specifies different methods for measuring the reverberation time in enclosures. It is a standard that applies to auditoriums, concert halls, venues for words and music. It also describes the measurement procedure, the necessary equipment and the method to evaluate the data. In addition, information is given on obtaining the acoustic parameters of the room that are derived from the impulse response.

2. The ISO 18233: 2006 standard describes the methodology of considered new measurement techniques in buildings through digital signal processing. Depending on the type of excitation signal, MLS or Sine Sweep, the processing technique that relates the excitation signal and the response of the room, differs. These methods are alternatives to those described in the ISO 3382-1: 2009 standard, and raise indirect processes to obtain the impulse response through operations such as the Hadamard transform, in the case of MLS signals; and deconvolution, if Sine Sweep test signals are used.

