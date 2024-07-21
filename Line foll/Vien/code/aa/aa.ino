#include "AdaptivePID.h"  // Bao gồm thư viện AdaptivePID

#define PINBUZZER 10
#define PINBOTON 2
#define PINLED 13
#define PINDRON 3
#define PIN_Sensor_ON 11

AdaptivePID pid(1.0, 0.0, 0.0);  // Khởi tạo đối tượng PID với các hệ số Kp, Ki, Kd ban đầu

int velocidad_1 = 50;
int velocidad_2 = 80;
int velocidad_3 = 100;
int velocidad_4 = 120;
int velocidad_5 = 150;

int posicion_ideal = 0;

int base = 50;
int error_pasado = 0;

void setup() {
  Serial.begin(115200);
  Peripherals_init();
  TB6612FNG_init();
  Sensors_init();

  pinMode(PINDRON, OUTPUT);
  digitalWrite(PINLED, LOW);
  digitalWrite(PINDRON, LOW);
  delay(500);

  Motores(0, 0);

  Serial.println("Programa Básico");

  delay(500);
  WaitBoton();
  calibracion();

  while (!digitalRead(PINBOTON)) {
    change();
  }
  tone(PINBUZZER, 2000, 100);
  delay(1000);
}

void loop() {
  digitalWrite(PINDRON, HIGH);

  int p = GetPos();

  detectGeo();

  int error = p - posicion_ideal;

  // Tính toán PID bằng AdaptivePID.h
  double correction_power = pid.compute(error);

  if (abs(error) < 5) {
    // Nếu sai số nhỏ hơn 5, ngừng điều chỉnh
    digitalWrite(PINDRON, LOW);
  }

  Serial.println(error);

  if (correction_power > 255) {
    correction_power = 255;
  } else if (correction_power < -255) {
    correction_power = -255;
  }

  Motores(base + correction_power, base - correction_power);
  error_pasado = error;
}



int setpoint = 0;
int last_error = 0;
int pot_limite = 250;

