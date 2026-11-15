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

## Installer, placer et câbler

1. Dans Studio, ouvrez la face avant et choisissez **Tools → Install Local Widget…**. Sélectionnez le fichier `.wfrog`.
2. Ouvrez **Widgets → Classic → Data Containers**, sélectionnez **Mesure**, puis posez-le à la racine de la face avant. Studio crée un groupe déplaçable d’enfants natifs. Le diagramme reçoit leurs terminaux habituels.
3. Placez une boucle While et une structure Event. Configurez **Mesure / activation → Value Change**. Déplacez les terminaux des deux indicateurs dans ce cas. Lisez `NewVal` du nœud de données d’événement et reliez-le à la LED et au sélecteur de **Select**. Reliez `24.68` à l’entrée vraie, `12.34` à l’entrée fausse et le résultat à l’afficheur. Câblez une constante False au terminal d’arrêt de la boucle et utilisez la commande Stop de Studio à la fin de l’essai.
4. Lancez **Run**. Cliquez deux fois sur **Mesurer**. Le premier appui donne 24,68 et allume la LED. Le second donne 12,34 et éteint la LED. Les valeurs initiales du montage sont 0 et False.
5. Sauvegardez, fermez et rouvrez le document. Le fichier enregistre le groupe, ses enfants, le diagramme et l’identité exacte du package. En cas de package manquant, réinstallez le fichier d’origine pour retrouver cette identité. Conservez une copie de chaque version utilisée par vos documents.
6. Construisez une application avec face avant. Déplacez le dossier produit dans son ensemble, fermez Studio puis lancez l’EXE et répétez les deux appuis. Le package d’origine figure dans `widgets/`. Les ressources SVG nécessaires à l’hôte graphique viennent des réalisations natives capturées à la construction.

Le diagramme de démonstration est également disponible directement dans `Mesure.frog`.
