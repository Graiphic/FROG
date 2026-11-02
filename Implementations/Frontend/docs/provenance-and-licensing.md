# Provenance et licences — originaux Apache-2.0 et décodeur tiers BSD

Mise à jour du 2 novembre 2026. **Le choix Apache-2.0 est acquis pour les portions originales
Graiphic retenues dans ce Frontend et sa CLI**, y compris les fichiers sélectionnés
extraits de Studio et Runtime. La décision acceptée est conservée dans
`FROG-Context/Decisions/frog-common-frontend-apache2.md`. Cette déclaration ne
relicencie pas Studio ou Runtime en bloc. Les contenus tiers éventuels conservent
leurs droits ; aucun nouveau choix de licence n'est demandé pour les originaux.

[LICENSE](../LICENSE) reproduit exactement le texte Apache-2.0 de FROG, sans
inventer de titulaire ni d'année. [NOTICE](../NOTICE) précise le périmètre.
La déclaration par fichier figure dans [license-inventory.json](license-inventory.json). Son relevé v3 du 1 novembre est historique ; l’intégrateur doit le réconcilier avec l’extraction `0.2.0-local` et le décodeur tiers décrit ci-dessous. La présente mise à jour documentaire ne modifie ni API ni sémantique et ne déclare aucun nouveau résultat compilé.

## Captures et droits distincts

Le relevé historique de `source-origin.json` comportait 178 entrées ; ce chiffre ne décrit pas la nouvelle fermeture de `0.2.0-local`. Pour les 181 anciennes
entrées de l'inventaire v2, le v3 conserve `upstream`, `earlierOrigins`,
`original_hash`, sa base et les observations historiques complètes de licence,
y compris les `NOASSERTION`, hashes et `publicationRightsReviewed:false` alors
constatés. Un fichier déplacé peut évoluer ; sa capture n'est pas une exigence
d'immobilité. La parité d'un snapshot reste un contrôle séparé.

`NOASSERTION` décrivait l'absence de concession explicite observée à cette date.
La décision du 1 novembre apporte cette concession aux originaux sélectionnés ;
le déplacement dans FROG ne l'avait pas apportée. Ces constats historiques restent
lisibles et ne sont pas réécrits rétroactivement.

| Champ v3 | Sens |
| --- | --- |
| `original_hash` | Capture historique conservée ; null si absente. |
| `current_hash`, `current_bytes` | Octets lus dans le propriétaire actuel. |
| `capture_relation` | Capture identique, évolution ou parité du snapshot à examiner. |
| `historicalLicenseObservations` | Ancien constat par fichier, avec son empreinte. |
| `licenseEvidence`, `declaredLicenseScope` | Apache-2.0 pour les portions originales Graiphic, sans concession inventée pour un tiers. |
| `thirdPartyReviewFindingIds` | Points concrets de revue, distincts de la concession acquise. |

La provenance Runtime des contrats `frog/semantics/*` est conservée. Catalogues,
headers générés, fixtures, outils, CMake, exemples et docs ont leur entrée propre.
Le v3 recensait les fichiers présents au moment de sa capture ; les extractions ultérieures et leurs notices doivent figurer dans le relevé actuel avant diffusion.
L'inventaire s'inclut mais exclut son propre hash, qui serait cyclique. LICENSE,
NOTICE et cette note ont leurs hashes exacts ; leurs simples liens vers
l'inventaire ne forment pas une chaîne de hashes circulaire.

## Fermeture et kit réellement consultés

Le relevé historique du contrôle lexical `tools/audit_frontend_closure.py` portait sur les 45 unités d’alors et les trois adaptateurs API/Operation/CLI : **48 unités, 179 fichiers, 313 arêtes, 0 refus**. Il ne qualifie pas les nouveaux lecteurs, codecs, résolveurs ni le décodeur de `0.2.0-local`. Ses chemins,
empreintes et arêtes sont enregistrés dans le v3. Aucun rapport auxiliaire,
compilateur, linker ou test produit n'a été lancé. Ce contrôle ne remplace pas
l'analyse de symboles d'un binaire.

Le kit E3 a été consulté dans son archive historique, reliée à
`final-kit-receipt.json` ; la copie temporaire avait été supprimée. Le v3 distingue
les fichiers support provenant de FROG, leurs hashes d'archive et leurs octets
actuels. Les binaires installés, copies générées de qualification et preuves
historiques ne sont pas transformés en sources de la nouvelle release. Leurs
reçus restent historiques. Les fichiers support FROG relèvent de `FROG/LICENSE`
et des notices existantes ; la nouvelle décision ne licence pas d'autres
portions privées par extension. L'inventaire ne remplace pas un manifeste du
paquet candidat effectivement assemblé.

Le rafraîchissement final recense également l'exporteur durable
`tools/export_frontend_candidate.py`, son test `tests/test_candidate_export.py`
et `examples/installed-c-consumer/CMakeLists.txt`. La sélection courante est lue
par la fonction `source_selection` de cet exporteur ; le v3 en conserve les
hashes sans lancer la capture du kit. Les 56 fichiers support du kit E3 restent
un ensemble historique distinct, avec chaque ancien relevé préservé lorsqu'un
fichier public a évolué. Un hash de cette sélection n'est pas un reçu d'archive.

