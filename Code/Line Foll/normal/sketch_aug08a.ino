
#define PINBUZZER  10
#define PINBOTON  2
#define AIN1 8    // pin 1 de dirección del Motor Izquierdo
#define AIN2 9    // pin 2 de dirección del Motor Izquierdo
#define PWMA 5    // pin PWM del Motor Izquierdo


#define BIN1 4    // pin 1 de dirección del Motor Derecho
#define BIN2 7    // pin 2 de dirección del Motor Derecho
#define PWMB 6    // pin PWM del Motor Derecho


int base = 0;
float Kprop = 1.2;
float Kderiv = 7.5;
float Kinte = 0.0;
int pos;


void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);


  digitalWrite(13, LOW);
  Serial.println("hola");
  Motores(0, 0);
  WaitBoton();
  digitalWrite(13, HIGH);
  Peripherals_init();

  beep();
  delay(1000);
  calibracion();

  digitalWrite(13, LOW);
  tone(PINBUZZER, 1500, 50);
  delay(70);
  tone(PINBUZZER, 1500, 50);
  delay(70);

  WaitBoton();
  delay(1000);
  digitalWrite(13, HIGH);

}


void loop() {

  int line_position = GetPos();
  int Correction_power = PIDLambo(line_position, Kprop, Kderiv, Kinte);
  Motores(base + Correction_power, base + -Correction_power);
  Serial.print(line_position);
  Serial.print("\t");
  Serial.println(Correction_power);


}


int setpoint = 0;
int last_error = 0;
int pot_limite = 250;

int PIDLambo(int POS, float Kp, float Kd, float Ki) {

  int error = pos - setpoint;
  int derivative = error - last_error;
  last_error = error;
  int pot_giro = (error * Kp + derivative * Kd);



  if (pot_giro > pot_limite)
    pot_giro = pot_limite;
  else if (pot_giro < -pot_limite)
    pot_giro = -pot_limite;
  return pot_giro;


}



void Peripherals_init() {
  pinMode(PINBOTON, INPUT);
  pinMode(PINBUZZER, OUTPUT);
}


void WaitBoton() {   // Entra en un bucle infinito de espera.
  while (!digitalRead(PINBOTON));  // Se sale del bucle cuando se aprieta el botón
  tone(PINBUZZER, 2000, 100);      // Cuando sale del bucle, suena el buzzer
}

void beep() {
  tone(PINBUZZER, 2000, 100);
  delay(200);
}



int v_s_min[6] = {1023, 1023, 1023, 1023, 1023, 1023};
int v_s_max[6] = {0, 0, 0, 0, 0, 0};
volatile int s_p[6];
boolean online;

int l_pos;



void calibracion() {

  int v_s[6];

  for (int j = 0; j < 100; j++) {
    delay(10);
    v_s[0] = analogRead(A6);
    v_s[1] = analogRead(A5);
    v_s[2] = analogRead(A4);
    v_s[3] = analogRead(A3);
    v_s[4] = analogRead(A2);
    v_s[5] = analogRead(A1);

    for (int i = 0; i < 6; i++) {

      Serial.print(v_s[i]);
      Serial.print("\t");

    }
    Serial.println();

    for (int i = 0; i < 6; i++) {
      if (v_s[i] < v_s_min[i]) {
        v_s_min[i] = v_s[i];
      }
    }


    for (int i = 0; i < 6; i++) {
      if (v_s[i] > v_s_max[i]) {
        v_s_max[i] = v_s[i];
      }
    }
  }

  beep();
  beep();

  Serial.println();
  Serial.print("Mínimos ");
  Serial.print("\t");

  for (int i = 0; i < 6; i++) {

    Serial.print(v_s_min[i]);
    Serial.print("\t");

  }
  Serial.println();
  Serial.print("Máximos ");
  Serial.print("\t");

  for (int i = 0; i < 6; i++) {

    Serial.print(v_s_max[i]);
    Serial.print("\t");

  }
  Serial.println();
  Serial.println();
  Serial.println();






}

void readSensors() {

  volatile int s[6];



  s[0] = analogRead(A6);
  s[1] = analogRead(A5);
  s[2] = analogRead(A4);
  s[3] = analogRead(A3);
  s[4] = analogRead(A2);
  s[5] = analogRead(A1);

  for (int i = 0; i < 6; i++) {
    if (s[i] < v_s_min[i]) {
      s[i] = v_s_min[i];
    }

    if (s[i] > v_s_max[i]) {
      s[i] = v_s_max[i];
    }
    s_p[i] = map(s[i], v_s_min[i], v_s_max[i], 100, 0);
  }


  volatile int sum = s_p[0] + s_p[1] + s_p[2] + s_p[3] + s_p[4] + s_p[5];
  if (sum > 100) {
    online = 1;

  } else {
    online = 0;
    sum = 100;
  }

  
    if (online) {
      for (int i = 0; i < 6; i++) {
        Serial.print(s_p[i]);
        Serial.print("\t");
      }
      //Serial.println();
    }

  

}




int GetPos() {
  readSensors();
  int prom = -2.5 * s_p[0] - 1.5 * s_p[1] - 0.5 * s_p[2] + 0.5 * s_p[3] + 1.5 * s_p[4] + 2.5 * s_p[5];
  int sum = s_p[0] + s_p[1] + s_p[2] + s_p[3] + s_p[4] + s_p[5];

  if (online) {
    pos = int(100.0 * prom / sum);
  } else {
    if (l_pos < 0) {
      pos = -255;
    }
    if (l_pos >= 0) {
      pos = 255;
    }
  }
  l_pos = pos;
  return pos;
}


void TB6612FNG_init() {

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

}

void MotorIz(int value) {
  if (value >= 0) {
    // si valor positivo vamos hacia adelante

    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else {
    // si valor negativo vamos hacia atras

    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    value *= -1;
  }

  // Setea Velocidad

  analogWrite(PWMA, value);
}


void MotorDe(int value) {
  if (value >= 0) {
    // Nếu giá trị dương chúng ta đi tiếp

    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } else {
    // nếu giá trị âm chúng ta đi lùi

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    value *= -1;
  }

  // Setea Velocidad

  analogWrite(PWMB, value);
}


void Motores(int left, int right) {
  MotorIz(left);
  MotorDe(right);
}
