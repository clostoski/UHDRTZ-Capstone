#include <ArduinoBLE.h>
#define inputCLK 4
#define inputDT 5
BLEService rotaryService("13012F00-F8C3-4F4A-A8F4-15CD926DA146");
BLEUnsignedCharCharacteristic rotaryConverted("13012F00-F8C3-4F4A-A8F4-15CD926DA146", BLERead | BLENotify);
int counter = 0;
int displacement = 0;
int millisCount = 0;
unsigned long saved_millis = 0;


int currentStateCLK;
int prevStateCLK;

String encdir = "";

void setup() {
  Serial.begin(9600);
  //while (!Serial);
  pinMode(inputCLK,INPUT);
  pinMode(inputDT,INPUT);
  
  pinMode(LED_BUILTIN, OUTPUT);
  prevStateCLK = digitalRead(inputCLK);
  if (!BLE.begin()) 
  {
    Serial.println("starting BLE failed!");
    while (1);
  }
  
  BLE.setLocalName("RotaryArduino");
  BLE.setAdvertisedService(rotaryService);
  rotaryService.addCharacteristic(rotaryConverted);
  BLE.addService(rotaryService);
  
  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
  }

void loop() 
{
  BLEDevice central = BLE.central();
  
  if (central) 
  {
  Serial.print("Connected to central: ");
  Serial.println(central.address());
  digitalWrite(LED_BUILTIN, HIGH);
  while (central.connected()) {
        
        
        currentStateCLK = digitalRead(inputCLK);
        //input has occurred
        if (currentStateCLK != prevStateCLK){
          //rotating counter clockwise (towards person)
          if (digitalRead(inputDT) != currentStateCLK){
            counter == 0 ? counter = 39 : counter--;
            displacement--;
            encdir="CCW";
          }
          //rotating clockwise (away from person)
          else{
            counter == 39 ? counter = 0 : counter++;
            displacement++;
            encdir="CW";
          }
          
          //rotaryConverted.writeValue(counter);
        }

        prevStateCLK = currentStateCLK;
        
        
        //rotaryConverted.writeValue(elapsed_time);
        
        
        // Keeps track of milliseconds
        if(millis() != saved_millis){
          if(millisCount == 99){
            millisCount = 0;
            displacement = 0;
          }
          else{
            millisCount++;
          }
          
          
          saved_millis = millis();
        }

        if(millisCount == 99){
          rotaryConverted.writeValue(displacement);
        }
                  
          
          
        //here change it to read digital data from rotary encoder
        //int voltage = analogRead(A0);
        //int digitalVoltage = map(battery, 0, 1023, 0, 100);
        //Serial.print("Voltage is now: ");
        //Serial.println(voltage);
        //voltageConverted.writeValue(voltage);
          
  }
  }
  digitalWrite(LED_BUILTIN, LOW);
  //Serial.print("Disconnected from central: ");
  //Serial.println(central.address());
}
