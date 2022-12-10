//Author: Dave Danadiva Agusta Peerera
//Project: Thermocycler in a PCR Chamber
//Visit: https://github.com/davedapp

//LCD Library
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//LCD Address
LiquidCrystal_I2C lcd(0x27,16,2);

//Temperature Reading
const int    SAMPLE_NUMBER      = 15;
const double BALANCE_RESISTOR   = 100000.0;
const double MAX_ADC            = 1023.0;
const double BETA               = 3997.0;
const double ROOM_TEMP          = 298.15; 
const double RESISTOR_ROOM_TEMP = 10000.0;

//Save Current Temperature
double currentTemperature = 0;

//Inputs:
int thermistorPin = 0;

//Outputs:
int RelayHeaterPin = 9;
int RelayPeltierPin = 11;

//Number of Desired Cycle:
int cycle = 2;

//Thermocycle Process Time:
int Dentime = 20;
int Anntime = 20;
int Extentime = 20;

void setup() 
{ 
  // Set the port speed for serial window messages
  Serial.begin(19200);
  pinMode(RelayHeaterPin, OUTPUT);
  pinMode(RelayPeltierPin, OUTPUT);

  // initialize the lcd 
  lcd.init();                      
  lcd.backlight();
}


//Read Thermistor
double readThermistor() 
{
  // variables that live in this function
  double rThermistor = 0;            // Holds thermistor resistance value
  double tKelvin     = 0;            // Holds calculated temperature
  double tcelsius    = 0;            // Hold temperature in celsius
  double adcAverage  = 0;            // Holds the average voltage measurement
  int    adcSamples[SAMPLE_NUMBER];  // Array to hold each voltage measurement

  //Calculate thermistore average resistance
  for (int i = 0; i < SAMPLE_NUMBER; i++) 
  {
    adcSamples[i] = analogRead(thermistorPin);  // read from pin and store
    delay(10);        // wait 10 milliseconds
  }

  //average the sample
  for (int i = 0; i < SAMPLE_NUMBER; i++) 
  {
    adcAverage += adcSamples[i];      // add all samples up 
  }
  adcAverage /= SAMPLE_NUMBER;        // average it w/ divide

  // Thermistor resistance
  rThermistor = BALANCE_RESISTOR * ( (MAX_ADC / adcAverage) - 1);

  //Temperature reading (conversion)[Kelvin]
  tKelvin = (BETA * ROOM_TEMP) / 
            (BETA + (ROOM_TEMP * log(rThermistor / RESISTOR_ROOM_TEMP)));

  //Convert Temperature to Celsius
  tcelsius = tKelvin - 273.15;  // convert kelvin to celsius 

  return tcelsius;    // Return the temperature in Celsius
}