int PIDLambo(int pos, float Kp, float Kd) {

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

int fin = 0;

int l_geo, ll_geo, lll_geo;

int umbral = 830;
int geo = 0;

int HL, HR = 0;

void Read_hits() {

  HL = analogRead(A0);
  HR = analogRead(A7);

  if (HL > umbral) {
    HL = 0;
  } else {
    HL = 1;
  }

  if (HR > umbral) {
    HR = 0;
  } else {
    HR = 1;
  }
}

void detectGeo() {

  Read_hits();


  if ((HL == 0) && (HR == 0)) {
    geo = 0;
  }

  if ((HL == 1) && (HR == 0)) {
    geo = 1;
  }

  if ((HL == 0) && (HR == 1)) {
    geo = 2;
  }

  if ((HL == 1) && (HR == 1)) {
    geo = 3;
  }

  if (l_geo != geo) {




    if (geo == 0 && l_geo == 2 && ll_geo == 0) {

      funcion_HL();
    }
    if (geo == 0 && l_geo == 1 && ll_geo == 0) {

      funcion_HR();
    }

    if (geo == 0 && ((l_geo == 3) || (ll_geo == 3) || (lll_geo == 3))) {

      funcion_Cruce();
    }
    lll_geo = ll_geo;
    ll_geo = l_geo;
    l_geo = geo;
  }
}

void funcion_HL() {

  tone(PINBUZZER, 2000, 50);
}

void funcion_HR() {

  tone(PINBUZZER, 1500, 50);
  fin++;

  switch (fin) {
    case 1:
      Serial.println("Hit 1");
      break;
    case 2:
      delay(50);
      Serial.println("Hit 2");

      base = 0;
      Motores(0, 0);
      while (!digitalRead(PINBOTON)) {
        change();
      }
      tone(PINBUZZER, 2000, 100);

      delay(3000);
      break;
    case 3:
      Serial.println("Hit 3");
      break;
    case 4:
      Serial.println("Hit 4");

      delay(50);
      base = 0;
      Motores(0, 0);
      while (!digitalRead(PINBOTON)) {
        change();
      }
      tone(PINBUZZER, 2000, 100);

      delay(3000);
      setSpeed();
      break;
    case 5:
      Serial.println("Hit 5");
      break;
    case 6:
      Serial.println("Hit 6");

      delay(50);
      base = 0;
      Motores(0, 0);
      while (!digitalRead(PINBOTON)) {
        change();
      }
      tone(PINBUZZER, 2000, 100);

      delay(3000);
      setSpeed();
      break;
    case 7:
      Serial.println("Hit 7");
      break;
    case 8:
      Serial.println("Hit 8");

      delay(50);
      base = 0;
      Motores(0, 0);
      while (!digitalRead(PINBOTON)) {
        change();
      }
      tone(PINBUZZER, 2000, 100);

      delay(3000);
      setSpeed();
      break;

    default:
      // if nothing else matches, do the default
      // default is optional
      break;
  }
}

void funcion_Cruce() {

  tone(PINBUZZER, 2500, 50);
}




// Función para esperar el botón
void WaitBoton() {
  // Entra en un bucle infinito de espera hasta que se presione el botón
  while (!digitalRead(PINBOTON)); // No hace nada dentro del bucle, solo espera
  beep(); // Se llama a la función beep() cuando se presiona el botón
}

// Función para emitir un sonido
void beep() {
  tone(PINBUZZER, 2000, 100); // Emite un tono de 2000 Hz durante 100 ms
  delay(200); // Espera 200 ms antes de continuar
}

void Peripherals_init() {
  // Configuración de pines
  pinMode(PINBOTON, INPUT); // El pin del botón se configura como entrada
  pinMode(PINBUZZER, OUTPUT); // El pin del buzzer se configura como salida
  pinMode(PINLED, OUTPUT); // El pin del LED se configura como salida
}
int cont = 0;
int tiempo = 0;
long ahora = 0;



int flagR = 0;



void change() {

  digitalWrite(PINDRON, LOW);

  if (analogRead(A0) < umbral) {

    ahora = millis();
    flagR = 1;

    while (analogRead(A0) < umbral + 40) {
      if (tiempo > 2000) {
        setSpeed();
        Serial.println("Set velocidad ");
        Serial.println("\t");
        Serial.println(base);


        for (int i = 1; i <= cont; i++) {

          tone(PINBUZZER, 4000, 50);
          delay(500);

        }
        delay(1000);
        tiempo = 0;
        flagR = 0;
        return;

      } else {
        tiempo = millis() - ahora;
      }
    }
  }

  if ((analogRead(A0) > umbral) && (flagR == 1) ) {
    flagR = 0;
    cont++;
    tone(PINBUZZER, 3500, 50);
    delay(250);
    Serial.println(cont);

  }

  if (analogRead(A7) < umbral) {

    cont--;
    tone(PINBUZZER, 3000, 50);
    delay(250);
    Serial.println(cont);

  }


}


void setSpeed() {

  base = cont * 10;

}

int v_s_min[6] = { 1023, 1023, 1023, 1023, 1023, 1023 };
int v_s_max[6] = { 0, 0, 0, 0, 0, 0 };
volatile int s_p[6];
boolean online;

int pos;
int l_pos;


void Sensors_init() {

pinMode(PIN_Sensor_ON, OUTPUT);


}


void calibracion() {
  digitalWrite(PIN_Sensor_ON, HIGH);

  int v_s[6];

  for (int j = 0; j < 100; j++) {
    delay(30);
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
  Serial.println();

  Serial.print("Minimos");
  Serial.print("\t");

  for (int i = 0; i < 6; i++) {
    Serial.print(v_s_min[i]);
    Serial.print("\t");
  }
  Serial.println();

  Serial.print("Maximos");
  Serial.print("\t");

  for (int i = 0; i < 6; i++) {
    Serial.print(v_s_max[i]);
    Serial.print("\t");
  }
  Serial.println();
  digitalWrite(PIN_Sensor_ON, LOW);
}

void readSensors() {

  digitalWrite(PIN_Sensor_ON, HIGH);

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
    Serial.println();
  }

  digitalWrite(PIN_Sensor_ON, LOW);

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



# define AIN1 8    // pin 1 de dirección del Motor Izquierdo
# define AIN2 9    // pin 2 de dirección del Motor Izquierdo
# define PWMA 5    // pin PWM del Motor Izquierdo


# define BIN1 4    // pin 1 de dirección del Motor Derecho
# define BIN2 7    // pin 2 de dirección del Motor Derecho
# define PWMB 6    // pin PWM del Motor Derecho


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
    // si valor positivo vamos hacia adelante

    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } else {
    // si valor negativo vamos hacia atras

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