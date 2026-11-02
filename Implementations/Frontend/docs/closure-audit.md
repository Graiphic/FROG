# Audit de fermeture du Frontend P28

L'outil `tools/audit_frontend_closure.py` contrôle les vraies unités de production
énumérées dans `production-sources.cmake`. Il ne compile pas, ne configure pas
CMake, ne copie aucun fichier source et ne crée aucun paquet. Python 3.10 ou
ultérieur suffit ; aucune dépendance Python externe n'est nécessaire.

Depuis la racine du workspace ou d'un snapshot complet des trois dépôts :

```text
python FROG/Implementations/Frontend/tools/audit_frontend_closure.py --root FROG/Implementations/Frontend --output frontend-closure-audit.json
```

`--root` accepte aussi la racine FROG ou la racine commune des trois dépôts.
Pour contrôler en plus les adaptateurs C/CLI réellement présents, ajouter :

```text
--source src/FrontendApi.cpp --source src/FrontendOperation.cpp --source src/FrontendCli.cpp
```

L'absence d'un de ces fichiers est un échec, pas une exclusion silencieuse.
La liste par défaut reste exactement celle du générateur de production ; le
rapport conserve toutes les racines effectivement inspectées.

## Contrat du contrôle

- `set(FROG_FRONTEND_PRODUCTION_SOURCES ...)` doit apparaître exactement une fois
  et contenir des chemins littéraux. Les substitutions CMake sont refusées.
  `--sources` accepte aussi une liste texte ou JSON (`{"sources":[...]}`).
- Les includes sont résolus uniquement dans le propriétaire Frontend : `include`,
  `include/frog_native_engine`, `src`, `snapshot/api`, `snapshot/sdk/include` et
  `snapshot/generated`, puis les éventuels `--include-dir` confinés à ce même
  propriétaire. Un lien vers un fichier hors de ce propriétaire échoue.
- Toutes les branches conditionnelles sont parcourues. Les includes dynamiques,
  non résolus, privés Runtime/Studio, Canvas, contrôleurs, Win32 et LLVM/Clang
  sont refusés. Les headers standards C/C++20 sont une liste explicite ; un
  header plateforme inconnu reste visible.
- Les identifiants de modèles et sessions privés sont recherchés hors commentaires
  et littéraux C++. L’extraction admet désormais les propriétaires de données
  neutres **explicitement listés** par l’outil : `WidgetInstance`, `DiagramNode`,
  `FrogDocumentStateData` et leurs codecs se résolvent dans le Frontend public.
  Leur nom historique n’en fait pas un Canvas. Cette liste ne constitue aucune
  permission générale pour des fichiers préfixés `Widget` ou `FrogDocument` :
  leurs contenus et dépendances restent contrôlés. `FrontPanelCanvasScene`,
  `LoadedFrogDocument`, contrôleurs, réparation/récupération privées et sessions
  runtime restent exclus.
- Le décodeur documentaire Zstandard 1.5.7 est une dépendance statique bornée et
  explicite. Ses sources C et notices sont conservées ; seules les branches
  désactivées par les définitions effectives (`ZSTD_LEGACY_SUPPORT=0`) et les
  headers intrinsèques du compilateur déclarés par fichier bénéficient des
  exceptions documentées. Le compresseur et le runtime privés ne sont pas
  importés par cette exception.
- Les types de valeurs/schémas du SDK public `frog/sdk/runtime_*` et les espaces
  de noms historiques `frog::runtime` des contrats sémantiques restent admis.
  Cela ne permet aucun include privé `frog/runtime/*`.
- Chaque fichier visité et le manifeste portent un SHA-256 ; une modification
  pendant l'audit produit un échec. Le rapport contient les arêtes d'include,
  chemins relatifs, tailles et raisons localisées. Il ne publie aucune origine
  absolue de la machine.

Codes de sortie : `0` fermeture conforme, `1` violation, `2` configuration ou
lecture impossible. Les codes de diagnostic sont stables ; le message est une
preuve localisée et non une règle de classification sémantique du langage.

Ce contrôle lexical conservateur ne remplace ni le compilateur, ni l'audit des
symboles d'un binaire lié, ni les tests FIR/Event et C ABI/CLI/Studio. Un succès
ne prouve pas la parité d'une copie SDK avec son owner ; cette parité utilise
les captures `snapshot` de `source-origin.json` et la configuration CMake
existante. Les hashes `move` de ce manifeste restent les captures d'extraction,
sans empêcher les évolutions ultérieures du nouveau propriétaire. L'inventaire
de licences distingue donc `original_hash` et `current_hash`.

Le relevé complet E2 du 1 novembre 2026 a parcouru les 45 sources de production
et les trois adaptateurs ci-dessus : 48 unités racines, 179 fichiers et
313 arêtes, sans include privé Runtime/Studio, modèle éditeur concret, Win32,
LLVM ou include introuvable. La preuve est conservée sous
`FROG-STUDIO/native-portable-engine/artifacts/modularisation-etape2-p28-2026-11-01/independent-frontend-full-closure-audit.json`.
Ce résultat porte sur la fermeture lexicale de cet état source ; aucun binaire
ni lien réussi n'est déclaré par cet audit.

La phase A du 2 novembre a ensuite reconstruit et installé le kit autonome de
contenu `c640d266a4de9d9581e6394d80d7152ce3d91d64278b960ab6777a524a056a62`, sans sources,
en-têtes ni bibliothèques Studio/Runtime/LLVM dans ses entrées. Ses six CTest
ciblés, 6 614 contrôles sémantiques et le consommateur C du préfixe installé
ont réussi. Le reçu intégrateur
`artifacts/p28-implementation-2026-11-02/phase-a-result.json` identifie aussi
le manifeste du kit (`fed3baac219da2bc68c99b9d08e6560b73ca68c100e17cbfdfeb33d1e09f0102`)
et les binaires. Ce reçu est conservé par l’intégrateur ; son chemin n’est pas
une dépendance de construction du kit. Il ne qualifie ni Windows propre,
ni les changements ultérieurs de phase B, ni l’exécution par un backend.

## Même source snapshot que le produit

La capture complète réutilise
`FROG-STUDIO/native-portable-engine/tools/Export-StudioSourceSnapshot.ps1` :
manifeste `source-snapshot.json`, trois dépôts identifiés, révision et overlay
locaux, chemins portables et hashes vérifiés. Exécuter son mode `-VerifyDirectory`
avant d'auditer le Frontend dans cette capture. Le présent outil ajoute une
preuve de fermeture ; il ne remplace pas cet export et n'introduit pas un second
format de paquet. Ne pas exporter un snapshot pendant une édition active.

Un audit réussi ne change pas les droits de distribution. Lire
`provenance-and-licensing.md` et `license-inventory.json` avant toute diffusion.
