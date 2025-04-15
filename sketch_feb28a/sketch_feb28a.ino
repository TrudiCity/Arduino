const int relayPin = 12;
const int startSig = 11;
static unsigned long int onTime = 2000;
static unsigned long int offTime = 2000;
static unsigned int pause_flag = true;
static unsigned int manual_flag = false;
static enum {IDLE, OFF, ON} manual_mode = IDLE;
String textPart = "";
String numberPart = "";

void setup() {
  pinMode(relayPin, OUTPUT);
  pinMode(startSig, OUTPUT);
  digitalWrite(relayPin, HIGH);
  digitalWrite(startSig, HIGH);
  Serial.begin(9600);
  Serial.flush();  // Eingangspuffer der seriellen Schnittstelle leeren
}

void thread_manualMode(){

  switch (manual_mode){

    case IDLE:
      break;

    case OFF:
      digitalWrite(relayPin, HIGH); 
      digitalWrite(startSig, HIGH); 
      manual_mode = IDLE;
      break;

    case ON:
      digitalWrite(relayPin, LOW); 
      digitalWrite(startSig, LOW); 
      manual_mode = IDLE;
      break;

  }
}

void thread_taktgeber(){
  static enum {INIT, AUS, EIN, PAUSE} zustand = INIT;
  static unsigned long int warteSeit = 0;
  static int caseFlag = 0;

  switch (zustand)
  {
    case INIT:
      if (caseFlag == 0){
        Serial.println("INIT");
        caseFlag += 1;
      }
      warteSeit = millis();
      zustand = AUS;
      break;

    case AUS:   
      if ((caseFlag == 1) && !(caseFlag == 2)){
        Serial.println("AUS");
        caseFlag += 1;
      }

      if (pause_flag == true){
        zustand = PAUSE;
      }
      else if (millis() - warteSeit >= offTime) 
      {
        digitalWrite(relayPin, LOW); 
        digitalWrite(startSig, LOW); 
        warteSeit = warteSeit + offTime;
        zustand = EIN; 
      }          
      break;

    case EIN: 
      if (caseFlag == 2){
        Serial.println("EIN");
        caseFlag += 1;
      }

      if (millis() - warteSeit >= onTime) 
      {
        digitalWrite(relayPin, HIGH); 
        digitalWrite(startSig, HIGH); 
        warteSeit = warteSeit + onTime;
        zustand = AUS; 
      }          
      break;

    case PAUSE:
      if (caseFlag == 3){
        Serial.println("PAUSE");
        caseFlag += 1;
      }

      if (pause_flag == false){
        digitalWrite(relayPin, LOW); 
        digitalWrite(startSig, LOW); 
//        warteSeit = warteSeit + onTime;
//        zustand = EIN; 
        zustand = INIT; 
        caseFlag = 0;
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
    } else if (cmd == "mOn"){
      manual_mode = ON;
    } else if (cmd == "mOff"){
      manual_mode = OFF;
    } else if ((cmd == "a") || (cmd == "A")){
      manual_flag = false;
    } else if ((cmd == "m") || (cmd == "M")){
      manual_flag = true;
    } else {
      // hier String zerlegen für Eingaben von on... of... Time
      while (index < cmd.length() && !isDigit(cmd[index])){
        index++;
      }

      textPart = cmd.substring(0,index);
      numberPart = cmd.substring(index);
      
      if (textPart == "on"){
        onTime = numberPart.toInt();
        Serial.println("onTime: " + String(numberPart.toInt()));
      } else if (textPart == "of"){
        offTime = numberPart.toInt();
        Serial.println("offTime: " + String(numberPart.toInt()));
      }
    } 
  }
}

void loop() {
  thread_serial();
  if (manual_flag == false){
    thread_taktgeber();
  } else if (manual_flag == true){
    thread_manualMode();
  }
}
