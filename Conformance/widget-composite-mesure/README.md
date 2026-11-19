# Créer un widget composite Mesure

Mesure réunit un Text Button, un Numeric Indicator DBL et une LED ronde booléenne. Le package et son générateur de montage sont distribués sous Apache-2.0. Le montage `Mesure.frog` peut être régénéré avec `python generate.py` (Python 3.10 ou ultérieur). Pour créer ou installer le package dans Studio, un éditeur de texte suffit.

## Écrire le package

1. Copiez `Mesure.wfrog` dans un nouveau fichier UTF-8. Renseignez `package.id`, `version`, `name` et `license`. Donnez une identité à la classe dans `composite_classes` et exportez-la dans `exports.composite_classes`.
2. Déclarez chaque enfant dans `children`. Choisissez une identité stable et indiquez ses classe, rôle, type, réalisation, asset et position relative. Les dimensions sont en unités du document à 100 %. Pour le bouton, le libellé devient le texte de ses deux états.
3. Dans `imports`, listez chacune des réalisations natives utilisées. Dans `bindings`, exposez exactement une liaison par enfant. Le nom de la liaison est visible dans Studio sous la forme `Nom du package / identité de la liaison`.

| Enfant | Identité de classe | Réalisation et asset | Liaison |
| --- | --- | --- | --- |
| Text Button | `frog.widgets.button` | `button.default.wfrog`, `asset:button_rectangular_svg` | `activation`, `Value Change`, événement booléen |
| Numeric Indicator | `frog.widgets.numeric_indicator` | `numeric.default.wfrog`, `asset:numeric_rectangular_svg` | `mesure`, `Value`, entrée `f64` |
| LED ronde | `frog.widgets.boolean_indicator` | `boolean.default.wfrog`, `asset:boolean_circular_svg` | `etat`, `Value`, entrée `bool` |

Le profil `frog.widget.composite.native-parts@1` utilise `wfrog_version: "0.1"` et `package_kind: "widget_composite_library"`. Il accepte un seul export, deux à seize enfants, des bornes entières de 16 unités minimum dans un espace relatif de 4096 × 4096 et un fichier de 128 Kio maximum. Les identités et la version sont composées de minuscules ASCII, chiffres, points, traits d’union ou traits de soulignement. Les boutons utilisent une mécanique `switch_when_pressed` ou `switch_when_released`. Les indicateurs ont une action vide. Les réalisations sont fournies avec Studio, sous les identités complètes figurant dans le package. Les scripts, SVG propres au package et classes composites imbriquées sont hors profil. Les packages de réalisation conservent leur parcours existant.

## Création manuelle depuis la palette

1. Dans Studio, ouvrez la face avant et choisissez **Tools → Install Local Widget…**. Sélectionnez le fichier `.wfrog`.
2. Ouvrez **Widgets → Classic → Data Containers**, sélectionnez **Mesure**, puis posez-le à la racine de la face avant. Studio crée un groupe déplaçable d’enfants natifs. Le diagramme reçoit leurs terminaux habituels.
3. Passez au diagramme. Dans la palette Functions, choisissez **Structures → While Loop**, puis tracez la boucle. Déplacez les terminaux de l’afficheur et de la LED à l’intérieur. Choisissez **Structures → Event Structure** et tracez-la autour de ces deux terminaux, dans la boucle. Le terminal du bouton reste à l’extérieur : son événement est abonné par la structure Event.
4. Dans le menu de la structure Event, ouvrez **Edit Events Handled by This Case…**. Sélectionnez **Mesure / activation**, avec **Value Change**, et validez. Conservez un seul spécificateur pour ce bouton et cet événement ; n’en ajoutez pas un second. Sur le nœud de données d’événement à gauche, cliquez sur la ligne **Time**, puis choisissez **NewVal**. Cette commande remplace le champ Time ; Source et Type peuvent rester affichés.
5. Déposez **Comparison → Select** dans le cas événementiel. Reliez `NewVal` au terminal booléen de la LED et au terminal `select` de Select. Depuis le menu du terminal vrai de Select, choisissez **Create Constant** et saisissez `24.68`. Faites de même avec `12.34` sur son terminal faux. Reliez la sortie à l’afficheur. Depuis le terminal conditionnel d’arrêt de la boucle While, créez une constante **False**. La sélection numérique et l’état de la LED appartiennent ainsi au diagramme.
6. Lancez **Run**. Effectuez deux appuis distincts sur **Mesurer**. Le premier donne 24,68 et allume la LED. Le second donne 12,34 et éteint la LED. Les valeurs initiales sont 0 et False. Utilisez la commande **Stop** de Studio à la fin de l’essai.
7. Sauvegardez, fermez et rouvrez le document, puis répétez Run et les deux appuis. Le fichier enregistre le groupe, ses enfants, les six fils du montage et l’identité exacte du package. En cas de package manquant, réinstallez le fichier d’origine pour retrouver cette identité. Conservez une copie de chaque version utilisée par vos documents.
8. Construisez une application avec face avant. Déplacez le dossier produit dans son ensemble, fermez Studio puis lancez l’EXE et répétez les deux appuis. Le package d’origine figure dans `widgets/`. Les ressources SVG nécessaires à l’hôte graphique viennent des réalisations natives capturées à la construction.

Le parcours manuel, jusqu’aux deux appuis après Save/Open, a été vérifié dans Studio **0.0.8.524**. Dans certaines présentations de la palette, l’entrée **Data** ouvre le panneau intitulé **Data Containers**. Si la palette se referme avant le choix d’une fonction, son épingle permet de la conserver ouverte.

## Montage généré redistribuable

`Mesure.frog` est un témoin éditable déjà câblé. `python generate.py` le reproduit à partir du package ; cette commande exige Python 3.10 ou ultérieur, mais l’installation et le dépôt dans Studio n’en dépendent pas. Ouvrir ce fichier permet d’examiner la logique et ses liaisons. Cela ne remplace pas la reconstruction manuelle décrite ci-dessus.

## Application exportée

L’EXE est un troisième artefact, produit par **Build** depuis le document courant. Le fichier `.frog` et le package seuls ne constituent pas une application Windows exportée. Pour qualifier votre propre montage, effectuez le parcours de l’étape 8 avec les deux appuis, puis Stop. Déplacez toujours le dossier entier : il contient l’EXE, son inventaire de publication et ses dépendances, dont `widgets/`.

Le montage généré a été exécuté dans un véritable EXE déplacé, avec Studio fermé, à partir de Studio **0.0.8.364**. Cette preuve concerne ce montage généré. La reconstruction manuelle complète a ensuite été vérifiée dans **0.0.8.524** : Run, Save/Open et deux appuis, puis deux Build à la même destination après déplacement du premier export. Le véritable EXE de ce montage manuel a également été déplacé et testé avec Studio fermé : premier appui 24,68/LED allumée, second 12,34/LED éteinte, puis Stop. Ces deux réalisations conservent des preuves distinctes.

Après déplacement d’un export terminé, un nouveau Build doit pouvoir republier vers la même destination depuis le même document. Ne copiez pas le `.frog` et ne changez pas de destination pour contourner un refus. Un dossier préexistant sans inventaire d’appartenance reste protégé.

Si le package exact manque sur un poste d’édition, le document et les liaisons sont conservés ; la disponibilité doit signaler la dépendance et bloquer Run et Build. Réinstallez la même version et la même empreinte par **Tools → Install Local Widget…**, puis vérifiez à nouveau la disponibilité.
