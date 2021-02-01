const int BRAKE_BIAS_PIN1 = 15;
const int BRAKE_BIAS_PIN2 = 14;
 
 int counter = 0; 
 int aState;
 int aLastState;  
 
 void setup() { 
   pinMode (BRAKE_BIAS_PIN1,INPUT_PULLUP);
   pinMode (BRAKE_BIAS_PIN2,INPUT_PULLUP);
   
   Serial.begin(9600);
   delay(50);
   
   
   // Reads the initial state of the outputA
   aLastState = digitalRead(BRAKE_BIAS_PIN1);   
 } 
 void loop() { 
  //Serial.println("Starting the machine !");
  
   aState = digitalRead(BRAKE_BIAS_PIN1); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aState != aLastState){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(BRAKE_BIAS_PIN2) != aState) { 
       counter ++;
     } else {
       counter --;
     }
     Serial.print("Position: ");
     Serial.println(counter);
   } 
   aLastState = aState; // Updates the previous state of the outputA with the current state
 }
