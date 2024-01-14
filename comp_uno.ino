/*
라즈베리파이 - MEGA2560 간 교환되는 데이터 중계를 위한
UNO 호환보드측 코드 (디버깅용)

시리얼 통신을 통해 MEGA2560측에서 받은 데이터를
I2C 통신을 이용하여 UNO 호환보드측으로 전송합니다
*/

// I2C통신 라이브러리 포함,
// 슬레이브 주소와 동작확인을 위한 LED 핀 정의
#include <Wire.h>
#define SLAVE 4
#define LED_PIN 9

// 초기화 작업
void setup() {
  // I2C 통신 대상을 설정하고, 데이터 수신 콜백 후킹.
  // 그리고 시리얼모니터 출력을 위한 시리얼 통신 활성화,
  // 동작 확인을 위한 LED 활성화
  Wire.begin(SLAVE);
  Wire.onReceive(receiveFromMaster);
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // 동작 확인을 위해 0.1초 간격으로 LED 점등
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  delay(100);
}

// I2C 데이터를 수신했을때 호출되는 콜백함수
void receiveFromMaster(int bytes) {
  // 데이터는 총 9가지,
  // 표정 6가지에 대한 수치, 얼굴 검출여부,
  // 디스플레이 할 표정의 인덱스와 최대 수치
  char data[9] = { 0 };
  for (int i = 0; i < 9; ++i) {
    // MEGA2560 측에서 전송한 데이터 읽기
    data[i] = Wire.read();
  }
  Serial.print("I got ");
  for (int i = 0; i < 9; ++i) {
    // 전송받은 데이터를 10진수 형식으로 시리얼출력
    Serial.print(String(data[i], DEC) + " ");
  }
  Serial.println();
}