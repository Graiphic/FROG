# FROG Frontend — candidate locale API 3

Ce répertoire contient le cœur Frontend extrait des propriétaires sémantiques de Studio, sa bibliothèque à interface C et sa CLI autonome. Il lit les octets `frog.source` et les documents `frog.document.draft` explicitement déclarés, résout leurs dépendances fournies par l’hôte, valide le langage et produit la FIR avec diagnostics, correspondances et provenance. Les producteurs ordinaires, structurés, composés et Event partagent ce propriétaire. La lecture et les migrations sémantiques sans mutation de l’original sont communes ; les propositions de réparation, leur prévisualisation et leur application transactionnelle restent privées à Studio.

**Candidate locale `0.2.0-local`, API 3.** Studio et la CLI utilisent les mêmes lecteurs, règles de validation et producteurs FIR. Les résultats sont attribués à un instantané précis : les six CTest et 6 614 contrôles de la première qualification autonome portent sur `c640d266a4de9d9581e6394d80d7152ce3d91d64278b960ab6777a524a056a62`, pas automatiquement sur les modifications suivantes. Chaque kit final possède son manifeste et un reçu de qualification distinct. Il ne s’agit pas d’une release publiée, d’une qualification de toute la palette ou d’un Windows propre. Voir le [profil](docs/candidate-profile.md) et l’[architecture commune et procédure de maintenance](docs/architecture-and-maintenance.md).

## Contrat public et versions

Le consommateur utilise [`frog/frontend.h`](snapshot/api/frog/frontend.h), installé sous `include/frog/frontend.h`, et la cible `Frog::frog-frontend`. Les en-têtes `include/frog_native_engine`, les propriétaires C++ et `runOperation` sont des détails d'implémentation partagés avec Studio, sans ABI C++ publique garantie.

| Axe | État de cette candidate |
| --- | --- |
| Logiciel | `0.2.0-local` ; commit de base et SHA256 de contenu dans describe/provenance |
| ABI C | `3` ; tailles exactes des structures, champs réservés à zéro |
| Source public | `frog.source`, version `0.1` |
| Documents historiques | `frog.document.draft`, révisions explicites `0` à `9` |
| FIR | `frog.execution_ir`, version `0.1` |
| Résultats JSON | Diagnostics `0.2`, contexte/provenance `0.3`, capacités `0.2`, maps `0.1` |
| Profils sémantiques | Identités et révisions propres, conservées dans la FIR/provenance |

L'API 3 exige les identités Source et FIR explicites et indépendantes. `describe` expose les formats exacts, les principaux profils et les plafonds. Sa liste `profiles_exhaustive: false` ne constitue pas un catalogue exhaustif de fonctions. Les versions ABI, logiciel, Source, FIR, profils et protocole bridge ne se remplacent pas entre elles. Une FIR admise doit encore être admise par sa réalisation cible.

## Construire et installer sans Studio

Prérequis : C++20, CMake 3.24 ou ultérieur et une chaîne C/C++ configurée. Le kit contient ses snapshots de déclarations et le décodeur Zstandard 1.5.7 sous licence BSD ; il ne recherche aucun checkout privé Studio/Runtime, LLVM ou fournisseur numérique. La reconstruction autonome de phase A a utilisé Windows x64 et MSVC 14.44/19.44 (compilateur `19.44.35228`). Une autre plateforme ou une machine Windows propre ne sont pas déclarées qualifiées ici. Python est nécessaire aux tests CLI lorsqu'ils sont activés.

Depuis la racine **FROG** du kit, dans un environnement de compilation préparé et avec un chemin de build court :

```powershell
cmake -S Implementations/Frontend -B b -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TRY_COMPILE_CONFIGURATION=Release -DFROG_FRONTEND_BUILD_TESTS=ON
cmake --build b --parallel 6 --target frog-frontend-cli frog-frontend-c-consumer frog-frontend-integration frog-frontend-draft-integration frog-frontend-draft-dependency-smoke
ctest --test-dir b --output-on-failure -R '^frog-frontend-(c-consumer|cli-scalar|cli-type-definitions|integration|draft-integration|draft-dependency-smoke)$'
cmake --install b --prefix installed
```

