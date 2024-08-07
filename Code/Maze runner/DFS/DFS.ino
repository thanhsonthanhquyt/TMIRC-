#include <Arduino.h>

// Khai báo chân cảm biến
const int rightSensor = A7;
const int frontRightSensor = A6;
const int frontLeftSensor = A5;
const int leftSensor = A4;
const int buttonPin = A1;

// Khai báo chân điều khiển động cơ
const int pwmMotorA = 9;
const int pwmMotorB = 10;
const int motorA1 = 3;
const int motorA2 = 4;
const int motorB1 = 6;
const int motorB2 = 7;

// Khai báo biến vị trí robot
int robotX = 0;
int robotY = 0;

// Khởi tạo tọa độ đích (khu vực 2x2 ở trung tâm)
const int goalMinX = 7;
const int goalMaxX = 8;
const int goalMinY = 7;
const int goalMaxY = 8;

// Khai báo ngưỡng cảm biến
const int threshold = 512;

// Biến để lưu trạng thái nút nhấn
bool buttonPressed = false;

// Khai báo ma trận mê cung (16x16)
int maze[16][16];
bool visited[16][16];
int path[256];
int pathLength = 0;

// Khai báo hàm nguyên mẫu
void initializeMaze();
bool isAtGoal();
void updatePosition(int deltaX, int deltaY);
bool canGoForward();
bool canTurnLeft();
bool canTurnRight();
void navigate();
void goForward();
void turnLeft();
void turnRight();
void turnAround();
void stop();
int readSensors();
bool isSurroundedByWalls();
void findShortestPath();
void returnToStart();
void followShortestPath();

void setup() {
  // Khởi tạo các chân điều khiển động cơ
  pinMode(pwmMotorA, OUTPUT);
  pinMode(pwmMotorB, OUTPUT);
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  
  // Khởi tạo chân cảm biến
  pinMode(rightSensor, INPUT);
  pinMode(frontRightSensor, INPUT);
  pinMode(frontLeftSensor, INPUT);
  pinMode(leftSensor, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  
  Serial.begin(9600);
  initializeMaze();
}

void loop() {
  if (isAtGoal()) {
    Serial.println("Đã đến đích!");
    stop();
    findShortestPath();
    while (digitalRead(buttonPin) == HIGH) {
      // Chờ nút nhấn
    }
    followShortestPath();
  } else {
    navigate();
  }
}

// Hàm khởi tạo mê cung
void initializeMaze() {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      maze[i][j] = 0;
      visited[i][j] = false;
    }
  }
}

// Hàm kiểm tra nếu robot đã hoàn toàn ở trong khu vực đích
bool isAtGoal() {
  // Xác định nếu robot nằm trong khu vực đích
  bool withinGoalArea = (robotX >= goalMinX && robotX <= goalMaxX && robotY >= goalMinY && robotY <= goalMaxY);
  
  // Kiểm tra cảm biến để xác nhận lối đi xung quanh
  bool hasPathFront = canGoForward();
  bool hasPathLeft = canTurnLeft();
  bool hasPathRight = canTurnRight();
  
  // Robot được coi là ở đích nếu:
  // - Robot nằm trong khu vực đích
  // - Có ít nhất một lối đi ở phía trước và bên trái hoặc bên phải
  return withinGoalArea && (hasPathFront && (hasPathLeft || hasPathRight));
}

// Hàm kiểm tra nếu có lối đi phía trước
bool canGoForward() {
  return analogRead(frontLeftSensor) > threshold && analogRead(frontRightSensor) > threshold;
}

// Hàm kiểm tra nếu có lối đi rẽ trái
bool canTurnLeft() {
  return analogRead(leftSensor) > threshold;
}

// Hàm kiểm tra nếu có lối đi rẽ phải
bool canTurnRight() {
  return analogRead(rightSensor) > threshold;
}

// Hàm cập nhật vị trí của robot
void updatePosition(int deltaX, int deltaY) {
  robotX += deltaX;
  robotY += deltaY;
  Serial.print("Vị trí robot: (");
  Serial.print(robotX);
  Serial.print(", ");
  Serial.print(robotY);
  Serial.println(")");
}

// Hàm điều khiển di chuyển
void navigate() {
  int sensorValue = readSensors(); // Đọc giá trị từ cảm biến

  if (isAtGoal()) {
    Serial.println("Đã đến đích!");
    stop();
    findShortestPath();
    returnToStart();
    while (digitalRead(buttonPin) == HIGH) {
      // Chờ nút nhấn
    }
    followShortestPath();
  } else {
    // Di chuyển robot theo cảm biến
    switch (sensorValue) {
      case 0b1000: // [1000] - Ngõ 3 có lối đi thẳng và phải
        if (canGoForward()) {
          updatePosition(1, 0); // Di chuyển thẳng
          goForward();
        } else {
          turnRight();
          updatePosition(0, 1); // Di chuyển phải
          goForward();
        }
        break;
      case 0b0001: // [0001] - Ngõ 3 có lối đi thẳng và trái
        if (canGoForward()) {
          updatePosition(1, 0); // Di chuyển thẳng
          goForward();
        } else {
          turnLeft();
          updatePosition(0, -1); // Di chuyển trái
          goForward();
        }
        break;
      case 0b0110: // [0110] - Ngõ 3 có lối đi trái hoặc phải
        if (canTurnLeft()) {
          turnLeft();
          updatePosition(0, -1); // Di chuyển trái
          goForward();
        } else {
          turnRight();
          updatePosition(0, 1); // Di chuyển phải
          goForward();
        }
        break;
      case 0b1110: // [1110] - Rẽ phải
        turnRight();
        updatePosition(0, 1); // Di chuyển phải
        goForward();
        break;
      case 0b0111: // [0111] - Rẽ trái
        turnLeft();
        updatePosition(0, -1); // Di chuyển trái
        goForward();
        break;
      case 0b1111: // [1111] - Quay đầu
        turnAround();
        updatePosition(-1, 0); // Di chuyển quay lại
        goForward();
        break;
      case 0b0000: // [0000] - Ngõ 4 có lối đi thẳng, trái và phải
        if (canGoForward()) {
          updatePosition(1, 0); // Di chuyển thẳng
          goForward();
        } else if (canTurnLeft()) {
          turnLeft();
          updatePosition(0, -1); // Di chuyển trái
          goForward();
        } else if (canTurnRight()) {
          turnRight();
          updatePosition(0, 1); // Di chuyển phải
          goForward();
        } else {
          stop(); // Nếu không có lối đi nào, dừng lại
        }
        break;
      default:
        stop(); // Dừng lại nếu không xác định được tình huống
        break;
    }
  }
}

