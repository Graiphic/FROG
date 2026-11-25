# Réduire un fichier réservé de quatre à deux octets

En Simulation, saisir un nom de fichier de base dans un dossier accessible en écriture. Réserver un nouveau fichier, y écrire FROG, vider le tampon, puis ramener sa taille à deux octets. Le texte relu est FR. Seul le fichier réservé par le montage est supprimé.

En Simulation, saisir un nom de fichier de base dans un dossier accessible en écriture. Réserver un nouveau fichier, y écrire FROG, vider le tampon, puis ramener sa taille à deux octets. Le texte relu est FR. Seul le fichier réservé par le montage est supprimé.

En Simulation, saisir un nom de fichier de base dans un dossier accessible en écriture. Réserver un nouveau fichier, y écrire FROG, vider le tampon, puis ramener sa taille à deux octets. Le texte relu est FR. Seul le fichier réservé par le montage est supprimé.

Mode: interactive. Profile: studio-draft. Capability: Simulation avec réservation exclusive dans le dossier choisi.
Expected:
text-read (string) = "FR"
error-out (cluster<"status":bool,"code":i32,"source":string>) = {"status":false,"code":0,"source":""}


[Open the actual FROG document](tronquer.frog).
