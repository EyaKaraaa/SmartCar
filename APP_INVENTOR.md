# Application Android - guide MIT App Inventor

Le projet original `.aia` et l'APK ne sont pas présents. Ce document décrit les blocs à créer pour piloter le firmware fourni ; il ne représente pas une application compilée ou testée. L'image du dossier `images` est une maquette. BluetoothClient utilise SPP : construire cette version pour Android, même si la maquette ressemble à un iPhone.

## Designer

Créer un projet SmartCarControl dans MIT App Inventor.

| Nom | Composant | Propriété / rôle |
|---|---|---|
| LP_Connect | ListPicker | Texte : Connecter |
| BT | BluetoothClient | DelimiterByte = 10 ; DisconnectOnError = true |
| Clock_Link | Clock | TimerInterval = 250 ; TimerEnabled = false |
| Clock_Read | Clock | TimerInterval = 100 ; TimerEnabled = false |
| Btn_F, Btn_B | Button | Avancer, reculer |
| Btn_L, Btn_R | Button | Gauche, droite |
| Btn_Stop | Button | STOP |
| Btn_Auto, Btn_Manual | Button | Automatique, manuel |
| Btn_Disconnect | Button | Déconnecter |
| Label_Status | Label | Déconnecté |
| Label_Data | Label | Télémétrie brute |
| Notifier1 | Notifier | Erreurs / autorisations |

Couleurs : fond #121212, boutons #C8102E, texte #FFFFFF. STOP doit rester visible. Organiser les quatre directions en croix avec STOP au centre.

Variables globales : `held = "S"`, `auto = false`, `rx = ""`.

## Procédures à construire avec les blocs

`Send(command)` : si BT.IsConnected, appeler BT.SendText(command). Ne pas envoyer des mots complets ni des numéros de bouton.

`ResetUI()` : held = S ; auto = false ; Clock_Link.TimerEnabled = false ; Clock_Read.TimerEnabled = false ; Label_Status.Text = Déconnecté ; rx = texte vide.

## Appairage et permissions

1. Dans les paramètres Bluetooth Android, appairer SMART_CAR, puis revenir à l'application.
2. Demander et accepter les permissions Bluetooth adaptées à la version Android. Sur Android 12+, BluetoothClient requiert BLUETOOTH_CONNECT et BLUETOOTH_SCAN pour les appareils appairés. Ne remplir la liste qu'après octroi ; gérer Screen.PermissionDenied avec un message et désactiver les horloges.
3. `LP_Connect.BeforePicking` : une fois les permissions accordées, définir Elements = BT.AddressesAndNames.
4. `LP_Connect.AfterPicking` : si BT.Connect(LP_Connect.Selection) renvoie true : held = S ; auto = false ; rx = vide ; Send(M) ; activer les deux horloges ; statut = Connecté. Sinon afficher l'échec sans activer les horloges.

## Boutons directionnels

Pour Btn_F.TouchDown : si connecté ET auto = false, held = F, Send(F). Faire de même avec B/L/R.

Pour chacun des quatre boutons, TouchUp : si auto = false, held = S et Send(S). Le mouvement exige donc un appui maintenu. Envoyer S au relâchement même si le doigt a changé de zone ; le délai de 700 ms couvre un événement de relâchement manqué.

`Clock_Link.Timer` :
- Si non connecté : ResetUI().
- Sinon : Send(K), puis si auto = false ET held différent de S, Send(held).

`Btn_Auto.Click` : held = S ; Send(A) une seule fois ; auto = true ; statut = AUTO demandé.

`Btn_Manual.Click` : held = S ; auto = false ; Send(M) ; statut = MANUEL demandé.

`Btn_Stop.Click` : held = S ; auto = false ; Send(S) ; statut = ARRÊT demandé.

`Btn_Disconnect.Click` : Send(S), BT.Disconnect, ResetUI().

Ne pas envoyer A dans Clock_Link : cela relancerait continuellement la séquence d'évitement. Les textes « demandé » évitent de confondre l'état local avec une confirmation du robot.

## Réception sans attendre une ligne incomplète

`Clock_Read.Timer` : si connecté et BytesAvailableToReceive > 0, lire exactement ce nombre d'octets avec ReceiveText(nombre), puis ajouter au texte rx. Tant que rx contient un saut de ligne LF, extraire la première ligne, la retirer de rx et l'afficher dans Label_Data. Retirer CR si présent.

Si la ligne commence par MODE=MANUAL, auto = false. Si elle commence par MODE=AUTO, auto = true. Sur tout changement de mode reçu, remettre held = S pour ne pas relancer une ancienne direction. Limiter rx à 512 caractères ; le vider au-delà si aucune ligne complète n'est reçue.

La télémétrie confirme le mode logiciel, pas le mouvement physique. Les événements de permission refusée ou d'erreur Bluetooth doivent remettre l'interface en état arrêté, couper les horloges et déconnecter le client si nécessaire. En cas de mise en veille, le firmware arrête le robot quand le heartbeat cesse.

## Export

Après construction et essai sur Android : Projects > Export selected project (.aia) ; Build pour produire l'APK. Ajouter les fichiers réels dans `mobile/` après vérification. Ne pas renommer un texte en `.aia` ou `.apk`.

Référence : https://ai2.appinventor.mit.edu/reference/components/connectivity.html#BluetoothClient
