# Câblage proposé - à comparer au montage réel

Ce tableau est une proposition pour ESP32-WROOM-32 DevKit sans PSRAM, pas une lecture certaine des fils de la photo. Modifier `config.h` ou recâbler hors tension pour obtenir une correspondance exacte.

## Moteurs : un canal par moteur

L298N gauche : canal A = avant gauche, canal B = arrière gauche.
L298N droit : canal A = avant droit, canal B = arrière droit.

| Signal module | GPIO ESP32 |
|---|---:|
| Gauche IN1 | 13 |
| Gauche IN2 | 14 |
| Gauche IN3 | 16 |
| Gauche IN4 | 17 |
| Droit IN1 | 18 |
| Droit IN2 | 19 |
| Droit IN3 | 21 |
| Droit IN4 | 22 |
| HC-SR04 TRIG | 25 |
| HC-SR04 ECHO via diviseur | 34 |
| LED AUTO via 330 Ω | 26 |

Relier chaque moteur aux sorties du canal correspondant. Conserver les cavaliers **ENA/ENB** des deux modules pour cette version à vitesse fixe. Ne pas confondre ces cavaliers avec celui du régulateur `5V-EN`.

Un moteur tourne à l'envers : hors tension, inverser ses deux fils OU passer son champ `inverted` à `true` dans `config.h`. Vérifier chaque roue indépendamment, roues soulevées. En marche avant, les quatre roues doivent propulser le châssis vers l'avant malgré leur montage en miroir.

## HC-SR04 et adaptation ECHO

- VCC capteur : alimentation régulée 5 V ; GND commun.
- TRIG : GPIO25.
- ECHO du capteur -> résistance **1 kΩ** -> point milieu -> GPIO34.
- Point milieu -> résistance **2 kΩ** -> GND.

Le pont diviseur transforme environ 5 V en 3,33 V. Ne pas relier directement ECHO 5 V à une entrée ESP32. Le GPIO34 est une entrée uniquement.

## Alimentation

- Les moteurs sont alimentés par la batterie, à travers les L298N, jamais par une sortie ESP32.
- Choisir la tension selon les moteurs et la chute de tension du driver ; vérifier le courant de démarrage/blocage et l'échauffement.
- Alimenter l'ESP32 par USB pour les premiers essais, ou par un régulateur adapté à l'entrée de la carte utilisée.
- Utiliser une alimentation 5 V régulée adaptée pour les circuits qui la nécessitent.
- Relier les masses ESP32, capteur, drivers et alimentation moteur.
- Ne pas relier deux sorties 5 V de régulateurs entre elles. Ne pas injecter le 5 V d'un L298N vers l'USB/5 V de la carte sans schéma d'alimentation vérifié.
- La configuration `5V-EN` dépend du module et de la tension d'entrée : consulter sa documentation avant connexion. Une photo ne permet pas de déterminer ce réglage.
- Prévoir un interrupteur coupant l'alimentation moteur et protéger la batterie selon ses caractéristiques.

Avec les cavaliers ENA/ENB, les entrées de direction doivent rester définies au démarrage : des résistances de rappel de 10 kΩ vers GND sur les huit IN évitent des entrées flottantes pendant le boot/reset. L'arrêt logiciel ne remplace pas une coupure matérielle.

## Première mise sous tension

1. Moteurs débranchés, vérifier tensions et masse commune au multimètre.
2. Téléverser et confirmer l'arrêt initial.
3. Couper l'alimentation, connecter les moteurs et soulever les roues.
4. Tester des impulsions brèves. Confirmer la polarité de chaque moteur.
5. Régler les durées de recul/pivot seulement après essai physique.
