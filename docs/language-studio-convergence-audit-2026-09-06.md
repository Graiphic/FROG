# Audit du langage FROG et convergence avec Graiphic Studio

Date : 6 septembre 2026. Audit et corrections locales ; aucune publication distante
ni conversion générale de fichiers utilisateur effectuée.

## Verdict

Les principes du langage sont solides : source explicite, interfaces indépendantes
de l'IHM, séparation source/sens/FIR/cible, identités stables et contrats de classes.
Studio a apporté des améliorations concrètes d'édition et de typage à conserver.
Mais **le système n'est pas encore unifié de bout en bout** : plusieurs formats et
deux chemins de validation/dérivation coexistent. Les masquer sous le nom `.frog`
donnerait une impression de compatibilité que les tests ne démontrent pas.

Cette passe corrige des défauts démontrés et publie les décisions de convergence
dans [Source compatibility and profiles](../Expression/Source%20compatibility%20and%20profiles.md).
La migration canonique complète reste un chantier distinct, avec des critères
d'acceptation et sans réécriture destructive des anciens documents.

## Périmètre et méthode

Révisions locales de départ : FROG `55e4554`, FROG-STUDIO `606db16`,
FROG-Runtime `31944f5`, documentation produit Graiphic-Studio `19a4f4b`,
contexte FROG-Context `a2e03e0`. Les changements préexistants sont conservés.
Des contrats acceptés localement étaient encore non suivis ; leur présence ne
prouve pas leur publication sur GitHub. Aucun fetch/push n'est annoncé.

- Inventaire documentaire initial de 560 Markdown FROG ; contrôle de liens sur
  250 documents des couches Expression, Language, IR, IDE, Conformance, Versioning
  et entrées racine ; 32 documents produit et inventaire documentaire Studio.
- Lecture ciblée des schémas, types, structures, `.wfrog`, versions, chargement,
  sérialisation, ports, profils, validation et dérivation ; confrontation aux
  tests, aux exemples et à l'historique Git.
- Analyse de 119 projections Numeric/Boolean contre le catalogue compilé et des
  12 réalisations `.wfrog` communes aux dépôts public/Studio.
- Contrôle JSON strict et schéma racine des 48 sources `.frog` publiées dans les
  47 dossiers d'exemples, sans assimiler ce contrôle à une validation sémantique.
- Tests positifs, négatifs et de préservation ; contrôles Python, C++, PowerShell,
  compilation canonique Studio et vérifications de documentation.

