#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// --- Configuration ---
const int PIR_PIN = 12;
const int RX_PIN = 3;
const int TX_PIN = 2;

const unsigned long COOL_DOWN_MS = 300000; // 5 minutes
// const unsigned long COOL_DOWN_MS = 3000; // 3 seconds for testing
const int SPEAKER_VOLUME = 23;

const int TOTAL_TRACKS = 11; // 11 tracks total
int trackList[TOTAL_TRACKS];
int currentTrackIndex = 0;

// --- Global Variables ---
SoftwareSerial fxSerial(RX_PIN, TX_PIN);
DFRobotDFPlayerMini fxPlayer;

int lastPirState = LOW;

// Setting to negative COOL_DOWN_MS allows immediate execution on power-up
unsigned long lastPlayTime = -COOL_DOWN_MS;

// Setting lastPlayTime = 0 instead starts the cooldown before playing anything
// unsigned long lastPlayTime = 0;


// --- Helper Functions ---
void shuffleTracks() {
  for (int i = 0; i < TOTAL_TRACKS; i++) {
    trackList[i] = i + 1;
  }
  for (int i = TOTAL_TRACKS - 1; i > 0; i--) {
    int j = random(0, i + 1);
    int temp = trackList[i];
    trackList[i] = trackList[j];
    trackList[j] = temp;
  }
  currentTrackIndex = 0;
  Serial.println("Playlist shuffled!");
}

void setup() {
  pinMode(PIR_PIN, INPUT);
  Serial.begin(9600);
  
  randomSeed(analogRead(A0)); // Seeds randomizer for fresh order on boot
  shuffleTracks();

  fxSerial.begin(9600);
  if (fxPlayer.begin(fxSerial)) {
    fxPlayer.volume(SPEAKER_VOLUME);
    Serial.println("System Ready");
    
    // Play startup confirmation sound immediately on boot
    fxPlayer.play(1); 
  } else {
    Serial.println("DFPlayer error!");
  }
}

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
    int trackToPlay = trackList[currentTrackIndex];

    Serial.print("Playing sound track #");
    Serial.println(trackToPlay);

    fxPlayer.play(trackToPlay);
    lastPlayTime = currentTime;

    currentTrackIndex++;
    if (currentTrackIndex >= TOTAL_TRACKS) {
      Serial.println("All tracks played! Reshuffling pool...");
      shuffleTracks();
    }
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
