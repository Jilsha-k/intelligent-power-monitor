// AC Energy Meter with LCD By Solarduino

// Note Summary
// Note :  Safety is very important when dealing with electricity. We take no responsibilities while you do it at your own risk.
// Note :  This AC Energy Meter Code needs AC current module and AC Voltage Module to determine AC Power value.
// Note :  This Code monitors RMS Voltage, RMS current, RMS AC Power (Apparent Power) and Instantaneous AC Power (Active Power), Frequency, Power Factor, and Accumulate Energy.
// Note :  The value shown in Serial Monitor is refreshed every second, can be used for 50Hz and 60Hz.
// Note :  The frequency is measured by counting time and average it for every 46 samples taken (1 sample is 1 cycle).
// Note :  The code memory and speed almost reach to the peak. By increasing frequency sample number may cause miscalculation or out of accuracy.
// Note :  Not recommended to add datalogger shield for recording as measured values accuracy may be out due to memory problem.
// Note :  The auto calibration (voltageOffset1 & currentOffset1) is using averaged analogRead value of 1000 samples.
// Note :  The auto calibration (voltageOffset2 & currentOffset2) is using calculated RMS value including Offset1 value for calibration.
// Note :  The unit provides reasonable accuracy and may not be comparable with other expensive branded and commercial product.
// Note :  All credit shall be given to Solarduino.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


/* 0- General */

int decimalPrecision = 1;                         // decimal places for large values such as voltage, wattage, apparent power, and frequency shown in LED Display & Serial Monitor
// decimal places for small values such as current, power factor and accumulate energy will be decimal places x 2.

/* 1- AC Voltage Measurement */

int VoltageAnalogInputPin = A1;                   // Which pin to measure voltage Value
float voltageSampleRead  = 0;                     /* to read the value of a sample*/
float voltageLastSample  = 0;                     /* to count time for each sample. Technically 1 milli second 1 sample is taken */
float voltageSampleSum   = 0;                     /* accumulation of sample readings */
float voltageSampleCount = 0;                     /* to count number of sample. */
float voltageMean ;                               /* to calculate the average value from all samples*/
float RMSVoltageMean ;                            /* square roof of voltageMean*/


/*1.1 Offset AC Voltage */

int voltageOffsetRead = 0;                  /* to change the mode for offset */
float voltageOffset1 = 0;                   // to Offset deviation and accuracy. Offset any fake current when no current operates.
// Offset will automatically callibrate when SELECT Button on the LCD Display Shield is pressed.
// If you do not have LCD Display Shield, look into serial monitor to add or minus the value manually and key in here.
float voltageOffset2 = 0;                   // to offset value due to calculation error from squared and square root.
float voltageSampleSumOffset = 0;           /* accumulation of sample readings for offset */
float offsetVoltageMean = 0;                /* to calculate the average value from all samples for offset, in analog values*/
float voltageOffsetLastSample = 0;          /* to count time for each sample for offset purpose. */
float voltageOffsetSampleCount = 0;         /* to count number of sample for offset. */


/* 2- AC Current Measurement */

int CurrentAnalogInputPin = A2;                   // Which pin to measure Current Value
float mVperAmpValue = 100;                      // If using ACS712 current module : for 5A module key in 185, for 20A module key in 100, for 30A module key in 66
// If using "Hall-Effect" Current Transformer, key in value using this formula: mVperAmp = maximum voltage range (in milli volt) / current rating of CT
/* For example, a 20A Hall-Effect Current Transformer rated at 20A, 2.5V +/- 0.625V, mVperAmp will be 625 mV / 20A = 31.25mV/A */
float currentSampleRead  = 0;                     /* to read the value of a sample*/
float currentLastSample  = 0;                   /* to count time for each sample. Technically 1 milli second 1 sample is taken */
float currentSampleSum   = 0;                     /* accumulation of sample readings */
float currentSampleCount = 0;                     /* to count number of sample. */
float currentMean ;                               /* to calculate the average value from all samples*/
float RMSCurrentMean = 0 ;                        /* square roof of currentMean*/
float FinalRMSCurrent ;                           /* the final RMS current reading*/


/*2.1 Offset AC Current */

