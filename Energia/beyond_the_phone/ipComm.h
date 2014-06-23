/* 
* Description: This file is part of the application for the beyone_the_phone 
*              Challenge based on the MSP430F5529 USB LaunchPad
*
* --
* Copyright (C) 2014 Inderpreet Singh(er.inderpreet@gmail.com), 
			  Thought Process Designs
* Web      :  http://google.com/+InderpreetSingh
*		 	  http://embeddedcode.wordpress.com
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
#ifndef IPCOMM_H
#define IPCOMM_H

struct ipFrame{
      char startChar;
      char packingDummy;
      int fSize;
      int crateTemperature;
      int crateSwitch;
      int batteryVolts;
      int batteryCurrent;
      unsigned int batteryTTE;
      unsigned int batteryTTF;
      char dummy;
      char endOfFrame;
    };
      

#endif

