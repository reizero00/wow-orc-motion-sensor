#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// --- Configuration ---
const int PIR_PIN = 12;
const int RX_PIN = 3;
const int TX_PIN = 2;

const unsigned long COOL_DOWN_MS = 300000; // 5 minutes
// const unsigned long COOL_DOWN_MS = 3000; // 3 seconds for testing

// --- Global Variables ---
SoftwareSerial fxSerial(RX_PIN, TX_PIN);
DFRobotDFPlayerMini fxPlayer;

int lastPirState = LOW;

// Setting to negative COOL_DOWN_MS allows immediate execution on power-up
unsigned long lastPlayTime = -COOL_DOWN_MS;

// Setting lastPlayTime = 0 instead starts the cooldown before playing anything
// unsigned long lastPlayTime = 0;


void setup() {
  pinMode(PIR_PIN, INPUT);
  Serial.begin(9600);
  
  fxSerial.begin(9600);
  if (fxPlayer.begin(fxSerial)) {
    fxPlayer.volume(21);
    Serial.println("System Ready");
  } else {
    Serial.println("DFPlayer error!");
  }
}

// --- Helper Functions ---
bool isMotionDetected() {
  int currentPirState = digitalRead(PIR_PIN);
  bool triggered = (currentPirState == HIGH && lastPirState == LOW);
  lastPirState = currentPirState;
  return triggered;
}

bool isMotionEnded() {
  int currentPirState = digitalRead(PIR_PIN);
  bool ended = (currentPirState == LOW && lastPirState == HIGH);
  lastPirState = currentPirState;
  return ended;
}

void attemptToPlayAudio() {
  unsigned long currentTime = millis();
  unsigned long timePassed = currentTime - lastPlayTime;

  if (timePassed >= COOL_DOWN_MS) {
    Serial.println("Playing sound...");
    fxPlayer.play(random(1, 11));
    lastPlayTime = currentTime;
  } else {
    unsigned long remainingSeconds = (COOL_DOWN_MS - timePassed) / 1000;
    Serial.print("Cooldown active. Seconds remaining: ");
    Serial.println(remainingSeconds);
  }
}

// --- Main Loop ---
void loop() {
  if (isMotionDetected()) {
    Serial.println("Motion Detected");
    attemptToPlayAudio();
  }

  if (isMotionEnded()) {
    Serial.println("Motion Ended");
  }
}