#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define REST     0
const int BUZZER_PIN = 3; 
const int TEMPO = 110;



// Notele melodiei (Deșteaptă-te, române!)
int melody[] = {
  // Masura 1-4: Deșteaptă-te, române, din somnul cel de moarte[cite: 1]
  NOTE_A4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_GS4, NOTE_A4, NOTE_B4, NOTE_E4,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5,
  NOTE_C5, NOTE_B4, NOTE_A4,
  
  // Masura 5-8: În care te-adânciră barbarii de tirani![cite: 1]
  NOTE_A4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_GS4, NOTE_A4, NOTE_B4, NOTE_E4,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_B4,
  NOTE_A4, NOTE_GS4, NOTE_A4,

  // Masura 9-12: Acum ori niciodată croiește-ți altă soartă[cite: 1]
  NOTE_C5, NOTE_D5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_D5, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_D5,
  NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4,
  NOTE_C5, NOTE_B4, NOTE_A4,

  // Masura 13-16: La care să se-nchine și cruzii tăi dușmani![cite: 1]
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,
  NOTE_D5, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_D5,
  NOTE_C5, NOTE_B4, NOTE_A4, NOTE_GS4,
  NOTE_A4
};

// Durata fiecarui note: 4 = patrime, 8 = optime, 16 = saisprezecime etc.
int noteDurations[] = {
  // Masura 1-4
  4, 8, 4, 8, 4,
  4, 8, 4, 4,
  4, 8, 4, 8,
  4, 8, 2,

  // Masura 5-8
  4, 8, 4, 8, 4,
  4, 8, 4, 4,
  4, 8, 4, 8,
  4, 8, 2,

  // Masura 9-12
  4, 8, 4, 8, 4,
  4, 8, 4, 8,
  4, 8, 4, 8,
  4, 8, 2,

  // Masura 13-16
  4, 8, 4, 8, 4,
  4, 8, 4, 8,
  4, 8, 4, 8,
  2
};


int clockH = 0, clockM = 0, clockS = 0;
bool clockSet = false;

int stopH = 0, stopM = 0, stopS = 0;
bool stopSet = false;

char currentMode = 'c'; 

int alarmClockH = -1, alarmClockM = -1;
int melodyClock = 0;
int melodyStopwatch = 0;
bool alarmStopwatch = false; 

unsigned long previousMillis = 0;

void playMelody(int index) {
  int notesCount = sizeof(melody) / sizeof(melody[0]);
  
  // Calculul duratei unei patrimi in milisecunde
  int wholenote = (60000 * 4) / TEMPO;

  for (int thisNote = 0; thisNote < notesCount; thisNote++) {
    int noteDuration = 0;
    int durationValue = noteDurations[thisNote];

    if (durationValue > 0) {
      noteDuration = wholenote / durationValue;
    } else if (durationValue < 0) {
      // Note cu punct (durata crescuta cu 50%)
      noteDuration = wholenote / abs(durationValue);
      noteDuration *= 1.5;
    }

    // Redarea notei pe pinul setat
    if (melody[thisNote] != REST) {
      tone(BUZZER_PIN, melody[thisNote], noteDuration * 0.9);
    } else {
      noTone(BUZZER_PIN);
    }

    // Pauza scurta intre note pentru a le distinge clar
    delay(noteDuration);
    noTone(BUZZER_PIN);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis(); 
  
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis; 
    
    if (clockSet) {
      clockS++; 
      if (clockS >= 60) { clockS = 0; clockM++; }
      if (clockM >= 60) { clockM = 0; clockH++; }
      if (clockH >= 24) { clockH = 0; } 
      
      if (clockH == alarmClockH && clockM == alarmClockM && clockS == 0) {
        playMelody(melodyClock); 
      }
    }
    
    if (stopSet && (stopH > 0 || stopM > 0 || stopS > 0)) {
      stopS--;
      if (stopS < 0) { stopS = 59; stopM--; } 
      if (stopM < 0) { stopM = 59; stopH--; } 
      
      if (stopH == 0 && stopM == 0 && stopS == 0 && !alarmStopwatch) {
        playMelody(melodyStopwatch);
        alarmStopwatch = true;
      }
    }
  }

  if (Serial.available() > 0) {
    String comanda = Serial.readStringUntil('\n');
    comanda.trim();
    
    if (comanda.startsWith("clock ") && comanda.length() > 6 && !comanda.startsWith("clock alarm")) {
      if (!clockSet) {
        clockH = comanda.substring(6, 8).toInt();
        clockM = comanda.substring(9, 11).toInt();
        clockS = 0;
        clockSet = true;
      } else {
        Serial.println("Eroare!");
      }
    }
    else if (comanda == "clock") {
      currentMode = 'c'; 
    }
    else if (comanda.startsWith("stopwatch ") && comanda.length() > 10 && !comanda.startsWith("stopwatch alarm")) {
      if (!stopSet) {
        stopH = 0;
        stopM = comanda.substring(10, 12).toInt();
        stopS = comanda.substring(13, 15).toInt();
        stopSet = true;
        alarmStopwatch = false; 
      } else {
        Serial.println("Eroare!");
      }
    } 
    else if (comanda == "stopwatch") {
      currentMode = 's';
    }
    else if (comanda.startsWith("alarm clock")) {
      alarmClockH = comanda.substring(12, 14).toInt();
      alarmClockM = comanda.substring(15, 17).toInt();
      melodyClock = comanda.substring(18).toInt();
    }
    else if (comanda.startsWith("alarm stopwatch")) {
      melodyStopwatch = comanda.substring(16).toInt();
    }
  }
}