// Hàm kiểm tra xem robot có bị bao quanh bởi các bức tường hay không
bool isSurroundedByWalls() {
  return !canGoForward() && !canTurnLeft() && !canTurnRight();
}

// Hàm đọc giá trị cảm biến
int readSensors() {
  int left = analogRead(leftSensor) > threshold ? 1 : 0;
  int right = analogRead(rightSensor) > threshold ? 1 : 0;
  int frontLeft = analogRead(frontLeftSensor) > threshold ? 1 : 0;
  int frontRight = analogRead(frontRightSensor) > threshold ? 1 : 0;
  
  return (left << 3) | (right << 2) | (frontLeft << 1) | frontRight;
}

// Hàm điều khiển robot
void goForward() {
  digitalWrite(motorA1, HIGH);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, HIGH);
  digitalWrite(motorB2, LOW);
  analogWrite(pwmMotorA, 255);
  analogWrite(pwmMotorB, 255);
}

void turnLeft() {
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, HIGH);
  digitalWrite(motorB1, HIGH);
  digitalWrite(motorB2, LOW);
  analogWrite(pwmMotorA, 255);
  analogWrite(pwmMotorB, 255);
  while (analogRead(leftSensor) > threshold) {
    // Chờ cho đến khi thoát khỏi ngõ rẽ
  }
  goForward();
}

void turnRight() {
  digitalWrite(motorA1, HIGH);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, HIGH);
  analogWrite(pwmMotorA, 255);
  analogWrite(pwmMotorB, 255);
  while (analogRead(rightSensor) > threshold) {
    // Chờ cho đến khi thoát khỏi ngõ rẽ
  }
  goForward();
}

void turnAround() {
  digitalWrite(motorA1, HIGH);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, HIGH);
  analogWrite(pwmMotorA, 255);
  analogWrite(pwmMotorB, 255);
  delay(1000); // Thời gian quay đầu
  goForward();
}

void stop() {
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, LOW);
}

// Hàm tìm đường đi ngắn nhất bằng BFS
void findShortestPath() {
  // Giả định điểm bắt đầu là (0, 0)
  int startX = 0, startY = 0;

  // Khởi tạo hàng đợi cho BFS
  int queue[256][2];
  int front = 0, rear = 0;

  // Thêm điểm bắt đầu vào hàng đợi
  queue[rear][0] = startX;
  queue[rear][1] = startY;
  rear++;

  // Khởi tạo mảng visited
  bool visited[16][16] = {false};
  visited[startX][startY] = true;

  // Mảng để lưu trữ đường đi
  int parent[16][16][2];
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      parent[i][j][0] = -1;
      parent[i][j][1] = -1;
    }
  }

  // Mảng hướng đi
  int directions[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

  while (front < rear) {
    int x = queue[front][0];
    int y = queue[front][1];
    front++;

    if (x >= goalMinX && x <= goalMaxX && y >= goalMinY && y <= goalMaxY) {
      // Đã tìm thấy đường đi đến đích
      int pathLength = 0;
      while (parent[x][y][0] != -1 && parent[x][y][1] != -1) {
        path[pathLength] = x * 16 + y;
        pathLength++;
        int tempX = parent[x][y][0];
        int tempY = parent[x][y][1];
        x = tempX;
        y = tempY;
      }
      return;
    }

    for (int i = 0; i < 4; i++) {
      int newX = x + directions[i][0];
      int newY = y + directions[i][1];

      if (newX >= 0 && newX < 16 && newY >= 0 && newY < 16 && !visited[newX][newY]) {
        visited[newX][newY] = true;
        parent[newX][newY][0] = x;
        parent[newX][newY][1] = y;
        queue[rear][0] = newX;
        queue[rear][1] = newY;
        rear++;
      }
    }
  }
}

// Hàm đưa robot trở về điểm xuất phát
void returnToStart() {
  for (int i = pathLength - 1; i >= 0; i--) {
    int x = path[i] / 16;
    int y = path[i] % 16;
    // Di chuyển robot về điểm xuất phát
  }
}

// Hàm điều khiển robot đi theo đường đi ngắn nhất
void followShortestPath() {
  for (int i = 0; i < pathLength; i++) {
    int x = path[i] / 16;
    int y = path[i] % 16;
    // Di chuyển robot theo đường đi ngắn nhất
  }
}
