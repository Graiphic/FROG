# Frontend local : opérations et résolution explicite

Implémentation locale expérimentale `0.2.0-local`, ABI C3. Les formats
Source/FIR restent `0.1` ; `frog.document.draft` accepte des révisions explicites `0` à `9` ; les révisions de profils sont indépendantes de
l'ABI, du logiciel et du protocole bridge. Ce document décrit du code à
qualifier : sa présence ne prouve ni compilation, ni exécution, ni publication.

## Génération commune

`FrontendOperation.cpp` appelle les producteurs C++ de production
`CanonicalExecutionIr::derive` ou `CanonicalEventExecutionIr::derive`.
Le profil composé conserve son routage existant dans le premier producteur.
Le profil Event public distinct est sélectionné pour une Event Structure
publique hors profil composé. Il n’existe pas de repli vers un interprète, un Canvas, un backend ou une version linguistique différente. `frog.document.draft` est une entrée explicitement déclarée, pas un repli implicite de `frog.source`.

`validate` effectue la même dérivation et les mêmes contrôles de scellement
que `derive`, puis ne retourne pas la FIR. `derive` ne retourne une FIR
complète qu'en cas de succès. Un refus, une annulation ou une limite ne
retourne jamais un préfixe exécutable. La réussite du Frontend n'est pas
une admission LLVM, une réalisation d'opérateur, ni une preuve d'exécution.

Le source original est capturé dans un propriétaire immutable partagé avec les lecteurs communs ; les projections dérivées conservent leur identité distincte. Un contexte vide signifie `{}`, sans
héritage de l'OS, de l'environnement, de Studio ou d'une requête précédente.
La lecture, la résolution et les migrations sémantiques des drafts sont maintenant implémentées dans la bibliothèque commune. Elles produisent une projection détachée sans réécrire l’entrée. Les propositions, prévisualisations et applications de réparations avec historique/Undo restent privées à Studio. La qualification de cette extraction `0.2.0-local` n’est pas encore acquise.

## Profil de résolution `frog.frontend.resolution@1` — contrat conservé

La première réalisation est délibérément bornée aux définitions de types
réelles du profil composé. Le contexte est un objet fermé :

```json
{
  "profile": "frog.frontend.resolution@1",
  "dependencies": [
    {"kind": 2, "source_pointer": "/diagram/nodes/0/studio_type_schema"}
  ]
}
```

Le pointeur RFC6901 doit désigner un objet réellement écrit dans un champ
`studio_type_schema` ou dans un sous-schéma. Les cibles du contexte ne peuvent
pas se recouvrir. L'hôte ne fournit pas un remplacement arbitraire. L'objet
authored ciblé contient exactement :

```json
{
  "$ref": "dep:types",
  "identity": "example-types",
  "version": "1",
  "sha256": "<64 chiffres hexadécimaux minuscules du contenu exact>",
  "pointer": "/definitions/T"
}
```

Le résolveur reçoit le genre `TYPE_DEFINITION=2`, la référence non modifiée,
l'URI propriétaire réelle, son pointeur et les identité/version/empreinte
attendues. Il fournit des octets JSON exacts et une URI canonique explicite.
Le document retourné est lui-même un objet fermé :

```json
{
  "profile": "frog.frontend.type-definitions@1",
  "identity": "example-types",
  "version": "1",
  "definitions": {
    "T": {"canonical": "u64", "choice_kind": "none", "field_names": [], "choices": [], "children": []}
  }
}
```

Le schéma montré est illustratif : le contenu sélectionné doit être un schéma
complet réellement accepté par `canonicalObjectReferenceReadType`, propriétaire
du codec de types existant. Les identités nominales, enums, labels de champs,
rangs, dimensions et valeurs exactes ne sont pas simplifiés. Les références
imbriquées utilisent le même objet `$ref` et le même résolveur.

Après sélection du fragment `/definitions/...`, le Frontend développe les
références dans une projection détachée, valide chaque fragment avec ce codec,
puis soumet la projection au vrai producteur. Sans dépendance correcte, ou si
le type résolu contredit les autres types déclarés, la génération échoue.
Ce mécanisme a donc un effet sémantique réel et n'est pas un inventaire décoratif.

