
/*  Pulse Sensor Amped 1.5    by Joel Murphy and Yury Gitman   http://www.pulsesensor.com

  ----------------------  Notes ----------------------  ----------------------
  This code:
  1) Blinks an LED to User's Live Heartbeat   PIN 13
  2) Fades an LED to User's Live HeartBeat    PIN 5
  3) Determines BPM
  4) Prints All of the Above to Serial

  Read Me:
  https://github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino/blob/master/README.md
  ----------------------       ----------------------  ----------------------
*/
// May 30,2018
// ESP32 version by coniferconifer
// https://github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino/
// #define ESP32

#define PROCESSING_VISUALIZER 1
#define SERIAL_PLOTTER  2

//  Variables
#define ESP32
#ifdef ESP32
#define LEDC_CHANNEL_0     0
#define LEDC_CHANNEL_1     1
#define LEDC_TIMER_8_BIT  8
#define LEDC_BASE_FREQ     5000
int pulsePin = 34;                 // Pulse Sensor purple wire connected to analog pin 34 , ADC6
int blinkPin = 13;                 // pin to blink led at each beat
int fadePin = 5;                  // pin to do fancy classy fading blink at each beat
#else
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = 13;                // pin to blink led at each beat
int fadePin = 5;                  // pin to do fancy classy fading blink at each beat
#endif
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded!
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// SET THE SERIAL OUTPUT TYPE TO YOUR NEEDS
// PROCESSING_VISUALIZER works with Pulse Sensor Processing Visualizer
//      https://github.com/WorldFamousElectronics/PulseSensor_Amped_Processing_Visualizer
// SERIAL_PLOTTER outputs sensor data for viewing with the Arduino Serial Plotter
//      run the Serial Plotter at 115200 baud: Tools/Serial Plotter or Command+L
static int outputType = SERIAL_PLOTTER;
//static int outputType = PROCESSING_VISUALIZER;

void setup() {
  pinMode(blinkPin, OUTPUT);        // pin that will blink to your heartbeat!
  pinMode(fadePin, OUTPUT);         // pin that will fade to your heartbeat!
  Serial.begin(115200);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS
  // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE,
  // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN
  //   analogReference(EXTERNAL);
#ifdef ESP32
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_8_BIT) ; // 8bit precision
  ledcAttachPin(fadePin, LEDC_CHANNEL_0) ; // assaign LED1 to CH0
#endif
}


//  Where the Magic Happens
void loop() {

  serialOutput() ;

  if (QS == true) {    // A Heartbeat Was Found
    // BPM and IBI have been Determined
    // Quantified Self "QS" true when arduino finds a heartbeat
    fadeRate = 255;         // Makes the LED Fade Effect Happen
    // Set 'fadeRate' Variable to 255 to fade LED with pulse
    serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.
    QS = false;                      // reset the Quantified Self flag for next time
  }

  ledFadeToBeat();                      // Makes the LED Fade Effect Happen
  delay(20);                             //  take a break
}





void ledFadeToBeat() {
  fadeRate -= 15;                         //  set LED fade value
  fadeRate = constrain(fadeRate, 0, 255); //  keep LED fade value from going into negative numbers!
#ifdef ESP32
  ledcWrite(LEDC_CHANNEL_0, fadeRate) ;
#else
  analogWrite(fadePin, fadeRate);         //  fade LED
#endif

}