La cible `frog-frontend-cli` produit `frog-frontend` (`frog-frontend.exe` sur Windows). La bibliothèque installée est statique dans ce build. Choisir un runtime C/C++ cohérent pour toutes les cibles et le consommateur ; un kit MSVC peut fournir `-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded`. Aucun choix de CRT universel n'est imposé par l'API.

Pour vérifier un consommateur installé, placer une copie de `tests/frontend_c_consumer.c` dans un projet externe avec ce `CMakeLists.txt` :

```cmake
cmake_minimum_required(VERSION 3.24)
project(FrontendConsumer LANGUAGES C CXX)
find_package(FrogFrontend CONFIG REQUIRED)
add_executable(frontend-consumer frontend_c_consumer.c)
set_target_properties(frontend-consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(frontend-consumer PRIVATE Frog::frog-frontend)
```

Configurer avec `-DCMAKE_PREFIX_PATH=<chemin-absolu-vers-installed>`, construire puis exécuter `frontend-consumer`. Il inclut seulement `<frog/frontend.h>` et lie les bibliothèques de ce préfixe. Ce parcours a réussi pour l’instantané de phase A ci-dessus ; les empreintes des bibliothèques, de la CLI et du consommateur figurent dans le reçu `phase-a-result.json`. La recette ne réattribue pas ce résultat à une nouvelle capture. Dans le workspace Studio, toute compilation reste coordonnée par son wrapper et son verrou canoniques.

## Utiliser la CLI

Exemple Ninja/Windows depuis la racine FROG ; adapter le suffixe `.exe` sur une autre plateforme :

```powershell
./b/frog-frontend.exe describe --descriptor-output descriptor.json
./b/frog-frontend.exe validate --source Implementations/Frontend/tests/fixtures/pure-add.frog --source-uri fixture:/add.frog --document-id source:add --fir-document-id fir:add --source-format frog.source --source-version 0.1 --fir-version 0.1 --diagnostics-output diagnostics.json --source-maps-output maps.json --provenance-output provenance.json
./b/frog-frontend.exe derive --source Implementations/Frontend/tests/fixtures/pure-add.frog --source-uri fixture:/add.frog --document-id source:add --fir-document-id fir:add --source-format frog.source --source-version 0.1 --fir-version 0.1 --output result.fir.json --diagnostics-output diagnostics.json --source-maps-output maps.json --provenance-output provenance.json
```

`validate` utilise les mêmes producteurs et contrôles de scellement que `derive`, mais ne retourne pas le document FIR. `derive` exige `--output`; `validate` le refuse. `describe` ne prend aucune Source, contexte ou dépendance. Il écrit son descripteur sur stdout ; les deux autres opérations écrivent leur tableau de diagnostics sur stdout. Les sorties optionnelles sont des documents JSON distincts.

Options supplémentaires : `--semantic-context FILE`, `--dependencies LOCK.json`, et `--max-source-bytes`, `--max-dependency-bytes`, `--max-total-dependency-bytes`, `--max-result-bytes`, `--max-nodes`, `--max-connections`, `--max-nesting-depth`, `--max-dependencies`, `--max-diagnostics`. Les nombres sont des entiers décimaux non signés. Zéro sélectionne le défaut ; une valeur supérieure ne relève pas le plafond du producteur. Ctrl+C demande une annulation coopérative. La CLI actuelle n'a pas de sortie `--resolved-source-output` ; la projection complète est accessible par `resolved_source_json` dans l'API C.

Codes de sortie : `0` succès, `2` programme refusé, `3` format/profil non pris en charge, `4` annulation, `5` limite/ressource, `64` argument ou erreur de l'hôte CLI/API. Un refus de dérivation ne remplace pas le dernier FIR valide. Les écritures se font par fichiers complets ; l'ensemble des fichiers auxiliaires n'est pas une transaction atomique unique.

## Dépendances explicites et profil fini