int currentOffsetRead = 0;                  /* to change the mode for offset */
float currentOffset1 = 0;                   // to Offset deviation and accuracy. Offset any fake current when no current operates.
// Offset will automatically callibrate when SELECT Button on the LCD Display Shield is pressed.
// If you do not have LCD Display Shield, look into serial monitor to add or minus the value manually and key in here.
// 26 means add 26 to all analog value measured
float currentOffset2 = 0;                   // to offset value due to calculation error from squared and square root.
float currentSampleSumOffset = 0;           /* accumulation of sample readings for offset */
float offsetCurrentMean = 0;                /* to calculate the average value from all samples for offset, in analog values*/
float currentOffsetLastSample = 0;          /* to count time for each sample for offset purpose. */
float currentOffsetSampleCount = 0;         /* to count number of sample for offset. */


/* 3- AC Power Measurement */

float sampleCurrent1 ;                            /* use to calculate current*/
float sampleCurrent2 ;                            /* use to calculate current*/
float sampleCurrent3 ;                            /* use to calculate current*/
float apparentPower;                              /* the apparent power reading (VA) */
float realPower = 0;                              /* the real power reading (W) */
float powerSampleRead  = 0;                       /* to read the current X voltage value of a sample*/
float powerLastSample   = 0;                      /* to count time for each sample. Technically 1 milli second 1 sample is taken */
float powerSampleCount  = 0;                      /* to count number of sample. */
float powerSampleSum    = 0;                      /* accumulation of sample readings */
float powerFactor = 0;                            /* to display power factor value*/


/*3.1 Offset AC Power */

int powerOffsetRead = 0;                    /* to change the mode for offset */
float powerOffset = 0;                      // to Offset deviation and accuracy. Offset any fake current when no current operates.
// Offset will automatically callibrate when SELECT Button on the LCD Display Shield is pressed.
// If you do not have LCD Display Shield, look into serial monitor to add or minus the value manually and key in here.
float powerOffsetLastSample = 0;            /* to count time for each sample for offset purpose. */
float powerOffsetSampleCount = 0;           /* to count number of sample for offset. */


/* 4 - Daily Energy Measurement*/

float dailyEnergy =   0;                /* recorded by multiplying RMS voltage and RMS current*/
float energyLastSample = 0;             /* Use for counting time for Apparent Power */
float energySampleCount = 0;            /* to count number of sample. */
float energySampleSum  = 0;             /* accumulation of sample readings */
float finalEnergyValue = 0;             /* total accumulate energy */
float accumulateEnergy = 0;             /* accumulate of energy readings*/


/* 5- frequency measurement */

unsigned long startMicros;                /* start counting time for frequency (in micro seconds)*/
unsigned long currentMicros;              /* current counting time for frequency (in micro seconds) */
int expectedFrequency = 46;               // This is to collect number of samples. for 50Hz use 46 or below. For 60Hz use 54 or below.
// Use exact number of frequency number (50/60) will have calculation error
float frequencySampleCount = 0;           /* count the number of sample, 1 sample equivalent to 1 cycle */
float frequency = 0 ;                     /* shows the value of frequency*/
float a;                                  /* use for calculation purpose*/
float switch01 = 9;                       /* use for switching function */
float vAnalogRead = 0;                    // read analog value, highly recommend AC voltage sensor
// Automatically bonded with "VoltageAnalogInputPin = A2" reading

bool b_flag = 0, bt_flag = 0, R_flag = 0;
/* 6 - LCD Display  */

#include<LiquidCrystal.h>                   /* Load the liquid Crystal Library (by default already built-it with arduino solftware)*/
LiquidCrystal LCD(2 , 3, 4, 5, 6, 7);        /* Creating the LiquidCrystal object named LCD. The pin may be varies based on LCD module that you use*/

unsigned long startMillisLCD;               /* start counting time for LCD Display */
unsigned long currentMillisLCD;             /* current counting time for LCD Display */
const unsigned long periodLCD = 1000;       // refresh every X seconds (in seconds) in LED Display. Default 1000 = 1 second
int page = 1;                               /* flip page to display values*/
int z = 0;
String dataToSend;// if z=1 activate frequency calculation, if z=0, activate LCD display
// need to seperate LCD display with frequency calculation may be due to low memory speed of the board that cannot do both at the same time


void setup()                                        /*codes to run once */

