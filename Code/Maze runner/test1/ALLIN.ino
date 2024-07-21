

int standBy = 10;

// Motor A
int PWMA = 3;  
int AIN1 = 9;  
int AIN2 = 8;  

// Motor B
int PWMB = 5;  /
int BIN1 = 11; 
int BIN2 = 12; 
void setup() {
    
    pinMode(standBy, OUTPUT);
    
    // Motor A
    pinMode(PWMA, OUTPUT);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    
    // Motor B
    pinMode(PWMB, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    
    Serial.begin(19200);
    Serial.println("................");
}

void loop() {
    Tien(128);        
    delay(2000);        
    stop();            
    delay(250);         
    
    Lui(255);        
    delay(2000);         
    stop();             
    delay(250);         
    
    Trai(64);        
    delay(2000);         
    stop();              
    delay(250);          
    
    Phai(255);     
    delay(2000);         
    stop();              
    delay(2000);         
}
/////////////////////////////////
void runMotor(int motor, int spd, int dir) {
    digitalWrite(standBy, HIGH); 
    
    boolean dirPin1 = LOW;   // Biến boolean để thiết lập hướng của động cơ
    boolean dirPin2 = HIGH;
  // Nếu hướng là 0 (lùi), đổi giá trị của dirPin1 và dirPin2
    if(dir == 0) {     
        dirPin1 = HIGH;  
        dirPin2 = LOW;
    }
  // Kiểm tra động cơ nào đang được điều khiển
    if(motor == 0) {   //A
        digitalWrite(AIN1, dirPin1);
        digitalWrite(AIN2, dirPin2);
        analogWrite(PWMA, spd);
    } else if(motor == 1) {   //B
        digitalWrite(BIN1, dirPin1);
        digitalWrite(BIN2, dirPin2);
        analogWrite(PWMB, spd);
    }
}
//
void Tien(int spd) {
    runMotor(0, spd, 1);
    runMotor(1, spd, 1);
}

void Lui(int spd) {
    runMotor(0, spd, 0);
    runMotor(1, spd, 0);
}

void Trai(int spd) {
    runMotor(0, spd, 0);
    runMotor(1, spd, 1);
}

void Phai(int spd) {
    runMotor(0, spd, 1);
    runMotor(1, spd, 0);
}

void Dung() {
    runMotor(0, 0, 0);
    runMotor(1, 0, 0);
}



