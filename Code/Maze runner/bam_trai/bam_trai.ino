/*
 1---  1     dừng
 -1--  2      
 --1-  3
 ---1  4     dừng
 11--  5
 -11-  6     ngõ 3
 --11  7
 1--1  8     real thẳng
 1-1-  9
 -1-1  10
 -111  11    quay trái
 1-11  12
 11-1  13
 111-  14    quay phải
 ----  15    hoàn thành/ngõ 4
 1111  16    quay đầu
 */
 
int coi = 12;
int led = 9;
int nut1 = A1;
int nut2 = A0;



// IR cambien
int cambien1 = A4;  
int cambien2 = A5;
int cambien3 = A6;
int cambien4 = A7;      
int cambien[4] = {0, 0, 0, 0};

// Drive
int ENA = 10;              
int motorA1 = 3;
int motorA2 = 4;
int motorB1 = 5;
int motorB2 = 6;
int ENB = 9 ;  
int STBY = 2;

int toc_do_ban_dau = 250; 



// PID
float Kp = 25;
float Ki = 0;
float Kd = 15;

float tin_hieu = 0, P = 0, I = 0, D = 0, gia_tri_PID = 0;
float tin_hieu_tam = 0, I_tam = 0;

int flag = 0;
unsigned long time;

void setup()
{
  pinMode(cambien1, INPUT_PULLUP);
  pinMode(cambien2, INPUT_PULLUP);
  pinMode(cambien3, INPUT_PULLUP);
  pinMode(cambien4, INPUT_PULLUP);
  pinMode(nut1, INPUT_PULLUP);

  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  digitalWrite(STBY, HIGH);
  time = millis();
  Serial.begin(9600);                   
  delay(500);
  Serial.println("San Sang");
  delay(1000);
}

void loop ()
{
   analogWrite(ENA, 90);
      analogWrite(ENB, 100);
  digitalWrite(motorA1, HIGH);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, HIGH);
  digitalWrite(motorB2, LOW);
// test1_bam_trai();
}

