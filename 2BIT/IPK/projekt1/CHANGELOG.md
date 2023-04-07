## Implementácia 
Klient *ipkcpc* je naimplementovaný v jazyku C, revízia C11 v prostredí UNIX OS. Projekt sa prekladá príkazom *make*, na ktorý je potreba program GNU MAKE, doporučený je prekladač gcc verzie 7.5.0

## Limitácie
* Maximálna dĺžka načítaného vstupu v TCP móde je 1024 znakov
* Maximálna dĺžka načítaného vstupu v UDP móde je 255 znakov, pretože dĺžka dát v response je maximálne 8 bitov(= 255 znakov)