void setup() {
  Serial.begin(115200);
  pinMode(GPIO_NUM_0, INPUT_PULLUP);
  pinMode(GPIO_NUM_1, INPUT_PULLUP); 
  pinMode(GPIO_NUM_8, OUTPUT);
}


int pressed = -1;

void loop() {
  if (digitalRead(GPIO_NUM_0) == LOW) {
    if (pressed != 0) {
      Serial.println("Pressed 0");
      digitalWrite(GPIO_NUM_8, LOW);
      pressed = 0;
      delay(100);
    }
  } else if (digitalRead(GPIO_NUM_1) == LOW) {
    if (pressed != 1) {
      Serial.println("Pressed 1");
      digitalWrite(GPIO_NUM_8, LOW);
      pressed = 1;
      delay(100);
    }
  } else {
    pressed = -1;
  }
  digitalWrite(GPIO_NUM_8, HIGH);
  delay(10);
}