void test1_bam_trai()
{
  int trang_thai_nut = digitalRead(nut1);
  if (trang_thai_nut == HIGH ){
    static bool kiem_tra_in = false;
    if (!kiem_tra_in) {
    Serial.println("San Sang Mode 1");
    delay(500);
    Serial.println("Mode 1");
    kiem_tra_in = true;
     }
    serial_blt();
  }
  else {
  static bool kiem_tra_in = false;
  if (!kiem_tra_in) {
  Serial.println("San Sang Mode 2");
  delay(2000);
  Serial.println("Mode 2");
  kiem_tra_in = true;
  }
   
 doc_cam_bien();
  
  Serial.print("tin hieu: " + String(tin_hieu));

//dừng
  if ((tin_hieu == 1) || (tin_hieu ==4)){
    dung();
    delay(100);
    if (flag == 0){
     analogWrite(ENA, 80);
     analogWrite(ENB, 80);   
     if (tin_hieu == 1) {
       do { quayphai180();
       doc_cam_bien();
       if (tin_hieu == 15) {
        dung();
        digitalWrite(coi, HIGH);
        in_trang_thai();
        flag = 1;
          }
         }
       while (tin_hieu == 8);
     }
     else if (tin_hieu == 4) {
      do { quaytrai180();
      doc_cam_bien();
      if (tin_hieu ==15) {
        dung();
        digitalWrite(coi, HIGH);
        in_trang_thai();
        flag =1;
        }
       }
      while (tin_hieu == 8);
     }
    }
  }
//quay trái
  else if (tin_hieu == 11){
    do {
      quaytrai90();
      analogWrite(ENA, 90);
      analogWrite(ENB, 100);
      in_trang_thai();
      doc_cam_bien();
     }
    while (tin_hieu == 8);
  }
//quay phải
  else if (tin_hieu == 14){
    do {
      quayphai90();
      analogWrite(ENA, 100);
      analogWrite(ENB, 90);
      in_trang_thai();
      doc_cam_bien();      
    }
     while (tin_hieu == 8);
  }
//quay 180
   else if (tin_hieu == 16) {
    do { 
      quaytrai180();
      analogWrite(ENA, 90);
      analogWrite(ENB, 90); 
      in_trang_thai();
      doc_cam_bien();          
    }
     while (tin_hieu == 8);
   }
//vượt ngõ 4
   else if (tin_hieu == 15) { tin_hieu = 8; in_trang_thai();}
//vượt ngõ 3
   else if (tin_hieu == 60) { tin_hieu = 11; in_trang_thai();}
//đi thẳng
   else {
   dieuchinhPID();
   dithangPID();
   in_trang_thai();
    }
   }
   
}


 
/////////////////////////////////////////////
void doc_cam_bien()
{
  cambien[0] = digitalRead(cambien1);
  cambien[1] = digitalRead(cambien2);
  cambien[2] = digitalRead(cambien3);
  cambien[3] = digitalRead(cambien4);


  if ((cambien[0] == 1) && (cambien[1] == 0) && (cambien[2] == 0) && (cambien[3] == 0))       
    tin_hieu = 1;
  else if ((cambien[0] == 0) && (cambien[1] == 1) && (cambien[2] == 0) && (cambien[3] == 0))
    tin_hieu = 2;
  else if ((cambien[0] == 0) && (cambien[1] == 0) && (cambien[2] == 1) && (cambien[3] == 0))
    tin_hieu = 3;
  else if ((cambien[0] == 0) && (cambien[1] == 0) && (cambien[2] == 0) && (cambien[3] == 1))
    tin_hieu = 4;
  else if ((cambien[0] == 1) && (cambien[1] == 1) && (cambien[2] == 0) && (cambien[3] == 0))
    tin_hieu = 5;
  else if ((cambien[0] == 0) && (cambien[1] == 1) && (cambien[2] == 1) && (cambien[3] == 0))  
    tin_hieu = 6;
  else if ((cambien[0] == 0) && (cambien[1] == 0) && (cambien[2] == 1) && (cambien[3] == 1))
    tin_hieu = 7;
 else if ((cambien[0] == 1) && (cambien[1] == 0) && (cambien[2] == 0) && (cambien[3] == 1))
    tin_hieu = 8;
  else if ((cambien[0] == 1) && (cambien[1] == 0) && (cambien[2] == 1) && (cambien[3] == 0))
    tin_hieu = 9;
  else if ((cambien[0] == 0) && (cambien[1] == 1) && (cambien[2] == 0) && (cambien[3] == 1))
    tin_hieu = 10;
  else if ((cambien[0] == 0) && (cambien[1] == 1) && (cambien[2] == 1) && (cambien[3] == 1))
    tin_hieu = 11;
  else if ((cambien[0] == 1) && (cambien[1] == 0) && (cambien[2] == 1) && (cambien[3] == 1))
    tin_hieu = 12;
  else if ((cambien[0] == 1) && (cambien[1] == 1) && (cambien[2] == 0) && (cambien[3] == 1))
    tin_hieu = 13;
  else if ((cambien[0] == 1) && (cambien[1] == 1) && (cambien[2] == 1) && (cambien[3] == 0))
    tin_hieu = 14;
  else if ((cambien[0] == 0) && (cambien[1] == 0) && (cambien[2] == 0) && (cambien[3] == 0)) 
    tin_hieu = 15;
  else if ((cambien[0] == 1) && (cambien[1] == 1) && (cambien[2] == 1) && (cambien[3] == 1)) 
    tin_hieu = 16;
}

void dieuchinhPID()
{
  P = tin_hieu;
  I = I + I_tam;
  D = tin_hieu - tin_hieu_tam;
  gia_tri_PID = (Kp * P) + (Ki * I) + (Kd * D); // 
  I_tam = I;
  tin_hieu_tam = tin_hieu;
}

