#include <Wire.h>                   // for I2C communication
#include <LiquidCrystal_I2C.h>      // for LCD
#include <RTClib.h>                 // for RTC
#define outputBUZ 12
#define outputLED 8
const int buttonPin = 2;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // create LCD with I2C address 0x27, 16 characters per line, 2 lines
RTC_DS3231 rtc;                     // create rtc for the DS3231 RTC module, address is fixed at 0x68

/*
   function to update RTC time using user input
*/
int count = 0;
int mode = 0;
int start = 0;
int a = 0;
int buttonState = 0;
void updateRTC()
{

    lcd.clear();  // clear LCD display
    lcd.setCursor(0, 0);
    lcd.print("Edit Mode...");

    // ask user to enter new date and time
    const char txt[6][15] = { "year [4-digit]", "month [1~12]", "day [1~31]",
                              "hours [0~23]", "minutes [0~59]", "seconds [0~59]" };
    String str = "";
    long newDate[6];

    while (Serial.available()) {
        Serial.read();  // clear serial buffer
    }

    for (int i = 0; i < 6; i++) {

        Serial.print("Enter ");
        Serial.print(txt[i]);
        Serial.print(": ");

        while (!Serial.available()) {
            ; // wait for user input
        }

        str = Serial.readString();  // read user input
        newDate[i] = str.toInt();   // convert user input to number and save to array

        Serial.println(newDate[i]); // show user input
    }

    // update RTC
    rtc.adjust(DateTime(newDate[0], newDate[1], newDate[2], newDate[3], newDate[4], newDate[5]));
    Serial.println("RTC Updated!");
}

void setup()
{
    pinMode(outputLED, OUTPUT);
    pinMode(outputBUZ, OUTPUT);
    Serial.begin(9600); // initialize serial

    lcd.begin(16, 2);       // initialize lcd
    lcd.backlight();  // switch-on lcd backlight
    pinMode(buttonPin, INPUT);

    rtc.begin();       // initialize rtc
}
int ss2, ss1, mm1, mm2, hh1, hh2 = 0;

void loop()
{
    int newDate[6];
    DateTime rtcTime1 = rtc.now();

    if (Serial.available()) {
        char input = Serial.read();
        if (input == 'u') updateRTC();  // update RTC time
        if (input == 'a') {
            lcd.clear();
            lcd.print("Set Alarm...");

            // ask user to enter new date and time
            const char txt[6][15] = { "year [4-digit]", "month [1~12]", "day [1~31]",
                                      "hours [0~23]", "minutes [0~59]", "seconds [0~59]" };
            String str = "";

            while (Serial.available()) {
                Serial.read();  // clear serial buffer
            }

            for (int i = 0; i < 6; i++) {

                Serial.print("Enter ");
                Serial.print(txt[i]);
                Serial.print(": ");

                while (!Serial.available()) {
                    ; // wait for user input
                }

                str = Serial.readString();  // read user input
                newDate[i] = str.toInt();   // convert user input to number and save to array

                Serial.println(newDate[i]); // show user input
            }
        }
        if (input == 'n') {
            mode = 0;
        }
        if (input == 's') {
            mode = 1;
            lcd.clear();
            lcd.print("Stoperica");
            count = 0;
        }
        if (input == 'o') {
            lcd.clear();
            lcd.print("Odbrojavanje");
            Serial.println("Unesite trajanje odbrojavanja: ");
            while (Serial.available()) {
                Serial.read();  // clear serial buffer
            }
            while (!Serial.available()) {
                ; // wait for user input
            }

            String startString = Serial.readString();  // read user input

            start = startString.toInt();
            a = 0;
            mode = 2;
        }
    }

    if (mode == 0) {
        lcd.clear();
        lcd.print(rtcTime1.twelveHour());
        lcd.print(":");
        lcd.print(rtcTime1.minute());
        lcd.print(":");
        lcd.print(rtcTime1.second());

        delay(1000);
    }

    if (rtcTime1.second() == newDate[5] && rtcTime1.minute() == newDate[4] && rtcTime1.twelveHour() == newDate[3]) {
        lcd.clear();
        lcd.print("DOBRO JUTRO!");
        for (int i = 0; i < 25; i++) {
            digitalWrite(outputLED, HIGH);
            beep(100, outputBUZ);
            delay(100);
        }
        digitalWrite(outputLED, LOW);
    }

    if (mode == 1) {
        buttonState = digitalRead(buttonPin);
        if (count == 0 && buttonState == HIGH) {
            ss1 = rtcTime1.second();
            mm1 = rtcTime1.minute();
            hh1 = rtcTime1.twelveHour();
            count++;

            lcd.clear();
            lcd.print("Start!");
            delay(500);
        }
        else if (count == 1 && buttonState == HIGH) {
            ss2 = rtcTime1.second();
            mm2 = rtcTime1.minute();
            hh2 = rtcTime1.twelveHour();

            count++;

            lcd.clear();
            lcd.print("Stop: ");
            lcd.print(ss2 - ss1);

        }
    }

    if (mode == 2) {
        while (start >= 0) {
            delay(1000);
            lcd.clear();
            lcd.print(start);
            start--;
        }
        if (a == 0) {
            lcd.clear();
            lcd.print("Odbrojano");
            beep(100, outputBUZ);
            a++;
        }
    }

}

void beep(byte delayms, int pin) {
    analogWrite(pin, 250);
    delay(delayms);
    analogWrite(pin, 0);
    delay(delayms);
}