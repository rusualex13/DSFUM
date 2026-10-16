int leduri[] = { 8, 9, 10, 11, 12, 13 };
int brightness[6] = { 0, 0, 0, 0, 0, 0 };
int step[6] = { 1, 1, 1, 1, 1, 1 };
int speed[6] = { 5, 10, 15, 20, 25, 30 };
unsigned long antTime[6] = { 0, 0, 0, 0, 0, 0 };

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 6; i++) { 
    pinMode(leduri[i], OUTPUT); 
  }
}

void loop() {
    String comanda = "";
    
    while (Serial.available() > 0) {
      char biti = Serial.read();
      if (biti == '\n') {
        break;
      }
      comanda += biti;
    }

    String comanda_bec = "";
    int led_numar = -1;
    int timp_becului = 0;

    if (comanda.length() > 0) {
      int index_prima_virgula = comanda.indexOf(",");
      int index_doua_virgula = comanda.indexOf(",", index_prima_virgula + 1);

      if (index_prima_virgula != -1 && index_doua_virgula != -1) {
        String becul = comanda.substring(0, index_prima_virgula);
        comanda_bec = comanda.substring(index_prima_virgula + 1, index_doua_virgula);
        String timp_bec = comanda.substring(index_doua_virgula + 1);
        
        led_numar = becul.toInt();
        timp_becului = timp_bec.toInt();
      }
    }

    int index_led = -1;
    for (int i = 0; i < 6; i++) {
      if (leduri[i] == led_numar) {
        index_led = i;
        break;
      }
    }

    if (index_led != -1) {
      if (comanda_bec == "fadein") {
        step[index_led] = 1;
        speed[index_led] = timp_becului / 255;
        if (speed[index_led] < 1) speed[index_led] = 1;
        brightness[index_led] = 0;
      }
      else if (comanda_bec == "fadeout") {
        step[index_led] = -1;
        speed[index_led] = timp_becului / 255;
        if (speed[index_led] < 1) speed[index_led] = 1;
        brightness[index_led] = 255;
      }
      else if (comanda_bec == "fadeinout") {
        step[index_led] = 1;
        speed[index_led] = timp_becului / 510; 
        if (speed[index_led] < 1) speed[index_led] = 1;
        brightness[index_led] = 0;
      }
      else if (comanda_bec == "blink") {
        step[index_led] = 0;
        speed[index_led] = timp_becului / 2; 
        if (speed[index_led] < 1) speed[index_led] = 1;
        brightness[index_led] = 255;
        analogWrite(leduri[index_led], 255);
      }
      else if (comanda_bec == "off") {
        brightness[index_led] = 0;
        analogWrite(leduri[index_led], 0);
      }
     
    }
     for (int i = 0; i < 6; i++) {
      if (millis() - antTime[i] >= speed[i]) {
        antTime[i] = millis();
        brightness[i] += step[i];
        
        if (brightness[i] >= 255) {
          brightness[i] = 255;
          if (step[i] > 0) step[i] = -1; 
        }
        if (brightness[i] <= 0) {
          brightness[i] = 0;
          if (step[i] < 0) step[i] = 1;  
        }

        analogWrite(leduri[i], brightness[i]);
      }
    
      }
  }
