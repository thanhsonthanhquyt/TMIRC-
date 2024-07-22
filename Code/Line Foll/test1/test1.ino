#include "pidautotuner.h"

// Khai báo các biến cần thiết
double targetInputValue = 100.0;  // Gán giá trị mục tiêu
long loopInterval = 1000;  // Khoảng thời gian vòng lặp tính bằng micro giây

// Khai báo các hàm cần thiết
double doSomethingToGetInput() {
    return analogRead(A0);  // Ví dụ: đọc giá trị từ chân analog A0
}

void doSomethingToSetOutput(double output) {
    analogWrite(9, output);  // Ví dụ: ghi giá trị đầu ra đến chân số 9
}

void setup() {
    PIDAutotuner tuner = PIDAutotuner();

    tuner.setTargetInputValue(targetInputValue);
    tuner.setLoopInterval(loopInterval);
    tuner.setOutputRange(0, 255);
    tuner.setZNMode(PIDAutotuner::ZNModeBasicPID);
    tuner.startTuningLoop(micros());

    long microseconds;
    while (!tuner.isFinished()) {
        long prevMicroseconds = microseconds;
        microseconds = micros();

        double input = doSomethingToGetInput();
        double output = tuner.tunePID(input, microseconds);
        doSomethingToSetOutput(output);

        while (micros() - microseconds < loopInterval) delayMicroseconds(1);
    }

    doSomethingToSetOutput(0);

    double kp = tuner.getKp();
    double ki = tuner.getKi();
    double kd = tuner.getKd();
}

void loop() {
    // ...
}
