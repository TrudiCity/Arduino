String input = "on2000";
String textPart = "";
String numberPart = "";

void setup() {
    Serial.begin(9600);

    // Den Punkt finden, an dem die Zahl beginnt
    int index = 0;
    while (index < input.length() && !isDigit(input[index])) {
        index++;
    }

    // Die Teile extrahieren
    textPart = input.substring(0, index);
    numberPart = input.substring(index);

    // Ausgabe
    Serial.print("Text: ");
    Serial.println(textPart);
    
    Serial.print("Zahl: ");
    Serial.println(numberPart.toInt()); // Wandelt die Zahl in einen Integer um
}

void loop() {
}