Le cœur ne lit ni disque, réseau, registre, variables d'environnement, PATH ou bibliothèques installées. Les octets et métadonnées sont fournis par l'appelant. Le résolveur retourne une capture immuable ; ses vues sont libérées avant le retour de l'opération. Seul l'hôte CLI lit les fichiers explicitement sélectionnés et les chemins de son lock.

`frog.frontend.resolution@1` conserve son contrat : fragments `TYPE_DEFINITION=2` dans de vrais champs `studio_type_schema`, sans contexte ConditionalDisable non vide. Le nouveau `frog.frontend.resolution@2` décrit explicitement les symboles `built_in`, `project` et `target` utilisés pour préparer les drafts ; aucune valeur n’est lue implicitement depuis Windows ou un backend. Les dépendances SubFROG `SOURCE=1` et TypeDef `TYPE_DEFINITION=2` des drafts sont résolues depuis leurs références enregistrées par le même callback, avec octets immuables et empreintes vérifiées. Les contrats bibliothèque/profil ne deviennent pas réalisés par cette extension. Le [contrat de résolution](frontend-resolution.md) distingue ces parcours, leurs enveloppes et leurs limites.

```powershell
./b/frog-frontend.exe derive --source Implementations/Frontend/examples/type-definition/main.frog --source-uri fixture:/nominal.frog --document-id source:nominal --fir-document-id fir:nominal --source-format frog.source --source-version 0.1 --fir-version 0.1 --semantic-context Implementations/Frontend/examples/type-definition/context.json --dependencies Implementations/Frontend/examples/type-definition/dependencies.json --output nominal.fir.json
```

Les fixtures identifiées couvrent addition scalaire, I64/U64 exacts, Record, Map/Array/Cluster/Enum imbriqués, Event et références conservées. Les régions masquées restent contrôlées. Les DTO de widgets et diagrammes, codecs et résolveurs sont les propriétaires neutres extraits ; ils ne contiennent ni Canvas, ni contrôleur, ni état de récupération privée. Ce corpus fini ne promet pas toutes les combinaisons de types, structures ou fonctions de la palette, ni leur exécution par un backend. Les contrats de durée de vie, concurrence, diagnostics et les limites résiduelles figurent dans [candidate-profile.md](docs/candidate-profile.md).

## Provenance, pin et licence

`source-origin.json` conserve les captures d'origine et les déclarations snapshot. `cmake/FrontendInputs.cmake` calcule l'identité actuelle ; Studio la compare à son pin logiciel/API/contenu. `frontend-content-identity.txt` identifie les entrées du build. Le SHA de contenu Frontend et le SHA de son exécutable sont deux preuves différentes. Les overlays locaux ne sont pas reconstitués par le seul commit de base.

**Apache-2.0 est accepté pour les fichiers originaux Graiphic sélectionnés pour le Frontend commun, sa CLI et leurs contrats/exemples/tests**, y compris les extractions nécessaires de Studio ou Runtime. La [formalisation publiée du 1 novembre 2026](https://github.com/Graiphic/FROG/blob/85a90ebe2c2a78cc4f42f7efce1c8ca990df4d26/sdk/ecosystem/docs/frontend-licensing.md), consignée localement dans `FROG-Context/Decisions/frog-common-frontend-apache2.md`, maintient les licences/notices des tiers et exclut le reste de Studio, ses réparations privées, le reste du Runtime et les recettes d'optimisation.

Les mentions `NOASSERTION` de l'[inventaire E2](docs/license-inventory.json) et de ses [observations](docs/provenance-and-licensing.md) décrivent l'état historique avant cette décision ; elles ne remettent pas en attente le choix Apache des fichiers Graiphic concernés. L'intégrateur doit rattacher l'inventaire de la candidate à cet accord, conserver les hashes d'origine/actuels et les notices tierces du contenu exact diffusé. La décision de licence ne vaut ni qualification du nouveau paquet ni autorisation de publier son code maintenant.

Le décodeur documentaire Zstandard et ses composants FSE/HUF/xxHash restent tiers. Voir [sa provenance et sa licence BSD](third_party/zstd/UPSTREAM.md) ; ils ne sont pas relicenciés sous Apache-2.0. Leur présence ne qualifie ni le runtime ni la lecture de tous les fichiers historiques.
