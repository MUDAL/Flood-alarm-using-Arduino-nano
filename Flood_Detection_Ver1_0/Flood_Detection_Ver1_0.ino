/*
 * Flood Detection System
 * Date:: 19-04-2021
 * Version:: 1.0
 * 
 * The flood detection system detects the occurence
 * of a flood and sends an SMS to the appropriate
 * agency to address the issue.
 * 
 * Components:
 * 1.)Arduino Nano
 * 2.)Ultrasonic sensor (HC-SR04)
 * 3.)SIM800L GSM module with SMA antenna and IPEX connector
 * 4.)LM338 voltage regulator
 * 5.)10k trimpot and 1k resistor for voltage division for SIM800L Rx pin (2.8v)
 * 6.)Li-ion battery (x2)
 * 7.)560ohm and 240ohm voltage divider to LM338 to deliver 4.2v to SIM800L
 * 
 * Improvements to make:
 * 1.)Minimization of delays
 * 2.)Sending of critical water level SMS once
 * 3.)Adding Rain water sensor functionality
 * 
*/

//private defines
#define CTRL_Z              26
#define TRIGGER_PIN         8
#define ECHO_PIN            9

//private global variables
static uint32_t drainageHeight;
static uint32_t criticalLevel;

//private functions
static uint32_t GetDistanceCM(void);
static void SIM800L_SendSMS(const char* msg);

void setup() 
{
  //Initialize hardware serial for GSM communication
  Serial.begin(9600);
  
  //Ultrasonic sensor initialization
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  //Initial ultrasonic sensor reading
  //Measurement of height of container and critical level
  drainageHeight = GetDistanceCM();
  criticalLevel = drainageHeight * 0.6;
  String str = "Drainage height = " + String(drainageHeight) + "\nCritical Level = " + String(criticalLevel);
  
  //Send drainage alert (height and critical level via SMS)
  delay(500);
  SIM800L_SendSMS(str.c_str());
 
}

void loop() 
{
  uint32_t waterToSensorDistance = GetDistanceCM();
  uint32_t depthOfWater = 0;
  
  if (drainageHeight >= waterToSensorDistance)
  {
    depthOfWater = drainageHeight - waterToSensorDistance;
  }
  
  if (depthOfWater >= criticalLevel)
  {
   
    //Send flood alert and current depth of water
    SIM800L_SendSMS("CRITICAL LEVEL DETECTED!!!!!!!!!!!");
  }
  delay(5000);

}

void SIM800L_SendSMS(const char* msg)
{
  /*
   * Sends SMS
   * Parameters: 
   * 1.) msg: message to send
   * 
   * Return:
   * None
  */
  Serial.println("AT+CMGF=1");
  delay(50);
  Serial.println("AT+CMGS=\"+2348167351641\"");
  delay(50);
  Serial.println(msg);
  delay(50);
  Serial.write(CTRL_Z);
  delay(50);
}

uint32_t GetDistanceCM(void)
{
  /*
   * Gets distance in centimeters
   * Parameters:
   * None
   * 
   * Return:
   * 1.) distance (unsigned 32-bit integer)
  */
  digitalWrite(TRIGGER_PIN,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN,LOW);
  uint32_t pulseWidth = pulseIn(ECHO_PIN,HIGH);
  
  return pulseWidth / 29 / 2;
}


