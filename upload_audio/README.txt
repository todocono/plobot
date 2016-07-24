
To write a binary file to the Plobot's flash chip on OSX: 

1. Connect FTDI. Have bootloader installed on Atmega644P. 
2. Upload arduino_stream.ino to the Atmega644P. 
3. Run "goSerial". Set modem port (/dev/something) to 57600 baud rate and press connect
	Note that you should see "---- Streaming Upload ready ----" in the window.
4. Build + run upload_stream.cc
	For example: g++ -o upload_stream ./upload_stream.cc
				 upload_stream ../audio_test/mambo.pcm /dev/cu.usbmodem1421 NORMAL
5. Watch goSerial for error messages. Note that "unknown command" means that the XSP device needs an initial character to get it out of command mode. Use "XSP" instead of "NORMAL" in your upload_stream command. 