void loop()
{
    currentTemperature = readThermistor();   
 
 //BIG THERMO CYCLER LOOP

  //Will automatically break after desired cycle achieved

  for (int cyclecount=0 ;cyclecount < cycle; cyclecount += 1){
    int DentimeCount = 0;
    int AnntimeCount = 0;
    int ExtentimeCount = 0;
    currentTemperature = readThermistor();   
    Serial.print("We start at ");
    Serial.print(currentTemperature);
    Serial.println("C.");
    
  //DENATURATION
    // OTW to Denaturation Temp
    Serial.println("Attaining Denaturation");
     while(currentTemperature < 95.2){
         digitalWrite(RelayHeaterPin, HIGH);
         currentTemperature = readThermistor();   
         if (currentTemperature > 0){
           Serial.print("it's ");
           Serial.print(currentTemperature);
           Serial.println("C.");
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print("Denaturation [^]"); 
           lcd.setCursor(0,1);
           lcd.print("Cycle: ");
           lcd.println(cyclecount);
           delay(2000);
         }
      // On until temp >= 95.2
      }
   
   digitalWrite(RelayHeaterPin, LOW);
   digitalWrite(RelayPeltierPin, LOW);
   currentTemperature = readThermistor();   
   Serial.print("it's ");
   Serial.print(currentTemperature);
   Serial.println("C.");       
   
    // Sustain Denaturation Temp
   Serial.println("Sustaining Denaturation");   
     while (DentimeCount < Dentime){

                
        if (currentTemperature < 95.2){

             digitalWrite(RelayHeaterPin, HIGH);
        
        }
        
        else if (currentTemperature >= 95.2){

             digitalWrite(RelayHeaterPin, LOW);
  
        }
       
       currentTemperature = readThermistor();
       Serial.print("it's ");
       Serial.print(currentTemperature);
       Serial.println("C.");
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Denaturation [-]"); 
       lcd.setCursor(0,1);
       lcd.print("Cycle: ");
       lcd.println(cyclecount);      
       DentimeCount += 1;
       delay(1000);
       currentTemperature = readThermistor();  
       //Break when reach Dentime
     }
  
   Serial.println("Denaturation Complete");
   digitalWrite(RelayHeaterPin, LOW);
   digitalWrite(RelayPeltierPin, LOW);

  //Annealing
    // OTW to Denaturation Temp
    Serial.println("Attaining Annealing");
     while(currentTemperature > 66){
         digitalWrite(RelayPeltierPin, HIGH);
         currentTemperature = readThermistor();   
         if (currentTemperature > 0){
           Serial.print("it's ");
           Serial.print(currentTemperature);
           Serial.println("C.");
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print("Annealing [^]"); 
           lcd.setCursor(0,1);
           lcd.print("Cycle: ");
           lcd.println(cyclecount);
           delay(2000);
         }
      // On until temp < 66
      }

   digitalWrite(RelayHeaterPin, LOW);
   digitalWrite(RelayPeltierPin, LOW);
   currentTemperature = readThermistor();   
   Serial.print("it's ");
   Serial.print(currentTemperature);
   Serial.println("C.");       
   
    // Sustain Annealing Temp
   Serial.println("Sustaining Annealing");
     currentTemperature = readThermistor();   
     while (currentTemperature < 150 && AnntimeCount < Anntime){
        currentTemperature = readThermistor();   
        if (currentTemperature < 64){

             digitalWrite(RelayHeaterPin, HIGH);
             currentTemperature = readThermistor();               
   
          }
          
        
        else if (currentTemperature >= 64);{
             digitalWrite(RelayHeaterPin, LOW);
             currentTemperature = readThermistor();   
          }

       Serial.print("it's ");
       Serial.print(currentTemperature);
       Serial.println("C.");
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Annealing [-]"); 
       lcd.setCursor(0,1);
       lcd.print("Cycle: ");
       lcd.println(cyclecount);     
       AnntimeCount += 1;
       delay(1000);
       //Break when reach Anntime
     }
  
   Serial.println("Annealing Complete");
   digitalWrite(RelayHeaterPin, LOW);
   digitalWrite(RelayPeltierPin, LOW);

  //EXTENSION
    // OTW to Extension Temp
    Serial.println("Attaining Extension");
     while(currentTemperature < 72.5){
         digitalWrite(RelayHeaterPin, HIGH);
         currentTemperature = readThermistor();   
         if (currentTemperature > 0){
           Serial.print("it's ");
           Serial.print(currentTemperature);
           Serial.println("C.");
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print("Extension [^]"); 
           lcd.setCursor(0,1);
           lcd.print("Cycle: ");
           lcd.println(cyclecount);
           delay(2000);
         }
      // On until temp > 72.5 
      }

   digitalWrite(RelayHeaterPin, LOW);
   digitalWrite(RelayPeltierPin, LOW);
   currentTemperature = readThermistor();   
   Serial.print("it's ");
   Serial.print(currentTemperature);
   Serial.println("C.");       
   
    // Sustain Extension Temp
   Serial.println("Sustaining Extension");
     currentTemperature = readThermistor();   
     while (currentTemperature > 30 && ExtentimeCount < Extentime){
        currentTemperature = readThermistor();   
        digitalWrite(RelayHeaterPin, LOW);
        digitalWrite(RelayPeltierPin, LOW);
        
        if (currentTemperature < 72){

             digitalWrite(RelayHeaterPin, HIGH);
             currentTemperature = readThermistor();             
        }
        
        else if (currentTemperature >= 72 && currentTemperature < 73){

             digitalWrite(RelayHeaterPin, LOW);
             currentTemperature = readThermistor();     
        }

        else if (currentTemperature >=73){

             digitalWrite(RelayPeltierPin, HIGH);
             currentTemperature = readThermistor();     
        }
          
       Serial.print("it's ");
       Serial.print(currentTemperature);
       Serial.println("C.");
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Extension [-]"); 
       lcd.setCursor(0,1);
       lcd.print("Cycle: ");
       lcd.println(cyclecount);
       ExtentimeCount += 1;
       delay(1000);
       //Break when reach Dentime
     }
  
   Serial.println("Extension Complete");

  //AFTER EACH CYCLE PROCESS ===================

   digitalWrite(RelayHeaterPin, LOW);
   digitalWrite(RelayPeltierPin, LOW);
  
   //cycle count notification
   Serial.print(cyclecount + 1);
   Serial.println(" CYCLE HAVE BEEN ACHIEVED");
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print(cyclecount + 1);
   lcd.println(" CYCLE HAVE");
   lcd.setCursor(0,1);
   lcd.print("BEEN ACHIEVED");
   delay(3000);

 }
  //After desired cycle had been achieved
  Serial.println("YOUR SAMPLE IS READY");
  delay(3000);
  digitalWrite(RelayHeaterPin, LOW);
  digitalWrite(RelayPeltierPin, LOW);

  for (;;){}
 
}

   
