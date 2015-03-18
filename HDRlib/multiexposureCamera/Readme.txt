Windows:
Pro kompilaci na windows je zapotřebí stáhnout opencv. Pokud se používá Qt Creator,
je dále nutné zkompilovat OpenCV pomocí MinGW (v aktuální verzi OpenCV jsou pouze
knihovny zkompilováne pomoci Visual Studia). 

Postup na kompilaci je dobře popsán zde: 
www.laganiere.name/opencvCookbook/chap1s1_2.shtml

Nebo lze stáhnout mou vlastně zkompilovanou verzi zde: 
http://pcjurzykowski.fit.vutbr.cz/data/opencv-mingw.zip

Dále je potřeba přidat cesty do PATH
C:\opencv\build\x86\mingw\bin
C:\Qt\Qt5.2.1\Tools\mingw48_32\bin
- Zkontrolovat podle skutečného umístění


Mělo by jít i použit verzi Qt Cratoru požívající Visual Studio a použít OpenCV
bez nutnosti kompilace. Ale nemám odzkoušeno.

Linux:
Nainstalovat balíčky OpenCV



Celý projekt lze otevřít a kompilovat pomocí QtCreator
