# Validation

## Vérifications exécutées pendant la préparation

Tests C++ hôtes compilés avec g++ (`-std=c++17 -Wall -Wextra -Werror`) et exécutés avec succès. Ils utilisent des substituts de millis(), GPIO, ultrason et Bluetooth.

Commande Linux/macOS avec g++ :

```bash
bash tests/run.sh
```

Scénarios couverts : arrêt initial, directions manuelles, priorité stop, expiration de commande malgré heartbeat, séquence automatique complète, arrêt sans heartbeat, heartbeat tardif sans reprise automatique, absence d'écho, obstacle frontal, déconnexion/reconnexion et débordement de l'horloge 32 bits.

**Non effectué : compilation avec le paquet Arduino-ESP32, téléversement, essais électriques et mécaniques, construction et tests de l'application App Inventor.** Les tests hôtes ne prouvent pas la compatibilité du binaire avec une carte ni la sécurité physique du montage.

## Tests à exécuter sur le prototype

| Test | Manipulation | Résultat attendu | Statut |
|---|---|---|---|
| Alimentation | Mesurer les tensions avant branchement | Niveaux conformes, masses communes | À faire |
| Compilation ESP32 | Compiler SmartCar.ino avec la carte correcte | Compilation sans erreur | À faire |
| Démarrage | Alimenter roues soulevées | Aucun moteur en mouvement | À faire |
| Polarités | F/B/L/R, appuis brefs | Directions correspondant aux commandes | À faire |
| Relâchement | Relâcher un bouton | Arrêt sur S | À faire |
| Commande perdue | Maintenir K mais cesser F/B/L/R | Arrêt après environ 700 ms + durée d'une boucle | À faire |
| Liaison silencieuse | Cesser tous les envois | Arrêt après environ 1 500 ms + durée d'une boucle | À faire |
| Bluetooth coupé | Couper Bluetooth | Arrêt dès détection de perte | À faire |
| Reconnexion | Reconnecter sans direction | Reste arrêté | À faire |
| Obstacle | Placer objet à 20 cm | Pas de marche avant | À faire |
| Capteur en défaut | Débrancher capteur hors tension, puis relancer | Pas d'avance ; AUTO arrêté | À faire |
| Automatique | A puis K toutes les 250 ms, zone dégagée | Avance, stop, recul, pivot selon obstacle | À faire |
| Arrêt pendant recul/pivot | Envoyer S | Arrêt et mode manuel | À faire |
| App Android | Permissions, appairage, commandes et veille | Fonctionnement conforme au protocole | À faire |

Ne jamais arracher des fils sous tension. Tester les défauts de capteur à l'arrêt. Le capteur frontal ne protège pas les reculs/pivots : dégager complètement la zone. Noter ici les résultats réellement mesurés et la version du paquet ESP32 après essai.
