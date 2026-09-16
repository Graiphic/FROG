# Audit documentaire FROG / Studio — 6 septembre 2026

## Périmètre et preuve

Audit des fichiers locaux et de leur historique Git, sans fetch, push, génération
de code ni lancement de Studio. Les références distantes locales ne prouvent pas
l'état courant des sites publiés. Les modifications déjà présentes ont été conservées.

| Dépôt | HEAD local au début de la passe | État observé |
| --- | --- | --- |
| FROG | `55e4554` — publication Timed Loop, 30 août | Branche `agent/fix-default-asset-path-case`, modifications et contrats locaux non suivis |
| FROG-STUDIO | `606db16` — authoring et structures typées, 5 septembre | Branche `agent/complete-studio-interaction-conformance`, nombreuses modifications locales |
| Graiphic-Studio | `19a4f4b` — documentation des réalisations, 25 août | `main`, propre avant cette passe |

Inventaire initial : 560 Markdown dans FROG. Le contrôle de liens a parcouru
250 documents : `Readme.md`, `GOVERNANCE.md`, les quatre `FROG-*.md` d'orientation,
et toutes les familles ci-dessous. Les nouveaux documents créés parallèlement
pendant cette intervention ne sont pas inclus dans ce décompte initial.

| Famille FROG parcourue | Markdown | Autorité examinée |
| --- | ---: | --- |
| Expression | 28 | Source canonique, types, widgets, structures et schémas |
| Language | 7 | Sens validé et règles d'exécution, avant FIR |
| IR | 12 | Représentation dérivée, conservation, lowering et handoff |
| IDE | 11 | Concepts d'édition et observation, sans redéfinir le langage |
| Conformance | 184 | Attentes publiques accept/reject/preserve, par étape |
| Versioning | 2 | Version du corpus et statut des surfaces |

Les 32 Markdown de Graiphic-Studio ont été parcourus pour leurs liens et leur
organisation. Dans FROG-STUDIO, 67 fichiers documentaires ont été inventoriés
dans `docs/` et `native-portable-engine/docs/`, dont 50 Markdown ; lecture ciblée
du README, des contrats de structures et des rapports de livraison/architecture.
Cet inventaire n'est pas une lecture sémantique exhaustive de chaque paragraphe,
une validation visuelle de chaque capture, ni une exécution de chaque exemple.

## Autorité à conserver

La séparation publiée est solide : `Expression/` définit ce qui est écrit ;
`Language/` définit le sens admis ; `IR/` dérive ce sens sans en devenir le premier
propriétaire ; `Conformance/` publie les résultats attendus. `Versioning/` décrit
les frontières de publication, pas de nouvelles sémantiques. Voir
`Language/Readme.md:53`, `GOVERNANCE.md:166` et `GOVERNANCE.md:330`.

`IDE/Editors.md:83` réserve à FROG source, sens, types et contrats inter-outils.
La documentation Graiphic-Studio décrit les workflows du produit ; les exigences
`STR-*` de FROG-STUDIO gouvernent ses gestes, visuels et preuves d'implémentation.
Une réalisation ou un validateur de référence ne devient pas normatif par sa
seule existence. Une apparence implémentée ne prouve pas l'exécution.

`Versioning/Readme.md:172` déclare **corpus `0.1-draft`**, tandis que sa ligne 187
déclare **source `spec_version = 0.1`**. Ce n'est pas une contradiction : la
distinction est explicite. Ne pas confondre non plus ces valeurs avec
`metadata.program_version`, `wfrog_version`, un profil d'extension ou le numéro
de build Studio.

## Constats prioritaires

1. **Schémas non garantis par une CI verte.** Au début de l'audit,
   `Validator/tests/test_public_structure_contract.py:10` rend `jsonschema`
   optionnel, puis les tests de validation sont conditionnellement sautés.
   `.github/workflows/reference-workspace.yml` et `reference-pipeline.yml`
   installent seulement `pytest`. Les assertions présentes ne garantissent donc
   pas l'exécution du moteur Draft 2020-12. La correction dépendances/CI relève
   de la passe centrale ; ce rapport n'en annonce pas le résultat.
