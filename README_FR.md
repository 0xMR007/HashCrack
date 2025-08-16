# HashCrack

HashCrack est un outil en ligne de commande simple et efficace pour casser des mots de passe hachés à l'aide d'une attaque par dictionnaire. Il prend en charge 10 algorithmes de hachage populaires incluant MD5, SHA1, SHA256, SHA512 et bien d'autres.

*🌍 [English version available here](README.md)*

> **Note :**  
> Ceci est la **première version** utilisant des fonctions spécifiques par algorithme (ex. `MD5()`, `SHA256()`), nécessitant l'inclusion du fichier d'en-tête correspondant.  
> Une **prochaine mise à jour** passera à un système universel basé sur `EVP`, ce qui permettra de supporter **tous** les algorithmes OpenSSL (y compris SM3, BLAKE2, SHA3, etc.) sans avoir à ajouter du code spécifique à chaque nouveau hash.

## Table des matières
- 🚀 [Fonctionnalités](#fonctionnalités)
- 🛠️ [Installation](#installation)
- 💻 [Utilisation](#utilisation)
- 📋 [Exemples](#exemples)
- 🔎 [Options](#options)
- 📁 [Structure du projet](#structure-du-projet)
- 📄 [Licence](#licence)

## Fonctionnalités
- Prise en charge de 10 algorithmes de hachage incluant :
  - **MD4, MD5, MD5-SHA1**
  - **RIPEMD160**
  - **SHA1, SHA224, SHA256, SHA384, SHA512**
  - **SHA256-192** (version tronquée)
- Cassage rapide de hashs par dictionnaire
- Interface CLI simple et claire
- Facilement extensible pour d'autres algorithmes de hachage

## Installation

### Prérequis
- GCC (ou un compilateur C compatible)
- Bibliothèques de développement OpenSSL

### Installation des dépendances
```sh
sudo apt-get install libssl-dev
```

### Cloner le repository
```sh
git clone https://github.com/0xMR007/Hashcrack.git
```

### Compilation
```sh
make
```

## Utilisation
```
./hashcracker <ALG_NAME> <HASH> <WORDLIST_PATH>
```

- `<ALG_NAME>` : Nom ou ID de l'algorithme de hachage (ex : `md5`, `sha1`, `sha256`, `sha512`, ou `0`, `1`, `2`, etc.)
- `<HASH>` : La valeur du hachage à casser
- `<WORDLIST_PATH>` : Chemin vers le fichier dictionnaire (ex : `rockyou.txt`)

## Exemples
```
./hashcracker md5 5f4dcc3b5aa765d61d8327deb882cf99 data/rockyou.txt
./hashcracker sha1 5baa61e4c9b93f3f0682250b6cf8331b7ee68fd8 data/rockyou.txt
./hashcracker sha256 a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3 data/rockyou.txt
./hashcracker sha512 9b71d224bd62f3785d96d46ad3ea3d73319bfbc2890caadae2dff72519673ca72323c3d99ba5c11d7c7acc6e14b8c5da0c4663475c2e5c3adef46f73bcdec043 data/rockyou.txt
```

## Options
```sh
./hashcracker -h, --help    # Afficher l'aide
./hashcracker -l, --list    # Lister les algorithmes supportés
```

## Structure du projet
- `src/` : Fichiers source
- `include/` : Fichiers d'en-tête
- `data/` : Dictionnaires d'exemple
- `assets/` : Ressources du projet (ex : GIF de démonstration)

## Licence
Ce projet est sous licence MIT.