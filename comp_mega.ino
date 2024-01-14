/*
라즈베리파이 - MEGA2560 통신구현과 처리와
MEGA2560 - UNO 데이터 중계를 위한 MEGA2560 측 코드

시리얼 통신을 통해 라즈베리파이와 여러 데이터를 교환하고
받은 데이터를 여러 출력장치로 디스플레이 합니다.
또한, 디버깅을 위해 전달받은 데이터를 UNO측으로 릴레이합니다.
*/

// GLCD, TLCD, I2C 라이브러리 포함,
// 슬레이브 주소와 버튼 주소 정의
#include <openGLCD.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#define SLAVE 4
#define SHOT_PIN 5
#define BACK 14
#define NEXT 15
#define NONG 16
#define SHOT 17

// TLCD 핀 정의
LiquidCrystal lcd(44, 45, 46, 47, 48, 49);

// 표정 스트링 리터럴 정의
char emotion[6][4] = { "ANG", "FEA", "HAP", "SAD", "SUR", "NAT" };
String emotion_full[6] = {
  "ANGRY",
  "FEARFUL",
  "HAPPY",
  "SAD",
  "SURPRISE",
  "NATURAL"
};

// 파이로부터 전송받은 데이터를 저장할 변수
String data;

void setup() {
  // 여러 하드웨어 초기화
  Wire.begin(SLAVE);
  Serial.begin(9600);
  lcd.begin(16, 2);
  GLCD.Init();
  pinMode(SHOT_PIN, OUTPUT);
  pinMode(BACK, INPUT);
  pinMode(NEXT, INPUT);
  pinMode(NONG, INPUT);
  pinMode(SHOT, INPUT);
}

void loop() {
  // 현재 눌린 버튼을 감지하여 저장
  char command = 0;
  command += digitalRead(BACK) << 3;
  command += digitalRead(NEXT) << 2;
  command += !digitalRead(NONG) << 1;
  command += !digitalRead(SHOT);
  Serial.println(byte(command));

  // 데이터 변수를 초기화하고 파이로부터 데이터 수집
  data = "";
  while (Serial.available()) {
    char ch = Serial.read(); // 한 바이트씩 읽고
    data.concat(ch); // 데이터 이어붙이기
  }
  delay(100); // 성능을 위해 딜레이 0.1초

  // GLCD 스크린 초기화 후 받은 데이터 수치 표시
  GLCD.ClearScreen();
  GLCD.SelectFont(Arial_14);
  for(int i = 0; i<3; i++){
    GLCD.CursorToXY(0, i * 20);
    GLCD.print(emotion[i]);
    GLCD.print(" : ");
    if((int)data[i + 1] < 10)
      GLCD.print("0");
    GLCD.print((int)data[i + 1]);
    GLCD.print("%");
    GLCD.CursorToXY(64, i * 20);
    GLCD.print(emotion[i + 3]);
    GLCD.print(" : ");
    if((int)data[i + 3 + 1] < 10)
      GLCD.print("0");
    GLCD.print((int)data[i + 3 + 1]);
    GLCD.print("%");
  }

  GLCD.SelectFont(System5x7);

  GLCD.DrawRect(0, 57, 7, 7);
  GLCD.CursorToXY(8, 57);
  GLCD.print("BACK");

  GLCD.DrawRect(32, 57, 7, 7);
  GLCD.CursorToXY(40, 57);
  GLCD.print("NEXT");

  GLCD.DrawRect(64, 57, 7, 7);
  GLCD.CursorToXY(72, 57);
  GLCD.print("NONG");

  GLCD.DrawRect(96, 57, 7, 7);
  GLCD.CursorToXY(104, 57);
  GLCD.print("SHOT");

  // 얼굴이 검출되었으면(data[7] == 1) LED 점등
  if (data[7] == 1) digitalWrite(SHOT_PIN, HIGH);
  else digitalWrite(SHOT_PIN, LOW);

  // TLCD에 현재 출력되고있는 표정과 수치 출력
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(emotion_full[data[8]]);
  lcd.setCursor(0, 1);
  lcd.print(String(data[9], DEC) + "%");

  if (data[0] == 's' && data[10] == 'e') {
    // 데이터 유효성 검증을 위해 데이터의 시작과 끝 확인 후
    // UNO로 데이터 중계
    i2c_communication();
  }  
}

void i2c_communication() {
  // 데이터 전송을 알리고 데이터 전송
  Wire.beginTransmission(SLAVE);
  for(int i = 1; i < 10; i++) {
    Wire.write(data[i]);
  }
  Wire.endTransmission(SLAVE);
}