## Revue tierce ciblée

La candidate contient désormais le décodeur documentaire Zstandard 1.5.7 et ses composants FSE/HUF/xxHash. Le constat historique « sans Zstd » ne s’applique plus à ces sources. Les noyaux LLVM, XSF/Cephes/AMOS, libffi, stb et le runtime privé restent hors de cette extraction ; le Frontend transporte leurs contrats utiles sans embarquer leurs réalisations. Le compilateur et la bibliothèque standard relèvent de leur distribution distincte.

Le vendor existant choisit la licence BSD, dont le texte est conservé dans [LICENSE.txt](../third_party/zstd/LICENSE.txt), avec le texte GPL alternatif dans COPYING.txt. Les 35 fichiers initialement copiés ont été comparés octet pour octet au vendor local ; l’archive amont n’a pas été téléchargée ni revalidée lors de cette extraction. L’[UPSTREAM](../third_party/zstd/UPSTREAM.md) conserve la version, la provenance et le périmètre exact. Meta Platforms et Yann Collet restent attribués ; ces sources ne deviennent ni maison ni Apache-2.0.

Les sources distribuées conservent leurs notices. Pour une distribution binaire, la licence BSD, ses conditions et son avertissement doivent accompagner le package. L’inventaire actuel et l’installation doivent inclure ces textes et distinguer les dix unités C compilées du reste des headers copiés. La compilation et la fermeture binaire de ce nouveau kit restent à qualifier.

Trois constats historiques restent détaillés dans `thirdPartyReview` de l’inventaire ; l’entrée Zstandard actuelle doit les compléter :

1. **Catalogue documentaire** —
   `manifests/function-bindings/studio.function-bindings.v2.inc` contient
   1 384 URL vers NI, NIST DLMF, Microsoft et des publications mathématiques,
   ainsi que des descriptions de contrats. Aucun noyau tiers n’avait été identifié dans ce catalogue documentaire ; ce constat ne concerne pas le décodeur désormais ajouté. En
   revanche, ce relevé n'a pas comparé chaque phrase aux pages citées : la
   présence éventuelle de citations/adaptations textuelles substantielles reste
   à déterminer avant publication. La licence des portions originales Graiphic
   est confirmée ; un URL n'accorde pas les droits sur son contenu. Les
   références sont conservées et aucun texte tiers n'est déclaré Apache par
   supposition.
2. **SHA256** — `src/Sha256.cpp` contient une implémentation courte et les
   constantes standard, avec origine Studio capturée. Il ne cite pas
   d'implémentation tierce et aucun emprunt à une bibliothèque externe n'a été
   établi. Les constantes standard ne suffisent ni à prouver un emprunt ni à
   attribuer l'origine du texte de code ; aucun titulaire ou copyright upstream
   n'est inventé. La concession des originaux s'applique. Toute ascendance
   tierce retrouvée ultérieurement devra être ajoutée à l'inventaire.
3. **Calibration et fixtures** — `numeric_calibration_contracts.hpp` porte des
   identités `nist-its90` et des paramètres par défaut, sans les tables de calcul
   thermocouple du Runtime. Les six fixtures sont attribuées dans
   `tests/fixtures/provenance.json`, avec leurs hashes vérifiés. Ce constat ne
   transforme ni les métadonnées ni les fixtures en implémentation tierce, et
   n'attribue pas les codes Graiphic à NIST.

Cette recherche est bornée aux contenus retenus, commentaires, références,
captures et provenance du kit. Elle n'est pas une preuve exhaustive de tout
l'historique Git ni une comparaison contre tous les codes/textes publiés. Les
incertitudes concrètes sont séparées du choix de licence déjà accepté.

## Publication et qualification

Le candidat reste local. La décision de licence n'autorise pas un push de code
immédiat : la publication est différée jusqu'à la fin du chantier et à la
qualification de la release. Le v3 distingue concession des droits sur les
originaux, revue tierce, validation technique et autorisation de publication.
Les anciennes captures E2/E3 et les observations v2 peuvent mentionner l'état
`NOASSERTION` antérieur à la décision ; elles restent historiques. Le README
et `candidate-profile.md` courants décrivent désormais la concession Apache
acquise ; leurs octets `0.2.0-local` doivent être rattachés au relevé actuel, sans réattribuer les anciennes empreintes. L'ancien texte du README
ne doit donc pas être présenté comme son affirmation actuelle.

L'intégrateur rattache le contenu exact du paquet à cet inventaire, traite les points documentaires identifiés et le décodeur tiers, conserve LICENSE/NOTICE et les attributions
applicables, puis produit les reçus du candidat réellement reconstruit. Cette
tâche n'a lancé aucun build, test produit, téléchargement ou publication distante
et n'a modifié aucune source de production.
