# Classes FROG par valeur — contrat local V1 en cours d’intégration

Cette page décrit le format de définition et le noyau sémantique testés. Elle n’étend pas la version publique `spec_version` ni la couverture Run ou Build annoncée. Un `.fclass` n’est pas lui-même un programme.

## Définition et dépendances

`format: "frog.class"`, `version: 1`, `id`, `name`, `revision`, `parent`, `fields` et `methods` sont obligatoires. La version de format décrit l’enveloppe. La révision positive U32 décrit la définition. Un changement de nom de classe ne modifie pas son identité. Une modification des données qui change la révision ne remplace pas automatiquement une dépendance sur l’ancienne révision.

- `parent` vaut `null` ou contient `id`, `path` et `revision`. Les chemins relatifs UTF-8 à séparateur `/` sont résolus par l’hôte par rapport au fichier qui déclare le lien. Le Frontend vérifie l’identité et la révision dans les octets capturés, conserve leur SHA-256 et refuse les cycles, les résolutions ambiguës et les changements pendant la capture.
- Les champs, dans l’ordre déclaré, portent `id`, `label`, `type` et `default`. Ils sont tous privés. Leur identité est qualifiée par la classe déclarante. Le noyau actuel réutilise le profil Record immuable : bool, i32, u32, i64, u64, f64, String, Path et Records imbriqués. Les nombres sont lus sans transiter par un Double. Un défaut hors type est refusé.
- Un champ peut aussi porter `layout` avec `x`, `y`, `width` et `height`. Ce rectangle de présentation est relatif au cluster privé de la classe déclarante. Chaque nombre est fini et borné à 10 000 000 en valeur absolue. Les dimensions sont strictement positives. Une ancienne définition peut omettre ce rectangle. Une modification de ce rectangle conserve l’ordre, les valeurs et types, l’identité des champs et la révision de la classe.
- Les méthodes portent `id`, `name`, `implementation` (référence `id/path/revision`), `visibility`, `dispatch`, `overrides` et `ports`. Chaque port a un `slot` stable dans la signature, un `port_id` stable dans le FROG membre, une `direction` (`input` ou `output`), un `kind` (`object` ou `data`) et un `type` (`null` pour le port objet implicitement spécialisé à la classe déclarante). Une méthode a un port objet d’entrée et au plus un port objet de sortie. Un membre qui modifie la valeur doit transmettre le résultat sur sa sortie objet.
- `overrides` vaut `null` ou contient `class_id` et `method_id` du slot dynamique initial. Une redéfinition conserve les slots, directions, catégories et types complets des ports de données, ainsi que la visibilité. Seuls les ports objet se spécialisent sur la classe qui déclare la redéfinition.

Une classe est un type nominal : deux classes indépendantes ayant les mêmes champs restent incompatibles. Un membre `private` n’est accessible qu’aux membres du déclarant ; `protected` ajoute ses descendants ; `public` est accessible de l’extérieur. Une méthode dynamique ne peut pas être privée. Seul un membre du déclarant accède directement à ses champs privés. Le contexte membre doit provenir d’une attribution validée par le Frontend, jamais d’une déclaration du nœud d’appel.

Un objet enfant peut prendre la vue parent sans perdre son type concret ni ses données. Pas de conversion descendante dans le profil initial. Une méthode statique pointe vers la déclaration appelée. Pour le dynamique, la table associe chaque type concret admis à la redéfinition la plus proche, ou à la déclaration héritée. Le choix est indépendant du nom de fichier et du mécanisme des FROG polymorphes.

## Représentation interne et frontières

L’abaissement expérimental utilise un Record immuable, privé au compilateur, avec un discriminant et un layout déterministe par famille dans la fermeture capturée. Les discriminants ne sont pas des identifiants de fichier. Ils ne sont jamais stockés comme valeurs d’objet persistantes ; la révision courante de toute la fermeture doit servir à l’abaissement. Les écritures Record existantes produisent une nouvelle valeur. Les sélections de champs sont fabriquées à partir des identités autorisées. Un Cluster d’auteur n’est pas implicitement un objet.

Le raccordement en cours capture les octets des membres réellement référencés, compare leurs connecteurs au contrat de classe, puis projette leurs opérations par les réalisations Record, Case et sous-FROG existantes. Les bras exécutent les vrais membres capturés. La sélection selon la valeur concrète reçue est distincte du choix de variantes par signature. Cette description du contrat ne constitue pas une preuve de Run ou Build : seules les versions et fixtures effectivement qualifiées établissent la couverture livrée. Interfaces objet, objets partagés par référence et import `.lvclass` sont hors V1.

