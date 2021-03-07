#include<ArduinoJson.h>

const byte ANODE_PINS[8] = {13, 12, 11, 10, 9, 8, 7, 6};
                           // {6, 7, 8, 9, 10, 11, 12, 13}
const byte CATHODE_PINS[8] = {2, 3, 4, 5, A0, A1, A2, A3};
                             // {A3, A2, A1, A0, 5, 4, 3, 2}
void setup() {
  Serial.begin(115200);

  // initialize pins as output mode
  for (byte i = 0; i < 8; i++) {
    pinMode(ANODE_PINS[i], OUTPUT);
    pinMode(CATHODE_PINS[i], OUTPUT);
  }

  // set pins as HIGH to make sure the circuit is off
  for (byte i = 0; i < 8; i++) {
    digitalWrite(ANODE_PINS[i], HIGH);
    digitalWrite(CATHODE_PINS[i], HIGH);
  }
  Serial.println("Setup completed");
}

void display(byte pattern[8][8]) {

  // loop through all leds
  for (byte x = 0; x < 8; x++) {
    for (byte y = 0; y < 8; y++) {

      // if LED is enabled, turn on cathode
      if (pattern[y][x]) digitalWrite(CATHODE_PINS[y], LOW);
    }

    // enable the LED row
    digitalWrite(ANODE_PINS[x], LOW);
    delay(1);

    // disable all LEDs on row
    for (byte y = 0; y < 8; y++) {
      if (pattern[y][x]) digitalWrite(CATHODE_PINS[y], HIGH);
    }
    digitalWrite(ANODE_PINS[x], HIGH);

  }
}

//recursively obtain digits of int
template <class T>
int split(int *a, T number)
{
  int index = 0;
  if (number > 9)
  {
    index = split(a, number / 10);
  }
  a[index] = number % 10;
  return index + 1;
}

//determining the digits of a number
template <class T>
int numDigits(T number)
{
  int digits = 0;
  if (number < 0)
    digits = 1;
  while (number)
  {
    number /= 10;
    digits++;
  }
  return digits;
}

//scrolling display of an integer
void displayNumber(unsigned long number)
{
  unsigned long lastUpdate = millis();
  //definition
  bool numberarrays[10][3][5] = {{{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}},
                                 {{0, 0, 0, 0, 0}, {1, 1, 1, 1, 1}, {0, 0, 0, 0, 0}},
                                 {{1, 0, 1, 1, 1}, {1, 0, 1, 0, 1}, {1, 1, 1, 0, 1}},
                                 {{1, 0, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 1, 1, 1, 1}},
                                 {{1, 1, 1, 0, 0}, {0, 0, 1, 0, 0}, {1, 1, 1, 1, 1}},
                                 {{1, 1, 1, 0, 1}, {0, 0, 1, 0, 0}, {1, 0, 1, 1, 1}},
                                 {{1, 1, 1, 1, 1}, {1, 0, 1, 0, 1}, {1, 0, 1, 1, 1}},
                                 {{1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}},
                                 {{1, 1, 1, 1, 1}, {1, 0, 1, 0, 1}, {1, 1, 1, 1, 1}},
                                 {{1, 1, 1, 0, 0}, {1, 0, 1, 0, 0}, {1, 1, 1, 1, 1}}};

  int numDigit = numDigits(number);
  int digits[numDigit];
  split(digits, number);

  bool pattern[4 * numDigit][5]{0};
  for (int pos = 0; pos < numDigit; pos++)
  {
    for (int j = 0; j < 4; j++)
    {
      for (int i = 0; i < 5; i++)
      {
        if (j % 4 != 3)
        { //space 1 column between 2 numbers
          pattern[4 * pos + j][i] = numberarrays[digits[pos]][j % 4][i];
        }
      }
    }
  }

  byte scrollCoord = 0;
  byte ledOn[8][8]{0};
  while (scrollCoord < 4 * numDigit + 16)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate > 170)
    { //update every 100ms
      scrollCoord++;
      lastUpdate = currentMillis;
      for (int j = 0; j < 8; j++)
      {
        for (int i = 1; i < 6; i++)
        {
          if (scrollCoord + j > 7 && scrollCoord + j < 4 * numDigit + 8)
          {
            ledOn[7-j][i] = pattern[scrollCoord + j - 8][i - 1];
          }
          else
          {
            ledOn[7-j][i] = 0;
          }
        }
      }
    }
    display(ledOn);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  StaticJsonDocument<400> doc;


  if (Serial.available()) {
    String str = Serial.readStringUntil('\n');
    DeserializationError error = deserializeJson(doc, str);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    unsigned long op = doc["temp"];
    displayNumber(op);
  }
}