2. **Le statut public est contradictoire et périmé.**
   `FROG-Project-Status.md:42` annonce les exemples 01–20, sa ligne 319 annonce
   01–24, alors que 47 dossiers numérotés existent. `Versioning/Matrix.md:23`
   borne correctement le runtime public à 01–15, mais ses tableaux de fermeture
   et de kinds ne listent que 01–07 ; ses lignes 254 et 285 parlent encore de
   01–09. Le runner `Pipeline/check_examples01_10_full.py:2` décrit bien 01–15
   et conserve volontairement son ancien nom pour compatibilité. Ne pas déduire
   une exécution publique des exemples 16–47 de leur seule présence.
3. **Contrats locaux acceptés ≠ publication Git vérifiée.** Les profils
   `Expression/Typed Binding Contract v1.md` et `Case Editing Profile v1.md`
   étaient non suivis tout en portant « accepted » en ligne 3. Le premier
   conserve explicitement des étapes inachevées en lignes 154–162, notamment
   la migration de l'enveloppe Studio. Conserver ces limites et associer toute
   publication de profil à une révision et une matrice de capacités vérifiables.
4. **Largeur des boucles : contradiction produit corrigée.** Le contrat FROG
   fixe déjà `i64` pour count/index (`Expression/Control structures.md:716`,
   `:721`, `:868` ; `Language/Control structures.md:410`). Les exigences produit
   `STR-FOR-022` et `STR-WHILE-010` présentaient encore cette largeur comme non
   tranchée. Elles sont alignées sur FROG ; l'I32 LabVIEW reste historique et
   n'impose pas sa règle de saturation au langage.
5. **Pas de contrôle général des schémas source/IR identifié dans le runner.**
   La recherche des références de schémas dans les Python de
   `Implementations/Reference/` a identifié le test dédié des nœuds de structure,
   mais pas d'appel généralisé au schéma racine `.frog` ou à la famille IR.
   Les contrôles manuels d'artefacts ne sont pas équivalents à une couverture
   Draft 2020-12 exhaustive. `Expression/schema/frog.schema.json:5` annonce
   honnêtement un périmètre conservateur, principalement top-level ; accepter
   ce schéma ne prouve ni typage complet, ni sens, ni lowering.

## Liens et navigation

Mise à jour documentaire après ces constats : `FROG-Project-Status.md` et
`Versioning/Matrix.md` distinguent désormais les 47 dossiers présents de la
fermeture runtime publique 01–15. Leurs anciens tableaux sont explicitement
historiques et partiels ; aucun kind ni niveau de couverture n'a été extrapolé
aux dossiers absents de ces tableaux. L'entrée `Versioning/Readme.md` renvoie
au contrat de compatibilité et de profils. La commande d'installation des tests
dans le statut projet utilise le fichier de dépendances explicites.

Le scan statique de 250 documents FROG a examiné 356 références locales HTML
`href/src` et Markdown inline. Un chemin inexistant certain a été trouvé et
corrigé : le logo de
`Conformance/invalid/12_public_interface_declaration_must_not_require_front_panel_widget_existence.md:2`
remonte désormais de deux niveaux. Les ancres, liens externes, liens Markdown
de référence et différences de casse sensibles à Linux ne sont pas certifiés
par ce scan Windows.

Le scan des 32 documents Graiphic-Studio a examiné 126 références locales.
Il a signalé 26 références de `docs/index.md` comme non résolubles relativement
au fichier : 25 liens `docs/...` et une image `assets/...`. Tous les fichiers
cibles existent depuis la racine du dépôt. Les 25 liens restent un problème
pour la lecture directe du Markdown GitHub. Le site utilise une homepage
Docsify et `relativePath: true` (`index.html:193–197`) ; le plugin en
`index.html:213` réécrit les **images**. Une vérification de la homepage et de
la route directe `#/docs/index` est nécessaire avant de qualifier toutes ces
alertes d'erreurs du site déployé ou de changer leurs chemins.

