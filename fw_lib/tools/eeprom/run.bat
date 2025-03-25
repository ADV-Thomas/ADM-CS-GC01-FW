gcc ^
    -D VIRTUAL ^
    -D F2803X ^
    -I ..\..\code ^
    -I ..\..\c2000\driverlib\f2803x ^
    ..\..\code\src\drv\eeprom.c ^
    ..\..\code\src\drv\file.c ^
    ..\..\code\src\lib\crc.c ^
    ..\..\code\src\lib\mal.c ^
    ..\..\code\src\lib\mem.c ^
    ..\..\code\src\net\eeprom.c ^
    ..\..\code\src\net\file.c ^
    ..\..\code\src\net\net.c ^
    main.c ^
    -o main.exe
REM INPUT ARGUMENTS: ID REV VAR ADR
REM ID VALUES: 1-LF45 2-UP25 3-LL25 4-BC25 7-BP25 8-BI25
.\main.exe 1 3 2 0