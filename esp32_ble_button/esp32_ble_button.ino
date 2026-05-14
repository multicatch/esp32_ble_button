#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>

#define DEBUG_MODE 0

#define BLE_DEV_NAME "ESP32BUT" // warning: please keep this dev name up to 8 characters, because BLE advertisement can have up to 31 bytes
#define BLE_ADV_SEPARATOR ";"
#define BLE_MAX_ADV_TIME 400
#define BLE_ADV_RETRIES 3

#define BTN1 GPIO_NUM_0
#define BTN2 GPIO_NUM_1
#define BTN_COUNT 2
uint8_t buttons[BTN_COUNT] = {BTN1, BTN2};

#define LED_PIN GPIO_NUM_8

void setup() {
  #if DEBUG_MODE
  Serial.begin(115200);
  delay(1000);
  #endif

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP); 
  pinMode(LED_PIN, OUTPUT);

  esp_sleep_wakeup_cause_t wakeup_cause  = esp_sleep_get_wakeup_cause();

  #if DEBUG_MODE
  Serial.print("Wakeup cause: ");
  Serial.println(wakeup_cause);
  #endif

  uint8_t wakeup_button = 0;
  bool is_woken_by_button = (wakeup_cause == ESP_SLEEP_WAKEUP_GPIO);
  
  if (is_woken_by_button) {
    wakeup_button = detect_btn_from_gpio_mask(esp_sleep_get_gpio_wakeup_status());
    advertise_button_press(wakeup_button);
  } else {
    wakeup_button = detect_currently_pressed();
  }

  go_to_sleep(wakeup_button);
}

void loop() {
}

uint8_t detect_btn_from_gpio_mask(uint64_t wakeup_status) {
  if (wakeup_status == 0) return 0;

  for (uint8_t i = 0; i < BTN_COUNT; i++) {
    uint8_t btn = buttons[i];
    if ((wakeup_status & (1 << btn)) != 0) {
      return i + 1;
    }
  }
  return 0;
}

uint8_t detect_currently_pressed() {
  for (uint8_t i = 0; i < BTN_COUNT; i++) {
    uint8_t btn = buttons[i];
    if (digitalRead(btn) == LOW) {
      return i+1;
    }
  }
  return 0;
}

void advertise_button_press(uint8_t wakeup_button) {
  #if DEBUG_MODE
  Serial.print("Button #");
  Serial.println(wakeup_button);
  #endif

  if (wakeup_button <= 0) return;

  uint32_t short_id = ESP.getEfuseMac() & 0xFFF;
  std::string dev_name = std::string(BLE_DEV_NAME) + std::to_string(short_id);

  BLEDevice::init(dev_name.c_str());
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  BLEAdvertisementData adv;
  
  std::string adv_id = std::to_string(esp_random() % 1000);
  std::string payload = std::to_string(wakeup_button) + BLE_ADV_SEPARATOR + adv_id;

  #if DEBUG_MODE
  Serial.print("Dev name:");
  Serial.println(dev_name.c_str());
  Serial.print("Advertisement: ");
  Serial.println(payload.c_str());
  #endif

  adv.setName(dev_name.c_str());
  adv.setManufacturerData(payload.c_str());

  pAdvertising->setAdvertisementData(adv);
  pAdvertising->setMinInterval(0x20);
  pAdvertising->setMaxInterval(0x40);
  pAdvertising->setScanResponse(false);

  for (int i = 0; i < BLE_ADV_RETRIES; i++) {
    pAdvertising->start();
    if (i == 0) {
      int32_t blink_time = blink(wakeup_button);
      int32_t delay_remaining = BLE_MAX_ADV_TIME - blink_time;
      if (delay_remaining > 0) {
        delay(delay_remaining);
      }
    } else {
      delay(BLE_MAX_ADV_TIME);
    }
    pAdvertising->stop();
  }
}

int32_t blink(uint8_t wakeup_button) {
  int32_t delay_time = 0;
  for (int i = 0; i < wakeup_button; i++) {
    digitalWrite(GPIO_NUM_8, LOW);
    delay(50);
    digitalWrite(GPIO_NUM_8, HIGH);
    delay(100);
    delay_time += 150;
  }
  return delay_time;
}

void go_to_sleep(uint8_t wakeup_button) {
  for (uint8_t i = 0; i < BTN_COUNT; i++) {
    if (wakeup_button == (i+1)) continue;
    uint8_t btn = buttons[i];
    
    #if DEBUG_MODE
    Serial.print("Deep sleep GPIO_NUM_");
    Serial.println(btn);
    #endif

    esp_deep_sleep_enable_gpio_wakeup(1 << btn, ESP_GPIO_WAKEUP_GPIO_LOW);
  }
  esp_deep_sleep_start();
}
