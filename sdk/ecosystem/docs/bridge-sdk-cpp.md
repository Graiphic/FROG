# Utiliser le SDK Bridge C++ local

Le point d'entrée est [`frog/bridge.hpp`](../include/frog/bridge.hpp), un en-tête
C++20 qui ne dépend que de la bibliothèque standard. La cible CMake existante
est **`frog-bridge-protocol`**. Elle fournit le codec JSON, la négociation et
`Exchange` ; elle ne crée aucun processus et n'exécute aucun programme.
Le contrat binaire tiers reste l'IPC UTF-8, pas l'ABI C++ de cet en-tête.

La reconstruction locale E5 a utilisé une copie autonome SDK + Frontend,
sans entrée Studio, Runtime privé ou LLVM dans les dépendances de compilation.
Le consommateur ci-dessous et le test du protocole ont réussi (2/2). Preuves :
`FROG-STUDIO/native-portable-engine/artifacts/modularisation-etape5-bridges-2026-11-01/`.
Cette qualification utilise Windows/MSVC déjà installés ; elle ne qualifie ni
Windows propre, ni Linux, ni un bridge tiers réalisant effectivement un calcul.

## CMake et consommateur minimal

Il faut CMake 3.24 ou ultérieur et une chaîne C/C++20. L'intégration actuelle
utilise `add_subdirectory` ; aucun paquet installé `find_package(FrogBridge)`
n'est fourni par le CMake Bridge actuel. Dans le `CMakeLists.txt` du consommateur :

```cmake
cmake_minimum_required(VERSION 3.24)
if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
endif()
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded)
project(BridgeConsumer LANGUAGES C CXX)
include(CTest)
set(FROG_BRIDGE_SDK_ROOT "" CACHE PATH "Chemin du SDK sdk/ecosystem")
add_subdirectory("${FROG_BRIDGE_SDK_ROOT}" frog-bridge-sdk EXCLUDE_FROM_ALL)
add_executable(bridge-encode-describe main.cpp)
target_link_libraries(bridge-encode-describe PRIVATE frog-bridge-protocol)
add_test(NAME bridge-external-consumer COMMAND bridge-encode-describe "${CMAKE_BINARY_DIR}/describe.frame")
```

Exemple `main.cpp` à créer dans le projet consommateur. Il écrit une requête
`describe` correctement encadrée dans un fichier binaire ; il ne constitue pas
un hôte de processus ni une preuve d'exécution de cible.

```cpp
#include <frog/bridge.hpp>
#include <array>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 2) return 2;
    try {
        frog::bridge::Message request;
        request.identity = {"example.describe", "example.session", 1};
        const auto json = frog::bridge::encodeMessage(request);
        frog::bridge::Message decoded;
        frog::bridge::Error error;
        if (!frog::bridge::decodeMessage(json, decoded, error) ||
            !frog::bridge::validatePayload(decoded, error)) {
            std::cerr << error.code << ": " << error.message << '\n';
            return 3;
        }
        const auto length = static_cast<std::uint32_t>(json.size());
        const std::array<char, 4> header{
            static_cast<char>(length & 255),
            static_cast<char>((length >> 8) & 255),
            static_cast<char>((length >> 16) & 255),
            static_cast<char>((length >> 24) & 255)};
        std::ofstream output(argv[1], std::ios::binary);
        output.write(header.data(), header.size());
        output.write(json.data(), json.size());
        output.close();
        return output ? 0 : 4;
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 5;
    }
}
```

Avec un environnement de compilation déjà préparé et un chemin de build court :

```text
cmake -S . -B b -G Ninja -DCMAKE_BUILD_TYPE=Release -DFROG_FRONTEND_BUILD_TESTS=OFF -DFROG_BRIDGE_SDK_ROOT=C:/kit/FROG/sdk/ecosystem
cmake --build b --target bridge-encode-describe
b/bridge-encode-describe.exe describe.frame
```

Qualification exécutée : CMake 3.31.6, Ninja 1.12.1, MSVC 19.44.35228 /
outils 14.44.35207, Windows SDK 10.0.26100.0, C++20 Release. Le programme
consommateur émet une trame `describe` de 185 octets ; son exécutable importe
uniquement Kernel32, UCRT et les bibliothèques standard MSVC 14.x. Ces dernières
restent nécessaires pour ces binaires `/MD` ; aucune compatibilité binaire C++
universelle n'est revendiquée. Le kit source est la référence reconstructible.

Le défaut de `FROG_BRIDGE_FRONTEND_ROOT` pointe vers
`FROG/Implementations/Frontend`. Ce paramètre CMake permet de désigner une copie
locale complète de ce même propriétaire. La dépendance privée de liaison
`frog-frontend-production` réutilise son vrai JSON/SHA ; le consommateur n'inclut
aucun en-tête de ce propriétaire. Aucun Studio, LLVM, Runtime privé, Python ou
`jsonschema` n'est requis par la cible de bibliothèque ci-dessus. Les outils
Python sont des consommateurs de schémas distincts.

## Fichiers à conserver dans un kit local

Le minimum avec les CMake actuels comprend :

- `sdk/ecosystem/CMakeLists.txt`, `include/`, `src/` et `schemas/` ;
- `sdk/ecosystem/tests/bridge_protocol_smoke.cpp`, car sa cible est déclarée même
  lorsqu'elle est exclue du build par défaut ;
