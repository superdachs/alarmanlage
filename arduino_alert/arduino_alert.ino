
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
int led_door =     9;

//switches
int switch_door  = 22;

//states
int current_alarm_level = 0; // 0: safe, 1: prearmed, 2: armed, 3: prealarm, 4: alarm

<<<<<<< HEAD
String pin = "123456";
String current_pin = "      ";
String empty_pin = "      ";
int pin_count = 0;
int tries = 3;

int buzzer = 8;
int horn = 53;

int ckey = 0;

int hard_alarm = 0;

void setup() {

  pinMode(switch_door, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(horn, OUTPUT);
  pinMode(led_door, OUTPUT);

  Serial.begin(9600);
  Serial.println("Starting");

  lcd.begin(16,2);
  lcd.print("Arduino Alert");
  lcd.setCursor(0,1);
  lcd.print("System 0.1");

  lcd_status();
}

void lcd_status() {
  switch (current_alarm_level) {
    case 0:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("UNSCHARF");
      break;
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("SCHARFSCHALTUNG");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("SCHARF");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("VORALARM");
      break;
    case 4:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ALARM");
      break;
  }
  lcd.setCursor(0,1);
  lcd.print("PIN:      VERS:");
  lcd.print(tries);
  
}

int fire_command(char key) {
  Serial.print(pin);
  Serial.print(current_pin);
  Serial.print(key);
  if (current_pin == pin) {
    Serial.print("match");
    if (key == '#') {
      if (current_alarm_level == 0) {
        current_alarm_level = 1;
      } else {
        current_alarm_level = 0;
        digitalWrite(horn, LOW);
      }
      tries = 3;
      pin_count = 0;
      current_pin = empty_pin;
      delay(300);
      beep_h(300);
      beep_h(300);
      lcd_status();
      delay_pre_armed.start(10000, AsyncDelay::MILLIS);
      return 0;
    }
    if (key == '*') {
      pin_count = 0;
      current_pin = empty_pin;
      lcd.setCursor(4,1);
      lcd.print("      ");
      lcd_status();
      return 0;
    }
    tries--;
    pin_count = 0;
    beep(100);
    beep(100);
    beep(100);
    current_pin = empty_pin;
    lcd.setCursor(4,1);
    lcd.print("      ");
    lcd_status();
    ckey = 1;
      
  } else {
    tries--;
    pin_count = 0;
    beep(100);
    beep(100);
    beep(100);
    current_pin = empty_pin;
    lcd.setCursor(4,1);
    lcd.print("      ");
    lcd_status();
    ckey = 1;  
  }
}

void loop() {

  //print pressed key
  char key = keypad.getKey();

  if (tries <= 0 && current_alarm_level < 4) {
    current_alarm_level = 3;
    tries = 3;
  }
  if (tries <= 0 && current_alarm_level >= 4) {
    lcd.clear();
    lcd.print("DAUERALARM");
    lcd.setCursor(0,1);
    lcd.print("015750131525");
    hard_alarm = 1;
  }
  
  if (key != NO_KEY && hard_alarm != 1) {
    beep(100);
    if (key == '*') {
      pin_count = 0;
      current_pin = empty_pin;
      lcd.setCursor(4,1);
      lcd.print("      ");
      lcd_status();
    }
    if (pin_count > 5) {
      fire_command(key);
      
    }
    if (key != '*' && key != '#' && ckey == 0) {
      current_pin[pin_count] = key;
      pin_count++;
      lcd.setCursor(3 + pin_count, 1);
      lcd.print('*');
    }
    ckey = 0;
   
    delay(300);
  }

  //print current alarm level
  check_level(current_alarm_level);

  //check if pre armed countdown is over
  if (current_alarm_level == 1 && delay_pre_armed.isExpired()) {
    current_alarm_level = 2;
    lcd.clear();
    lcd_status();
  }

  if (current_alarm_level == 3 && delay_pre_alarm.isExpired()) {
    current_alarm_level = 4;
    digitalWrite(horn, HIGH);
    lcd_status();
  }

  int doorsw = digitalRead(switch_door);

  if (doorsw == 0) {
    digitalWrite(led_door, HIGH);
  } else {
    digitalWrite(led_door, LOW);
  }

  //check open door if system is armed
  if (current_alarm_level == 2 && doorsw == 0) {
    current_alarm_level = 3;
    delay_pre_alarm.start(10000, AsyncDelay::MILLIS);
    lcd_status();
  }
  
}

void beep(unsigned char delayms){
  analogWrite(buzzer, 100);      // Almost any value can be used except 0 and 255
                           // experiment to get the best tone
  delay(delayms);          // wait for a delayms ms
  analogWrite(buzzer, 0);       // 0 turns it off
  delay(delayms);          // wait for a delayms ms   
}

void beep_h(unsigned char delayms){
  analogWrite(buzzer, 50);      // Almost any value can be used except 0 and 255
                           // experiment to get the best tone
  delay(delayms);          // wait for a delayms ms
  analogWrite(buzzer, 0);       // 0 turns it off
  delay(delayms);          // wait for a delayms ms   
}


void check_level(int level) {
  if (level == 0) {
    led_armed.off();
    led_prearmed.off();
    led_unarmed.on();
    
  }
  if (level == 1) {
    led_armed.off();
    led_prearmed.on();
    led_unarmed.off();
  }
  if (level == 2) {
    led_armed.on();
    led_prearmed.off();
    led_unarmed.off();
  }
  if (level == 3) {
    led_armed.blink(200);
    led_prearmed.off();
    led_unarmed.off();
  }
  if (level == 4) {
    led_armed.off();
    led_armed.blink(50);
    led_prearmed.blink(50);
    led_unarmed.blink(50);
    
  }


  
  
}

