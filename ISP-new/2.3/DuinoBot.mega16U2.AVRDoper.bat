
rem Erase memory (-e); Disables safe mode (-u); Writes fuses (-U):

.\avrdude -C .\avrdude.conf -p atmega16u2 -c stk500v2 -P avrdoper -e -u -Uefuse:w:0xf6:m
.\avrdude -C .\avrdude.conf -p atmega16u2 -c stk500v2 -P avrdoper -e -u -Uhfuse:w:0xd9:m 
.\avrdude -C .\avrdude.conf -p atmega16u2 -c stk500v2 -P avrdoper -e -u -Ulfuse:w:0xff:m

rem Burns the bootloader in flash (does not verifies -V):
.\avrdude -C .\avrdude.conf -p atmega16u2 -c stk500v2 -P avrdoper -V -U flash:w:".\DuinoBot23-Bridge.hex":i

rem Lockbits:
.\avrdude -C .\avrdude.conf -p atmega16u2 -c stk500v2 -P avrdoper -U lock:w:0xEF:m

rem Reading:
rem .\avrdude -C .\avrdude.conf -p atmega32u4 -c usbasp -U lock:r:lock.hex:i

pause
