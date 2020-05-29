const byte ANODE_PINS[8] = {6,7,8,9,10,11,12,13};
const byte CATHODE_PINS[8] = {A3,A2,A1,A0,5,4,3,2}; 
const byte BUTTON1 = A4;
const byte BUTTON2 = 1;
const byte delayTime = 100;
const int DEBOUNCE_DELAY = 75;
const int MOVE_DELAY = 300;

byte pattern[8][8];

int snakeX[25];
int snakeY[25];
int tail = 3;

int appleX;
int appleY;

byte state;
const byte playing = 0;
const byte paused = 1;
const byte lost = 2;

int pointing;
const int up = 0;
const int right = 1;
const int down = 2;
const int left = 3;


void setup() {
  for (byte i = 0; i < 8; i++) {
    pinMode(ANODE_PINS[i], OUTPUT);
    digitalWrite(ANODE_PINS[i],HIGH);
    pinMode(CATHODE_PINS[i], OUTPUT);
    digitalWrite(CATHODE_PINS[i],HIGH);
  }
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  state = paused;
  pointing = up;
  appleX = 2;
  appleY = 2;
  snakeX[0] = 4;
  snakeY[0] = 4;
}

void display() {
  for (byte anode = 0; anode < 8; anode++) {
    for (byte cathode = 0; cathode < 8; cathode++) {
      if (pattern[anode][cathode] == 1) {
        digitalWrite(CATHODE_PINS[cathode],LOW);
      } else {
        digitalWrite(CATHODE_PINS[cathode],HIGH);
      }
    }  
    digitalWrite(ANODE_PINS[anode],LOW);
    delayMicroseconds(delayTime);
    digitalWrite(ANODE_PINS[anode],HIGH);
    delayMicroseconds(delayTime);
    delayMicroseconds(2000);
  }
}

void generateApple(){
  appleX = random(0,7);
  appleY = random(0,7);
  for (int i = 0; i < tail; i++) {
    if (snakeX[i] == appleX && snakeY[i] == appleY)
    {
        generateApple();
    }
  }
}

int moveSnake() {
  int x = snakeX[0];
  int y = snakeY[0];
  switch (pointing) {
    case up:
      y += 1;
      break;
    case down:
      y -= 1;
      break;
    case left:
      x-= 1;
      break;
    case right:
      x +=1;
      break;
    default:
    break;
  }
  if (x == -1) {
    x = 7;
  }
  if (y == -1) {
    y = 7;
  }
  if (x == 8) {
    x = 0;
  }
  if (y == 8) {
    y = 0;
  }
  
  
  for (int i = tail - 1; i > 0; i--) {
    snakeX[i] = snakeX[i-1];
    snakeY[i] = snakeY[i-1];
    
    if (snakeX[i] == x && snakeY[i] == y) {
      return -1;
    }
    
  }
    snakeX[0] = x;
    snakeY[0] = y;
    return 1;
}



void eaten() {
  if (appleX == snakeX[0] && appleY == snakeY[0]) {
    increaseSize();
    generateApple();
  }
}

void increaseSize() {
  int x = snakeX[tail-1];
  int y = snakeY[tail-1];
      if (tail == 1) {
          switch (pointing) {
        case up:
          y -= 1;
          break;
        case down:
          y += 1;
          break;
        case left:
          x += 1;
          break;
        case right:
          x -=1;
          break;
        default:
        break;
        }
      } else {
        if (x == snakeX[tail - 2]) {
          if (snakeY[tail -2] > y) {
            y -= 1;
          } else {
            y +=1;
          }
        } else {
           if (snakeX[tail -2] > y) {
            x -= 1;
          } else {
            x +=1;
          }
        }
      }
    tail++;
    snakeX[tail -1] = x;
    snakeY[tail -1] = y;
}

void reset() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
       pattern[i][j] = 0; 
    }
  }
}

void makePattern() {
  reset();
   
  for (int i = 0; i < tail; i++) {
    pattern[snakeX[i]][snakeY[i]] = 1;
  }
    pattern[appleX][appleY] = 1;
}

void turnLeft() {
  pointing++;
  if (pointing == 4) pointing = 0;
}

void turnRight() {
  pointing--;
  if (pointing == -1) pointing = 3;
}



void loop() {
  
  static  long  nextUpdateTime;

  static bool left = false;
  static bool right = false;

  static byte button_state1 = HIGH;
  static byte last_reading1 = HIGH;
  static long last_reading_change1 = 0;

  static byte button_state2 = HIGH;
  static byte last_reading2 = HIGH;
  static long last_reading_change2 = 0;

  byte reading1 = digitalRead(BUTTON1);
  byte reading2 = digitalRead(BUTTON2);
  unsigned long now = millis();
  static long move_change_time = 0;

  

  makePattern();
  display();
  
  
  

  if (now - last_reading_change1 > DEBOUNCE_DELAY) {
    if (reading1 == LOW && button_state1 == HIGH) { 
      left = true;
    }
     button_state1 = reading1;
  }

   if (now - last_reading_change2 > DEBOUNCE_DELAY) {
    if (reading2 == LOW && button_state2== HIGH) {
      right = true;
    }
     button_state2 = reading2;
  }
   
  if (reading1 != last_reading1) last_reading_change1 = now;
  last_reading1 = reading1;

  if (reading2 != last_reading2) last_reading_change2 = now;
  last_reading2 = reading2;


 


  switch (state) {
    case playing: 
      if (now - move_change_time > MOVE_DELAY) {
        if (moveSnake() == -1) {
          state = lost;
          
        }
        eaten();
        move_change_time = now;
       }

      if (right) {
          turnRight();
      } else if (left) {
          turnLeft();
      }
      break;
    case paused:
      if (right == true || left == true) {
          state = playing;
      }
      break;
    case lost:
      tail = 0;
      reset();
      break;
    default:
      break;
  }

  
  right = false;
  left = false;

}
