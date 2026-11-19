# Capteur — fixture de classes par valeur

Cette fixture redistribuable est destinée aux contrôles ciblés des vrais membres. Sa présence ne qualifie pas à elle seule une version de Studio.

Ouvrir `Main.frog`, en conservant les quatre `.fclass` et les six corps membres dans le même dossier. Les identités et chemins sont déclarés dans les documents ; aucun nom de fixture ne déclenche une implémentation spéciale.

Le contrôle booléen choisit Analogique ou Numérique avant Run. Les deux objets passent par le même Select typé Capteur, le même Set Offset statique et le même Mesurer dynamique. Une branche mesure la valeur originale. Un troisième enfant ne redéfinit pas Mesurer.

| Choix | original | offset7 | hérité |
|---|---:|---:|---:|
| Analogique |107|104|10|
| Numérique |218|215|10|

`Mesurer Analogique.frog` lit sa calibration privée−3 et appelle le Get Offset protégé du parent, puis ajoute100. `Mesurer Numérique.frog` lit sa résolution privée8, la convertit explicitement en I64, et ajoute200 à l’offset. Set Offset renvoie une nouvelle valeur ; la branche originale conserve l’offset10 et les données enfant.

Le contrôle I64 Calibration est consommé par le vrai membre enfant Set Calibration avant l’upcast. Avec19, les résultats analogiques deviennent129 et126 : Set Offset doit conserver cette donnée enfant non issue du défaut, son type concret et la branche originale. Le même artefact est réutilisé pour ces valeurs.

Le test modifie la constante100 du vrai corps analogique en101 : les résultats deviennent108 et105. La préparation doit produire une nouvelle empreinte, sans ancien corps réutilisé. Cette mutation n’est pas inscrite dans les fichiers redistribués.

Depuis l’explorateur de projet, Class Properties permet de créer les membres, d’ouvrir leur FROG, de placer valeur par défaut, accès privés et appels, puis de renommer sans changer les identités. Une redéfinition part de la signature dynamique héritée. Après édition de son connecteur ordinaire, l’action explicite Adopt edited connector vérifie les bindings réels et la compatibilité de la hiérarchie ; elle ne répare pas silencieusement une redéfinition devenue incompatible.

Les opérations privées se déposent depuis la définition déclarante quand le document actif est son véritable membre. Les membres enfants accèdent au parent par ses méthodes accessibles. Le connecteur objet sert au transport par l’appelant, sans valeur initiale fabriquée sur la face avant du membre.

Commandes CTest ciblées enregistrées : `frog-frontend-class-definition-smoke`, `frog-class-members-execution-smoke`. Le second réutilise le compilateur et les sessions natifs, en NativeFast et NativeDebuggable. Les réceptions graphiques, FIR et véritable EXE doivent être attribuées au binaire testé ; elles ne sont pas remplacées par ces contrôles.
