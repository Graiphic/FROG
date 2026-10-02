# Execution diagnostics in Graiphic Studio

Revision 1.1 — 2026-09-16. This is the IDE presentation contract, not a new
language profile. Semantic authority belongs to
[Execution eligibility](../Language/Execution%20eligibility.md).

## Lecture et blocages

- Le bouton **Lecture / Run** conserve son triangle. Un sens interdit rouge est
  **superposé** lorsque la validation ou la compilation est bloquée ou non prise
  en charge. Une indisponibilité du runtime seule ne dessine pas ce symbole.
  La surimpression vectorielle doit englober le triangle, sans le remplacer,
  sans débordement et avec anticrénelage aux tailles réelles de la barre.
- Le deuxième bouton, **Run continuously**, garde son icône normale. Il n'est pas
  utilisé comme indicateur d'erreur. Cela n'autorise pas à contourner la validation.
- Lecture reste cliquable pour consulter les diagnostics, y compris pour une
  source publique dont le validateur/lowerer n'est pas disponible.
- Le clic sur Lecture bloqué ouvre une fenêtre d'explication ; il ne démarre pas
  un ancien artefact ni un programme incomplet.
- Front Panel et Block Diagram consultent le même résultat du document courant.

## Fenêtre de diagnostics

Chaque ligne indique l'impact : compilation, exécution, les deux, ou non bloquant.
Deux états distincts sont présentés en tête : validation de compilation et
disponibilité d'exécution. « Validation réussie » ne signifie pas qu'un artefact
a déjà été généré, chargé ou exécuté.
La sélection affiche le motif, le code stable et, lorsque disponibles, le terminal,
la structure et la région concernés.

Les motifs bloquants restent visibles, même s'ils ont la sévérité « avertissement ».
La case des messages non bloquants ne doit jamais masquer la raison du refus.
Une cible indisponible est explicitement distinguée d'un fil erroné.
Lorsqu'aucun runtime n'est inclus, un diagnostic global suffit : les avertissements
de capacité d'exécution de chaque primitive ne doivent pas faire croire qu'Add ou
les constantes sont des fonctions invalides. Aucun motif bloquant la compilation
ne doit être supprimé par ce regroupement.

La fenêtre utilise la charte des propriétés / Tools → Options : titre personnalisé,
police et métriques de l'IDE, boutons et sélection thémés, couleurs clair/sombre.
La liste présente le sujet et le motif sur plusieurs lignes ; les détails complets
sont copiables et passent à la ligne. La fenêtre peut être redimensionnée. Tab,
Entrée et Échap restent utilisables.

**Voir l'erreur** ou un double-clic sur une ligne localisable révèle la région
concernée et centre/sélectionne l'objet. La navigation ne modifie pas les câbles,
les valeurs par défaut ou la sélection de compilation. Un diagnostic global
(runtime absent, par exemple) n'invente pas de position dans le diagramme.

La fenêtre est réutilisée par document et se rafraîchit après édition, undo/redo
ou changement du modèle. Une ligne ancienne doit être actualisée avant navigation.
Ouvrir la fenêtre libère la capture souris du bouton de la barre d'outils pour
que la liste soit immédiatement interactive.

## Interprétation

Le symbole signifie **validation / compilation bloquée ou non prise en charge**,
pas simplement « runtime absent ». Une version de Studio sans runtime peut valider
et éditer un diagramme sans pouvoir l'exécuter : Lecture garde alors son triangle
normal et son clic explique la limite d'exécution, sans lancer fictivement le code.

Un document d'édition neuf et vide est un programme sans opération (void), valide
si aucune obligation d'interface n'est laissée insatisfaite. Deux constantes
numériques câblées aux entrées obligatoires d'Add sont également valides lorsque
sa sortie ordinaire n'est pas consommée. En revanche, une projection d'édition
vide ne prouve pas la validité d'une source publique dont la validation est absente.

Les règles officielles sont celles du langage et du profil sélectionné.
La présence d'une fonction dans une palette ne garantit pas son implémentation
sur une cible. Les résultats effectivement vérifiés sont suivis dans la
[matrice de couverture Studio](../docs/execution-eligibility-studio-coverage.md).

## Évolution

Toute modification de cette présentation doit conserver l'accès aux causes,
la séparation sévérité/blocage et la distinction entre erreur du programme et
limitation de cible. Tester au minimum les deux panneaux, les sources publiques,
les thèmes clair/sombre, les différentes tailles d'icône, la navigation dans une
région masquée, l'actualisation après correction et l'indépendance des documents.