Le vérificateur existant
`.github/scripts/verify-pages-navigation.ps1` a d'abord été exécuté en lecture seule :
**échec**, premier chemin absent du sidebar :
`/Expression/Case%20Editing%20Profile%20v1.md`. Cela concerne un ajout local,
pas une preuve de panne de la publication distante. Son périmètre n'inclut pas
`Versioning/` dans les sections générées et ne remplace pas un contrôle de tous
les liens éditoriaux.

Après création des documents de convergence et accord de coordination, le script
officiel `build-pages-navigation.ps1` puis son vérificateur officiel ont été
exécutés sous `pwsh`, comme dans le workflow : **PASS**, 728 chemins de recherche
et 19 entrées racine. Le contrat de compatibilité et le profil Case figurent dans
la navigation. Le générateur est inchangé ; les rapports restent découvrables
par les liens éditoriaux du README et du contrat. Un contrôle complémentaire des
sept documents nouveaux ou d'entrée modifiés a vérifié 47 références locales :
aucune cible manquante. Ce résultat ne certifie pas les ancres ou le site distant.

## Corrections de tests vérifiées

Les échecs Tab/Tree provenaient de tests antérieurs aux contrats publiés, pas
d'une justification a posteriori par le commit `fa8373f` : ce dernier n'a modifié
ni Tab ni Tree.

- `7f20ae55` (19 mai) publie les cinq parties `page_label_display*`, leur propriété
  et leurs marqueurs SVG. Le test Tab conserve un ensemble exact et vérifie
  aussi les bindings `svg_or_host_surface`, conformément à `Default/Tab.md:111`.
- `ca21e49e` puis `5a884a34` (22 mai) publient respectivement les en-têtes de
  colonnes Tree et `active_cell`, tous deux overlays hôte.
- `d8935c9c` (22 mai) ajoute les cinq SVG de symboles Tree. Ils ne sont pas la
  géométrie shell. Les tests vérifient désormais séparément les six marqueurs
  shell exacts, les overlays, le registre exact des cinq symboles et le seul
  marqueur `item_symbol_path` dans chaque ressource de symbole.

Validation ciblée réelle : Python 3.11, `-B -m pytest -p no:cacheprovider`, les
deux fichiers `test_tab_deepening.py` et `test_tree_deepening.py` : **7 PASS**.
Cela ne constitue pas un résultat global du workspace ni une preuve runtime.

## Conclusion actionnable

Le bon rangement est celui des responsabilités existantes, pas une fusion des
trois dépôts. La convergence doit utiliser un registre de capacités versionné
qui relie **contrat FROG → forme source/profil → fixture positive et négative →
test Studio → statut validation/lowering/runtime → documentation produit**.
Les matrices humaines peuvent être dérivées de ce registre ; les décisions
normatives restent relues et appartiennent à leur couche.

Priorités recommandées : rendre les dépendances de tests explicites sans skip
silencieux ; synchroniser les matrices 01–15 / 16–47 ; publier les profils avec
leurs limites et preuves ; contrôler séparément liens GitHub et routes Docsify ;
relier les pages produit à un build/capture vérifiés. Conserver les profils
incomplets comme tels et refuser explicitement un export non pris en charge.
Ni une migration générale d'enveloppe, ni un build, ni une publication distante
n'est annoncé comme accompli par cette passe documentaire.

## Annexe — échecs du rendu Python UI après audit complet

Reproduction bornée : huit fichiers `Runtime/python/tests/test_runtime_ui_slice`
05, 06, 08, 09, 10, 11, 12 et 13_15, Python 3.11, dépendances isolées, cache
pytest désactivé et répertoire temporaire neuf. Avant correction : **9 FAIL,
21 PASS**. Après les adaptations ci-dessous et trois cas de régression ajoutés :
**1 FAIL, 32 PASS en 2,70 s**. Il ne s'agit ni du bilan global Reference ni d'une
validation visuelle dans un navigateur. Les rendus concernés empruntent le
lecteur JSON local de `Runtime/python/runtime_core.py`, pas le nouveau lecteur
commun durci ; les écarts examinés préexistent donc à cette modification.

