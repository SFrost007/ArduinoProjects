#define MSGEQ_CHANNELS 7
//#define ATTINY_BUILD

/*#ifdef ATTINY_BUILD
const int PIN_ANALOG = 1; // MSGEQ7 OUT
const int PIN_STROBE = 5; // MSGEQ7 STROBE
const int PIN_RESET = 2; // MSGEQ7 RESET
const int PIN_LED_R = 0;
const int PIN_LED_G = 3;
const int PIN_LED_B = 4;
#else*/
//#define DEBUG
const int PIN_ANALOG = 0; // MSGEQ7 OUT
const int PIN_STROBE = 2; // MSGEQ7 STROBE
const int PIN_RESET = 3; // MSGEQ7 RESET
const int PIN_LED_R = 9;
const int PIN_LED_G = 10;
const int PIN_LED_B = 11;
//#endif


int inputValues[MSGEQ_CHANNELS];
int cleanValues[MSGEQ_CHANNELS];
int outputValues[MSGEQ_CHANNELS];

// Values used for adaptive tracking
int currentLoop = 0;
int peakValues[MSGEQ_CHANNELS];
int prevValues[MSGEQ_CHANNELS];
int deltas[MSGEQ_CHANNELS];

// MSGEQ Values are 63Hz, 160Hz, 400Hz, 1kHz, 2.5kHz, 6.25kHz and 16kHz
// MSGEQ7 OUT pin produces values around 50-80
// when there is no input, so use this value to
// filter out a lot of the chaff.
const int MSGEQ_FILTER = 80;
const int SILENCE_FILTER = MSGEQ_FILTER + 10;

// MSGEQ Channels used for each LED colour. These will be
// updated based on the stat tracking to adapt to the
// current music.
int channelR = 0;
int channelG = 1;
int channelB = 2;

void setup()
{
#ifdef DEBUG
  Serial.begin(9600);
#endif
  // Read from MSGEQ7 OUT
  pinMode(PIN_ANALOG, INPUT);
  // Write to MSGEQ7 STROBE and RESET
  pinMode(PIN_STROBE, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);

  // Set analogPin's reference voltage
  analogReference(DEFAULT); // 5V

  // Set startup values for pins
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_STROBE, HIGH);

  // Initialize our arrays
  for (int i = 0; i < MSGEQ_CHANNELS; i++) {
    peakValues[i] = 0;
    deltas[i] = 0;
    prevValues[i] = 0;
  }
}

void loop()
{
  readInputValues();
  cleanInputValues();
  writeOutputs();
  updateStats();

#ifdef DEBUG
  writeDebug();
#endif

  currentLoop++;
  if (currentLoop % 100 == 0) {
    resetLevels();
  }
  if (currentLoop == 100) {
    reconfigureChannels();
    resetStats();
  } else if (currentLoop == 1000) {
    currentLoop = 0;
  }
}


void readInputValues()
{
  // Set reset pin low to enable strobe
  digitalWrite(PIN_RESET, HIGH);
  digitalWrite(PIN_RESET, LOW);

  // Read all the seven values
  for (int i=0; i<MSGEQ_CHANNELS; i++) {
    digitalWrite(PIN_STROBE, LOW);
    delayMicroseconds(30); // Allow output to settle
    inputValues[i] = analogRead(PIN_ANALOG);
    digitalWrite(PIN_STROBE, HIGH);
  }
}


void cleanInputValues()
{
  for (int i=0; i<MSGEQ_CHANNELS; i++) {
    if (peakValues[i] < SILENCE_FILTER) {
      // Guard against cases where the peak is only marginally above the input
      // filter level (i.e. near-silent)
      cleanValues[i] = 0;
    } else {
      cleanValues[i] = inputValues[i];
  
      // Constrain any value above peak or below filterValue
      cleanValues[i] = constrain(cleanValues[i], MSGEQ_FILTER, peakValues[i]);
  
      // Remap the value to a number between 0 and 255
      cleanValues[i] = map(cleanValues[i], MSGEQ_FILTER, peakValues[i], 0, 255);
  
      // Guard against negative values that crop up somehow despite the constrain
      if (cleanValues[i] < 0) {
        cleanValues[i] = 0;
      }
    }
  }
}


