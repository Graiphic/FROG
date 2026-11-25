# Lire la position et la taille d’un fichier

En Simulation, saisir le chemin absolu d’un fichier non vide, par exemple le Readme.md de cet exemple. Ouvrir en lecture seule, avancer d’un octet et lire la position. Après fermeture, vérifier la taille et le type du fichier.

En Simulation, saisir le chemin absolu d’un fichier non vide, par exemple le Readme.md de cet exemple. Ouvrir en lecture seule, avancer d’un octet et lire la position. Après fermeture, vérifier la taille et le type du fichier.

En Simulation, saisir le chemin absolu d’un fichier non vide, par exemple le Readme.md de cet exemple. Ouvrir en lecture seule, avancer d’un octet et lire la position. Après fermeture, vérifier la taille et le type du fichier.

Mode: interactive. Profile: studio-draft. Capability: Simulation avec un fichier lisible choisi par l’utilisateur.
Expected:
position (i64) = 1
nonempty-result (bool) = true
directory (bool) = false
error-out (cluster<"status":bool,"code":i32,"source":string>) = {"status":false,"code":0,"source":""}


[Open the actual FROG document](inspection.frog).
