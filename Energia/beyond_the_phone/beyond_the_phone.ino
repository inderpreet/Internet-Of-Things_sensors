/* 
* Description: This file is part of the application for the beyone_the_phone 
*              Challenge based on the MSP430F5529 USB LaunchPad
* 
* --
* Copyright (C) 2014 Inderpreet Singh(er.inderpreet@gmail.com), 
			  Thought Process Designs
* Web      :  http://google.com/+InderpreetSingh
*	      http://embeddedcode.wordpress.com
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL2) as published by the Free Software
* Foundation and appearing in the file LICENSE.TXT included in the packaging of
* this file. Please note that GPL2 Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL2 ("Copyleft").
*
* We put a lot of time and effort into our project and hence this copyright 
* notice ensures that people contribute as well as each contribution is 
* acknowledged. Please retain this original notice and if you make changes
* please document them below along with your details.
*
* The latest copy of this project/library can be found at: 
* https://github.com/inderpreet/
*
*/
// ----------------------------------------------------------------------------

#include <stdio.h>
#include "bq27510.h"
#include <Wire.h>
#include "ipComm.h"

#define  rfSerial Serial1
#define backchannelSerial Serial

// Global Variables
int iVoltage 	= 0;
int iCurrent 	= 0;
int iTemp 		= 0;
int iSOC 		= 0; 
int iDCAP 		= 0;
int iRCAP 		= 0;
unsigned int uiTTE = 0;
unsigned int uiTTF = 0;

static struct ipFrame *rfFrame;

//int CHARGE_PIN A5;
//int EN_PIN     P1_6;
//int PG_PIN     P3_2

int tempSensor           = A0;          // Pin for Temperature Sensor
int sensorTemperature    = 0;  // Temperature Value in Celcius
int crateSwitchPin       = P6_1;
int crateSwitch          =0;
// Function for printing data
// Comment out anything you don't want
void printFuel(void){
	// Temperature
	//backchannelSerialbackchannelSerial.print(" Temp ");
	//backchannelSerial.print(iTemp);
	// Voltage
	backchannelSerial.print(" V ");
	backchannelSerial.print(iVoltage);
	// Current
	backchannelSerial.print(" I ");
	backchannelSerial.print(iCurrent);
	// State of Charge
	//backchannelSerial.print(" SOC ");
	//backchannelSerial.print(iSOC);
	// RCAP
	//backchannelSerial.print(" RCAP ");
	//backchannelSerial.print(iRCAP);
	// DCAP
	//backchannelSerial.print(" DCAP ");
	//backchannelSerial.print(iDCAP);
	// Time to Empty
	//backchannelSerial.print(" TTE ");
	//backchannelSerial.print(uiTTE);
	// Time to Full
	//backchannelSerial.print(" TTF ");
	//backchannelSerial.print(uiTTF);

        backchannelSerial.print("\r");
}

// Function to access the data on the Fuel Gauge via I2C
// Function forked from code posted by Peter Oakes at the element14 forums
int getValue(int port, int cmd){
	int tmp1=0;
	int tmp2 = 0;
	int response = 0;
	Wire.beginTransmission(port);
	Wire.write(byte(cmd));
	Wire.endTransmission(true);
	// according to all the demos, first reading is unstable so ignore it ??
	// Seems yo have to do this twice to get it to work, if you dont is gets screwed up
	Wire.beginTransmission(port);
	Wire.write(byte(cmd));
	// request 2 bytes from slave device port
	response = Wire.requestFrom(port, 2);   
	Wire.endTransmission(true);
	while(Wire.available()){    // slave may send less than requested
		tmp1 = Wire.read() ;
		tmp2 = Wire.read() ;
	}
  return transBytes2Int(tmp2, tmp1);
}

void rfFrameUpdate(void){
        rfFrame->startChar='{';
        rfFrame->packingDummy=0x00;
        rfFrame->fSize = 18;    // 18 bytes per frame
        rfFrame->crateTemperature = sensorTemperature;
        rfFrame->crateSwitch = crateSwitch;
        rfFrame->batteryVolts = iVoltage;
        rfFrame->batteryCurrent = iCurrent;
        rfFrame->batteryTTE = uiTTE;
        rfFrame->batteryTTF = uiTTF;
        rfFrame->dummy = 0x00;
        rfFrame->endOfFrame = '}';   
}

/**
  * @brief  Translate two bytes into an integer
  * @param  
  * @retval The calculation results
  */
unsigned int transBytes2Int(unsigned char msb, unsigned char lsb){
    unsigned int tmp;
    tmp = ((msb << 8) & 0xFF00);
    return ((unsigned int)(tmp + lsb) & 0x0000FFFF);
}

/**
  * @brief  Function for performing Operations on the Fuel Gauge
  * @param  
  * @retval void
  */
void taskFuelGauge(void){
      iTemp  = getValue(bq27510_ADR, bq27510CMD_TEMP_LSB )/ 10 - 273;
      iVoltage = getValue(bq27510_ADR, bq27510CMD_VOLT_LSB );
      iCurrent = getValue(bq27510_ADR, bq27510CMD_AI_LSB);
      iSOC = getValue(bq27510_ADR, bq27510CMD_SOC_LSB);
      iDCAP = getValue(bq27510_ADR, bq27510CMD_DCAP_LSB);
      iRCAP = getValue(bq27510_ADR, bq27510CMD_RM_LSB);
      uiTTE = getValue(bq27510_ADR, bq27510CMD_TTE_LSB);
      uiTTF = getValue(bq27510_ADR, bq27510CMD_TTF_LSB);
}

int getTemperature(void){
      int temp;
      float tempC;
      temp = (analogRead(tempSensor));
      tempC = temp*330.0;    // 3300mV Reff /10
      tempC /= 4096.0;    // 12bit res 4096
      temp = (int)tempC;
      return temp;
}

void rfSendSerial(void){
        char *ptr = (char*)rfFrame;
        int counter = 0;
        while ( counter < sizeof(ipFrame) ){
            rfSerial.write( (*(ptr+counter)) );
            counter++;
        }
}

// Run Once Loop
void setup(void){
	Wire.begin();        // join i2c bus (address optional for master)
	backchannelSerial.begin(57600);  // start serial for output
        rfSerial.begin(57600); // start serial for wireless transmission
        backchannelSerial.println("\nSmart Container with Wireless Charging");
        backchannelSerial.println("By: Inderpreet Singh");
        //rfSerial.println("Test Message");        
        rfFrame = (ipFrame*)malloc(sizeof(ipFrame));
        //analogReference(INTERNAL);
        pinMode(tempSensor, INPUT);
        pinMode(crateSwitch, INPUT_PULLUP);
        pinMode(crateSwitchPin, INPUT);
}

// Run forever Loop
void loop(void){
	taskFuelGauge();
        //printFuel();
        sensorTemperature = getTemperature();
        crateSwitch = digitalRead(crateSwitchPin);
        rfFrameUpdate();
        rfSendSerial(); 
        backchannelSerial.print(sensorTemperature);   
        // power off and sleep for about 5 seconds
        delay(5000);
}


