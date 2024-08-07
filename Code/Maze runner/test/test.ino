int a1 = 3;
int a2 = 4;
int b1 = 6;
int b2 = 5;
int ea = 9;
int eb = 10;
int cb1 = A4;
int cb2 = A5;
int cb3 = A6;
int cb4 = A7;
void setup() {
pinMode(a1, OUTPUT);
pinMode(a2, OUTPUT);
pinMode(b1, OUTPUT);
pinMode(b2, OUTPUT);
pinMode(ea, OUTPUT);
pinMode(eb, OUTPUT);
pinMode(cb1, INPUT_PULLUP);
pinMode(cb2, INPUT_PULLUP);
pinMode(cb3, INPUT_PULLUP);
pinMode(cb4, INPUT_PULLUP);
Serial.begin(9600);
}

void loop() {
//analogRead((cb1)+ (cb2) + (cb3) + (cb4));
//int a = cb1, b = cb2, c = cb3, d = cb4;
//Serial.println(String(a) + "     " + String(b) + "     " + String(c) + "      " + String(d));
analogWrite(ea, 255);
analogWrite(eb, 255);
digitalWrite(a2, HIGH);
digitalWrite(b1, HIGH);
digitalWrite(a1, LOW);
digitalWrite(b2, LOW);
//int a = analogRead(cb1);
//Serial.println(a);
//delay(400);

}
