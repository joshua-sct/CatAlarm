----------
# Siren
## handlePlay()
- Eviter les cas de dépassement sur endTime/currentTime/startTime
e.g : Event millis overflow => Reinitialiser

## Sonnerie modulée en fréquence
void Siren::playSirenSound() {}

----------

# Sigfox
- A revoir
- Vérifier s'il y a assez de timer matériel

----------

# Error
- A compléter
	- Error hasBeenPlayingForTooLong => comparer ERROR_CODE à ERROR_CODE_COPY pour voir si l'erreur à changé

----------

# Gyro
- A faire

----------
----------

!!! Utiliser millis() seul du côté de du test logique (?) pour éviter les problemes d'overflow (?) ???
