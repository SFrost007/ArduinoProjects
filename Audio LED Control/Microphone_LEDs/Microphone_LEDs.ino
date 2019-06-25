/* David Wang
 * Code that takes audio input from a 3.5mm cable
 * and flashes an LED strip based on the frequency
 * of the music.
 *
 * HUGE thanks to the arduino community
 * If you see your code here, I owe you my gratitude
 *
 */

//#if defined(__AVR_ATmega328__)
int analogPin = 0; // MSGEQ7 OUT
int strobePin = 2; // MSGEQ7 STROBE
int resetPin = 3; // MSGEQ7 RESET
int ledPinR = 9;
int ledPinG = 10;
int ledPinB = 11;

/*
#elif defined(__AVR_ATtiny85__)
int analogPin = 1; // MSGEQ7 OUT
int strobePin = 5; // MSGEQ7 STROBE <- ATtiny Reset re-fused
int resetPin = 2; // MSGEQ7 RESET
int ledPinR = 0;
int ledPinG = 3;
int ledPinB = 4;
#endif
*/


int spectrumValue[7];
int rawValue[7];
 
// MSGEQ7 OUT pin produces values around 50-80
// when there is no input, so use this value to
// filter out a lot of the chaff.
const int filterValue = 80;

// Use this value as the peak received to base the
// LED output levels on
const int MAX_INPUT_LEVEL = 150;
 
 
void setup()
{
  Serial.begin(9600);
  // Read from MSGEQ7 OUT
  pinMode(analogPin, INPUT);
  // Write to MSGEQ7 STROBE and RESET
  pinMode(strobePin, OUTPUT);
  pinMode(resetPin, OUTPUT);
 
  // Set analogPin's reference voltage
  analogReference(DEFAULT); // 5V
 
  // Set startup values for pins
  digitalWrite(resetPin, LOW);
  digitalWrite(strobePin, HIGH);
}
 
void loop()
{
  // Set reset pin low to enable strobe
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
 
  // Get all 7 spectrum values from the MSGEQ7
  for (int i = 0; i < 7; i++)
  {
    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // Allow output to settle
 
    rawValue[i] = analogRead(analogPin);
    spectrumValue[i] = rawValue[i];
 
    // Constrain any value above 1023 or below filterValue
    spectrumValue[i] = constrain(spectrumValue[i], filterValue, MAX_INPUT_LEVEL);
 
 
    // Remap the value to a number between 0 and 255
    spectrumValue[i] = map(spectrumValue[i], filterValue, MAX_INPUT_LEVEL, 0, 255);
 
    // Remove serial stuff after debugging
    if (spectrumValue[i] == 0) {
      Serial.print("   ");
    } else {
      if (spectrumValue[i] < 100) {
        Serial.print(" ");
        if (spectrumValue[i] < 10) {
          Serial.print(" ");
        }
      }
      Serial.print(spectrumValue[i]);
    }
    Serial.print(" ");
    digitalWrite(strobePin, HIGH);
   }

   Serial.print("       ");

   for (int i=0; i<7; i++) {
     if (rawValue[i] < 100) {
       Serial.print(" ");
     }
     Serial.print(rawValue[i]);
     Serial.print(" ");
   }
   
   Serial.println();
 
   // Write the PWM values to the LEDs
   // I find that with three LEDs, these three spectrum values work the best
   // Values are 63Hz, 160Hz, 400Hz, 1kHz, 2.5kHz, 6.25kHz and 16kHz
   // 1kHz and 6.25kHz don't seem to work
   analogWrite(ledPinR, 255 - spectrumValue[0]);
   analogWrite(ledPinG, 255 - spectrumValue[1]);
   analogWrite(ledPinB, 255 - spectrumValue[2]);
}

