const int relayPin = 12;
const int startSig = 11;
static unsigned int onTime = 2000;
static unsigned int offTime = 2000;
static unsigned int pause_flag = true;
static unsigned int man_flag = true;
static enum {IDLE, ON, OFF} manual_mode;
String textPart = "";
String numberPart = "";

void setup() {
  pinMode(relayPin, OUTPUT);
  pinMode(startSig, OUTPUT);
  digitalWrite(relayPin, HIGH);
  digitalWrite(startSig, HIGH);
  Serial.begin(2400);
  Serial.flush();  // Eingangspuffer der seriellen Schnittstelle leeren
}

void thread_taktgeber(){
  static enum {INIT,AUS, EIN, MAN, PAUSE} zustand = INIT;
  static unsigned long int warteSeit = 0;

  switch (zustand)
  {
    case INIT:
      warteSeit = millis();
      zustand = AUS;
//      Serial.println(onTime);
//      Serial.println(offTime);
      break;

    case AUS:   
      if (pause_flag == true){
        ;
        zustand = PAUSE;
      }
      else if (millis() - warteSeit >= offTime) 
      {
        digitalWrite(relayPin, HIGH); 
        digitalWrite(startSig, HIGH); 
        warteSeit = warteSeit + onTime;
        zustand = EIN; 
      }          
      break;

    case EIN: 
      if (millis() - warteSeit >= onTime) 
      {
        digitalWrite(relayPin, LOW); 
        digitalWrite(startSig, LOW); 
        warteSeit = warteSeit + offTime;
        zustand = AUS; 
      }          
      break;

    case MAN:
      switch (manual_mode){
        case OFF:
          digitalWrite(relayPin, HIGH);
          digitalWrite(startSig, HIGH);
          manual_mode = IDLE;
        case ON:
          digitalWrite(relayPin, LOW);
          digitalWrite(startSig, LOW);
          manual_mode = IDLE;
        case IDLE:
          break;
      }

    case PAUSE:
      if (pause_flag == false){
        digitalWrite(relayPin, HIGH);
        digitalWrite(startSig, HIGH); 
        warteSeit = warteSeit + onTime;
//        zustand = EIN; 
        zustand = INIT; 
      }
      break;
  }
}

void thread_serial(){
  String cmd; // Command-String
  int index = 0;

  if (Serial.available() > 0){
    cmd = Serial.readStringUntil('\r');
    Serial.flush();  // Eingangspuffer der seriellen Schnittstelle leeren
    if (cmd == "P"){
      pause_flag = true;
      Serial.println(cmd + " " + pause_flag);
    } else if (cmd == "p"){
      pause_flag = true;
      Serial.println(cmd + " " + pause_flag);
    } else if (cmd == "W"){
      pause_flag = false;
      Serial.println(cmd + " " + pause_flag);
    } else if (cmd == "w"){
      pause_flag = false;
      Serial.println(cmd + " " + pause_flag);
    } else if (cmd == "manON"){
      man_flag = false;
    } else if (cmd == "manOFF"){
      man_flag = true;
    }
      {
      // hier String zerlegen für Eingaben von on... of... Time
      while (index < cmd.length() && !isDigit(cmd[index])){
        index++;
      }

      textPart = cmd.substring(0,index);
      numberPart = cmd.substring(index);
      
      if (textPart == "on"){
        onTime = numberPart.toInt();
        Serial.print("Text: ");
        Serial.println(textPart);
        Serial.print("Zahl: ");
        Serial.println(numberPart.toInt()); // Wandelt die Zahl in einen Integer um
        Serial.println(onTime);
      } else if (textPart == "of"){
        offTime = numberPart.toInt();
        Serial.print("Text: ");
        Serial.println(textPart);
        Serial.print("Zahl: ");
        Serial.println(numberPart.toInt()); // Wandelt die Zahl in einen Integer um
        Serial.println(offTime);
      }
    } 
  }
}

void loop() {
  thread_serial();
  thread_taktgeber();
//  thread_manMode();
//  thread_autMode();
}
