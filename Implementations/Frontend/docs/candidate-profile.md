# Profil de la candidate Frontend commune

Candidate locale `0.2.0-local` / API 3, intégrée à Studio par liaison statique et à la CLI. Source/FIR restent `0.1` ; les drafts déclarent leur révision exacte `0` à `9`. Les qualifications portent sur des empreintes précises et restent distinctes des recettes ci-dessous. L’instantané autonome initial `c640d266a4de9d9581e6394d80d7152ce3d91d64278b960ab6777a524a056a62` a été reconstruit, installé et appelé par un consommateur C externe (six CTest, 6 614 contrôles). Les captures ultérieures nécessitent leur propre reçu, joint à leur inventaire ; ce succès initial ne qualifie pas leurs nouveaux octets ni un exécutable Studio ultérieur.

## Propriétaire et frontière

`src/FrontendApi.cpp` possède le contrat C et les résultats immuables. `src/FrontendOperation.cpp` possède capture, résolution explicite, budgets, diagnostics et orchestration des producteurs réels `CanonicalExecutionIr` et `CanonicalEventExecutionIr`. Le premier conserve les chemins structurés et composés existants. Studio et la CLI consomment ce même propriétaire.

La bibliothèque reçoit des octets, identités, options et callbacks, sans accès implicite au disque, réseau, registre, projet Studio, environnement ou PATH. La CLI est un hôte fichier explicite de l'API C. La lecture et la migration sémantique des drafts appartiennent au Frontend commun. `FrogDraftData`, `FrogWidgetDataView`, les DTO de widgets et leurs codecs portent les données nécessaires sans Canvas ni `LoadedFrogDocument`. Les façades privées réutilisent ces propriétaires ; les contrôleurs et l’état de récupération restent dans Studio. Les propositions et l’application des réparations restent privées à Studio ; l’admission cible, les fournisseurs, la compilation et l’exécution restent dans leurs propriétaires distincts. L'interface installée est `frog/frontend.h` et `Frog::frog-frontend` ; les headers C++ internes et les propriétaires typés retenus par Studio ne sont pas une seconde ABI publique.

## Opérations et versions

| Opération | Travail réel | Résultat admis |
| --- | --- | --- |
| `describe` | Identité de l'implémentation, opérations API, formats, profils principaux et plafonds | Descripteur complet, sans Source ni FIR |
| `validate` | Même résolution, producteur et scellement que derive | Diagnostics/maps/provenance ; aucun document FIR retourné |
| `derive` | Même travail puis publication du document admis | FIR complète, diagnostics/maps/provenance |

Chaque demande validate/derive donne les octets authored immuables, URI explicite, `source_document_id`, `fir_document_id`, format/version Source et version FIR exacte. Les identités Source et FIR restent distinctes. Un nom d'affichage ou chemin de sortie ne les remplace pas. Les URI ne déclenchent aucun accès implicite ; les références relatives sont transmises au résolveur avec leur URI propriétaire et JSON Pointer.

| Identité/version | Valeur ou rôle |
| --- | --- |
| API C | `3`, version et tailles exactes, réservés à zéro |
| Logiciel | `0.2.0-local`, indépendamment de l'ABI |
| `frontend.commit` | Révision de base enregistrée à la compilation, sans les overlays |
| `frontend.sha256` | Contenu selon `FrontendInputs.cmake`, distinct du hash PE/archive |
| Source public / FIR | `frog.source/0.1` / `frog.execution_ir/0.1` |
| Documents historiques | `frog.document.draft`, révisions `0` à `9` explicitement déclarées |
| Diagnostics | Tableau de `frog.diagnostic`, version `0.2` |
| Provenance | `frog.frontend.context`, version de schéma `0.3` |
| Capacités / maps | `frog.frontend.capabilities/0.2` / `frog.frontend.source_maps/0.1` |
| Profils et bridge | Révisions sémantiques propres ; protocole aval distinct |