Il ne s'agit pas d'une preuve formelle de tout le langage, d'une lecture manuelle
de chaque paragraphe, ni d'une certification de tous les backends. Le détail du
périmètre documentaire est dans [l'annexe documentaire](language-documentation-audit-2026-09-06.md).

## Les formats réellement présents

| Format | Rôle et constat | Décision |
| --- | --- | --- |
| `.frog` public : `spec_version`, `metadata`, `interface`, `diagram` | Source canonique ; front panel facultatif. | Autorité source à conserver. |
| Studio : `frog.document.draft`, `draft_revision`, `frontPanel` | Format privé d'édition, encore écrit par Studio dans des `.frog`. | Compatibilité legacy maintenue ; migration explicite, pas renommage des clés. |
| `.wfrog` | Famille d'artefacts de classes/composition/réalisation, avec contrats distincts. | Ne pas confondre géométrie de réalisation et valeur/ports de la classe. |
| `frog_fir_unit` des exemples | Artefact de référence par motifs, utile et testé. | Ne pas le présenter comme conforme au schéma canonique Execution IR. |
| Execution IR `schema_family: frog.execution_ir` | Contrat public avec unité, objets, connexions, régions, source_map et correspondance. | Conserver cette cible d'interopérabilité ; adaptateur depuis la référence à réaliser. |

Le test `test_canonical_artifact_boundaries.py` rend explicites les deux
distinctions de formats. Il contrôle les schémas source/IR, les fixtures publiques
de racine, et une fixture IR structurelle volontairement vide. Cette dernière
n'est pas un programme exécutable ni une preuve de couverture sémantique IR.
Les 48 sources d'exemples sont aussi testées systématiquement. L'exemple 36
contenait six propriétés de style redéclarées dans les mêmes objets. Les
occurrences masquées ont été retirées : le document effectif reste identique
au décodage historique du HEAD Git qui conservait la dernière valeur. Ce
nettoyage élimine l'ambiguïté sans changer les valeurs de style consommées.

## Exigences et vérification individuelle

« Corrigé » ci-dessous est toujours limité au consommateur et aux tests cités ;
cela ne signifie pas que les autres implémentations ont été automatiquement migrées.

| ID | Exigence | Résultat de cette passe |
| --- | --- | --- |
| LF-01 | Une autorité explicite par couche. | Clarifiée dans le contrat de convergence et le README produit. |
| LF-02 | Version de source distincte du build, du corpus, du programme et du draft. | Confirmé : source 0.1, corpus 0.1-draft, draft_revision 2 ; pas de nouvelle version inventée. |
| LF-03 | Une version inconnue ne doit pas être interprétée comme 0.1. | Corrigé dans le précontrôle Python et l'import public Studio ; diagnostic sans acceptation. |
| LF-04 | Les sections d'une version future ne doivent pas être jugées selon l'enveloppe ancienne. | Test future source sans metadata/interface/diagram ; statut unsupported_source. |
| LF-05 | UTF-8/JSON ambigus ou hors capacité donnent un diagnostic. | Lecteur commun Python renforcé : clés dupliquées, tokens NaN/Infinity, limites entières/flottantes et profondeur. |
| LF-06 | Les entiers exacts ne passent pas par binary64. | Entiers Python et chaînes exactes préservés ; cas U64 maximal et limites testés. |
| LF-07 | Le front panel reste facultatif. | Règle publique confirmée ; Studio accepte le headless vide mais pas encore les graphes publics avec interface indépendante. |
| LF-08 | L'interface publique n'est pas reconstruite exclusivement depuis les widgets. | Non livré : modèle Studio à extraire. Import actuellement refusé pour éviter la perte des déclarations. |
| LF-09 | Un import ne supprime pas silencieusement les constructions exécutables qu'il ne représente pas. | Garde Studio ajoutée : interfaces, kinds/champs ignorés, pertes après aplatissement, ambiguïtés d'IDs et endpoints. Protection bornée, pas importeur universel lossless. |
| LF-10 | Draft historique et source publique ne forment pas deux autorités dans le même document. | Enveloppe mixte format/spec_version refusée ; drafts seuls et import UI historique testés. |
| LF-11 | IDs stables et non ambigus. | Validation Python des ports sur toute l'interface, nœuds et arêtes top-level ; rejet des identifiants malformés. |
| LF-12 | État auteur sauvegardable distinct de programme valide. | Conservé : fils incomplets et Case drafts bloquent validation/dérivation ; pas de normalisation réparatrice implicite. |
| LF-13 | Ports des manifestes cohérents avec l'implémentation. | 26 divergences corrigées dans 13 entrées Numeric ; 119 projections contrôlées strictement sur les champs exportés. |
| LF-14 | Une entrée de palette n'implique ni signature ni exécution. | Increment/decrement restent explicitement bloqués/catalog-only ; faux profils de réduction déplacés vers les bons opérateurs. |
| LF-15 | Toute signature provient d'une autorité déclarative. | Partiel : Add et Case ont leurs autorités/projections ; généralisation fonctions/widgets/autres structures encore nécessaire. |
| LF-16 | Types agrégés/enum/unknown ont une identité définie. | Contrat typed-binding conservé : rang, champs ordonnés, domaine enum, unknown non universel ; support Runtime distinct. |
| LF-17 | Conditional Disable a une règle Default unique. | Reliquat « exactement un » remplacé par « au plus un » ; sélection/default/no-match restent distincts. |
| LF-18 | Largeurs des compteurs de boucles explicites. | Exigences Studio alignées sur i64 public ; pas d'import implicite de la règle I32 LabVIEW. |
| LF-19 | Politiques host/execution ne sont pas devinées. | Versions non prises en charge et champs execution_policy inconnus bloqués ; valeurs host connues contrôlées. Les extensions host restent opaques : leur validité/sémantique n'est pas certifiée. |
| LF-20 | Métadonnées descriptives correctement typées et champs inconnus préservés. | Contrôle Python des champs standard ; extension descriptive opaque préservée. Studio ne revendique pas une préservation générale des extensions. |
| LF-21 | Une dérivation ne certifie pas un sous-graphe en ignorant le reste. | Garde d'enveloppe et inventaires exacts récursifs ajoutés ; les quatre connexions internes de l'accumulateur sont aussi vérifiées. 156 tests Deriver passent. Les deux validateurs sémantiques ne sont pas fusionnés. |
| LF-22 | FIR de référence et IR canonique clairement distingués. | Documentation et test de frontière ajoutés ; adaptateur canonique non livré. |
| LF-23 | Tests schémas réellement exécutés en CI. | Dépendances communes déclarées, import jsonschema obligatoire, suppression des skips conditionnels ; tests WidgetValidator ajoutés au runner --include-pytest. |
| LF-24 | Exemples, statut et navigation concordent. | 47 dossiers distingués des 15 exemples du corridor runtime public ; anciens tableaux partiels explicités ; navigation officielle générée et vérifiée, 728 chemins de recherche et 19 entrées racine. |
| LF-25 | Classes et réalisations publiques/Studio concordent. | Écarts Cluster/Picture/Waveform identifiés ; consommateurs Button Python corrigés. Defaults Boolean Python résolus lors de la reprise du 6 septembre, test initial et onze régressions supplémentaires réussis ; aucune compatibilité globale annoncée. |

## Corrections et choix concrets

### Import Studio

Avant correction, `Examples/01_pure_addition/main.frog`, accepté par le validateur
de référence, était refusé par Studio pour frontPanel manquant. Ajouter ce panel
ne résolvait pas le problème : les nœuds interface_input/output étaient hors du
filtre conservé ; les ports publics ne sont pas représentés indépendamment des
widgets. Une ouverture plus permissive aurait pu détruire le programme à la sauvegarde.

La correction choisie est un refus explicite avant import/conversion quand le
modèle ne sait pas conserver le contenu exécutable, avec contrôles avant et après
aplatissement. Le writer reste draft revision 2. La garde privée est isolée dans
`src/FrogDocumentPublicImport.hpp` ; elle ne crée pas une deuxième définition
normative du langage. Le test existant `frog-document-revision-smoke` est enrichi.

### Fonctions déclaratives

Les neuf unaires concernés déclaraient `value` dans la projection privée contre
`in` dans le contrat public et le C++ : la projection suit maintenant `in`.
Les faux profils de réduction attribués à increment/decrement sont retirés ;
les profils existants vont à Add/Multiply Array Elements. Aucun ID de port C++
persisté n'est modifié.

Le contrôle compare ordre/direction/nombre des ports, TypeExpr, requirement,
connection policy, fallback lorsqu'il est déclaré, résolution, implémentation et
bornes variadiques. Dix-sept mutations doivent être rejetées. Le CSV historique
ne fournit pas une preuve complète de cardinalité, defaults ou TypeExpr arbitraire
contenant ses séparateurs : un export structuré et la génération commune restent
préférables à long terme. 119 projections ne couvrent pas les 239 opérations.

### Validation et dérivation

Le validateur pédagogique `validate_source` et le dériveur `fir_deriver` n'ont pas
le même sous-ensemble. Brancher brutalement le premier devant tout le second
rejetterait les exemples déjà pris en charge. La sonde initiale montrait qu'une
version 99.0 ou un nœud supplémentaire ignoré pouvait donner le même FIR que
l'addition valide. Les corrections bornées ferment ces passages sans
prétendre transformer les motifs en compilateur général. Chaque règle possède
un inventaire statique de nœuds/types/arêtes et de régions autorisées, vérifié
avant son exécution. L'accumulateur vérifie également ses quatre connexions
internes : une reconnexion à nombre de nœuds/arêtes constant ne doit pas produire
le calcul précédent. L'ordre et les IDs d'arêtes ne changent pas ce contrat.

Ce contrôle ne prouve pas l'équivalence sémantique de tous les câblages dans
toutes les règles. L'indépendance au nom de métadonnée est démontrée pour
l'addition ; les gardes héritées sur `metadata.name` des règles 06–16 restent
présentes et sont maintenant documentées et testées. Ce sont encore des profils
de référence bornés, pas une implémentation générale du langage.

Le précontrôle d'enveloppe consomme required/properties/versions du schéma public,
avec tests de parité. Les champs locaux restent des validations spécialisées.
Le lecteur commun distingue JSON invalide, dépassement de capacité et source
future non comprise. Une profondeur lisible par JSON mais excessive pour le
validateur donne aussi un diagnostic sans modification de la source chargée.
Aucun échec ne produit de programme validé partiel.

### Widgets et réalisations

Sur 12 réalisations communes, huit sont identiques octet pour octet. Enum/Ring
diffèrent dans le libellé Item 0/Item 1. Les autres écarts sont plus profonds :

- Cluster public : classe commune et composition ; Studio : projection réduite
  control/indicator. Il faut un adaptateur et des tests de composition/round-trip.
- Picture : public image/control+indicator/viewport contre Studio indicateur
  matrice U32/affichage 1:1. Choisir un profil explicite avant migration des valeurs.
- Waveform : classe/réalisation locales Studio ; leur enveloppe n'est pas celle
  du package de classe public. Le document public actuel annonce déjà les limites.

Ces contrats ne doivent pas être fusionnés par synchronisation brute de fichiers.

La suite Python élargie a aussi révélé un défaut réel de consommation du contrat
Button : `style.button_face` et les marqueurs `button_face` publics n'étaient pas
appliqués partout. Le passage des propriétés, la géométrie, les overlays et les
sélecteurs sont corrigés, avec un adaptateur borné pour les anciens skins `face`.
Le SVG de l'indicateur Boolean associé est maintenant effectivement fourni au
renderer. Les tests obsolètes sont distingués des défauts réels grâce à
l'historique Git, détaillé dans l'annexe documentaire.

La passe initiale laissait un échec : les defaults typographiques de
`boolean.default.wfrog` n'étaient pas hérités quand l'instance les omettait.
Le renderer utilisait 18/18 px au lieu des 12/13 px publics. La reprise du
6 septembre décrite ci-dessous ferme ce défaut par résolution des manifests
avant les surcharges de package puis d'instance. Les tests de chaîne HTML ne
constituent pas une vérification visuelle CSS/JavaScript en navigateur.

## Vérifications de la passe initiale

Les chiffres Studio ci-dessous restent historiques. La livraison native
ultérieure et ses contrôles sont consignés dans le
[bilan de modularisation et d'optimisation](../../FROG-STUDIO/docs/source-optimization-follow-up-2026-09-06.md).

- Studio canonique **0.0.1.245**, compilation `all`, liaison et signature valides ;
  le raccourci du Bureau pointe vers `build-nmake-verify/frog-engine-shell.exe`.
  Aucun second exécutable Studio n'a été créé.
- Analyse statique ciblée `FrogDocument.cpp` et sa garde d'import : clang-tidy
  passe après retrait de deux concaténations temporaires inutiles. L'analyse
  statique globale de la passe précédente n'est pas présentée comme relancée ici.
- CTest hors UI/analysis/coverage : **115 réussites sur 117** sur le build final
  0.0.1.245, en 97,66 s. Les deux échecs préexistants sont
  `frog-studio-architecture-budget-contract` (20 fichiers hors budget) et
  `frog-studio-front-panel-ownership-contract`. Les seuils n'ont pas été relevés.
  Catalogue, empreintes et index d'architecture sont régénérés et vérifiés :
  427 fichiers, 5 886 fonctions compilateur, 239 157 lignes, aucun index périmé.
  Les tests directs de révision et limites des documents passent également.
- Suite Python complète `pytest Implementations/Reference` : **502 réussites,
  1 échec, aucun test ignoré**, en 25,66 s. L'échec restant est
  `Runtime/python/tests/test_runtime_ui_slice10.py:91`, l'héritage Boolean décrit
  ci-dessus. Les 48 sources d'exemples passent le contrôle JSON strict/schéma
  racine ; les 156 tests Deriver passent, dont les références 01–16 API et CLI.
- Pipeline standard `check_reference_workspace.py` : **réussi**, Examples 01–15,
  en 6,54 s. Validation de 20 familles `.wfrog` : 0 erreur, 1 avertissement
  préexistant sur `Picture.placement_bounds` absent du document de classe.
  Ce succès du pipeline n'annule ni l'échec pytest UI ni les écarts de formats.
- Ponts natifs Python/LLVM 05–15 : **11 tests réussis**, bibliothèques temporaires
  compilées avec la chaîne WinLibs locale. Ces tests avaient d'abord été ignorés
  faute de compilateur visible dans le PATH du processus Python ; ils ont été
  relancés avec la chaîne explicitement configurée.
- Navigation officielle générée/vérifiée ; **47 liens locaux de 7 documents
  contrôlés, aucune cible manquante**. Les 47 dossiers d'exemples ne sont pas
  présentés comme 47 corridors Runtime implémentés.

Les limites restent explicites : pas de certification de tous les backends,
de l'IR canonique complet, des performances UI ou d'un import/export universel.

Commandes reproductibles depuis la racine FROG, avec Python 3.11 et la chaîne
native visible dans le PATH du processus pour les ponts dynamiques :

```text
python -m pip install -r Implementations/Reference/requirements-test.txt
python -m pytest Implementations/Reference -q
python Implementations/Reference/check_reference_workspace.py --include-pytest
pwsh -File .github/scripts/verify-pages-navigation.ps1
```

Depuis `FROG-STUDIO/native-portable-engine`, après la compilation par le wrapper
canonique et la génération officielle du catalogue :

```text
ctest --test-dir build-nmake-verify -LE "^(ui|analysis|coverage)$" --output-on-failure -j 1
```

Ces commandes ne sont pas une demande de publication Git. Les nouveaux fichiers
et modifications doivent être versionnés avec leurs dépendances locales avant
publication ; une exécution locale réussie ne prouve pas une CI distante verte.

## Suite ordonnée

1. **Defaults `.wfrog` de réalisation** : correction Boolean Python terminée à
   la reprise du 6 septembre ; généralisation aux autres familles à traiter
   selon leurs contrats respectifs.
2. **Modèle source public indépendant de l'IHM** : interfaces, références,
   régions et extensions préservables ; tests sur source pure sans front panel.
3. **Lecteur/writer canonique transactionnel** : golden files inter-outils,
   migration explicite depuis draft, conservation de l'original et diagnostics.
4. **Registre commun de contrats et capacités** : classes, fonctions, structures,
   champs complets et projections générées ; statuts read/preserve/edit/validate/
   lower/execute séparés, pas un simple booléen « supported ».
5. **Validation sémantique et FIR unifiés** : mêmes diagnostics et type resolver,
   fermeture de chaque profil, attribution source et adaptateur vers l'IR public.
6. **Profils Cluster/Picture/Waveform/Variant** : source, packages, sémantique,
   runtime et migrations documentés ensemble avant d'annoncer leur portabilité.

La bonne convergence n'est pas de fusionner les dépôts ou de renommer toutes les
structures. C'est de rendre chaque contrat unique, chaque compatibilité prouvée,
chaque limitation visible et chaque évolution migrable sans perdre le travail.

## Reprise du 6 septembre 2026 : defaults Boolean

Le défaut LF-25 est corrigé dans les consommateurs Python Boolean et Button.
`boolean_runtime.merge_boolean_realization_defaults` charge les manifests
déclarés par `realization_refs`, filtre leurs `target_classes`, puis réutilise
la fusion existante par classe et rôle. L'ordre est explicite : defaults du
manifest, defaults locaux du package, propriétés/visuel de l'instance. Les
valeurs explicites `false` et `0` restent prioritaires. Les documents source,
packages et manifests ne sont pas modifiés. Aucune taille de police publique
n'est recopiée dans le renderer.

La résolution intervient une fois à l'initialisation du runtime ; les rendus
et événements suivants réutilisent les propriétés résolues. La lecture stricte
commune produit une erreur si un manifest déclaré manque ou contient un JSON
ambigu. La portée reste les deux classes Boolean et les références directes
des packages d'exemples, exprimées depuis la racine du dépôt. Les defaults
Button, notamment les alias historiques `style.face`, conservent leur chemin
existant. Ce lecteur borné ne résout pas un graphe général de dépendances de
packages ni une bibliothèque arbitraire de widgets.

Preuves locales de cette reprise, avec Python 3.11 et les dépendances déjà
présentes dans `Source/.tools/frog-language-audit-python` :

- Tests ciblés `test_boolean_realization_defaults.py`, `test_runtime_ui_slice06.py`
  et `test_runtime_ui_slice10.py` : **22 réussites**, en 0,84 s. Le test initial
  confirme désormais les 12/13 px publics. Les onze régressions ajoutées
  couvrent les trois niveaux de priorité, classe/rôle, préservation des sources,
  valeurs explicites, chargement unique et diagnostics des manifests invalides,
  y compris les sélecteurs `class_id` / `target_class` non textuels.
- Passe finale unique `pytest Implementations/Reference -q` : **514 réussites,
  aucun échec et aucun test ignoré**, en **25,79 s**, avec
  `Source/.tools/WinLibs/mingw64/bin` dans les variables PATH/Path du processus
  Python. Ce résultat inclut les onze ponts dynamiques LLVM 05–15 et remplace
  les résultats fragmentés des premiers passages.
- `python Implementations/Reference/check_reference_workspace.py` : **réussi**,
  Examples 01–15, en 8,34 s. Validation des 20 familles `.wfrog` : 0 erreur,
  l'avertissement préexistant `Picture.placement_bounds` reste présent.
- Navigation officielle régénérée et vérifiée après l'ajout du fichier de tests :
  **729 chemins de recherche**. `git diff --check` réussit sur les modifications
  locales.

Le premier lancement ciblé avait rencontré le répertoire temporaire global
pytest non accessible ; les exécutions rapportées utilisent un nouveau
`--basetemp` sous `.pytest_tmp` dans FROG. Le premier essai de suite complète
manquait aussi `jsonschema` dans le Python utilisateur : les dépendances locales
existantes ont été ajoutées au `PYTHONPATH`, sans installer de dépendance globale.
Sous cet hôte Windows, le PATH de clang a dû être normalisé dans Python pour
les tests natifs ; les tests ignorés n'ont pas été comptés comme des succès.

Les autres limites de convergence demeurent : modèle Studio indépendant de
l'IHM, writer public transactionnel, adaptateur FIR vers l'IR canonique,
registre partagé de capacités et profils Cluster/Picture/Waveform. Cette
correction ne modifie aucun artefact Studio et ne remplace pas une vérification
visuelle CSS/JavaScript en navigateur. Les preuves Studio de la section
précédente restent celles de la passe initiale, datées par leur version de build.
