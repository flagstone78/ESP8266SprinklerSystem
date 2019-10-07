const int relayPins[] = {12,16,4,10,15,5,13,14}; //0,2,3,4,6,7 
const int numberOfRelayPins = (int)( sizeof(relayPins) / sizeof(relayPins[0]));
bool relayState[numberOfRelayPins];


void relaySetup(){
    for(int i = 0; i<numberOfRelayPins; i++){
    pinMode(relayPins[i],OUTPUT);
    digitalWrite(relayPins[i],LOW);
    relayState[i] = false;
  }
}

void setRelay(int pin){
  if(pin >= 0 && pin < numberOfRelayPins){
    relayState[pin] = true;
  }
}

void outputRelayState(){
  Serial.print("Activating relays");
  for(int i = 0; i < numberOfRelayPins; i++){
    digitalWrite(relayPins[i], relayState[i]); //write to output
    Serial.print(relayState[i]);
    Serial.print(", ");
  }
  Serial.println();
}

//sets relay state to 0 but does not change pin states
void resetRelayState(){
  for(int i = 0; i < numberOfRelayPins; i++){
    relayState[i] = false;
  }
}