Le descripteur annonce les principaux profils `frog.canonical.widget-f64-add.v1`, `frog.canonical.typed-structures.v1`, `frog.studio.composed-program@1`, `frog.canonical.event-scalar.v1`, `frog.canonical.event-captured.v1`, `frog.frontend.resolution@1` et `frog.frontend.resolution@2`. `profiles_exhaustive: false` interdit de présenter cette liste comme une matrice exhaustive d'opérateurs ou de réalisations. Sur succès, provenance.profiles et provenance.operations proviennent de la FIR réellement produite. Le descripteur conserve `native_execution_claim: false` et la diffusion locale.

## Périmètre fini de langage

Le corpus `tests/fixtures/provenance.json` conserve hash et origine de chaque Source :

| Fixture | Contrat exercé |
| --- | --- |
| `pure-add.frog` | Contrôles/indicateur et addition f64 par Source public |
| `record-exact.frog` | Record imbriqué, I64/U64 exacts et schémas conservés |
| `nominal-nested.frog` | Map/Array/Cluster/Enum complets ; identités et valeurs non réduites aux porteurs physiques |
| `event-values.frog` | Régions Event et valeurs capturées typées, régions masquées comprises |
| `pane-unbundle.frog` | Références de panneau/contrôle et données Event conservées |
| `event-product.frog` | Graphe Event de production via les mêmes frontières publiques |

Les six CTest de phase A exécutés sont `frog-frontend-c-consumer`, `frog-frontend-cli-scalar`, `frog-frontend-cli-type-definitions`, `frog-frontend-integration`, `frog-frontend-draft-integration` et `frog-frontend-draft-dependency-smoke`. Ils couvrent notamment la substitution TypeDef, les refus de Source malformée/future, références/cycles/conflits, budgets, annulation, leases, concurrence et durée de vie des résultats. Les tests draft exercent types exacts, conditions et dépendances ; ils n’exécutent ni LLVM ni Runtime. Les 6 614 contrôles se rapportent à ce corpus fini, pas à toutes les fonctions.

Les fichiers Studio `regressions/native-qmh1-joined-cleanup.frog` et `regressions/native-state-machine-calculate3.frog` ont aussi passé validate/derive dans l’API/CLI autonome, avec octets d’entrée inchangés. Trois refus de fixtures historiques sont conservés et attribués aux gardes préexistants : ancien `class_ref` de cluster sans adaptation, constante scalaire `u8` hors du profil scalaire choisi, Unbundle par nom dépourvu de chemins d’identifiants stables. La comparaison aux sources antérieures n’est pas un rejeu d’un ancien binaire.

Les schémas admis conservent champs, noms, identités nominales, enums, rangs et enfants. Les I64/U64 ne passent pas par un `double`. Les structures conservent propriétaires de régions, terminaux, connexions et attribution. Une région masquée n'est pas dispensée de validation. L'acceptation reste celle du producteur et du profil réel ; ce corpus ne généralise pas son résultat à toutes les combinaisons de type/structure/opération de la palette.

E7 réutilise ce Frontend pour dériver ses Sources scalaires, I64/U64, Array32 et For statique avant un peer C17. Le profil fini du peer, ses contraintes de valeurs et ses refus d'Enum/opération ne sont pas les règles générales du Frontend. Event, références ou opérations présentes dans une FIR ne promettent pas leur exécution par un peer donné.

## Résolution TypeDef implémentée

Sur le parcours Source public `0.1`, un contexte vide vaut `{}`, sans réglage hérité. Le profil initial utilise l’enveloppe fermée `frog.frontend.resolution@1` décrite dans [frontend-resolution.md](../frontend-resolution.md). Ses entrées `{kind:2, source_pointer:...}` visent un vrai `studio_type_schema` ou son sous-schéma. Champs inconnus, cibles arbitraires, pointeurs recouvrants et références contradictoires sont refusés.

