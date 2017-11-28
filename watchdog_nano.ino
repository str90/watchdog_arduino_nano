#define RESET_PIN 2
#define PWR_PIN 3

boolean connEstFlag = false;
boolean rebootFlag = false;
boolean pwrFlag = false;

String connEstSend = "1460";
String uartInput;
const unsigned long etalonTime = 20000000;
unsigned long idleTime = 0;
const unsigned long rebootTimeConst = 60000; //600000;
unsigned long rebootTime = 0;

void failRoutine() {
  digitalWrite(RESET_PIN, HIGH);
  delay(1500);
  digitalWrite(RESET_PIN, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  rebootFlag = true;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(RESET_PIN, LOW);
  digitalWrite(PWR_PIN, LOW);

  Serial.begin(9600);
  while(connEstFlag == false) {
    Serial.print(connEstSend);
    delay(1200);
    if(Serial.available() > 0) {
      uartInput = Serial.readStringUntil('\n');
      if(uartInput.equals("PC")) {
        digitalWrite(LED_BUILTIN, HIGH);
        connEstFlag = true;
      }
    }
  }
  delay(1000);
  connEstFlag = false;
}

void loop() {
  if(rebootFlag == true) {
    pwrFlag = false;
    rebootTime = millis();
    while(connEstFlag == false) {
      Serial.print(connEstSend);
      delay(1000);
      if(Serial.available() > 0) {
        uartInput = Serial.readStringUntil('\n');
        if(uartInput.equals("PC")) {
          digitalWrite(LED_BUILTIN, HIGH);
          connEstFlag = true;
        }
     }
     
    else if(pwrFlag == false) if(millis() - rebootTime > rebootTimeConst) {
      digitalWrite(PWR_PIN, HIGH);
      delay(1500);
      digitalWrite(PWR_PIN, LOW);
      pwrFlag = true;
    } 
  }
  delay(1000);
  connEstFlag = false;
  rebootFlag = false;
  }
  
  if(Serial.available() > 0) {
    uartInput = Serial.readStringUntil('\n');
    if(!(uartInput.equals("AL"))) {
      if(micros() - idleTime > etalonTime) failRoutine();
    }
    else idleTime = micros();
  }

  else if(micros() - idleTime > etalonTime) failRoutine();
}


