
#include <AsyncDelay.h>
#include <Keypad.h>
#include <LED.h>
#include <LiquidCrystal.h>

// Display
int drs = 12;
int den = 11;
int dd4 = 5;
int dd5 = 4;
int dd6 = 3;
int dd7 = 2;
// RW - GND
// VCC - GND
// V0 - 10k
// VDD = 5V
LiquidCrystal lcd(drs, den, dd4, dd5, dd6, dd7);

// Timers
AsyncDelay delay_pre_alarm;
AsyncDelay delay_pre_armed;
AsyncDelay blink_time;
AsyncDelay blink_time_a;

//Keypad
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
 {'1','2','3'},
 {'4','5','6'},
 {'7','8','9'},
 {'*','0','#'}
};
byte rowPins[ROWS] = {45, 43, 41, 39}; //connect to the row pinouts of the keypad 
byte colPins[COLS] = {51, 49, 47}; //connect to the column pinouts of the keypad
// HINT: 5vPin = before 22 going  to shunts to row pins, obsolete
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//LEDs
LED led_armed    = LED(30);
LED led_prearmed = LED(28);
LED led_unarmed  = LED(26);

//switches
int switch_door  = 22;

//states
int current_alarm_level = 0; // 0: safe, 1: prearmed, 2: armed, 3: prealarm, 4: alarm

void lcd_status() {
  switch (current_alarm_level) {
    case 0:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("UNSCHARF");
      lcd.setCursor(0,1);
      lcd.print("");
      break;
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("SCHARF IN");
      lcd.setCursor(0,1);
      lcd.print("");
      break;

  }
}

void setup() {

  pinMode(switch_door, INPUT);


  Serial.begin(9600);
  Serial.println("Starting");

  blink_time.start(5000, AsyncDelay::MILLIS);
  blink_time_a.start(2000, AsyncDelay::MILLIS);

  lcd.begin(16,2);
  lcd.print("Arduino Alert");
  lcd.setCursor(0,1);
  lcd.print("System 0.1");

  delay(5000);

  
  //arm
  current_alarm_level = 1;
  delay_pre_armed.start(10000, AsyncDelay::MILLIS);
  
  lcd_status();

}

void loop() {

  //print pressed key
  char key = keypad.getKey();
 
  if (key != NO_KEY) {
    lcd.print("*");
    delay(300);
  }

  //print current alarm level
  check_level(current_alarm_level);

  //check if pre armed countdown is over
  if (current_alarm_level == 1 && delay_pre_armed.isExpired()) {
    current_alarm_level = 2;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SCHARF");
    lcd.setCursor(0,1);
    lcd.print("");
    
  }

  if (current_alarm_level == 3 && delay_pre_alarm.isExpired()) {
    current_alarm_level = 4;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ALARM");
    lcd.setCursor(0,1);
    lcd.print("");
  
  }

  //check open door if system is armed
  if (current_alarm_level == 2 && digitalRead(switch_door) == 0) {
    current_alarm_level = 3;
    delay_pre_alarm.start(10000, AsyncDelay::MILLIS);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("VORALARM");
    lcd.setCursor(0,1);
    lcd.print("");
    lcd.blink();
  
  }
  
}

void check_level(int level) {
  if (level == 0) {
    led_armed.off();
    led_prearmed.off();
    led_unarmed.off();
  }
  if (level == 1) {
    led_armed.off();
    if (blink_time_a.isExpired()) {
      led_prearmed.blink(100, 3);
      blink_time_a.repeat();
    }
    led_unarmed.off();
  }
  if (level == 2) {
    if (blink_time.isExpired()) {
      led_armed.blink(100, 2);
      blink_time.repeat(); 
    }
    led_prearmed.off();
    led_unarmed.off();
  }
  if (level == 3) {
    led_armed.blink(200);
    led_prearmed.off();
    led_unarmed.off();
  }
  if (level == 4) {
    led_armed.blink(50);
    led_prearmed.blink(50);
    led_unarmed.blink(50);
  }


  
  
}

