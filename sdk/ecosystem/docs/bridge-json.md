# JSON et SHA pour les auteurs de bridges

`frog/bridge_json.hpp` fournit une API source C++20 dans
`frog::bridge::json`. Le contrat binaire tiers reste le protocole IPC Bridge.
L'en-tête public ne contient aucun type ou include privé Frontend, Studio,
Runtime ou LLVM. L'implémentation délègue au même `FrogJson` en mode
`PreserveLexeme`, au même encodeur et au même SHA-256 que le Frontend de
production ; aucun parseur ou catalogue sémantique parallèle n'est introduit.

```cpp
#include <frog/bridge_json.hpp>

frog::bridge::Error error;
auto root = frog::bridge::json::parse(payloadBytes, error);
if (!root) { /* refus attribué par error.code/message/pointer */ }
else if (root->kind() == frog::bridge::json::Kind::Object) {
    auto count = root->member("count");
    if (count && count->kind() == frog::bridge::json::Kind::Number) {
        auto exact = count->numberLexeme(); // aucune conversion en double
        // Le profil du bridge possède la conversion numérique et ses bornes.
    }
}
```

`Value` est un handle copiable d'arbre immuable. Un enfant obtenu par `member`,
`elements` ou `members` conserve l'arbre complet, même après destruction du
handle racine et des octets d'entrée. Une `string_view` retournée reste valide
tant qu'un handle du même arbre subsiste. `members()` copie les clés et énumère
l'ordre déterministe de l'encodeur ; `elements()` conserve l'ordre du tableau.

`member` absent retourne `nullopt`. Un membre présent contenant `null` retourne
un `Value` de `Kind::Null`. Les accesseurs exigent leur type exact : mauvais
type ou handle déplacé produisent `std::logic_error`, sans valeur fabriquée.
`numberLexeme()` conserve I64/U64, `-0`, exposants et précision décimale. Il
n'admet aucune conversion de langage ou de cible ; un nombre JSON tel que
`1e400` peut être lisible puis refusé par un profil f64 fini.

`parse(bytes, error, maxBytes)` utilise un plafond effectif de
`min(maxBytes, 16 MiB)`. Le parseur commun refuse clés dupliquées, UTF-8 malformé,
surrogates isolés, JSON incomplet, profondeur supérieure à 128 et plus d'un
million de valeurs. La taille est contrôlée avant parsing. Un succès efface
`Error` ; un refus retourne `nullopt`, jamais un arbre partiel. `Error.pointer`
peut contenir le curseur réel `@byte:N` ; ce n'est pas un JSON Pointer Source.

`encode` appelle l'encodeur commun déterministe et conserve les lexèmes
numériques. Il ne conserve pas la mise en forme ou l'ordre des membres du
document d'entrée. Une empreinte d'artefact doit donc être calculée sur les
**octets originaux**, pas sur ce réencodage. `quote` échappe avec le même owner,
vérifie l'UTF-8 par le même parseur, et borne son résultat à 16 MiB ; elle lève
`invalid_argument` pour une chaîne invalide et `length_error` en dépassement.
`sha256` digère les octets exacts, y compris NUL, sans interprétation de texte.

La lecture JSON et le SHA ne prouvent ni validité FIR, ni compatibilité d'un
profil, ni confiance, ni admission d'exécution. Ces étapes gardent leurs owners.
Les handles retiennent l'arbre complet ; leur taille n'est pas une mesure de
mémoire indépendante de ce dernier. Cette API n'ajoute aucun transport ou
thread, et ne lit pas de fichier, réseau, registre ou variable d'environnement.
# Exact spans

`Value::originalUtf8()` returns the shared parser's exact token span, excluding surrounding whitespace. Child handles retain both the parsed tree and original input bytes. Use this accessor when binding a receipt to provenance bytes; `encode()` deliberately produces deterministic JSON and can change ordering or whitespace.
