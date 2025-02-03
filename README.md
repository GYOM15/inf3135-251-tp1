# Projet kover : Gestionnaire de positionnement d'antennes de communication

## Description

`kover` est une application en ligne de commande permettant de gérer le positionnement d'antennes de communication en tenant compte des bâtiments environnants. L'application lit une description de scène depuis l'entrée standard et peut effectuer diverses opérations d'analyse comme le calcul de la boîte englobante, la description détaillée de la scène ou un résumé des éléments présents.

Ce projet a été développé dans le cadre du cours INF3135 - Construction et maintenance de logiciels à l'UQAM, sous la supervision du professeur Alexandre Blondin Massé.

## Enoncé

[sujet.md](https://gitlab.info.uqam.ca/millimounou.guy_olivier_yanouba/inf3135-251-tp1/-/blob/master/sujet.md?ref_type=heads)

## Auteur

Guy Olivier Yanouba Millimouno (MILG69360006)

## Fonctionnement

### Compilation

Le projet utilise un Makefile pour automatiser la compilation. Pour compiler l'application :

```sh
$ make
```

Cette commande générera l'exécutable `kover`.

### Utilisation

L'application accepte une sous-commande obligatoire et lit la description de la scène depuis l'entrée standard. Les sous-commandes disponibles sont :

* `bounding-box` : Calcule et affiche la boîte englobante de la scène
* `describe` : Fournit une description détaillée de la scène
* `help` : Affiche l'aide de l'application
* `summarize` : Présente un résumé de la scène

Exemple d'utilisation :
```sh
$ ./kover describe < scene.txt
```

### Format de la scène

La scène doit respecter la syntaxe suivante :
* Première ligne : `begin scene`
* Dernière ligne : `end scene`
* Entre ces lignes : définitions de bâtiments et d'antennes

Format des bâtiments :
```
building ID X Y W H
```
où :
* `ID` : identifiant unique du bâtiment
* `X,Y` : coordonnées du centre
* `W,H` : demi-largeur et demi-hauteur

Format des antennes :
```
antenna ID X Y R
```
où :
* `ID` : identifiant unique de l'antenne
* `X,Y` : coordonnées de l'antenne
* `R` : rayon de portée

## Tests

Les tests automatiques peuvent être exécutés avec :

```sh
$ make test
```

La suite de tests utilise Bats (Bash Automated Testing System) et vérifie :
* La validation des arguments
* Le traitement des erreurs
* Le calcul de la boîte englobante
* La détection des chevauchements
* La gestion des identifiants uniques
* Le formatage des sorties

## Dépendances

* [GCC](https://gcc.gnu.org/) (≥ 9.4.0) : Compilateur C
* [GNU Make](https://www.gnu.org/software/make/) (≥ 4.2.1) : Automatisation de la compilation
* [Bats](https://github.com/bats-core/bats-core) (≥ 1.2.0) : Framework de tests
* [Valgrind](https://valgrind.org/) (≥ 3.15.0) : Détection des fuites mémoire

## Références

* [The GNU C Reference Manual](https://www.gnu.org/software/gnu-c-manual/)
* [GCC Command Options](https://gcc.gnu.org/onlinedocs/gcc/Invoking-GCC.html)
* [Bats Documentation](https://bats-core.readthedocs.io/)
* [C Programming Language Specification](https://www.iso.org/standard/74528.html)

## État du projet

* [X] Le nom du dépôt GitLab est exactement `inf3135-251-tp1`
* [X] L'URL du dépôt GitLab est exactement `https://gitlab.info.uqam.ca/millimounou.guy_olivier_yanouba/inf3135-251-tp1`
* [X] Les utilisateurs `blondin_al` et `guite-vinet.julien` ont accès au projet en mode *Maintainer*
* [X] Le dépôt GitLab est un *fork* du gabarit fourni
* [X] Le dépôt GitLab est privé
* [X] Le dépôt contient un fichier `.gitignore` approprié
* [X] Toutes les sections du fichier `README.md` sont complétées
* [X] Toutes les fonctions du fichier `kover.c` sont documentées avec des docstrings
* [X] Les tests automatiques passent avec succès
* [X] Le code est bien formaté et respecte les conventions de style
* [X] La mémoire est correctement gérée (vérifié avec Valgrind)
* [X] Le projet compile sans avertissements avec `-Wall -Wextra -Werror`

