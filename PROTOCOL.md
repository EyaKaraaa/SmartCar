# Protocole Bluetooth SPP

Service local Bluetooth Classic ; appareil `SMART_CAR`. Chaque commande est un caractère ASCII. Les minuscules sont acceptées. Les fins de ligne et caractères inconnus sont ignorés et ne renouvellent pas le délai de liaison.

| Commande | Effet |
|---|---|
| F | Avant en manuel, uniquement si mesure valide et distance > 25 cm |
| B | Arrière en manuel, sans détection arrière |
| L | Pivot gauche : roues gauches arrière, droites avant |
| R | Pivot droite : roues gauches avant, droites arrière |
| S | Stop, désactivation automatique, retour manuel |
| M | Manuel arrêté |
| A | Entrée automatique, réinitialisation de la séquence |
| K | Heartbeat ; aucune commande de mouvement |

Les directions F/B/L/R sont ignorées en automatique. Pour reprendre la main, envoyer M puis une direction. S ou M interrompt une séquence et vide les commandes déjà en attente ; envoyer une nouvelle direction dans un envoi ultérieur.

## Temporisations

- K toutes les 250 ms, dans les deux modes.
- Direction manuelle renouvelée toutes les 250 ms tant que le bouton est maintenu.
- Sans direction valide depuis plus de 700 ms : arrêt manuel.
- Sans commande reconnue de maintien depuis plus de 1 500 ms : arrêt et manuel.
- Déconnexion détectée : arrêt et manuel.
- Au rétablissement de la liaison, aucun mouvement automatique n'est repris.

La mesure ultrason est demandée toutes les 80 ms et l'attente ECHO est bornée à 25 ms. Une absence d'écho est une mesure invalide, pas une distance libre. Une mesure ancienne de plus de 300 ms est invalide. Une transition de sens peut ajouter une pause de 20 ms ; ce firmware n'est pas un système temps réel à échéances garanties.

## Automatique

Distance > 25 cm : avancer. Sinon : stop 200 ms, recul 300 ms, stop 150 ms, pivot droite 450 ms, stop 150 ms, puis nouvelle décision. Une mesure invalide interrompt la manœuvre et remet la séquence à son point initial, à l'arrêt jusqu'au retour d'une mesure valide.

## Télémétrie

Une ligne environ toutes les 500 ms :

```text
MODE=AUTO;DIST_CM=42.5;MOTION=1,1
MODE=MANUAL;DIST_CM=INVALID;MOTION=0,0
```

MOTION contient la commande logique gauche,droite (-1,0,1), pas une vitesse réellement mesurée. L'application peut ignorer la télémétrie ; la lire régulièrement évite d'accumuler des données côté téléphone.
