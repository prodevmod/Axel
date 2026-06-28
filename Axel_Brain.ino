#include <LiquidCrystal.h>
#include <Servo.h>

#define PIR_PIN 2
#define TRIG_PIN 5
#define ECHO_PIN 4
#define SPEAKER_PIN 3
#define SERVO_PIN 6

#define LCD_RS 7
#define LCD_EN 8
#define LCD_D4 9
#define LCD_D5 10
#define LCD_D6 11
#define LCD_D7 12

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
Servo neck;

unsigned long lastMotionTime = 0;
unsigned long lastEmotionTime = 0;
const unsigned long TIMEOUT = 300000; 

bool searching = false;
bool scared = false;
bool rageQuit = false;
int scareCount = 0;

String facts[] = {
  "Robots can learn",
  "First bot: 1954",
  "Mars rover rocks",
  "Bots need code",
  "Servo is precise"
};

byte eye_one[8] = {B00000, B00000, B11111, B11111, B11111, B11111, B00000, B00000};
byte backslash[8] = {0b00000, 0b10000, 0b01000, 0b00100, 0b00010, 0b00001, 0b00000, 0b00000};
byte magnifyingGlass[8] = {0b11110, 0b10010, 0b10010, 0b11110, 0b00001, 0b00001, 0b00001, 0b00000};

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
  neck.attach(SERVO_PIN);
  neck.write(90);
  randomSeed(analogRead(0));
  lcd.begin(16, 2);
  lcd.createChar(0, eye_one);
  lcd.createChar(1, backslash); 
  lcd.createChar(2, magnifyingGlass); 
  
  lcd.print("Robot Booting");
  delay(1500);
  lcd.clear();
  lcd.print("Soldering at 2am");
  lcd.setCursor(0, 1);
  lcd.print("is fire...");
  delay(3000);
  idleFace();
  lastMotionTime = millis();
  lastEmotionTime = millis();
}

void loop() {
  long dist = getDistance();
  if (digitalRead(PIR_PIN) == HIGH && !rageQuit) {
    lastMotionTime = millis();
    searching = false;
  }
  if (!searching && !rageQuit && millis() - lastMotionTime > TIMEOUT) {
    searching = true;
    lcd.clear();
    lcd.print("SEARCHING...");
  }
  if (searching) {
    searchForPerson();
  } else if (!rageQuit) {
    if (dist > 0 && dist < 10) {
      if (!scared) {
        scared = true;
        scareCount++;
        if (scareCount >= 4) rageQuitMode();
        else isScared();
      }
      lastEmotionTime = millis();
    } else {
      scared = false;
      if (millis() - lastEmotionTime > 12000) {
        int action = random(0, 4);
        if (action == 0) isBored();
        else if (action == 1) isHappy();
        else if (action == 2) isAngry();
        else sayFact();
        lastEmotionTime = millis();
      }
    }
  } else if (rageQuit && dist < 10) {
    rageQuit = false;
    scareCount = 0;
    lcd.clear();
    lcd.print("FINE. IM BACK.");
    delay(2000);
    idleFace();
  }
}

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  return pulseIn(ECHO_PIN, HIGH) * 0.034 / 2;
}

void searchForPerson() {
  lcd.setCursor(6, 0); lcd.write(byte(0)); lcd.setCursor(8, 0); lcd.write(byte(0));
  lcd.setCursor(6, 1); lcd.print("___"); lcd.setCursor(9, 1); lcd.write(2);
  if (getDistance() > 0 && getDistance() < 30) foundPerson();
}

void foundPerson() {
  searching = false;
  lcd.clear(); lcd.print("WELCOME BACK");
  neck.write(90);
  tone(SPEAKER_PIN, 600, 150); delay(200);
  tone(SPEAKER_PIN, 800, 150); delay(200);
  tone(SPEAKER_PIN, 1200, 300);
  delay(3000);
  idleFace();
  lastEmotionTime = millis();
}

void idleFace() {
  neck.write(90);
  lcd.clear(); lcd.setCursor(6, 0); lcd.write(byte(0)); lcd.setCursor(8, 0); lcd.write(byte(0));
  lcd.setCursor(6, 1); lcd.write(byte(1)); lcd.setCursor(7, 1); lcd.print("_/");
}

void isScared() {
  lcd.clear(); lcd.setCursor(6, 0); lcd.write(byte(0)); lcd.setCursor(8, 0); lcd.write(byte(0));
  lcd.setCursor(7, 1); lcd.print("O");
  for(int i = 0; i < 6; i++) {
    neck.write(i % 2 == 0 ? 120 : 60);
    tone(SPEAKER_PIN, 2000, 50);
    delay(100);
  }
  neck.write(90);
  delay(2000);
  idleFace();
}

void isAngry() {
  lcd.clear(); lcd.setCursor(2, 0); lcd.print("Type faster"); lcd.setCursor(6, 1); lcd.print(">_<");
  if (random(0, 2) == 0) {
    for(int i = 0; i < 7; i++) {
      neck.write(135); tone(SPEAKER_PIN, 165, 200); delay(200);
      neck.write(45); tone(SPEAKER_PIN, 175, 200); delay(200);
    }
  } else {
    for(int i = 0; i < 16; i++) {
      neck.write(i % 2 == 0 ? 110 : 70);
      tone(SPEAKER_PIN, 1100, 50);
      delay(max(50, 500 - (i * 30)));
    }
    tone(SPEAKER_PIN, 1200, 1200); delay(1200);
  }
  noTone(SPEAKER_PIN);
  delay(3000);
  idleFace();
}

void isBored() {
  lcd.clear(); lcd.setCursor(2, 0); lcd.print("Start typing"); lcd.setCursor(6, 1); lcd.print("-_-");
  neck.write(135); tone(SPEAKER_PIN, 300, 500); delay(600);
  neck.write(45); tone(SPEAKER_PIN, 200, 800);
  delay(4500);
  idleFace();
}

void isHappy() {
  lcd.clear(); lcd.setCursor(4, 0); lcd.print("GOOD JOB"); lcd.setCursor(6, 1); lcd.print("^_^");
  int melody[] = { 330, 330, 349, 392, 392, 349, 330, 294, 262, 262, 294, 330, 330, 294, 294 };
  for (int i = 0; i < 15; i++) {
    neck.write(i % 2 == 0 ? 110 : 70);
    tone(SPEAKER_PIN, melody[i], 250);
    delay(300);
  }
  neck.write(90);
  delay(3000);
  idleFace();
}

void sayFact() {
  lcd.clear(); lcd.setCursor(0, 0); lcd.print("Robot Fact:");
  lcd.setCursor(0, 1); lcd.print(facts[random(0, 5)]);
  for(int i = 0; i < 3; i++) {
    neck.write(45); delay(500); neck.write(135); delay(500);
  }
  neck.write(90);
  delay(2000);
  idleFace();
}

void rageQuitMode() {
  rageQuit = true;
  lcd.clear(); lcd.print("Thats absolute"); lcd.setCursor(4, 1); lcd.print("ragebait");
  neck.write(180);
  tone(SPEAKER_PIN, 150, 400); delay(500);
  tone(SPEAKER_PIN, 100, 800); delay(1500);
  lcd.clear(); lcd.setCursor(2, 0); lcd.print("I am leaving");
  delay(3000);
  lcd.clear(); 
}
