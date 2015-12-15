
rem Erase memory (-e); Disables safe mode (-u); Writes fuses (-U):

.\avrdude -C .\avrdude.conf -p atmega1284p -c usbtiny -e -u -Uefuse:w:0xFD:m
.\avrdude -C .\avrdude.conf -p atmega1284p -c usbtiny -e -u -Uhfuse:w:0xDE:m 
.\avrdude -C .\avrdude.conf -p atmega1284p -c usbtiny -e -u -Ulfuse:w:0xFF:m

rem Burns the bootloader in flash (does not verifies -V):
.\avrdude -C .\avrdude.conf -p atmega1284p -c usbtiny -V -F -U flash:w:".\optiboot_DB23_1284p.hex":i

rem Lockbits:
.\avrdude -C .\avrdude.conf -p atmega1284p -c usbtiny -U lock:w:0xEF:m

rem Reading:
rem .\avrdude -C .\avrdude.conf -p atmega1284p -c usbtiny -U lock:r:lock.hex:i

pause