{

  /* 0- General */

  Serial.begin(9600);  /* to display readings in Serial Monitor at 9600 baud rates */
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  /* 5- frequency measurement */

  startMicros = micros();                     /* Start counting time for frequency measurement */

  /* 6 - LCD Display  */

  LCD.begin(16, 2);                           /* Tell Arduino that our LCD has 16 columns and 2 rows*/
  LCD.setCursor(0, 0);                        /* Set LCD to start with upper left corner of display*/
  startMillisLCD = millis();                  /* Start counting time for LCD display*/

}


void loop()                                                                                       /*codes to run again and again */
{

  /* 0- General */


  /* 0.1- Button Function */

  int L_buttonRead;
  L_buttonRead = digitalRead(9); // Read button state from pin 9

  // Check if the button is pressed and b_flag is not set
  if (L_buttonRead == 0 && b_flag == 0 && bt_flag == 0)
  {
    b_flag = 1; // Set b_flag to 1 to indicate the button is pressed
  }
  // Check if the button is released and b_flag is set
  else if (L_buttonRead == 1 && b_flag == 1)
  {
    page = (page == 1) ? 2 : 1; // Toggle between page 1 and 2
    b_flag = 0; // Reset b_flag
  }

  // Check if the button is pressed and bt_flag is not set
  if (L_buttonRead == 0 && b_flag == 0 && bt_flag == 0)
  {
    bt_flag = 1; // Set bt_flag to 1 to indicate the button is pressed
  }
  // Check if the button is released and bt_flag is set
  else if (L_buttonRead == 1 && bt_flag == 1)
  {
    page = (page == 1) ? 2 : 1; // Toggle between page 1 and 2
    bt_flag = 0; // Reset bt_flag
  }


  ///////////////////////////////////////////////////////



  /* Left button is pressed */
  int R_buttonRead;
  R_buttonRead = digitalRead(10);
  if (R_buttonRead == 0 && R_flag == 0)
  {
    LCD.setCursor(0, 0); LCD.print ("CALLIBRATING.... ");
    LCD.setCursor(0, 1); LCD.print ("                ");
    delay(1000);
    R_flag = 1;
  }

  if (R_buttonRead == 1 && R_flag == 1)
  {

    currentOffsetRead = 1;                                                              // to activate offset for current
    voltageOffsetRead = 1;                                                              // to activate offset for voltage
    powerOffsetRead = 1;                                                                // to activate offset for power
    LCD.setCursor(0, 0);                                                                /* set display words starting at upper left corner*/
    LCD.print ("INITIALIZING..... ");
    LCD.setCursor(0, 1);                                                                /* set display words starting at lower left corner*/
    LCD.print ("WAIT 5 SEC ..... ");
    R_flag = 0;

  }

  /* Select button is pressed */




  /* 1- AC Voltage Measurement */

  if (millis() >= voltageLastSample + 1 )                                                   /* every 1 milli second taking 1 reading */
  {
    voltageSampleRead = 2 * (analogRead(VoltageAnalogInputPin) - 512) + voltageOffset1;   /* read the sample value */
    voltageSampleSumOffset = voltageSampleSumOffset + voltageSampleRead;                  /* values accumulate for offset purpose every milli second */

    voltageSampleSum = voltageSampleSum + sq(voltageSampleRead) ;                         /* accumulate value with older sample readings*/
    voltageSampleCount = voltageSampleCount + 1;                                          /* to move on to the next following count */
    voltageLastSample = millis() ;                                                        /* to reset the time again so that next cycle can start again*/
  }

  if (voltageSampleCount == 1000)                                                           /* after 1000 count or 1000 milli seconds (1 second), do the calculation and display value*/
  {
    offsetVoltageMean = voltageSampleSumOffset / voltageSampleCount;                      /* average the offset reading*/

    voltageMean = voltageSampleSum / voltageSampleCount;                                  /* calculate average value of all sample readings taken*/
    RMSVoltageMean = sqrt(voltageMean) + voltageOffset2;                                  /* square root of the average value*/
    RMSVoltageMean = (RMSVoltageMean > 240) ? 240 : (RMSVoltageMean < 20) ? 0 : RMSVoltageMean;
    //    Serial.print(RMSVoltageMean);
    //    Serial.println(" V   ");
    if(RMSVoltageMean<200)
    {
      RMSVoltageMean=0;
    }
    voltageSampleSum = 0;                                                                 /* to reset accumulate sample values for the next cycle */
    voltageSampleCount = 0;                                                               /* to reset number of sample for the next cycle */
    voltageSampleSumOffset = 0;
  }


  /* 1.1 - Offset AC Voltage */

  if (voltageOffsetRead == 1)                                                         /* Run this code when button SELECT is pressed */
  {
    voltageOffset1 = 0;
    if (millis() >= voltageOffsetLastSample + 1)                                    /* keep countng time for offset1*/
    {
      voltageOffsetSampleCount = voltageOffsetSampleCount + 1;                    /* 1 milli second add 1 count*/
      voltageOffsetLastSample = millis();                                         /* to reset the time again so that next cycle can start again*/
    }
    if (voltageOffsetSampleCount == 2000)                                           /* after 2 seconds, run this codes.  */
    {
      voltageOffset1 = -1 * (offsetVoltageMean);                                  /* set the offset values */
      voltageOffsetRead = 2;                                                      /* go for second offset Settings */
      voltageOffsetSampleCount = 0;                                               /* to reset the time again so that next cycle can start again */
    }
  }

  if (voltageOffsetRead == 2)                                                         /* Run this code after first offset done */
  {
    voltageOffset2 = 0;                                                             /* set back currentOffset2 as default*/
    if (millis() >= voltageOffsetLastSample + 1)                                    /* keep countng time for offset2*/
    {
      voltageOffsetSampleCount = voltageOffsetSampleCount + 1;
      voltageOffsetLastSample = millis();
    }
    if (voltageOffsetSampleCount == 2000)                                           /* after 2 seconds, run this codes.  */
    {
      voltageOffset2 = - RMSVoltageMean;                                          /* set the offset values */
      voltageOffsetRead = 0;                                                      /* change the offset mode to original, wait until the button is pressed again */
      voltageOffsetSampleCount = 0;                                               /* to reset the time again so that next cycle can start again */
    }
  }


  /* 2- AC Current Measurement */

  if (millis() >= currentLastSample + 1)                                                    /* every 1 milli second taking 1 reading */
  {
    currentSampleRead = analogRead(CurrentAnalogInputPin) - 512 + currentOffset1;         /* read the sample value */

    currentSampleSumOffset = currentSampleSumOffset + currentSampleRead;                  /* accumulate offset value */

    currentSampleSum = currentSampleSum + sq(currentSampleRead) ;                         /* accumulate value with older sample readings*/
    currentSampleCount = currentSampleCount + 1;                                          /* to move on to the next following count */
    currentLastSample = millis();                                                         /* to reset the time again so that next cycle can start again*/
  }

  if (currentSampleCount == 1000)                                                           /* after 1000 count or 1000 milli seconds (1 second), do the calculation and display value*/
  {
    offsetCurrentMean = currentSampleSumOffset / currentSampleCount;                      /* average offset value*/

    currentMean = currentSampleSum / currentSampleCount;                                  /* calculate average value of all sample readings taken*/
    RMSCurrentMean = sqrt(currentMean) + currentOffset2 ;                                 /* square root of the average value*/
    FinalRMSCurrent = abs((((RMSCurrentMean / 1024) * 5000) / mVperAmpValue));   
    FinalRMSCurrent = FinalRMSCurrent-0.40;
    if(FinalRMSCurrent<2.70)
    {
      FinalRMSCurrent=0;
    }
//    Serial.print(FinalRMSCurrent, decimalPrecision * 2);
//    Serial.println(" A   ");
    currentSampleSum = 0;                                                                 /* to reset accumulate sample values for the next cycle */
    currentSampleCount = 0;                                                               /* to reset number of sample for the next cycle */
    currentSampleSumOffset = 0;                                                           /* to reset accumulate offset value for the next cycle*/
  }

  /* 2.1 - Offset AC Current */

  if (currentOffsetRead == 1)                                                         /* Run this code when button SELECT is pressed */
  {
    currentOffset1 = 0;                                                             /* set currentOffset back to default value*/
    if (millis() >= currentOffsetLastSample + 1)                                    /* keep countng time for offset1*/
    {
      currentOffsetSampleCount = currentOffsetSampleCount + 1;
      currentOffsetLastSample = millis();
    }
    if (currentOffsetSampleCount == 2000)                                           /* after 2 seconds, run this codes.  */
    {
      currentOffset1 = - offsetCurrentMean;                                       /* set the offset values */
      currentOffsetRead = 2;                                                      /* go for second offset Settings */
      currentOffsetSampleCount = 0;                                               /* to reset the time again so that next cycle can start again */
    }
  }

  if (currentOffsetRead == 2)                                                         /* Run this code after first offset done */
  {
    currentOffset2 = 0;                                                             /* set back currentOffset2 as default*/
    if (millis() >= currentOffsetLastSample + 1)                                    /* keep countng time for offset2*/
    {
      currentOffsetSampleCount = currentOffsetSampleCount + 1;
      currentOffsetLastSample = millis();
    }

    if (currentOffsetSampleCount == 2000)                                           /* after 2 seconds, run this codes.  */
    {
      currentOffset2 = - RMSCurrentMean;                                          /* set the offset values */
      currentOffsetRead = 0;                                                      /* change the offset mode to original, wait until the button is pressed again */
      currentOffsetSampleCount = 0;                                               /* to reset the time again so that next cycle can start again */
    }
  }


  /* 3- AC Power with Direction */

  if (millis() >= powerLastSample + 1)                                                      /* every 1 milli second taking 1 reading */
  {
    sampleCurrent1 = analogRead(CurrentAnalogInputPin) - 512 + currentOffset1;            /* create variable for formula purpose */
    sampleCurrent2 = (sampleCurrent1 / 1024) * 5000;
    sampleCurrent3 = sampleCurrent2 / mVperAmpValue;
    voltageSampleRead = 2 * (analogRead(VoltageAnalogInputPin) - 512) + voltageOffset1 ;  // the formula is times 2 so that the amplitude can be reduced by half to overcome wave limit cut near 250Vac
    powerSampleRead = voltageSampleRead * sampleCurrent3 ;                                /* real power sample value */
    powerSampleSum = powerSampleSum + powerSampleRead ;                                   /* accumulate value with older sample readings*/
    powerSampleCount = powerSampleCount + 1;                                              /* to move on to the next following count */
    powerLastSample = millis();                                                           /* to reset the time again so that next cycle can start again*/
  }

  if (powerSampleCount == 1000)                                                             /* after 1000 count or 1000 milli seconds (1 second), do the calculation and display value*/
  {
    realPower = abs((((powerSampleSum / powerSampleCount) + powerOffset) / 3)-2) ; 
    realPower=realPower-2;
    if(realPower<15)
    {
      realPower=0;
    }
    //    Serial.print(realPower, decimalPrecision);
    //    Serial.println(" W   ");
    apparentPower = abs(FinalRMSCurrent * RMSVoltageMean);                                     /*Apparent power do not need to recount as RMS current and RMS voltage values available*/
    //    Serial.print(apparentPower, decimalPrecision);
    //    Serial.println(" VA   ");
    powerFactor = realPower / apparentPower;
    if (powerFactor > 1 || powerFactor < 0)                                               /* if power factor more than 1 or less than 0, key in power factor = 0 */
    {
      powerFactor = 0;
    }
    powerSampleSum = 0;                                                                   /* to reset accumulate sample values for the next cycle */
    powerSampleCount = 0;                                                                 /* to reset number of sample for the next cycle */
  }


  /* 3.1 - Offset AC Power */

  if (powerOffsetRead == 1)                                                           /* Run this code after first offset done */
  {
    powerOffset = 0;                                                                /* set back currentOffset2 as default*/
    if (millis() >= powerOffsetLastSample + 1)                                      /* keep countng time for offset2*/
    {
      powerOffsetSampleCount = powerOffsetSampleCount + 1;
      powerOffsetLastSample = millis();
    }
    if (powerOffsetSampleCount == 5000)                                             /* after 5 seconds, run this codes.  */
    {
      powerOffset = -realPower;
      powerOffsetRead = 0;                                                        /* change the offset mode to original, wait until the button is pressed again */
      powerOffsetSampleCount = 0;                                                 /* to reset the time again so that next cycle can start again */
    }
  }


  /* 4 - Accumulate & Daily Energy Measurement*/

  if (millis() >= energyLastSample + 1)                                               /* every 1 milli second taking 1 reading */
  {
    energySampleCount = energySampleCount + 1;
    energyLastSample = millis();
  }
  if (energySampleCount == 1000)                                                      /* after 1000 count or 1000 milli seconds (1 second), do the calculation and display value*/
  {
    accumulateEnergy = apparentPower / 3600;                                          /* daily and accumulative seperated*/
    finalEnergyValue = finalEnergyValue + accumulateEnergy;
    //    Serial.print(finalEnergyValue , decimalPrecision);
    //    Serial.println(" kWh   ");

    energySampleCount = 0 ;                                                           /* Set the starting point again for next counting time */
  }


  /* 5- frequency measurement */

  if (z == 1)                                                           // Once LCD displayed values, the frequency then only start calculation
  {
    vAnalogRead = voltageSampleRead;                                    /* read the analog value from sensor */
    if (vAnalogRead < 0 && switch01 == 9)                               /* the begining stage make sure counting started with a clean value = 0 */
    {
      switch01 = 8;
    }

    if (vAnalogRead >= 0 && switch01 == 8)                              /* start counting time when analog value = 0 going up trend */
    {
      startMicros = micros();
      switch01 = 7;
    }

    if (vAnalogRead < 0 && switch01 == 7)                               /* going downtrend do nothing, just acknowledgement */
    {
      switch01 = 6;
    }

    if (vAnalogRead >= 0 && switch01 == 6)                              /* after 1 cycle when going back to value 0 uptrend */
    {
      currentMicros = micros();                                        /* record the current time for frequency calculation*/
      frequencySampleCount = frequencySampleCount + 1 ;                /* count the sample or cycle. accumulate the number oc cycle*/
      switch01 = 7;                                                    /* standby to go to next downtrend cycle*/
    }

    if (frequencySampleCount == expectedFrequency)                    /* if couting sample reach the set number of cycle (example 46 cycles) */
    {

      a = currentMicros - startMicros ;                           /* get the time taken throughout the total cycles */
      frequency = 1 / ((a / 1000000) / frequencySampleCount);     /* formula for frequency value by averaging */
      //      Serial.print(frequency, decimalPrecision);
      //      Serial.println(" Hz  ");
      
      frequencySampleCount = 0;                                   /* reset the total sample taken become 0 for next cycle */
      switch01 = 9;                                               /* go back to initial stage looking for a confirmation signal to start count time*/
      z = 0;                                                      /* close the frequency calculation until next LCD Display*/
    }
  }


  /* 6 - LCD Display  */

  currentMillisLCD = millis();                                                              /* Set current counting time */
  if (currentMillisLCD - startMillisLCD >= periodLCD && page == 1)                          /* for every x seconds, at page 1*/
  {
    LCD.clear();
    LCD.setCursor(0, 0); /* Set cursor to first colum 0 and second row 1  */
    LCD.print("Voltage:");
    LCD.setCursor(8, 0);
    LCD.print(String(RMSVoltageMean, decimalPrecision * 0) + "V" );
    LCD.setCursor(0, 1);
    LCD.print("Current:");
    LCD.setCursor(8, 1);
    LCD.print(FinalRMSCurrent, decimalPrecision * 2);                                     /* display current value in LCD in first row  */
    LCD.setCursor(13, 1);
    LCD.print("A");
    startMillisLCD = currentMillisLCD ;                                                   /* Set the starting point again for next counting time */
    z = 1;                                                                                /* initiate frequency calculation */
    Serial.println(dataToSend);
  }

  if ( currentMillisLCD - startMillisLCD >= periodLCD && page == 2)                         /* for every x seconds, at page 2*/
  {
    LCD.clear();
    LCD.setCursor(0, 0);                                                                  /* Set cursor to first colum 0 and second row 1  */
    LCD.print("Unit:");
    LCD.setCursor(6, 0);
    LCD.print(finalEnergyValue, decimalPrecision * 1);
    LCD.setCursor(10, 0);
    LCD.print("wattH");
    LCD.setCursor(0, 1);
    LCD.print("Watt:");
    LCD.setCursor(6, 1);
    LCD.print(realPower, decimalPrecision);
    LCD.setCursor(11, 1);
    LCD.print("W");
    startMillisLCD = currentMillisLCD ;                                                   /* Set the starting point again for next counting time */
    z = 1;                                                                                /* initiate frequency calculation */
   // Serial.println(dataToSend);
  }
  dataToSend =  String(RMSVoltageMean, decimalPrecision * 0) + "," +  String(FinalRMSCurrent, decimalPrecision * 2) + "," + String(finalEnergyValue, decimalPrecision * 1) + "," + String(realPower, decimalPrecision);
  //  Serial.println(dataToSend); // Send the data over serial
  //  delay(1000); // Wait for a second
  if(Serial.available()>0){
    String res_data = Serial.readString();
    Serial.print(res_data);
  }
}
