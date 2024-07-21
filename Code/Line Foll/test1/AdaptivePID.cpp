#include "AdaptivePID.h"

AdaptivePID::AdaptivePID(double Kp, double Ki, double Kd) {
    setCoefficients(Kp, Ki, Kd);
    _gammaP = 500;
    _gammaI = 20;
    _gammaD = 100;
}

void AdaptivePID::setCoefficients(double Kp, double Ki, double Kd) {
    _Kp = Kp;
    _Ki = Ki;
    _Kd = Kd;
}

double AdaptivePID::compute(double input) {
    // Update input history
    _input[2] = _input[1];
    _input[1] = _input[0];
    _input[0] = input;

    // GP
    _outputP[2] = _outputP[1];
    _outputP[1] = _outputP[0];
    _outputP[0] = 4.2159 * _input[0] + 1.6000 * _outputP[1] - 0.6800 * _outputP[2];

    // GI
    _outputI[3] = _outputI[2];
    _outputI[2] = _outputI[1];
    _outputI[1] = _outputI[0];
    _outputI[0] = 4.2159 * _input[0] + 2.5886 * _outputI[1] - 2.2096 * _outputI[2] + 0.6210 * _outputI[3];

    // GD
    _outputD[2] = _outputD[1];
    _outputD[1] = _outputD[0];
    _outputD[0] = 84.318 * (_input[0] - _input[1]) + 1.9021 * _outputD[1] - 0.9048 * _outputD[2];

    // ham mau
    _outputM[2] = _outputM[1];
    _outputM[1] = _outputM[0];
    _outputM[0] = 12.2899 * _input[0] + 1.4548 * _outputM[1] - 0.5565 * _outputM[2];

    // PID voi luat MIT( MIT RULE)
    double pidOutput = (_gammaP * _outputP[0] + _gammaI * _outputI[0] + _gammaD * _outputD[0] + _outputM[0])/120;

    return pidOutput;
}
  
void AdaptivePID::printCoefficients() {
    Serial.print("Kp: ");
    Serial.println(_Kp);
    Serial.print("Ki: ");
    Serial.println(_Ki);
    Serial.print("Kd: ");
    Serial.println(_Kd);
}