La valeur `version` identifie la révision du document de types ; `@1` identifie
son profil d'enveloppe. Le Frontend vérifie le SHA-256 réellement calculé ainsi
que l'identité et la version du contenu contre les informations authored et
les métadonnées retournées. Une seule identité ne peut désigner plusieurs
URI, versions ou contenus pendant une requête. Une URI ne peut désigner
plusieurs identités. Les répétitions équivalentes partagent leur capture.
Les cycles et fragments absents sont des erreurs explicites.

Chaque callback `resolve` retourné normalement est suivi exactement une fois
de `release`, quel que soit son statut, y compris refus, métadonnées invalides,
limite et annulation. Aucun octet prêté n'est utilisé après `release` ; seules
les captures immutables appartenant à cette requête subsistent. Une exception
du résolveur ne constitue pas un retour normal. Les exceptions des callbacks
sont contenues et diagnostiquées ; les hôtes doivent respecter l'ABI C.
Il n'existe aucun cache implicite inter-requêtes ni accès fichier/réseau.

Les genres Source, contrat de bibliothèque et contrat de profil ne sont pas
réalisés par ce premier profil de résolution. Ils sont refusés explicitement,
sans supprimer les références ou accorder de nouvelles capacités. Il n'est pas
possible d'injecter du code natif via un document de types. Les appels SubFROG
restent soumis aux véritables contrats d'appel du producteur.

`conditional_disable` est un champ optionnel du contexte de résolution.
Dans `@1`, seul l’objet vide est accepté : un contexte non vide est refusé
avec `frontend.conditional_context_unsupported`, à son emplacement exact.
Le parcours Source de `@1` ne reçoit pas de contexte symbolique non vide.
L'ignorer ou le remplacer par des paramètres du backend serait incorrect.

## Drafts et contexte `frog.frontend.resolution@2`

Cette extension est implémentée séparément du contrat `@1`, qui n’acquiert pas implicitement les nouveaux paramètres. L’appel déclare `source_format_id="frog.document.draft"` et une version décimale exacte de `"0"` à `"9"`, correspondant à la révision de l’enveloppe reçue. La FIR reste `frog.execution_ir/0.1`. Les formats futurs, les révisions incohérentes et les sources malformées sont refusés avec un diagnostic attribué.

Le lecteur commun, la normalisation du stockage (y compris les blocs Zstandard), les schémas de widgets, les liens TypeDef, les appels SubFROG, la spécialisation conditionnelle, la validation et la migration vers la Source publique réutilisent leurs propriétaires existants. Ils ne dépendent pas du Canvas ni d’une session runtime. Une révision reconnue ne rend pas automatiquement tout document exécutable : données incomplètes, diagramme indisponible, dépendance absente et capacité non réalisée restent des refus explicites.

Exemple de contexte pour préparer un draft :

```json
{
  "profile": "frog.frontend.resolution@2",
  "dependencies": [],
  "conditional_disable": {
    "built_in": {},
    "project": {"MODE": "release"},
    "target": {}
  }
}
```

Les trois cartes optionnelles contiennent des noms et valeurs String explicites. Aucun symbole n’est découvert via l’OS, le registre, PATH ou le backend. Le contexte vide conserve l’absence de configuration ; des conditions non résolues restent diagnostiquées. Le contexte v2 est transmis à la préparation des drafts. Il ne prétend pas ajouter une seconde spécialisation à la Source publique déjà projetée.

Les liens enregistrés dans un draft déclenchent des demandes `SOURCE=1` pour les SubFROG et `TYPE_DEFINITION=2` pour les TypeDef. Le callback reçoit le libellé de référence original, l’URI propriétaire, le pointeur source et les contraintes d’identité/version/empreinte présentes. Il retourne des contenus immuables, leur URI canonique, identité, version et empreinte ; le Frontend recalcule le SHA-256, vérifie ces contraintes et conserve la provenance des sources incorporées. Les appels ne nécessitent ni fichier ouvert dans Studio ni recherche implicite sur le disque.

Ces liens de documents ne sont pas l’enveloppe JSON de fragments `frog.frontend.type-definitions@1` décrite plus haut : cette dernière reste le contrat du parcours Source `@1`. Les genres bibliothèque/profil ne sont pas réalisés. Des demandes inconnues ou incohérentes ne déclenchent aucun fallback.

