
#define ULTRASOUND_TRIG_PIN         	6       // trigger PIN
#define ULTRASOUND_ECHO_PIN         	7       // echo PIN
#define DELAY_BETWEEN_MEASUREMENTS 	  60000  // wait XX miliseconds between measurements (10 minutes = 600000  5 minutes = 300000  2 minutes = 120000)
#define DIFFERENCE_TO_RECORD			    3       // record only if difference is greater than XX cm (default = 3) 
#define CHANGE_COUNT_MIN              3       // record only there is a difference against the rev number XX times in row (default = 3) 

unsigned long echo = 0;
unsigned long ultrasoundValue = 0;
unsigned int last_dist=0;
unsigned long dist = 0;  
unsigned int changeCount = CHANGE_COUNT_MIN;

void setup()
{
  Serial.begin(9600);
  pinMode(ULTRASOUND_ECHO_PIN,INPUT);
  pinMode(ULTRASOUND_TRIG_PIN,OUTPUT);
  Serial.print("name=Start&value=");
  Serial.print(String(millis()/1000,DEC));
  Serial.println("s");
  changeCount = CHANGE_COUNT_MIN;
}

void loop()
{
 dist = ping();
 delay(100); 

   /* DEBUG
 Serial.print("Distance (last): ");
 Serial.print(dist);
 Serial.print(" (");
 Serial.print(last_dist);
 Serial.println(")");
// */

 if (dist>0) {
  
     if ((dist==0) or (dist>999999)) dist=last_dist;  
    
     if (abs(last_dist-dist)>=DIFFERENCE_TO_RECORD) {
         changeCount++;
       /*   DEBUG
        Serial.print("ChangeCount: ");
        Serial.println(changeCount);
        // */   
         if (changeCount >= CHANGE_COUNT_MIN) {
            Serial.print("name=Distance&value=");
            Serial.println(dist);
            last_dist=dist;
            changeCount=0;
         }
      }
      else changeCount=0;
      
    delay(DELAY_BETWEEN_MEASUREMENTS); //delay 
 }
}

unsigned long ping()
{ 
  pinMode(ULTRASOUND_TRIG_PIN, OUTPUT);     // Switch signalpin to output
  digitalWrite(ULTRASOUND_TRIG_PIN, LOW);   // Send low pulse 
  delayMicroseconds(5);             // Wait for 2 microseconds
  digitalWrite(ULTRASOUND_TRIG_PIN, HIGH);  // Send high pulse
  delayMicroseconds(10);             // Wait for 5 microseconds
  digitalWrite(ULTRASOUND_TRIG_PIN, LOW);   // Holdoff
  pinMode(ULTRASOUND_ECHO_PIN, INPUT);    // Switch signalpin to input
  digitalWrite(ULTRASOUND_ECHO_PIN, HIGH);  // Turn on pullup resistor
  // please note that pulseIn has a 1sec timeout, which may
  // not be desirable. Depending on your sensor specs, you
  // can likely bound the time like this -- marcmerlin
  // echo = pulseIn(ultraSoundSignal, HIGH, 38000)
  echo = pulseIn(ULTRASOUND_ECHO_PIN, HIGH);  //Listen for echo
  ultrasoundValue = (echo / 58.138);      //convert to CM
  return ultrasoundValue;
}
