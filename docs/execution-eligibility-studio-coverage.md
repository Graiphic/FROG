# Execution eligibility — Studio implementation coverage

Révision 1 — 16 septembre 2026. Ce relevé est **informatif**.
Les [règles EXEC](../Language/Execution%20eligibility.md) sont le contrat ;
cette page indique la preuve disponible, pas une équivalence automatique entre
le modèle privé d'édition et toutes les sources publiques FROG.

## Qualification

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
Cette preuve est celle du code local testé ; aucun commit d'implémentation
publié ni aucune validation exhaustive des 137 scénarios n'est revendiqué.

Lors d'une qualification, enregistrer la règle EXEC et le scénario VAL, le commit
de l'implémentation, la commande de test, la configuration/cible, le résultat et
l'artefact de preuve. Un échec ou une limite restent explicites. Les versions
d'exécutable ne remplacent pas ces informations.