La CLI fournit les mêmes snapshots par son lock de dépendances et appelle la même bibliothèque que le consommateur C. Les textes de diagnostics, les identités et les maps proviennent de la préparation réelle. La prise en charge de la lecture et de la migration ne donne pas au Frontend l’autorité de modifier le document original ou d’approuver une réparation Studio.

**État : implémenté, NON encore qualifié sur le nouveau kit.** Les tests ciblés draft/dépendances doivent être exécutés avec l’identité exacte des sources, bibliothèque et CLI. Aucune réussite historique Source `@1` n’est présentée comme qualification automatique de `@2`.

## Limites et annulation

Les valeurs zéro sélectionnent les plafonds documentés ; une valeur non nulle
peut seulement les réduire. Les plafonds du producteur existant priment :

| Entrée ou travail | Plafond effectif maximal |
| --- | ---: |
| Source et contexte JSON, chacun | 8 Mio |
| Une dépendance | 8 Mio |
| Total des captures de dépendances uniques | 64 Mio |
| Résultats JSON cumulés, FIR comprise | 32 Mio |
| FIR du producteur | 16 Mio |
| Nœuds Source et dépendances cumulés | 1 024 |
| Connexions cumulées | 4 096 |
| Profondeur sémantique/résolution | 64 |
| Dépendances uniques | 256 |
| Diagnostics | 256 |
| Visites d'expansion des fragments de types | 4 096 |

Les limites octets sont contrôlées avant la copie et le balayage UTF-8 des
grands buffers. Le budget agrégé et le nombre de captures sont contrôlés avant
une nouvelle copie de dépendance. Les régions masquées participent aux comptes.
Les contraintes supplémentaires du parseur, du codec et des producteurs sont
conservées. Le calcul de taille des résultats précède leur sérialisation.
Si même le diagnostic complet et la provenance ne tiennent pas dans le budget,
l'ABI renvoie une erreur de ressource, sans JSON tronqué.

L'annulation est coopérative : elle est consultée avant les travaux, avant
résolution, entre unités bornées et après le producteur. Un callback retournant
autre chose que 0/1 est défectueux. Les producteurs synchrones existants ne sont
pas préemptés en leur milieu ; aucune borne de latence fine n'est revendiquée.

## Résultats et provenance

- Diagnostics : tableau d'objets `frog.diagnostic`, version `0.2` ; code stable,
  étape, sévérité, catégorie, URI/document/pointeur. Une capacité Frontend
  manquante reste distincte d'une erreur sémantique. Un pointeur du FIR
  `/unit/...` reste `derived_pointer`, sans être présenté comme pointeur Source.
- Descripteur : `frog.frontend.capabilities`, `schema_version=0.2` ; identités
  logiciel/API/commit/empreinte réellement fournies à la compilation, formats,
  opérations de l'API, principaux profils et limites effectives. La liste des
  profils est signalée non exhaustive ; elle ne constitue pas une matrice
  d'exécution des opérateurs de la palette.
- Provenance : `frog.frontend.context`, `schema_version=0.3` ; source original
  exact, dépendances capturées et empreintes, contexte explicite, limites,
  transformations et état `closure_complete`. Les profils et opérations d'une
  réussite proviennent du FIR réellement produit. Un refus conserve les faits
  connus sans prétendre que la fermeture est complète.
  Le champ `fir.document_id` conserve l'identité FIR distincte du document Source ;
  cette extension utilise `0.3` sans modifier les schémas historiques fermés.
- Correspondances : enveloppe `frog.frontend.source_maps`, version de schéma
  `0.1`, contenant les `source_map` et `correspondence` du FIR ainsi que les
  transformations de résolution et les empreintes authored/FIR.

En présence de résolution, le FIR reste scellé sur les octets de la projection
effectivement soumise au producteur ; son empreinte n'est jamais remplacée
artificiellement par celle du source original. La provenance distingue
`source.sha256` et `projected_source_sha256`, et les transformations relient
les emplacements authored aux fragments de dépendances. Sans résolution,
les octets originaux sont transmis directement au propriétaire commun.

Après annulation immédiate ou refus de taille avant capture, l'empreinte Source
peut être vide : le Frontend ne parcourt pas un buffer qu'il vient de refuser.
L'ordre des captures/callbacks n'est pas une identité sémantique ; la provenance
des dépendances est ordonnée par identité. Aucun timestamp ou chemin implicite
du poste ne participe aux résultats déterministes.