L'objet authored `$ref` déclare référence, identité, version, SHA256 et pointeur de fragment. Le callback retourne un document `frog.frontend.type-definitions@1` avec sa propre identité/version et ses octets. Le Frontend calcule le hash, vérifie métadonnées et contenu, développe les références dans une projection détachée puis appelle le même producteur. Les répétitions équivalentes partagent une capture dans la requête ; URI/identités/contenus contradictoires et cycles sont refusés. Aucun cache de dépendance implicite ne survit à une nouvelle opération.

Resolve et release sont définis ensemble, ou absents ensemble. Après chaque retour normal de resolve, release est appelé exactement une fois pour tous les statuts, avant le retour de l'opération. Les captures détenues restent indépendantes des vues prêtées. Les callbacks sont synchrones sur le thread appelant ; aucune exception C++ ne doit traverser l'ABI C.

La CLI charge à l'avance les snapshots d'un lock `{"format":"frog.frontend.dependencies","version":"1","dependencies":[...]}`. Une entrée contient kind, reference, source_uri, identity, version et path, avec requesting_source_uri optionnel. Un path relatif est résolu par rapport au lock, pas par le cœur. La sélection kind/reference/propriétaire doit être non ambiguë. Le lock ne peut pas accorder au langage un genre de dépendance qu'il ne réalise pas.

Le contrat `@1` ci-dessus reste inchangé, y compris le refus d’un `conditional_disable` non vide. Le nouveau contrat `frog.frontend.resolution@2` porte les cartes explicites de symboles `built_in`, `project`, `target` pour le chemin draft. Le Frontend lit les drafts `0..9`, résout leurs liens TypeDef et appels SubFROG par captures fournies au callback, puis utilise les mêmes producteurs FIR. Il conserve le document original et les identités ; aucune proposition de réparation n’est appliquée implicitement. Les parcours ciblés de cette implémentation ont été exécutés en phase A ; les preuves des parcours Studio et de chaque nouvelle capture sont conservées dans leurs reçus propres.

Pour les drafts, SOURCE=1 et TYPE_DEFINITION=2 sont des demandes effectives du résolveur, issues des références du document et localisées à leur URI/pointeur. LIBRARY_CONTRACT=3 et PROFILE_CONTRACT=4 ne sont pas réalisés. La lecture d’une révision reconnue ne garantit ni sa validité sémantique ni son admission par un backend ; une copie de récupération sans diagramme exploitable peut rester refusée. Voir [le contrat v2](../frontend-resolution.md).

## Résultats, erreurs et publication

`FROG_FRONTEND_OK` signifie que l'appel C a retourné un résultat, même si le programme est refusé. `outcome` distingue SUCCESS, REJECTED, UNSUPPORTED, CANCELLED et LIMIT_EXCEEDED. Un statut C non-OK — argument, ABI, BUSY, mémoire ou erreur interne — ne fournit aucun résultat consommable. Les arguments invalides ne deviennent pas artificiellement une erreur du programme.

Les diagnostics portent code stable, étape, sévérité, catégorie et emplacement connu. Capacité Frontend absente, erreur sémantique, intégrité et dépendance restent distinctes. Les messages humains ne sont pas une API de classification. Un emplacement FIR `/unit/...` est `derived_pointer`, sans faux pointeur authored. Les emplacements liés sont optionnels et ne sont pas inventés.

Chaque résultat contient diagnostics, maps et provenance complets. Un refus/une annulation/une limite ne renvoie aucun FIR exécutable ni projection partielle. La provenance conserve les faits déjà connus ; avant capture, le hash Source peut être vide. Describe ne fabrique pas de Source/FIR demandé.

Après résolution admise, `resolved_source_json` fournit seulement si différente la **Source complète projetée** utilisée par le producteur, sur succès validate/derive. La provenance distingue `source.sha256`, `projected_source_sha256` et transformations authored→dépendance. Le sceau source du FIR reste celui des octets dérivés. La CLI n'expose pas actuellement ce buffer par une option dédiée.

