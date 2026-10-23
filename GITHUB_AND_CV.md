# Mettre le projet sur GitHub

## Préparer la publication

1. Décompresser SmartCar_GitHub.zip.
2. Lire README.md, confirmer les crédits et préciser la contribution individuelle. La présentation fournie cite Aya Karaa et Aya Hichri : vérifier ces noms, sans les remplacer automatiquement par le propriétaire du compte GitHub.
3. Vérifier/adapter config.h et tester sur le matériel. Garder l'indication « firmware reconstruit » tant qu'elle décrit l'origine réelle du code.
4. Le fichier de présentation d'origine n'est pas inclus : ses 35 Mo peuvent gêner l'upload web et il faut vérifier les noms/contenus avant publication. Ajouter une version compressée ou un lien seulement après relecture.

## Méthode simple sur le site

- Créer un repository nommé `smartcar-esp32-bluetooth`.
- Description proposée : `Voiture robotisée ESP32 : commande Bluetooth Android et évitement d'obstacles par HC-SR04.`
- Choisir Public si le projet doit être consultable depuis le CV.
- Dans le dépôt, utiliser Add file > Upload files.
- Glisser le **contenu du dossier SmartCar**, pas l'archive ZIP. README.md doit être à la racine du dépôt.
- Message de commit : `Add SmartCar reference firmware and documentation`.
- Vérifier que les images s'affichent et que les liens de documentation fonctionnent.

Le fichier .gitignore peut être masqué par Windows : il est inclus dans l'archive et peut être ajouté avec Git si le navigateur ne le sélectionne pas.

## Méthode Git, depuis le dossier SmartCar

Après avoir créé un dépôt GitHub vide et copié sa vraie URL :

```bash
git init
git add .
git commit -m "Add SmartCar reference firmware and documentation"
git branch -M main
```

Ajouter ensuite le remote avec `git remote add origin` suivi de l'URL réelle copiée depuis GitHub, puis :

```bash
git push -u origin main
```

Ne pas publier de mots de passe ni de jetons. Aucun compte GitHub n'a été connecté ni aucun dépôt distant créé par cette préparation.

## Présentation dans le CV

Choisir les verbes selon le travail réellement réalisé : conception, programmation, intégration ou participation. Ne pas attribuer les ajouts de cette nouvelle version au projet initial sans les avoir intégrés et validés.

Intitulé possible : **SmartCar - Voiture robotisée à commande Bluetooth**.

Éléments à adapter et vérifier avant usage :

- Participation à la réalisation d'un prototype à quatre moteurs basé sur ESP32, deux drivers L298N et un capteur ultrason HC-SR04.
- Mise en œuvre d'une commande Android via Bluetooth et d'un mode automatique d'évitement d'obstacles, si cette partie a réellement été développée et validée par vous.
- Technologies : C/C++, ESP32, Bluetooth Classic, HC-SR04, L298N ; MIT App Inventor uniquement si utilisé personnellement.

Ajouter la période réelle, le contexte académique, la contribution personnelle et le vrai lien GitHub. Ne pas présenter la logique à seuil comme un modèle d'IA entraîné.
