# Execution diagnostics in Graiphic Studio

Revision 1.0 — 2026-09-16. This is the IDE presentation contract, not a new
language profile. Semantic authority belongs to
[Execution eligibility](../Language/Execution%20eligibility.md).

## Lecture et blocages

- Le bouton **Lecture / Run** conserve son triangle. Un sens interdit rouge est
  **superposé** lorsque la validation, la compilation ou le lancement est bloqué.
- Le deuxième bouton, **Run continuously**, garde son icône normale. Il n'est pas
  utilisé comme indicateur d'erreur. Cela n'autorise pas à contourner la validation.
- Lecture reste cliquable pour consulter les diagnostics, y compris pour une
  source publique dont le validateur/lowerer n'est pas disponible.
- Le clic sur Lecture bloqué ouvre une fenêtre d'explication ; il ne démarre pas
  un ancien artefact ni un programme incomplet.
- Front Panel et Block Diagram consultent le même résultat du document courant.

## Fenêtre de diagnostics

Chaque ligne indique l'impact : compilation, exécution, les deux, ou non bloquant.
La sélection affiche le motif, le code stable et, lorsque disponibles, le terminal,
la structure et la région concernés.

Les motifs bloquants restent visibles, même s'ils ont la sévérité « avertissement ».
La case des messages non bloquants ne doit jamais masquer la raison du refus.
Une cible indisponible est explicitement distinguée d'un fil erroné.

**Voir l'erreur** ou un double-clic sur une ligne localisable révèle la région
concernée et centre/sélectionne l'objet. La navigation ne modifie pas les câbles,
les valeurs par défaut ou la sélection de compilation. Un diagnostic global
(runtime absent, par exemple) n'invente pas de position dans le diagramme.

La fenêtre est réutilisée par document et se rafraîchit après édition, undo/redo
ou changement du modèle. Une ligne ancienne doit être actualisée avant navigation.
Ouvrir la fenêtre libère la capture souris du bouton de la barre d'outils pour
que la liste soit immédiatement interactive.

## Interprétation

Le symbole signifie **demande d'exécution indisponible**, pas nécessairement
« programme mal écrit ». En particulier, une version de Studio sans runtime peut
valider et éditer un diagramme sans pouvoir l'exécuter.

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