La CLI publie diagnostics/maps/provenance séparément, puis le FIR uniquement sur succès derive. Un refus ou une erreur auxiliaire avant cette publication laisse le dernier FIR valide intact. Les fichiers ne forment pas une transaction unique. Les options et codes de sortie sont dans le [README](../README.md).

## Limites, annulation et concurrence

Zéro sélectionne le défaut ; une demande non nulle ne peut que réduire le plafond effectif maximal. Ces budgets ne garantissent pas l'admission de toute combinaison de langage de cette taille.

| Budget maximal effectif | Valeur |
| --- | ---: |
| Source et contexte JSON, chacun | 8 Mio |
| Une dépendance / total des captures uniques | 8 Mio / 64 Mio |
| Résultats JSON cumulés, projection comprise | 32 Mio |
| FIR du producteur | 16 Mio |
| Nœuds / connexions Source et dépendances cumulés | 1 024 / 4 096 |
| Profondeur sémantique/résolution | 64 |
| Dépendances uniques / diagnostics | 256 / 256 |
| Visites d'expansion TypeDef | 4 096 |

Les octets sont contrôlés avant copies et scans des grands buffers. Les régions masquées participent aux comptes. Les contraintes du parseur, des codecs et producteurs restent actives. L'épuisement du budget diagnostics conserve un diagnostic terminal ; il ne transforme pas un refus en succès. Si aucun résultat complet ne tient, l'appel renvoie une erreur de ressource plutôt qu'un JSON tronqué.

L'annulation est coopérative : 0 continue, 1 annule ; une autre valeur est une défaillance du callback. Les polls entourent les unités bornées et le producteur synchrone, sans préemption au milieu de celui-ci. Un résolveur bloquant reste sous la responsabilité de l'hôte. Aucune latence maximale ou garantie temps réel n'est annoncée.

Entrées, identités et vues restent immuables jusqu'au retour synchrone. La configuration est copiée à la création du contexte. Un seul appel opère par contexte ; un appel simultané ou réentrant retourne BUSY. Des contextes indépendants peuvent travailler concurremment. Aucun worker caché n'est lancé.

Un résultat possède ses buffers et survit à son contexte. Ses vues sont valides jusqu'à `frog_frontend_result_destroy` et ne sont pas libérées/modifiées individuellement. `result_get` exige taille exacte et réservés à zéro. Détruire contexte ou résultat pendant leur usage viole le contrat appelant ; les fonctions destroy du même propriétaire libèrent leurs allocations.

## Historique et qualification de candidate

| Lot | Portée des preuves locales | Limite conservée |
| --- | --- | --- |
| E2 | Extraction et audit lexical des producteurs/adaptateurs | Audit source distinct d'un lien ; hashes d'origine distincts des hashes actuels |
| E3 | Bibliothèque/CLI API3, installation/consommateur C, parité des documents Studio/C ABI/CLI sur corpus borné | Résultats liés aux octets E3, sans suite Studio complète ni Windows propre |
| E5 | SDK bridge, wire indépendant et consommateurs source publics | Déclarations et fixtures de protocole distincts d'une exécution native |
| E7 | Reconstruction du kit public et peer C17, FIR communs, sorties et artefacts/commandes réels | Profil C fini et chaîne locale ; aucune couverture universelle de palette ou de Windows propre |
| E8 | Contrôles pin/identités, frontière publique, reçus et qualification ciblée | Préserver les échecs voisins et contrôles non exécutés ; rattacher les résultats aux dernières empreintes |
| P28 phase A, 2 novembre | Kit autonome reconstruit, six CTest réussis, 6 614 contrôles, installation/consommateur C et drafts courants QMH/état | Contenu `c640d266a4de9d9581e6394d80d7152ce3d91d64278b960ab6777a524a056a62` uniquement ; pas de Windows propre, de nouvelle preuve mémoire ou d’exécution backend |
| P28 phase B | Raccordement Studio et parité sur les octets originaux en cours | Non qualifié ; aucun résultat d’application livrée revendiqué |

