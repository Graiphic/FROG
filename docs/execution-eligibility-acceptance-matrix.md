# Matrice d'acceptation de l'exécutabilité

Révision 1 — 16 septembre 2026. **Document de qualification, non preuve de conformité.**

Source : étude fournie « FROG / Graiphic Studio — Validation complète du diagramme
et des structures », révision 2 du 5 septembre 2026. Ses 137 identifiants VAL sont
conservés pour assurer la traçabilité. Les résultats ci-dessous sont les attentes
initiales de l'étude, pas des résultats d'exécution.

Les [règles EXEC](../Language/Execution%20eligibility.md) et les contrats publiés
du profil/cible choisi priment. Lorsqu'une ligne propose une extension, elle ne
publie pas à elle seule le format ou le runtime correspondant. Par exemple,
VAL-100 traite l'absence de priorité : un profil publiant explicitement une
priorité doit être testé selon cette priorité, sans être déclaré ambigu par défaut.

Pour remplacer « À qualifier », fournir un lien de preuve identifiant le test,
le commit, la configuration/cible et le résultat. Les tests globaux Studio sont
résumés [séparément](execution-eligibility-studio-coverage.md).

| ID | Situation | Attente de l'étude | Qualification / preuve |
| --- | --- | --- | --- |
| VAL-001 | Document JSON non analysable. | Chargement sémantique refusé ; conserver l’accès au texte si possible. | À qualifier — aucune preuve individuelle rattachée |
| VAL-002 | Deux identifiants de nœud identiques dans la même portée. | Erreur d’identité bloquante. | À qualifier — aucune preuve individuelle rattachée |
| VAL-003 | Même identifiant local dans deux régions différentes, avec chemins qualifiés distincts. | Valide si les références de portée sont correctes. | À qualifier — aucune preuve individuelle rattachée |
| VAL-004 | Fil vers un nœud supprimé. | Erreur de référence ; ne pas compter l’entrée comme alimentée. | À qualifier — aucune preuve individuelle rattachée |
| VAL-005 | Fil vers un port supprimé après changement d’interface. | Erreur de port ou de signature. | À qualifier — aucune preuve individuelle rattachée |
| VAL-006 | Deux sources pour la même entrée. | Erreur de cardinalité. | À qualifier — aucune preuve individuelle rattachée |
| VAL-007 | Une source vers plusieurs destinations. | Valide si chaque destination est compatible. | À qualifier — aucune preuve individuelle rattachée |
| VAL-008 | Croisement de deux fils sans binding partagé. | Aucune connexion ajoutée. | À qualifier — aucune preuve individuelle rattachée |
| VAL-009 | Fil direct entre deux régions sans frontière explicite. | Erreur de portée. | À qualifier — aucune preuve individuelle rattachée |
| VAL-010 | Fragment de fil incomplet dans une région active. | Enregistrement possible ; validation exécutable bloquée. | À qualifier — aucune preuve individuelle rattachée |
| VAL-011 | Sortie ordinaire valide sans consommateur. | Pas d’erreur du seul fait de son inutilisation. | À qualifier — aucune preuve individuelle rattachée |
| VAL-012 | Nœud isolé avec une entrée requise vide. | Erreur ; ne pas l’effacer implicitement pour autoriser Run. | À qualifier — aucune preuve individuelle rattachée |
| VAL-013 | Entrée Required absente. | Erreur bloquante. | À qualifier — aucune preuve individuelle rattachée |
| VAL-014 | Entrée Optional absente avec défaut contractuel valide. | Valide ; valeur par défaut définie. | À qualifier — aucune preuve individuelle rattachée |
| VAL-015 | Entrée Recommended absente mais contrat complet. | Valide, avertissement possible. | À qualifier — aucune preuve individuelle rattachée |
| VAL-016 | Port présent graphiquement mais absent du contrat effectif. | Erreur, pas de port inventé par le renderer. | À qualifier — aucune preuve individuelle rattachée |
| VAL-017 | Connexion avec coercition numérique autorisée. | Valide ; diagnostic ou point de coercition approprié. | À qualifier — aucune preuve individuelle rattachée |
| VAL-018 | Connexion avec conversion non autorisée. | Erreur de type. | À qualifier — aucune preuve individuelle rattachée |
| VAL-019 | Type générique non résolu en fin de validation. | Blocage explicite, pas de repli implicite vers f64. | À qualifier — aucune preuve individuelle rattachée |
| VAL-020 | Deux formes fixes incompatibles pour une opération qui les exige identiques. | Erreur avant exécution. | À qualifier — aucune preuve individuelle rattachée |
| VAL-021 | Deux tailles dynamiques pouvant différer. | Validation possible si le nœud prévoit un contrôle runtime. | À qualifier — aucune preuve individuelle rattachée |
| VAL-022 | Producteur câblé mais lui-même invalide. | Consommateur bloqué par la cause racine, pas déclaré prêt. | À qualifier — aucune preuve individuelle rattachée |
| VAL-023 | Case booléenne avec sélecteur valide et sorties définies dans Vrai et Faux. | Valide. | À qualifier — aucune preuve individuelle rattachée |
| VAL-024 | Case sans sélecteur. | MISSING_STRUCTURE_SELECTOR. | À qualifier — aucune preuve individuelle rattachée |
| VAL-025 | Case booléenne avec branche Faux absente. | MISSING_CASE_REGION. | À qualifier — aucune preuve individuelle rattachée |
| VAL-026 | Case booléenne avec deux branches Vrai. | DUPLICATE_CASE_MATCH ou cardinalité invalide. | À qualifier — aucune preuve individuelle rattachée |
| VAL-027 | Case texte sans branche default. | MISSING_DEFAULT_REGION. | À qualifier — aucune preuve individuelle rattachée |
| VAL-028 | Case texte avec deux branches default. | Erreur de sélection ambiguë. | À qualifier — aucune preuve individuelle rattachée |
| VAL-029 | Case texte avec deux correspondances identiques. | DUPLICATE_CASE_MATCH. | À qualifier — aucune preuve individuelle rattachée |
| VAL-030 | Sortie définie dans Vrai mais absente dans Faux. | UNBOUND_REGION_OUTPUT. | À qualifier — aucune preuve individuelle rattachée |
| VAL-031 | Branche default présente mais sa sortie result non produite. | UNBOUND_REGION_OUTPUT : default de sélection ne couvre pas la sortie. | À qualifier — aucune preuve individuelle rattachée |
| VAL-032 | Deux sorties ; une seule est couverte dans toutes les branches. | Structure invalide pour la sortie restante. | À qualifier — aucune preuve individuelle rattachée |
| VAL-033 | Branche affichée correcte, autre branche masquée incorrecte. | Structure et Run marqués invalides. | À qualifier — aucune preuve individuelle rattachée |
| VAL-034 | Sortie incomplète sans aucun consommateur extérieur. | Erreur tant que cette sortie reste déclarée. | À qualifier — aucune preuve individuelle rattachée |
| VAL-035 | Case sans sortie de données et deux branches valides. | Valide, sous réserve du sélecteur et des autres contrats. | À qualifier — aucune preuve individuelle rattachée |
| VAL-036 | Entrée de frontière fournie mais inutilisée dans une des branches. | Pas d’erreur du seul fait de cette inutilisation. | À qualifier — aucune preuve individuelle rattachée |
| VAL-037 | Case interne incomplète alimentant une sortie externe. | Erreur propagée jusqu’à la structure externe. | À qualifier — aucune preuve individuelle rattachée |
| VAL-038 | Sélecteur constant mais autre branche ordinaire incomplète. | Erreur dans la politique stricte proposée ; aucune exclusion implicite. | À qualifier — aucune preuve individuelle rattachée |
| VAL-039 | Sortie absente dans Faux avec type connu et défaut explicite valide. | Valide ; la valeur de remplacement est présente dans le sens validé. | À qualifier — aucune preuve individuelle rattachée |
| VAL-040 | Défaut activé mais type de sortie non résolu. | UNRESOLVED_TYPE ; le drapeau ne suffit pas. | À qualifier — aucune preuve individuelle rattachée |
| VAL-041 | Valeur de défaut incompatible avec le type de sortie. | INVALID_OUTPUT_DEFAULT. | À qualifier — aucune preuve individuelle rattachée |
| VAL-042 | Défaut activé et fil incompatible déjà présent dans la branche. | Erreur ; ne pas ignorer le fil. | À qualifier — aucune preuve individuelle rattachée |
| VAL-043 | Défaut activé et deux producteurs dans la branche. | Erreur de cardinalité ; pas de choix arbitraire. | À qualifier — aucune preuve individuelle rattachée |
| VAL-044 | Défaut activé et fragment incomplet associé au binding. | Erreur d’édition incomplète, pas de remplacement silencieux. | À qualifier — aucune preuve individuelle rattachée |
| VAL-045 | Default booléen false. | Valeur présente et valide, à ne pas confondre avec champ absent. | À qualifier — aucune preuve individuelle rattachée |
| VAL-046 | Default numérique zéro. | Valeur présente et valide, à ne pas traiter comme absence. | À qualifier — aucune preuve individuelle rattachée |
| VAL-047 | Default chaîne vide. | Valeur présente si le type l’autorise. | À qualifier — aucune preuve individuelle rattachée |
| VAL-048 | Tableau de taille fixe avec défaut tableau vide. | Erreur de forme, sauf définition de type explicitement différente. | À qualifier — aucune preuve individuelle rattachée |
| VAL-049 | Type de ressource sans défaut constructible. | Défaut refusé ; producteur ou type nullable explicite requis. | À qualifier — aucune preuve individuelle rattachée |
| VAL-050 | Suppression du mode défaut avec branches non câblées. | Les branches deviennent immédiatement invalides. | À qualifier — aucune preuve individuelle rattachée |
| VAL-051 | Ajout d’une branche avec politique stricte. | Nouvelle cellule de sortie non couverte ; erreur jusqu’à complétion. | À qualifier — aucune preuve individuelle rattachée |
| VAL-052 | Ajout d’une branche avec politique persistante valide. | Défaut applicable selon le contrat, visible et sérialisé. | À qualifier — aucune preuve individuelle rattachée |
| VAL-053 | Round-trip d’une politique de défaut. | Même résultat, même type et même couverture après réouverture. | À qualifier — aucune preuve individuelle rattachée |
| VAL-054 | Commande ponctuelle de génération de defaults puis ajout d’une branche. | Ne pas appliquer une politique persistante qui n’a jamais été déclarée. | À qualifier — aucune preuve individuelle rattachée |
| VAL-055 | Sélecteur entier dans un lecteur ne supportant que bool/string. | Refus explicite du profil ou du type ; pas de conversion cachée. | À qualifier — aucune preuve individuelle rattachée |
| VAL-056 | Sélecteur entier avec intervalles non chevauchants et default. | Valide après normalisation du contrat étendu. | À qualifier — aucune preuve individuelle rattachée |
| VAL-057 | Deux intervalles de cas qui se chevauchent sans règle de priorité. | OVERLAPPING_CASE_MATCH. | À qualifier — aucune preuve individuelle rattachée |
| VAL-058 | Valeur de cas hors plage du type du sélecteur. | Erreur statique. | À qualifier — aucune preuve individuelle rattachée |
| VAL-059 | Enum exhaustif dont on ajoute un élément, sans default. | Revalidation ; couverture insuffisante tant qu’aucune branche ne couvre le nouvel élément. | À qualifier — aucune preuve individuelle rattachée |
| VAL-060 | Enum avec default autorisé et nouveau membre. | Couverture de sélection conservée ; sorties du default toujours vérifiées. | À qualifier — aucune preuve individuelle rattachée |
| VAL-061 | Constante flottante reliée à une case entière par arrondi non explicite. | Refus si le contrat ne prévoit pas cette adaptation. | À qualifier — aucune preuve individuelle rattachée |
| VAL-062 | For de base sans count. | MISSING_LOOP_COUNT. | À qualifier — aucune preuve individuelle rattachée |
| VAL-063 | For avec count valide et corps vide sans sortie. | Valide. | À qualifier — aucune preuve individuelle rattachée |
| VAL-064 | For avec count constant négatif. | INVALID_LOOP_COUNT avant exécution. | À qualifier — aucune preuve individuelle rattachée |
| VAL-065 | For avec count dynamique devenant négatif. | Contrôle runtime selon le contrat, sans lancer des itérations invalides. | À qualifier — aucune preuve individuelle rattachée |
| VAL-066 | For pouvant exécuter zéro tour, sortie last_value sans zero_iteration_value. | MISSING_ZERO_ITERATION_VALUE. | À qualifier — aucune preuve individuelle rattachée |
| VAL-067 | For count = 0 avec zero_iteration_value valide. | La sortie vaut cette valeur et non un contenu mémoire indéfini. | À qualifier — aucune preuve individuelle rattachée |
| VAL-068 | For count = 3 avec sortie body complète et fallback valide. | Sortie de la dernière itération ; fallback non utilisé. | À qualifier — aucune preuve individuelle rattachée |
| VAL-069 | Fallback zéro itération valide mais sortie absente dans le corps. | Erreur de couverture du corps. | À qualifier — aucune preuve individuelle rattachée |
| VAL-070 | Fallback zéro itération de type incompatible. | Erreur de valeur ou de type. | À qualifier — aucune preuve individuelle rattachée |
| VAL-071 | For count = 0 contenant un nœud invalide dans le corps. | Erreur dans la politique stricte ; le compte zéro ne désactive pas le programme. | À qualifier — aucune preuve individuelle rattachée |
| VAL-072 | Deux régions body dans une For de base. | Erreur structurelle. | À qualifier — aucune preuve individuelle rattachée |
| VAL-073 | Écriture vers le terminal index en lecture seule. | Erreur de direction ou de contrat. | À qualifier — aucune preuve individuelle rattachée |
| VAL-074 | Index non utilisé. | Valide. | À qualifier — aucune preuve individuelle rattachée |
| VAL-075 | For auto-indexée sans count sous un profil qui l’autorise. | Compte dérivé conformément au contrat ; pas de faux mandatory. | À qualifier — aucune preuve individuelle rattachée |
| VAL-076 | Collecte conditionnelle sans aucun élément retenu. | Sortie vide typée, et non dernière valeur indéfinie. | À qualifier — aucune preuve individuelle rattachée |
| VAL-077 | Sortie anticipée et tableau annoncé de taille fixe non garantie. | Erreur de contrat de forme ou type dynamique requis. | À qualifier — aucune preuve individuelle rattachée |
| VAL-078 | While post-test sans producteur de condition. | MISSING_LOOP_CONDITION. | À qualifier — aucune preuve individuelle rattachée |
| VAL-079 | While de base avec condition false constante. | Exactement une itération normale. | À qualifier — aucune preuve individuelle rattachée |
| VAL-080 | While de base avec condition true constante. | Valide au sens du langage ; avertissement de non-terminaison possible. | À qualifier — aucune preuve individuelle rattachée |
| VAL-081 | Condition calculée par une case interne incomplète. | While invalide par propagation. | À qualifier — aucune preuve individuelle rattachée |
| VAL-082 | Présentation Stop if True sans inversion/normalisation de la polarité publiée. | Divergence à détecter par test de sémantique. | À qualifier — aucune preuve individuelle rattachée |
| VAL-083 | While post-test avec sortie définie et sans fallback zéro itération. | Valide pour ce point précis. | À qualifier — aucune preuve individuelle rattachée |
| VAL-084 | While pré-test d’un profil futur avec sortie sans cas zéro. | Erreur si zéro tour est possible et non défini. | À qualifier — aucune preuve individuelle rattachée |
| VAL-085 | Delay sans champ initial. | MISSING_STATE_INITIAL. | À qualifier — aucune preuve individuelle rattachée |
| VAL-086 | Delay avec initial de type incompatible. | Erreur de type ou de valeur. | À qualifier — aucune preuve individuelle rattachée |
| VAL-087 | Delay correctement initialisé dans une récurrence valide. | Cycle temporel autorisé sous réserve des autres règles. | À qualifier — aucune preuve individuelle rattachée |
| VAL-088 | Cycle A → B → A sans mémoire. | INVALID_COMBINATIONAL_CYCLE. | À qualifier — aucune preuve individuelle rattachée |
| VAL-089 | Composante avec Delay mais sous-cycle A ↔ B sans Delay. | INVALID_COMBINATIONAL_CYCLE malgré la présence d’une mémoire dans la composante. | À qualifier — aucune preuve individuelle rattachée |
| VAL-090 | Nœud stateful non temporel dans un cycle. | Ne rend pas le cycle légal par sa seule classification. | À qualifier — aucune preuve individuelle rattachée |
| VAL-091 | Cycle combinatoire dessiné à l’intérieur d’une boucle. | Toujours invalide ; la bordure ne fournit pas de mémoire. | À qualifier — aucune preuve individuelle rattachée |
| VAL-092 | Branche qui ne met pas à jour un état et ne définit pas de hold/passthrough. | Erreur de prochain état incomplet selon le contrat de la mémoire. | À qualifier — aucune preuve individuelle rattachée |
| VAL-093 | Deux instances indépendantes du même sous-FROG avec mémoire. | États distincts conformément au contrat, pas d’alias caché. | À qualifier — aucune preuve individuelle rattachée |
| VAL-094 | Région explicitement désactivée avec erreur sémantique interne. | Ne bloque pas la variante active si le profil d’exclusion le définit ; erreur conservée pour inspection. | À qualifier — aucune preuve individuelle rattachée |
| VAL-095 | Même région simplement masquée graphiquement. | Son erreur reste bloquante. | À qualifier — aucune preuve individuelle rattachée |
| VAL-096 | Diagram Disable sans région active dans le contrat proposé. | Configuration invalide. | À qualifier — aucune preuve individuelle rattachée |
| VAL-097 | Diagram Disable avec deux régions actives. | Configuration ambiguë. | À qualifier — aucune preuve individuelle rattachée |
| VAL-098 | Région active de Disable avec sortie non couverte. | UNBOUND_REGION_OUTPUT. | À qualifier — aucune preuve individuelle rattachée |
| VAL-099 | Conditional Disable avec symbole requis inconnu. | UNKNOWN_COMPILE_SYMBOL. | À qualifier — aucune preuve individuelle rattachée |
| VAL-100 | Plusieurs conditions de compilation vraies sans priorité définie. | AMBIGUOUS_COMPILE_SELECTION. | À qualifier — aucune preuve individuelle rattachée |
| VAL-101 | Dépendance uniquement présente dans une variante exclue. | Ne bloque pas la construction active ; contrôlable par audit toutes variantes. | À qualifier — aucune preuve individuelle rattachée |
| VAL-102 | Changement du symbole activant une branche incorrecte. | Invalidation et nouveau diagnostic bloquant. | À qualifier — aucune preuve individuelle rattachée |
| VAL-103 | Structure Event avec timeout optionnel non câblé. | Valide si l’attente indéfinie est définie par le contrat. | À qualifier — aucune preuve individuelle rattachée |
| VAL-104 | Cas Timeout dont une sortie commune est absente. | Erreur ou défaut explicitement autorisé, comme tout cas terminant normalement. | À qualifier — aucune preuve individuelle rattachée |
| VAL-105 | Lecture d’un champ de données propre à un autre événement. | Erreur de portée/type. | À qualifier — aucune preuve individuelle rattachée |
| VAL-106 | Séquence utilisant une donnée seulement produite dans un cadre futur. | Erreur de dépendance ou de portée. | À qualifier — aucune preuve individuelle rattachée |
| VAL-107 | Timed Loop avec période statiquement interdite. | Erreur de configuration. | À qualifier — aucune preuve individuelle rattachée |
| VAL-108 | Timed Loop correcte sur cible sans garantie requise. | Blocage de capacité, sans accuser un fil valide. | À qualifier — aucune preuve individuelle rattachée |
| VAL-109 | Structure In Place avec paire de bornes incohérente. | Erreur de contrat de frontière. | À qualifier — aucune preuve individuelle rattachée |
| VAL-110 | Référence temporaire sortant de sa durée de vie autorisée. | Erreur de portée/ressource. | À qualifier — aucune preuve individuelle rattachée |
| VAL-111 | Région parallèle avec deux producteurs non arbitrés d’une sortie. | Erreur ; pas de fusion implicite. | À qualifier — aucune preuve individuelle rattachée |
| VAL-112 | Add déplié avec a et b valides et six emplacements optionnels vides. | Valide ; géométrie ignorée pour l’arité active. | À qualifier — aucune preuve individuelle rattachée |
| VAL-113 | Add avec a valide et b absent. | Entrée requise manquante. | À qualifier — aucune preuve individuelle rattachée |
| VAL-114 | Add avec input_3 connecté mais de type invalide. | Erreur ; operand actif non ignoré. | À qualifier — aucune preuve individuelle rattachée |
| VAL-115 | Multiply variadique dans un lecteur dont le contrat reste binaire. | Refus explicite ou migration documentée ; pas d’omission d’opérande. | À qualifier — aucune preuve individuelle rattachée |
| VAL-116 | Appel dont le sous-FROG actif possède une erreur. | BROKEN_DEPENDENCY avec accès à la cause racine. | À qualifier — aucune preuve individuelle rattachée |
| VAL-117 | Fichier de fonction invalide mais non référencé par l’entrée lancée. | Ne bloque pas cette entrée ; peut bloquer un build global demandé. | À qualifier — aucune preuve individuelle rattachée |
| VAL-118 | Interface appelée modifiée après la validation de l’appelant. | Invalidation transitive et revalidation des ports. | À qualifier — aucune preuve individuelle rattachée |
| VAL-119 | Chemin d’appel statique introuvable. | MISSING_DEPENDENCY. | À qualifier — aucune preuve individuelle rattachée |
| VAL-120 | Sortie publique promise mais sans producteur ni politique explicite. | Erreur d’interface, même si aucun appelant n’est actuellement ouvert. | À qualifier — aucune preuve individuelle rattachée |
| VAL-121 | Widget supprimé encore référencé par le diagramme. | Erreur de référence sémantique. | À qualifier — aucune preuve individuelle rattachée |
| VAL-122 | Écriture vers une propriété UI en lecture seule. | Erreur de contrat. | À qualifier — aucune preuve individuelle rattachée |
| VAL-123 | Icône de palette présente, fournisseur runtime absent. | Pas de faux statut runnable. | À qualifier — aucune preuve individuelle rattachée |
| VAL-124 | Ajout d’une branche qui rend une sortie incomplète. | Run ne conserve pas l’ancien succès. | À qualifier — aucune preuve individuelle rattachée |
| VAL-125 | Undo de cet ajout. | Couverture et état restaurés, après contrôle de la bonne révision. | À qualifier — aucune preuve individuelle rattachée |
| VAL-126 | Déplacement d’un coude sans modification de binding. | Aucun changement de sens ni de validité. | À qualifier — aucune preuve individuelle rattachée |
| VAL-127 | Déplacement d’un nœud à travers une frontière structurelle. | Transaction sémantique et revalidation de portée. | À qualifier — aucune preuve individuelle rattachée |
| VAL-128 | Validation de R terminant après édition de R+1. | Résultat marqué ancien ; ne valide pas R+1. | À qualifier — aucune preuve individuelle rattachée |
| VAL-129 | Cible changée après un résultat de validation positif. | Capacités et variantes revalidées. | À qualifier — aucune preuve individuelle rattachée |
| VAL-130 | Artefact construit pour une ancienne révision. | ARTIFACT_REVISION_MISMATCH ; pas de lancement silencieux. | À qualifier — aucune preuve individuelle rattachée |
| VAL-131 | Programme valide mais runtime non disponible. | Blocage de lancement distinct de la validité du langage. | À qualifier — aucune preuve individuelle rattachée |
| VAL-132 | Avertissement de précision sans politique stricte. | Pas de blocage sémantique automatique. | À qualifier — aucune preuve individuelle rattachée |
| VAL-133 | Profil strict transformant une coercition risquée en refus explicite. | Refus cohérent dans Studio, CLI et backend. | À qualifier — aucune preuve individuelle rattachée |
| VAL-134 | Sortie non liée injectée artificiellement dans le FIR. | Backend refuse ; aucune valeur indéfinie de remplacement. | À qualifier — aucune preuve individuelle rattachée |
| VAL-135 | Structure inconnue sans validateur enregistré. | Unsupported ou erreur explicite ; jamais valid par défaut. | À qualifier — aucune preuve individuelle rattachée |
| VAL-136 | Annulation d’une exécution avant terminaison normale. | Aucun résultat de succès artificiel ; protocole d’annulation. | À qualifier — aucune preuve individuelle rattachée |
| VAL-137 | Round-trip d’un graphe incomplet avec wire_fragments. | Travail conservé sans devenir exécutable artificiellement. | À qualifier — aucune preuve individuelle rattachée |
