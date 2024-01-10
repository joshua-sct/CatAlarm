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


# MAJUSCULES 
User
Il y a des majuscule en général au début des noms de fichiers .h ??

ChatGPT
Il n'y a pas de règle stricte concernant l'utilisation de majuscules pour les noms de fichiers d'en-tête (.h). Cela dépend souvent des conventions de codage spécifiques au projet ou à l'équipe de développement.

Cependant, une convention courante dans de nombreux projets C++ consiste à utiliser des majuscules pour les noms de fichiers d'en-tête, en particulier si ces fichiers contiennent des déclarations et des interfaces destinées à être incluses dans d'autres parties du code. Cela peut aider à distinguer les fichiers d'en-tête des fichiers sources (.cpp) qui contiennent généralement l'implémentation du code.

Par exemple, vous pourriez avoir des fichiers d'en-tête nommés MyClass.h et des fichiers sources correspondants nommés MyClass.cpp. Encore une fois, il s'agit d'une convention, et l'essentiel est d'adopter une approche cohérente au sein de votre projet ou de votre équipe.