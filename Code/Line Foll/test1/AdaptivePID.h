#ifndef AdaptivePID_h
#define AdaptivePID_h

#include "Arduino.h"

class AdaptivePID {
public:
    AdaptivePID(double Kp, double Ki, double Kd);
    void setCoefficients(double Kp, double Ki, double Kd);
    double compute(double input);
    void printCoefficients();  

private:
    double _Kp, _Ki, _Kd;
    double _gammaP, _gammaI, _gammaD;
    double _input[3];
    double _outputP[3];
    double _outputI[4];
    double _outputD[3];
    double _outputM[3];
};

#endif