void writeOutputs()
{
  analogWrite(PIN_LED_R, 255-cleanValues[channelR]);
  analogWrite(PIN_LED_G, 255-cleanValues[channelG]);
  analogWrite(PIN_LED_B, 255-cleanValues[channelB]);
}


void updateStats()
{
  for (int i = 0; i < MSGEQ_CHANNELS; i++) {
    peakValues[i] = max(peakValues[i], inputValues[i]);
    deltas[i] += abs(prevValues[i] - inputValues[i]);
    prevValues[i] = inputValues[i];
  }
}


void resetStats()
{
#ifdef DEBUG
  Serial.println("Resetting stats");
#endif
  for (int i = 0; i < MSGEQ_CHANNELS; i++) {
    deltas[i] = 0;
    prevValues[i] = 0;
  }
}


void resetLevels()
{
#ifdef DEBUG
  Serial.println("Resetting levels");
#endif
  for (int i = 0; i < MSGEQ_CHANNELS; i++) {
    peakValues[i] = 0;
  }
}

struct ChannelData {
  int delta;
  int channel;
};

void reconfigureChannels()
{
#ifdef DEBUG
  Serial.println("Reconfiguring channels....");
  Serial.print("Deltas:");
  printArray(deltas);
#endif

  // Build a structure to sort
  ChannelData info[MSGEQ_CHANNELS];
  for (int i = 0; i < MSGEQ_CHANNELS; i++) {
    info[i] = {deltas[i], i};
  }

  // Just use a lazy bubble-sort
  for(int i=0; i<(MSGEQ_CHANNELS-1); i++) {
    for (int o=0; o<(MSGEQ_CHANNELS-(i+1)); o++) {
      if (info[o].delta < info[o+1].delta) {
        ChannelData tmp = info[o];
        info[o] = info[o+1];
        info[o+1] = tmp;
      }
    }
  }

  // Now sort the top three back in order of frequency
  for (int i=0; i<3-1; i++) {
    for (int j=0; j<(3-(i+1)); j++) {
      if (info[j].channel < info[j+1].channel) {
        ChannelData tmp = info[j];
        info[j] = info[j+1];
        info[j+1] = tmp;
      }
    }
  }

  channelR = info[0].channel;
  channelG = info[1].channel;
  channelB = info[2].channel;

#ifdef DEBUG
  Serial.print("  R Channel: "); Serial.print(channelR);
  Serial.print("  G Channel: "); Serial.print(channelG);
  Serial.print("  B Channel: "); Serial.print(channelB);
  Serial.println();
#endif
}


void writeDebug()
{
#ifdef DEBUG
  printArray(cleanValues);
  Serial.print("  ");
  printArray(inputValues);
  //Serial.print("  ");
  //printArray(deltas);
  String chInfo = "";
    chInfo = chInfo + "  R:" + channelR + ":" + peakValues[channelR];
    chInfo = chInfo + "  G:" + channelG + ":" + peakValues[channelG];
    chInfo = chInfo + "  B:" + channelB + ":" + peakValues[channelB];
  Serial.print(chInfo);
  Serial.print("  ");
  Serial.println(currentLoop);
#endif
}

void printArray(int arr[])
{
#ifdef DEBUG
  String str = "[";
  for (int i = 0; i < MSGEQ_CHANNELS; i++) {
    if (arr[i] == 0) {
      str = str + "   ,";
    } else {
      if (arr[i] < 100) {
        str = str + " ";
        if (arr[i] < 10) {
          str = str + " ";
        }
      }
      str = str + arr[i] + ",";
    }
    str = str + " ";
  }
  str = str.substring(0, str.length()-2) + "]";
  Serial.print(str);
#endif
}

