
//modified to match my sparkfun connector
#define xLow  17
#define xHigh 15
#define yLow  16
#define yHigh 14

// 마지막 유효한 값 리턴 시간 
long preEventTime = 0;
// 마지막 유효한 값 리턴 시간 이후의 경과 시간
long elapsedTime = 0;
// 터치 유무
int touched = 0;
// 마지막 유효한 값의 좌표 X
int preX = 0;
// 마지막 유효한 값의 좌표 Y
int preY = 0;
// 드래깅 방향 
int vertical = 0;
// 드래깅 시의 연속된 유효한 값 리턴 횟수
int moveEventCnt = 0;
 
void setup(){
  Serial.begin(9600);
}
 
void loop(){
  pinMode(xLow,OUTPUT);
  pinMode(xHigh,OUTPUT);
  digitalWrite(xLow,LOW);
  digitalWrite(xHigh,HIGH);
 
  digitalWrite(yLow,LOW);
  digitalWrite(yHigh,LOW);
 
  pinMode(yLow,INPUT);
  pinMode(yHigh,INPUT);
  delay(10);
 
  //xLow has analog port -14 !!
  int x=analogRead(yLow -14);
 
  pinMode(yLow,OUTPUT);
  pinMode(yHigh,OUTPUT);
  digitalWrite(yLow,LOW);
  digitalWrite(yHigh,HIGH);
 
  digitalWrite(xLow,LOW);
  digitalWrite(xHigh,LOW);
 
  pinMode(xLow,INPUT);
  pinMode(xHigh,INPUT);
  delay(10);
 
  //xLow has analog port -14 !!
  int y=analogRead(xLow - 14);
  
  // 더미 값에 대한 처리
  if (x < 70 || x > 950 || y < 140 || y > 850) {
     // blocked 되지않은 상태에서 700ms 경과시, 클릭으로 판단
     if (touched == 1 && millis() - preEventTime > 700) {
       doTouched(preX, preY);   
       preX = 0; 
       preY = 0;
       touched = 0;
       moveEventCnt = 0;
     } 
     else {
       if (touched != 1) {
         preX = 0; 
         preY = 0;
         moveEventCnt = 0;
       }
     }
  } 
  // 유효한 값에 대한 처리
  else {
    Serial.print(x,DEC);
    Serial.print(",");
    Serial.println(y,DEC); 
    
    event(x, y);
   
    delay(100);
  }
}

/*
 * 원 터치에 대한 이벤트 처리
 */
void doTouched(int x, int y) {
  Serial.println("do something for \"touched\" event!!");
  
  if (x > 158 && x < 313 && y > 373 && y < 645) {
    Serial.println("x touched");
  }
  else if (x >= 313 && x < 494 && y > 373 && y < 500) {
    Serial.println("1 touched");
  }
  else if (x >= 494 && x < 670 && y > 373 && y < 500) {
    Serial.println("2 touched");
  }
  else if (x >= 670 && x < 820 && y > 373 && y < 500) {
    Serial.println("3 touched");
  }
  else if (x > 158 && x < 494 && y >= 500 && y < 645) {
    Serial.println("4 touched");
  }
  else if (x >= 494 && x < 670 && y >= 500 && y < 645) {
    Serial.println("5 touched");
  }
  else if (x >= 670 && x < 820 && y >= 500 && y < 645) {
    Serial.println("6 touched");
  }
}

/*
 * 더블 터치에 대한 이벤트 처리
 */
void doDoubleTouched(int x, int y) {
  Serial.println("do something for \"double touched\" event!!");  
  // TODO
  if (x > 158 && x < 313 && y > 373 && y < 645) {
    Serial.println("x touched");
  } 
}

/*
 * 유효한 시그널 이벤트에 대한 처리
 */
void event(int x, int y) {
  // 터치된 상태
  touched = 1;
  if (preX == 0) preX = x;
  if (preY == 0) preY = y;
  
  elapsedTime = millis() - preEventTime;
  // 경과시간이 300 보다 작다면, 클릭중인상태
  if (elapsedTime < 200) {    
    int dis = distance(x, y, preX, preY);
    if (dis > 10) {
      moving(x, y, dis);
    } 
  }
  // 경과시간이 300 보다 크고, 800보다 작으면
  else if (elapsedTime >= 200 && elapsedTime < 700) {
    int diffX = x - preX;
    int diffY = y - preY;
    // 두번째 좌표와의 차이가 5px미만 이라면, 더블클릭   
    if (abs(diffX) <= 20 && abs(diffY) <= 20) {    
        Serial.println("double clicked");
        // 클릭 
        doDoubleTouched(preX, preY);
        touched = 0;
    }
  }
  
  preEventTime = millis();
}

/*
 * 드래깅에 대한 처리
 */
void moving(int x, int y, int dis) {
  // 더미 값에 대한 보정
  if (moveEventCnt == 0) {    
    moveEventCnt++;
    return;
  }
  else if (moveEventCnt == 1) {
    preX = x;
    preY = y; 
  }
  
  // 이전 좌표와의 차이 계산
  int diffX = x - preX;
  int diffY = y - preY;
  
  // 차이를 기준으로 좌우/상하 드래깅 판단   
  if (abs(diffX) > abs(diffY)) {
    vertical = 0;
  } else {
    vertical = 1;
  }
  
  // 상하 드래깅
  if (vertical == 1) {
    if (diffY > 0) {
      Serial.print("moving down ");
      Serial.print(dis);
      Serial.println("px"); 
    }
    else if (diffY < 0) {
      Serial.print("moving up "); 
      Serial.print(dis);
      Serial.println("px"); 
    } 
  }
  // 좌우 드래깅 
  else {    
    if (diffX > 0) {
      Serial.print("moving right ");     
      Serial.print(dis);
      Serial.println("px");  
    }
    else if (diffX < 0) {
      Serial.print("moving left ");
      Serial.print(dis);
      Serial.println("px");  
    }
  }
  moveEventCnt++;
  touched = 0;
}

/*
 * 두 좌표점 사이의 거리를 계산한다
 */
int distance(int x1, int y1, int x2, int y2) {
  double d1 = pow((double)x1 - (double)x2 ,2); 
  double d2 = pow((double)y1 - (double)y2 ,2); 

  return sqrt(d1 + d2);  
}