L'intégrateur conserve les preuves dans les artefacts locaux `modularisation-etape{2,3,5,7,8}-...` de Studio. Ces chemins internes ne sont pas des dépendances du kit public. La mention historique E3 de Studio `.738` ne désigne pas la version courante de la candidate.

Le reçu `artifacts/p28-implementation-2026-11-02/phase-a-result.json` conserve la qualification A : manifeste de kit `fed3baac219da2bc68c99b9d08e6560b73ca68c100e17cbfdfeb33d1e09f0102`, contenu Frontend et SHA des bibliothèques/CLI/consommateur installés. Il est conservé chez l’intégrateur et n’est pas une entrée du kit. Le rapport `public-phase-benchmark.json` reste une mesure attribuée à ce même instantané, sans conclusion de gain ; la mémoire n’a pas été mesurée.

Pour sceller la phase B : contenu exact mis à jour, SHA des bibliothèques/CLI et binaires Studio, identité retournée, pin Studio correspondant, versions de chaîne, audit et parités pertinentes. Réutiliser les tests publics Frontend existants et les nouveaux tests ciblés draft/dépendances, les modes de parité Studio, le test SDK de protocole et l’audit existant. Ne pas attribuer les réussites historiques à ces nouveaux octets sans vérification. Un succès de protocole ne qualifie pas un backend.

Depuis la racine FROG, ces commandes sont des recettes, pas des résultats nouveaux :

```text
python -B Implementations/Frontend/tools/audit_frontend_closure.py --root Implementations/Frontend --source src/FrontendApi.cpp --source src/FrontendOperation.cpp --source src/FrontendCli.cpp --output frontend-closure.json
python -B Implementations/Frontend/tools/audit_frontend_closure.py --root . --public-consumer --output public-consumer-closure.json
```

Le second contrôle exige les owners publics SDK/peer C présents. La fermeture lexicale ne remplace ni compilation ni inspection des imports directs/différés du binaire. Les dépendances OS chargées à l'exécution restent un prérequis séparé.

## Licence et diffusion

La [formalisation Apache-2.0 publiée le 1 novembre 2026](https://github.com/Graiphic/FROG/blob/85a90ebe2c2a78cc4f42f7efce1c8ca990df4d26/sdk/ecosystem/docs/frontend-licensing.md), consignée dans `FROG-Context/Decisions/frog-common-frontend-apache2.md`, accepte cette licence pour les fichiers originaux Graiphic sélectionnés pour le Frontend/CLI et leurs contrats/exemples/tests, extractions nécessaires comprises. Les origines tierces conservent licences et notices. Le reste de Studio, ses réparations privées, le reste du Runtime et les recettes d'optimisation sont exclus.

L'inventaire E2 `NOASSERTION` reste une preuve historique préalable à la décision, sans redemander le choix acquis. L'inventaire de diffusion doit relier les fichiers actuels à cet accord, conserver original_hash/current_hash et attribuer les contenus tiers éventuels. Un audit ou une mention SPDX ne remplace pas cette traçabilité du paquet exact.

La décision de licence est publiée séparément ; elle ne publie pas cette candidate ni son code. Aucune publication distante, qualification globale Studio, qualification Windows propre ou nouvelle version binaire n'est produite par cette mise à jour documentaire.

Le décodeur Zstandard 1.5.7 (FSE/HUF et xxHash inclus) est maintenant une dépendance statique de lecture documentaire. Il conserve sa licence BSD et ses notices ; aucune réalisation numérique tierce, LLVM ou runtime privé n’est embarquée pour cette lecture. Sa fermeture source et son lien dans le kit ont été exercés par la reconstruction autonome de phase A. Cela ne démontre ni la compatibilité de tous les documents historiques ni la qualification sur Windows propre. Les notices et la provenance du décodeur restent obligatoires dans chaque nouvelle capture.
