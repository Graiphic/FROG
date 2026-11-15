# Classes FROG par valeur — contrat local V1 en cours d’intégration

Cette page décrit le format de définition et le noyau sémantique testés. Elle n’étend pas la version publique `spec_version` ni la couverture Run ou Build annoncée. Un `.fclass` n’est pas lui-même un programme.

## Définition et dépendances

`format: "frog.class"`, `version: 1`, `id`, `name`, `revision`, `parent`, `fields` et `methods` sont obligatoires. La version de format décrit l’enveloppe. La révision positive U32 décrit la définition. Un changement de nom de classe ne modifie pas son identité. Une modification des données qui change la révision ne remplace pas automatiquement une dépendance sur l’ancienne révision.

- `parent` vaut `null` ou contient `id`, `path` et `revision`. Les chemins relatifs UTF-8 à séparateur `/` sont résolus par l’hôte par rapport au fichier qui déclare le lien. Le Frontend vérifie l’identité et la révision dans les octets capturés, conserve leur SHA-256 et refuse les cycles, les résolutions ambiguës et les changements pendant la capture.
- Les champs, dans l’ordre déclaré, portent `id`, `label`, `type` et `default`. Ils sont tous privés. Leur identité est qualifiée par la classe déclarante. Le noyau actuel réutilise le profil Record immuable : bool, i32, u32, i64, u64, f64, String, Path et Records imbriqués. Les nombres sont lus sans transiter par un Double. Un défaut hors type est refusé.
- Les méthodes portent `id`, `name`, `implementation` (référence `id/path/revision`), `visibility`, `dispatch`, `overrides` et `ports`. Chaque port a un `slot` stable dans la signature, un `port_id` stable dans le FROG membre, une `direction` (`input` ou `output`), un `kind` (`object` ou `data`) et un `type` (`null` pour le port objet implicitement spécialisé à la classe déclarante). Une méthode a un port objet d’entrée et au plus un port objet de sortie. Un membre qui modifie la valeur doit transmettre le résultat sur sa sortie objet.
- `overrides` vaut `null` ou contient `class_id` et `method_id` du slot dynamique initial. Une redéfinition conserve les slots, directions, catégories et types complets des ports de données, ainsi que la visibilité. Seuls les ports objet se spécialisent sur la classe qui déclare la redéfinition.

Une classe est un type nominal : deux classes indépendantes ayant les mêmes champs restent incompatibles. Un membre `private` n’est accessible qu’aux membres du déclarant ; `protected` ajoute ses descendants ; `public` est accessible de l’extérieur. Une méthode dynamique ne peut pas être privée. Seul un membre du déclarant accède directement à ses champs privés. Le contexte membre doit provenir d’une attribution validée par le Frontend, jamais d’une déclaration du nœud d’appel.

Un objet enfant peut prendre la vue parent sans perdre son type concret ni ses données. Pas de conversion descendante dans le profil initial. Une méthode statique pointe vers la déclaration appelée. Pour le dynamique, la table associe chaque type concret admis à la redéfinition la plus proche, ou à la déclaration héritée. Le choix est indépendant du nom de fichier et du mécanisme des FROG polymorphes.

## Représentation interne et frontières

L’abaissement expérimental utilise un Record immuable, privé au compilateur, avec un discriminant et un layout déterministe par famille dans la fermeture capturée. Les discriminants ne sont pas des identifiants de fichier. Ils ne sont jamais stockés comme valeurs d’objet persistantes ; la révision courante de toute la fermeture doit servir à l’abaissement. Les écritures Record existantes produisent une nouvelle valeur. Les sélections de champs sont fabriquées à partir des identités autorisées. Un Cluster d’auteur n’est pas implicitement un objet.

Le code actuel produit des descripteurs Record scellés et les bras de sélection ; il ne projette pas encore un nœud d’appel dynamique en FIR et ne réserve pas de syntaxe d’objet dans un diagramme. L’inspection du connecteur a un contrat de comparaison, mais le chargement et l’attribution d’un vrai FROG membre n’y sont pas encore raccordés. Ces frontières empêchent d’annoncer le format comme une capacité d’exécution. Interfaces objet, objets partagés par référence et import `.lvclass` sont hors V1.

Référence conceptuelle : [choix de conception de LabVIEW](https://www.ni.com/en/support/documentation/supplemental/06/labview-object-oriented-programming--the-decisions-behind-the-de.html) et [règles NI pour les redéfinitions](https://knowledge.ni.com/KnowledgeArticleDetails?id=kA00Z000000g0JCSAY&l=en-US). Le format et les identités FROG ne sont pas des formats LabVIEW.
