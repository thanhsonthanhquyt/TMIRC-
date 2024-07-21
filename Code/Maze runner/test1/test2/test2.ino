
boolean onoff = 0;
int val, cnt = 0, v[3]; 

float Kp = 0;
float Ki = 0;
float Kd = 0;
uint8_t multiP = 1;
uint8_t multiI = 1;
uint8_t multiD = 1;


void setup() {
  Serial.begin(9600);
  
}

void loop() {
  if (Serial.available()) {
    while(Serial.available() == 0);
    valuesread();
    processing();
  }

}

//  phân chia từng lệnh.
//  mỗi lệnh nó sẽ nhận được 2 byte.
void valuesread() {
  val = Serial.read();
  cnt++;
  v[cnt] = val;
  if (cnt == 2)
    cnt = 0;
}

//2 giá trị đọc được gán.
void processing() {
  int a = v[1];
  if (a == 1) {
    Kp = v[2];
  }
  if (a == 2) {
    multiP = v[2];
  }
  if (a == 3) {
    Ki = v[2];
  }
  if (a == 4) {
    multiI = v[2];
  }
  if (a == 5) {
    Kd = v[2];
  }
  if (a == 6) {
    multiD = v[2];
  }
  if (a == 7) {
    onoff = v[2];
  }
}
