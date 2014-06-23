#!/usr/bin/python
import serial 
import struct

RF_START_BYTE = '{'
RF_END_BYTE = '}'
PACKET_SIZE = 18
#Frame Structure types declared below
frame_struct = struct.Struct('<cchhhhhHHcc')

ser = serial.Serial('/dev/ttyO1', 57600)
frameOK=0;

# Cosmetic Stuff
print "\nBBB Application for the Beyond The Phone Challenge"
print "Written by: Inderpreet Singh"
print "Many Thanks to Element14, Wurth Electronics & TI\n\n"

while 1:
	print "Searching..."
	x = ser.read()
	# End Of Frame Detect
	while( x != '}' ):		# detect end of character
		x = ser.read()
	#EOF detected... move on
	frameOK=1;
	while(frameOK==1):
		frame = ser.read(PACKET_SIZE)	# read entire frame
		if(frame[0]=='{' and frame[PACKET_SIZE-1]=='}'):	# authenticate start and end of frame
			print "\nGot Good Frame"
			data = frame_struct.unpack(frame)
			print("Temperature: {3: d}, Switch {4: d}, Volts {5: d}, Curr {6: d}".format(*data))
		else:
			print "Frame Corrupt"
			frameOK=0;
	
