Premièrement, brancher la caméra dans un port USB.

Pour compiler le pilote et ouvrir le programme de l'usager:

	ouvrir un terminal dans le dossier et entrer les commande suivante:

	1. sudo ./delCamDrivers.sh
	2. sudo make
	3. sudo ./runDriver.sh
	4. gcc user.c -o user.exe
	5. ./user.exe

**Note: 1) les fonctions SET et GET ne sont pas fonctionnel 
		2) La photo sera nommé testPic.jpg et elle se trouvera dans le dossier 