- `Implementations/Frontend/CMakeLists.txt`, `production-sources.cmake`,
  `source-origin.json`, `cmake/` et tous les chemins recensés par ce manifeste :
  `src/`, `include/`, `manifests/`, `snapshot/sdk/`, `snapshot/generated/` et
  `snapshot/api/`. Conserver aussi les sources C API/CLI référencées par le CMake.

Il ne suffit donc pas de copier seulement `FrogJson.cpp` et `Sha256.cpp` : le
CMake lie le propriétaire de production et vérifie son inventaire. Il ne
recherche aucun checkout Studio/Runtime lorsque la copie locale complète est
fournie. Pour construire puis lancer `frog-bridge-protocol-smoke`, conserver
aussi les fixtures `sdk/ecosystem/fixtures/bridge-*.json` et
`Implementations/Frontend/tests/fixtures/` ; le test utilise les vraies
dérivations API 3 de sources publiques.

Conserver les notices et les [observations de provenance et licences du
Frontend](../../../Implementations/Frontend/docs/provenance-and-licensing.md).
Une copie locale et ses empreintes ne changent pas les droits des corps extraits
ni ne constituent une autorisation générale de redistribution.

## Versions et empreintes P28

| Axe | Contrat actuel |
| --- | --- |
| Source / FIR | `frog.source` / `frog.execution_ir`, chacun en `0.1` |
| API C Frontend / logiciel | API `3` / logiciel `0.1.0-local`, indépendants |
| Provenance / capacités Frontend | `frog.frontend.context` `0.3` / `frog.frontend.capabilities` `0.2` |
| Wire / description / payload Bridge | `0.1` / `0.1` historique ou `0.2` explicite / `0.1` |
| Diagnostics Frontend / Bridge | `frog.diagnostic` `0.2` / `0.1`, non interchangeables |
| Profils sémantiques | ID complet et version propre `.vN` ou `@N` ; aucune version déduite du FIR |

`graphRequirements(provenance, fir, out, error)` consomme les documents complets
réellement émis par l'API 3 et conserve leur provenance. Le `document_id` Source
reste distinct du `document_id` FIR. Les empreintes sont :

- `firSha256` : octets UTF-8 exacts du FIR ;
- `contextSha256` : octets exacts de la provenance ; pour l'insérer comme objet
  dans un payload, utiliser le token JSON exact émis par l'API, sans ajouter
  d'espaces ou de fin de ligne extérieurs ;
- `frontendFingerprint` : SHA-256 de l'objet `frontend` encodé canoniquement,
  contenant logiciel, API, commit et SHA du propriétaire P28.

Le CMake Frontend produit `frontend-content-identity.txt` à partir des entrées
actuelles de son inventaire. Ce SHA n'est pas l'empreinte du paquet Bridge ou
d'un backend. Le verrou de sources Studio consigne séparément les trois arbres
SDK `include`, `src`, `schemas` et son `CMakeLists.txt`. Un hash prouve une
identité d'octets ; il ne prouve ni confiance ni admission d'exécution.

## Transport, méthodes et limites

Chaque trame est un uint32 little-endian suivi d'autant d'octets JSON UTF-8.
Vérifier la taille avant allocation, réserver stdout aux trames et séparer
stderr. `decodeMessage` conserve l'enveloppe historique ; appeler ensuite
`validatePayload` avant utilisation. `Exchange` suit le tuple exact
requête/session/génération, borne les progrès et n'accepte qu'une réponse finale.
Les sorties périmées ou annulées ne sont pas publiées.

`describe` reçoit `{}` et retourne directement une Description. `probe` décrit
un service sans admettre de graphe. `validate` reçoit le FIR complet, sa
provenance et la sélection ; son reçu doit précéder `lower`, `build`, `deploy`
ou `run/start`. `stop` et `observe` utilisent un handle de run opaque, lié à la
session/génération. Pause, step, contrôles et Events sont des commandes `run`
explicitement annoncées. Une liste d'opérations non exhaustive diffère la
décision vers `validate`, sans autoriser Run. Voir les
[payloads et règles détaillées](bridge-methods.md).

| Ressource | Défaut | Bornes annoncées / plafond |
| --- | ---: | --- |
| Trame JSON | 1 048 576 octets | 1 024 à 16 777 216 ; minimum des limites hôte/bridge |
| Délai d'une requête | 30 000 ms | 1 à 300 000 ms ; minimum des limites hôte/bridge |
| Messages de progrès | 256 | 1 à 65 536 dans Description 0.2 |
| stderr conservé | 65 536 octets | 0 à 1 048 576 dans Description 0.2 |
| Diagnostics d'une enveloppe | — | 4 096 |
| Listes de formats/profils/opérations/capacités | — | 4 096 éléments ; textes de 512 caractères au plus |
| Dépendances / transformations de provenance | — | 4 096 éléments chacune |
| IDs de requête/session, tokens, messages diagnostiques | — | 2 048 caractères au plus |
| URI/identités Source/FIR de provenance | — | 8 192 caractères au plus |
| JSON partagé | — | profondeur 128, 1 000 000 valeurs ; la trame reste plafonnée à 16 MiB |

Les limites de caractères comptent les scalaires Unicode ; les trames et hashes
portent sur les octets. Les générations sont des uint64 exacts. Le transport
reste responsable des délais, EOF, crashes, annulation et terminaison ; la
bibliothèque de codec ne crée aucun thread ni timer.