void dithangPID()
{
  
  int toc_do_motor_trai = toc_do_ban_dau - gia_tri_PID;   
  int toc_do_motor_phai = toc_do_ban_dau - gia_tri_PID;  

  toc_do_motor_trai = constrain(toc_do_motor_trai, 0, 255);  
  toc_do_motor_phai = constrain(toc_do_motor_phai, 0, 255); 
  Serial.println("toc do trai:  " + String(toc_do_motor_trai));
  Serial.println("toc do phai:  " + String(toc_do_motor_phai));
  analogWrite(ENA, toc_do_motor_trai); 
  analogWrite(ENB, toc_do_motor_phai); 
  //kiểm tra đầu ra tốc độ
  int kiem_tra_toc_do = analogRead(nut2);
  Serial.println("kiem tra toc do:  " + String(kiem_tra_toc_do));

  dithang();
}

void dithang()
{
  digitalWrite(motorA1, HIGH);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, HIGH);
  digitalWrite(motorB2, LOW);
}
void quaydau()
{
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, HIGH);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, HIGH);
}
void quayphai90()
{
  digitalWrite(motorA1, HIGH);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, LOW);
}
void quaytrai90()
{
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, HIGH);
  digitalWrite(motorB2, LOW);
}
void quaytrai180() {
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, HIGH);
  digitalWrite(motorB1, HIGH);
  digitalWrite(motorB2, LOW);
}
void quayphai180() {
  digitalWrite(motorA1, HIGH);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, HIGH);
}
void dung()
{
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, LOW);
}
////////////////////////////////////
void in_gia_tri() {
  static bool kiem_tra_in = false;
  if (!kiem_tra_in) {
  Serial.print("ki: " + String(Ki));
  Serial.print("    kp: " + String(Kp));
  Serial.print("    kd: " + String(Kd));
  Serial.println("    toc do ban dau: " + String(toc_do_ban_dau));
  kiem_tra_in = true;
  }
}

void in_trang_thai() {
//  static bool kiem_tra_in = false;
//  if (!kiem_tra_in) {
  Serial.println("Da thuc hien: " + String(tin_hieu));
//  kiem_tra_in = true;
  }

////////////////////////////////////////////////
void serial_blt(){
in_gia_tri();
if (Serial.available() > 0){
  String gia_tri_gui = Serial.readStringUntil('\n');
  if (gia_tri_gui == "it1"){
   Ki++;
   Serial.println("da tang Ki+1:" + String(Ki));
  }
  else if (gia_tri_gui == "ig1"){
   Ki--;
   Serial.println("da giam Ki-1:" + String(Ki));
  }
  else if (gia_tri_gui == "pt1"){
   Kp++;
   Serial.println("da tang Kp+1:" + String(Kp));
  }
  else if (gia_tri_gui == "pg1"){
   Kp--;
   Serial.println("da giam Kp-1:" + String(Kp));
  }
  else if (gia_tri_gui == "dt1"){
   Kd++;
   Serial.println("da tang Kd+1:" + String(Kd));
  }
  else if (gia_tri_gui == "dg1"){
   Kd--;
   Serial.println("da giam Kd-1:" + String(Kd));
  }
  else if (gia_tri_gui == "tdt1"){
   toc_do_ban_dau = toc_do_ban_dau + 5;
   Serial.println("da tang toc do ban dau +5:" + String(toc_do_ban_dau));
  }
  else if (gia_tri_gui == "tdg1"){
   toc_do_ban_dau = toc_do_ban_dau - 5;
   Serial.println("da tang toc do ban dau - 5:" + String(toc_do_ban_dau));
  }
  else if (gia_tri_gui == "kti")
   Serial.println("Ki:" + String(Ki));
  else if (gia_tri_gui == "ktp")
   Serial.println("Kp:" + String(Kp));
  else if (gia_tri_gui == "ktd")
   Serial.println("Kd:" + String(Kd));
  else if (gia_tri_gui == "kttd")
   Serial.println("toc do ban dau:" + String(toc_do_ban_dau));
  else if (gia_tri_gui == "kt"){
  Serial.print("ki: " + String(Ki));
  Serial.print("    kp: " + String(Kp));
  Serial.print("    kd: " + String(Kd));
  Serial.println("    toc do ban dau: " + String(toc_do_ban_dau));
  }
}
}

void delay_millis()
{
    if ( (unsigned long) (millis() - time) > 1000)  
  Serial.println("San Sang Mode 2");
  time = millis();
}
