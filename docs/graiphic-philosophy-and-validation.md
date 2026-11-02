# Graiphic : pilotage, partage et validation

Graiphic dirige le projet officiel tout en ouvrant une partie de son code et ses contrats à l’écosystème. Les propositions externes sont bienvenues ; l’acceptation reste une décision explicite des mainteneurs. Cette note explique les [règles de gouvernance](../GOVERNANCE.md#official-project-rules), sans modifier les licences ni créer une seconde politique.

**English summary:** Graiphic leads official development and releases. Contributions are open and reviewed. Shared production components are proven in Studio, qualified independently, then published as identified versions. Studio and public tools consume one implementation; updates are explicit and validated.

## Un produit pilote et un écosystème ouvert

Studio est l’environnement d’intégration et l’orchestrateur : édition des documents, diagnostics, réparations proposées, choix des backends et services, exécution, observation et Build. L’ambition est un environnement puissant, extensible et maîtrisé sur l’ensemble de la chaîne. Une ambition ne constitue pas la preuve qu’un matériel, une fonction ou un déploiement est déjà pris en charge : les profils et leurs résultats font foi.

Graiphic garde la direction de la feuille de route, de l’architecture et des releases officielles. Chacun peut signaler un problème ou proposer une amélioration. L’auteur d’une proposition ne reçoit pas une promesse d’intégration ; la revue évalue utilité, exactitude, compatibilité, sécurité, performances pertinentes, maintenance et licence. Une demande substantielle de modification, un report ou un refus est accompagné d’une raison concise. Les droits d’usage, de modification, de redistribution et de fork restent ceux des licences ; les noms et marques officiels sont distincts.

## Éprouver, partager, puis intégrer volontairement

![Cycle des composants partagés](diagrams/graiphic-shared-component-cycle.svg)

Le parcours des composants issus de Studio est : comportement éprouvé dans Studio, qualification autonome du composant commun, puis publication d’une version identifiée. Une contribution peut être proposée publiquement à tout moment ; les preuves requises précèdent l’intégration ou la release officielle concernée. Une correction purement documentaire ne demande pas une compilation de l’IDE.

Après extraction, **une seule implémentation** alimente Studio et les outils publics. La CLI est un hôte de la même bibliothèque Frontend ; elle n’est ni un second générateur FIR ni une étape externe obligatoire pour chaque édition dans Studio. Les contrats nécessaires sont publics ; l’éditeur complet, les réparations et les autres parties privées ne le deviennent pas automatiquement.

Studio épingle la version qualifiée, son identité de contenu et sa configuration. Une release publique plus récente ne remplace jamais silencieusement ce composant. La mise à jour d’un consommateur est une opération volontaire, avec les contrôles directement concernés.

## Valider chaque étage pour ce qu’il démontre

![Étages de validation FROG](diagrams/frog-validation-stages.svg)

| Décision | Question traitée |
|---|---|
| Lecture et intégrité | Les octets peuvent-ils être lus sans perte ni ambiguïté ? |
| Format et migration | Le format est-il reconnu ? Une migration explicite est-elle nécessaire ? |
| Langage et types | Dépendances, terminaux, connexions, régions, valeurs et effets sont-ils valides ? |
| Admission par cible | Le backend et ses fournisseurs réalisent-ils le graphe complet dans leur profil annoncé ? |
| Compilation et déploiement | Le programme peut-il être compilé, lié et empaqueté avec ses dépendances ? |
| Exécution | Le package a-t-il réellement fonctionné dans l’environnement décrit ? |

Le Frontend commun est l’autorité pour la validation sémantique et la FIR. Il ne répare pas implicitement le document. Studio propose et applique les réparations comme transactions d’édition, puis soumet le résultat au même Frontend.

Un programme valide mais non réalisé sur une cible est **non pris en charge sur cette cible**, pas corrompu. Un contrôle interrompu ou incomplet reste indéterminé. Une compilation réussie ne prouve pas l’exécution, et un lancement local ne qualifie pas Windows propre ou tous les matériels. Les diagnostics doivent conserver étape, catégorie, code, gravité et objets/emplacements concernés.

La peinture signifie simplement dessiner l’interface. Elle consomme un état préparé publié, attribué au document, à sa révision, aux dépendances et à la cible. Elle ne doit pas relancer les traitements lourds à chaque affichage. Cette séparation n’autorise ni Run sans admission correspondante, ni l’exécution silencieuse d’un ancien instantané.

## Dépendances choisies pour leur valeur

La maîtrise du produit ne demande pas de tout réécrire. Une bibliothèque éprouvée et optimisée peut être le meilleur choix. Un remplacement doit apporter un bénéfice démontré, préserver les contrats et être vérifié. Le nombre de dépendances ou l’étiquette « maison » ne constitue pas une mesure de qualité. Les origines, licences et notices restent conservées, y compris pour le code adapté de tiers.

## Des preuves attribuées, des limites visibles

Distinguer systématiquement : implémenté, compilé, testé, livré localement, publié sur GitHub et qualifié sur une machine indépendante. Une réussite finie ne qualifie pas toute une palette. Un échec conservé n’est pas transformé en PASS par une publication.

Le [checkpoint Frontend du 2 novembre 2026](frontend-checkpoint-2026-11-02.json) identifie le corpus et ses limites ; les [instructions de construction](../Implementations/Frontend/README.md) décrivent le composant autonome. Les évolutions gardent leurs versions et preuves propres. Les licences, le [CLA](../CLA.md) et le [parcours de contribution](../CONTRIBUTING.md) existants continuent de s’appliquer.