| Périmètre | Preuve et traitement |
| --- | --- |
| Numeric 05 | `a301cba` / `7af8c9a` publient les nouvelles dimensions et la géométrie `placement_bounds`. Le test suit désormais le panneau 500×128, les widgets 96×32, le SVG 220×88, les couleurs source et `value_face`, sans certifier les anciens paramètres `frame` devenus inopérants. Les routes, actions et égalité du snapshot partagé restent testées. |
| Boolean 06 | `869e10a` puis `5d2a24e` actualisent réalisation, couleurs et placement. Les assertions suivent le recadrage `placement_bounds`, les parties `state_face` / `focus_ring`, le faux blanc et le focus cyan 1px. Le bouton local porte `data-toggle-target=true` quand la valeur courante/entrée cachée est fausse ; l'ancienne assertion `value=true` confondait ces deux rôles. |
| Enum 08 | `f9af09e` migre les propriétés `style.dropdown` vers `style.popup` et les skins associés. Les assertions suivent le sélecteur masqué et les couleurs popup source. L'interdiction globale de `<pre>` est remplacée par la vérification du lien d'inspection source : ce balisage appartient désormais aux panneaux d'artefacts, pas à un dump permanent de l'état UI. |
| Path 09 | La réalisation actuelle possède une bordure 0,5px et un focus SVG masqué (`1bfc9a1`, contrat public de focus SVG). Les assertions correspondent à ces valeurs, tout en conservant les tests des deux paires indépendantes et des interactions. |
| Button 10–15 | `39f6e27` publie `style.button_face.*` et les marqueurs SVG `button_face`, mais ne migre pas le Python. Le core transmet maintenant ces propriétés ; le renderer lit leurs états et aligne géométrie, overlay, CSS et sélecteur JavaScript sur cette partie. Les anciens documents/skins `style.face` / `face` restent lisibles par un adaptateur borné ; les propriétés canoniques priment comme groupe lorsqu'elles existent. Les tests conservent les couleurs attendues et les comportements press/release/read. |
| Boolean dans Button | L'appel du renderer indicateur reçoit maintenant son chemin SVG résolu via `asset_map`, comme dans le runtime Boolean. Son absence produisait réellement `missing-skin`, ce n'était pas une assertion obsolète. Les textes source `Off` / `On` remplacent les anciennes attentes `FALSE` / `TRUE`. |

Les nouveaux cas vérifient les géométries personnalisées canonique et ancienne,
les six couleurs false/true/hover/pressed, la priorité canonique, le repli d'un
état absent sur sa base, les sélecteurs des deux générations et la présence du
SVG Boolean. Ils ne prouvent pas le rendu effectif CSS/JavaScript en navigateur.

**Défaut de la passe initiale : héritage des propriétés de réalisation.**
Corrigé lors de la reprise du 6 septembre 2026 : les références de réalisation
sont maintenant résolues avant les defaults du package et les surcharges
d'instance. Le test initial et onze tests de régression supplémentaires passent,
dont les diagnostics de sélecteurs `class_id` / `target_class` non textuels.
La dernière passe unique de la suite Python complète donne **514 réussites,
aucun échec et aucun test ignoré en 25,79 s**, avec la chaîne WinLibs explicitement
visible dans le processus Python ; voir les [preuves de la reprise](language-studio-convergence-audit-2026-09-06.md#reprise-du-6-septembre-2026--defaults-boolean).
Le constat ci-dessous est conservé comme historique de la passe initiale.

Les indicateurs des exemples Button omettent leur typographie source. Le core
ne fusionne pas les propriétés par défaut de `boolean.default.wfrog` ; le
renderer utilise alors 18px pour caption et state text. La réalisation circulaire
publique définit respectivement **12px et 13px** (lignes 511 et 518). Le test 10
conserve une assertion échouante sur ces valeurs publiques, au lieu d'accepter
le repli 18px ; ses anciennes attentes 18px/12px n'étaient pas non plus alignées
sur le contrat courant. Aucun mécanisme existant de fusion des defaults dans le
core Boolean ne pouvait être réutilisé par un simple paramètre local. La suite
ne peut donc pas être déclarée verte. Le chantier précis est de résoudre les
defaults de réalisation avant les surcharges d'instance et de tester cette
priorité, sans importer une politique générique nouvelle au cours de cette
correction limitée.
