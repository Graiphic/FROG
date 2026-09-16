# Execution eligibility — Studio implementation coverage

Révision 2 — 16 septembre 2026. Ce relevé est **informatif**.
Les [règles EXEC](../Language/Execution%20eligibility.md) sont le contrat ;
cette page indique la preuve disponible, pas une équivalence automatique entre
le modèle privé d'édition et toutes les sources publiques FROG.

## Qualification

### Checkpoint de publication du 16 septembre 2026

Les corrections décrites ci-dessous sont incluses dans FROG-STUDIO
[`48f300f`](https://github.com/Graiphic/FROG-STUDIO/commit/48f300fa2e9d45770f2d6a9b9f4af9d8168352b3),
publié sur `agent/complete-studio-interaction-conformance`, sans fusion sur `main`.
La livraison courante est **0.0.2.513**, Authenticode **Valid**, avec vérification
du raccourci Bureau canonique. Son relevé de livraison décrit aussi les évolutions
ultérieures : focus animé des diagnostics, formats d’icône et palette Horodatage.

Avant publication, **16/16 tests ciblés** ont été réexécutés avec succès :
catalogues/contrats et typage, navigation, Horodatage, sélection Variant,
diagnostics Win32, document readiness, Custom icon, binding profile, version
et contours graphiques. Ce n’est pas une reconstruction complète de toutes les
suites ni une qualification des 137 scénarios VAL. Runtime POC reste OFF.
L’échec clipboard du précédent audit n’est pas déclaré résolu.

### Livraison initiale : compilation et runtime séparés

Livraison **0.0.2.479**, signature Authenticode valide et raccourci Bureau canonique
vérifiés. Les preuves de cette section décrivent la livraison initiale locale ;
les sources sont désormais incluses dans le checkpoint Studio cité ci-dessus.

`win32_execution_validation_smoke.cpp` : **110 vérifications réussies**. Cinq
suites CTest ciblées passent : fixture conformance, document readiness, structure
execution validation, diagnostics Win32 et contrat de version du binaire.
Configuration : Windows, Runtime POC OFF, rendu de test WARP.

| Situation ciblée | Résultat local vérifié |
| --- | --- |
| Nouveau diagramme vide, sans obligation d’interface | Buildable, pas de sens interdit ; runtime absent expliqué séparément. |
| Deux constantes numériques vers a/b d’Add, sortie inutilisée (VAL-011) | Valide, un seul diagnostic global runtime, pas de fausse erreur pour Add/constantes. |
| Add avec entrées obligatoires absentes (VAL-012/013, périmètre Add) | Compilation bloquée et sens interdit ; navigation vers l’objet possible. |
| Source publique sans validateur/lowerer | Toujours bloquée / non prise en charge, pas assimilée au diagramme d’édition vide. |
| Lecture sur Front Panel et Block Diagram | Chaque gestionnaire réel ouvre le même service de diagnostics ; capture libérée. |
| Présentation | Clair/sombre, tailles 18/24/32, français à 15 points, retour à la ligne réel, Tab/Échap, seconde icône intacte. |

Le contrôle visuel avec le véritable exécutable a confirmé le document vide,
l’Add valide et le clic Lecture corrigé depuis le Block Diagram. Ce contrôle a
révélé un ancien trou du test : appeler le gestionnaire Front Panel avec un HWND
Diagram ne prouvait pas que le gestionnaire Diagram transmettait le clic.
Le test utilise désormais les deux gestionnaires distincts.

SHA-256 du binaire livré :
`CEEB01D39BF2B1E5B26D2291B8498016E35574368A28059BADD1439230F1F23E`.
La validation ne génère pas d’artefact et n’active pas le runtime. Ces preuves
ciblées ne qualifient pas à elles seules toutes les variantes des scénarios VAL
ni les backends publics.

### Qualification précédente publiée

| Règles | Couverture observée | Preuve / limite |
| --- | --- | --- |
| EXEC-001, 003–007 | Validation portable des identités, ports, entrées requises, fragments, bindings et types du modèle d'édition | Tests FROG-STUDIO : document_readiness_smoke.cpp. Ce n'est pas la qualification exhaustive de tout le langage public. |
| EXEC-008–015 | Contrôles du modèle Studio : sorties par région, défauts, compte/condition de boucle, cas zéro, frontière/portée, Event, Sequence, Timed, registres et cycles instantanés | structure_execution_validation_smoke.cpp : cas positifs et négatifs ciblés ; les profils et garanties backend restent à qualifier séparément. |
| EXEC-002, 020 | Cache par document/modèle, correction, undo/redo, lignes anciennes et navigation vers des régions imbriquées masquées | win32_execution_validation_smoke.cpp, bureau privé Windows. |
| EXEC-001, 018, 020 | Runtime absent et source publique non prise en charge restent des causes visibles ; aucune exécution fictive | La livraison Studio courante est configurée avec FROG_STUDIO_ENABLE_RUNTIME_POC=OFF. |
| Contrat IDE | Triangle conservé avec surimpression rouge ; deuxième icône inchangée ; Lecture inspectable ; capture souris libérée ; blocages non masqués par le filtre | Même test natif, 73 vérifications réussies le 16 septembre 2026, incluant clair/sombre, trois tailles, les clics réels des deux panneaux et les libellés français. Résultat sur le code testé, pas qualification globale du produit. |
| EXEC-016–019 | Fermeture complète des dépendances, profils publics, artefacts et toutes les cibles | Non qualifié par les tests ci-dessus. Aucun statut « entièrement pris en charge » n'est revendiqué. |
| EXEC-021 | Suivi détaillé des 137 scénarios de l'étude | La matrice conserve « À qualifier » tant qu'une preuve par scénario/révision/cible n'est pas rattachée. |

Dépôt d'implémentation : Graiphic/FROG-STUDIO, sous-dossier
native-portable-engine/tests/. Les tests cités ne prouvent pas 137 scénarios
individuels, ni tous les backends.

## Contrat public et extensions d'édition

La représentation canonique d'une source, son profil et son contrat sémantique
restent les autorités. Les primitives ou structures supplémentaires disponibles
dans une branche de développement de Studio ne sont pas promues silencieusement
dans le contrat de base. En particulier, les modes étendus de boucle, sélecteurs
enum/entiers, garanties temporelles et conversions de source publique exigent
des contrats et preuves propres.

Pour une source publique conservée sans projection exécutable validée, le
diagnostic public_source_readiness_unavailable indique une limite du validateur/
lowerer de Studio ; il ne démontre pas que cette source est invalide FROG.

## Mise à jour de la preuve

Livraison locale vérifiée le 16 septembre 2026 : Studio **0.0.2.470**,
signature Authenticode valide et raccourci Bureau pointant vers l'exécutable
canonique `build-nmake-verify/frog-engine-shell.exe`. Quatre suites CTest ciblées
réussies : `frog-ui-fixture-conformance-smoke`, `frog-document-readiness-smoke`,
`frog-structure-execution-validation-smoke` et
`frog-win32-execution-validation-smoke`. Le test natif s'exécute sur un bureau
Windows privé avec `FROG_UI_TEST_FORCE_WARP_SVG=1`.
Cette preuve est celle du code local testé, publié ensuite dans le dépôt
privé FROG-STUDIO au commit `ac8fa9618d671c2983cee69e9ee76b5af7aa2e0a`
(branche de travail, pas release de main). Aucune validation exhaustive des
137 scénarios n'est revendiquée.

Contrôle supplémentaire avant publication : 18/19 tests Studio ciblés réussis.
Le test `frog-win32-inline-text-clipboard-smoke` échoue sur `string_control`
avec « Studio exported incorrect clipboard text », y compris lors d'une relance
isolée. Sa cause reste à diagnostiquer ; cela ne transforme pas le succès ciblé
des diagnostics en qualification globale. Le backend Event/Program Session a
réussi ses deux tests séparés ; son intégration complète dans Studio reste distincte.

Lors d'une qualification, enregistrer la règle EXEC et le scénario VAL, le commit
de l'implémentation, la commande de test, la configuration/cible, le résultat et
l'artefact de preuve. Un échec ou une limite restent explicites. Les versions
d'exécutable ne remplacent pas ces informations.