## Profil de document de travail

Les opérations appartiennent au document `frog.document.draft`, révision privée **10**. Elles ne relèvent pas d’une nouvelle version de la Source canonique `spec_version`. Les anciens lecteurs doivent refuser cette révision au lieu de perdre les déclarations objet. Les anciens documents sans classe conservent leur propre révision et leurs valeurs.

Le type nominal est `object<"identité-de-classe">`. Il ne se convertit pas depuis un Cluster ou une référence. La hiérarchie capturée par le Frontend autorise l’enfant vers un ancêtre ; aucun downcast implicite ou explicite n’est livré dans ce profil. Select trouve l’ancêtre commun des deux valeurs, sans modifier leurs données concrètes.

Le champ racine facultatif `classes` est une liste de références `.fclass` `id/path/revision`. Un véritable membre porte aussi `class_member` avec `class_id`, `method_id`, `implementation_id` et `revision`. Ces quatre champs ne donnent aucune permission à eux seuls : l’URI et le SHA des octets doivent correspondre à l’implémentation capturée depuis la définition déclarante. Copier l’annotation ailleurs ne crée pas un membre.

Une opération utilise le nœud de travail `kind: "frog_call"`, sans `document_path` exécutable, avec `call.class_operation` :

```json
{"kind":"dynamic_call","class":{"id":"sensor","path":"Capteur.fclass","revision":1},"member_id":"measure"}
```

Les autres genres sont `default`, `read_field`, `write_field` et `static_call`. `default` a un `member_id` vide ; lecture/écriture désignent l’identité du champ ; appel désigne celle de la méthode. Les ports de `call.ports` gardent leurs identités et types déclarés ; leur géométrie ne sélectionne jamais la méthode. Le Frontend refuse un connecteur périmé, des champs inconnus ou une opération mélangée avec un appel de fichier/polymorphe. Un appel de fichier ordinaire à un membre ne contourne pas ces contrôles.

Les données objet entrent dans le graphe par l’opération de valeur par défaut vérifiée. Les porteurs de connecteur objet sont des présentations de terminal ; leur état Variant ne fabrique pas une valeur d’objet. Un corps membre comporte un vrai connecteur, des producteurs pour chaque entrée requise et ses résultats câblés. Un indicateur non câblé n’implémente pas la méthode.

La fermeture inclut les définitions, parents, sources membres et dépendances ordinaires, avec URI, révision et SHA. Elle est figée pour la préparation, bornée à1024 classes/membres et64MiB. Une définition ou un membre absent, modifié ou incompatible produit un diagnostic ; aucun ancien artefact n’est réutilisé silencieusement. Une évolution de données hors couverture requiert une migration explicite : les valeurs ne sont pas réinterprétées.

Le Record de transport, ses champs qualifiés et son discriminant ne sont pas des valeurs d’objet persistantes. Seul le Frontend les fabrique. Le bras par défaut de la Case physique couvre un des types concrets de cette fermeture finie ; ses autres types ont un bras exact. Le profil refuse les entrées permettant de fabriquer un discriminant arbitraire. Il ne promet pas un contrôle dynamique d’une valeur externe hors de ce domaine.

Les corps utilisent l’expansion de sous-FROG déjà admise. La récursion de membres, les données privées hors du profil Record ci-dessus et les connecteurs sans porteurs natifs ne sont pas annoncés couverts. Les diagnostics et correspondances source conservent la définition, l’opération d’appel et le vrai fichier membre.

La fixture de raccordement complète est sous `Implementations/Frontend/tests/fixtures/classes-v1/`. Les quatre définitions historiques de `Examples/compiler/classes-v1/` restent des exemples de définition seuls et ne qualifient pas un programme exécutable.

Référence conceptuelle : [choix de conception de LabVIEW](https://www.ni.com/en/support/documentation/supplemental/06/labview-object-oriented-programming--the-decisions-behind-the-de.html) et [règles NI pour les redéfinitions](https://knowledge.ni.com/KnowledgeArticleDetails?id=kA00Z000000g0JCSAY&l=en-US). Le format et les identités FROG ne sont pas des formats LabVIEW.
