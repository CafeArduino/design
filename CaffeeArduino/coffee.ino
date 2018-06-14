
/**************************************************************/
/**************************************************************/
/*                  Coffee Machine functions                  */
/**************************************************************/
/*                                                            */
/*                                                            */
/**************************************************************/

String inData;

void setup_CoffeeMachine() {
  inData = "";
  //Serial1.begin(9600);     // start serial communication at 9600bps
  //Serial.begin(9600);
}

// Read the data from the coffee machine
String fromCoffeeMachine() {
  String inputString = "";
  char d4 = 255;
  while (Serial1.available()) {    // if data is available to read
    byte d0 = Serial1.read();
    //Serial.print(d0, HEX);
    delay (1); 
    byte d1 = Serial1.read();
    //Serial.print(d1, HEX);
    delay (1);
    byte d2 = Serial1.read();
    //Serial.print(d2, HEX);
    delay (1);
    byte d3 = Serial1.read();
    //Serial.print(d3, HEX);
    delay (7);
    bitWrite(d4, 0, bitRead(d0,2));
    bitWrite(d4, 1, bitRead(d0,5));
    bitWrite(d4, 2, bitRead(d1,2));
    bitWrite(d4, 3, bitRead(d1,5));
    bitWrite(d4, 4, bitRead(d2,2));
    bitWrite(d4, 5, bitRead(d2,5));
    bitWrite(d4, 6, bitRead(d3,2));
    bitWrite(d4, 7, bitRead(d3,5));

    //Serial.print(d4, HEX);

    if (d4 != 10){ 
      inputString += d4;
    } 
    else { 
      inputString += d4;
      return(inputString);
    }
    
    delay(2);
  }
  delay(20);
}

// Send a command to the coffee machine
void toCoffeeMachine(String outputString) {
  for (byte i=0; i<outputString.length(); i++){
    byte d0 = 0xFF;
    byte d1 = 0xFF;
    byte d2 = 0xFF;
    byte d3 = 0xFF;
    bitWrite(d0, 2, bitRead(outputString.charAt(i),0));
    bitWrite(d0, 5, bitRead(outputString.charAt(i),1));
    bitWrite(d1, 2, bitRead(outputString.charAt(i),2));  
    bitWrite(d1, 5, bitRead(outputString.charAt(i),3));
    bitWrite(d2, 2, bitRead(outputString.charAt(i),4));
    bitWrite(d2, 5, bitRead(outputString.charAt(i),5));
    bitWrite(d3, 2, bitRead(outputString.charAt(i),6));  
    bitWrite(d3, 5, bitRead(outputString.charAt(i),7)); 
    Serial1.write(d0); 
    delay(1);
    Serial1.write(d1); 
    delay(1);
    Serial1.write(d2); 
    delay(1);
    Serial1.write(d3); 
    delay(7);
  }
}
/**************************************************************/






// Returns:
//   - OK
//   - FAIL
int brewCoffee(coffee_t type){
  String cmd = String();

  logging("brewCoffee");
  
  // TODO: implement coffee types
  cmd = "FA:04"; // << type;
  
  logging(String("brewCoffe: sending") + cmd);  
  toCoffeeMachine(cmd);
  
}
