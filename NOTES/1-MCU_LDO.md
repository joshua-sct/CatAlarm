## MCU 
Ceux basés sur Cortex M0/M0+

- STM32
	- developpement ez
	- Modèles
		- STM32L0..
			- peu cher en nombre (moins que atmega328p)
			- économe
			- STM32L05x ?
https://www.reddit.com/r/embedded/comments/13kpxl4/looking_for_help_selecting_a_lowcost_lowpower/

- ATmega328P
	- old
	- overpriced++ => bien pour hobby mais nulle pour de la grosse production
	- développement ez
	- atmega328-PB moins cher?
https://www.reddit.com/r/AskElectronics/comments/l1xvts/selecting_a_microcontroller_is_the_atmega328/

A voir :
- MSP430 ?
- PIC16F ou PIC18F
- nRF52 Series
- ATSAMD21

(+) COMPATIBILITE PlatformIO ? 
(?) Puissance de calcul / mémoire / Entrées-sorties


https://jaycarlson.net/2023/02/04/the-cheapest-flash-microcontroller-you-can-buy-is-actually-an-arm-cortex-m0/


### Linear regulator
Pertinent car :
- Peu cher
- Bonne efficacité pour des petites chuttes de tension (3V7 -> 3V3)

Critères du regulateur:
- Quiescent current : Courant consommé par le régu, important pour l'efficacité @ charge faible
- Dropout voltage : Différence entré-sortie pour foncionner correctement 
	=> Utiliser LDO
#### Modèles :


- MCP1700 ?
- LT1761 ?
- MIC5504-3.3YM5-TR ?
- LP2985-33DBVR ?
- AMS1117-3.3
	- Très peu cher
	- Quiescent current = 5-10mA
		(courant consommé par le régu <= important pour l'efficacité lorsque la charge est faible)
	- Dropout voltage = 1V