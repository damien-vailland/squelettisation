# TP2 - Morphologie Mathématique en C++

## 📌 Description

Ce projet a été réalisé dans le cadre du TP2 du module **Traitement d'Image Avancé** (TIA) à l'ESIR. Il met en œuvre plusieurs algorithmes de **morphologie mathématique** sur des images binaires et en niveaux de gris, à l’aide de la bibliothèque **ViSP (Visual Servoing Platform)**.

L’objectif est de concevoir et d’expérimenter les opérations suivantes :
- **Squelettisation par amincissement** utilisant la lettre L de l’alphabet de Golay
- **Transformées en distance** (L1 et max)
- **Érosion morphologique** basée sur les distances
- **Détection et comptage d’éléments connexes** sur une image complexe

---

## ⚙️ Fonctionnalités implémentées

### 1. Squelettisation (Amincissement)
- Application répétée d’éléments structurants pour réduire une structure binaire à son squelette.
- Arrêt de l’algorithme quand l’image n’est plus modifiée.
- Affichage de l’évolution de la squelettisation à plusieurs étapes.

### 2. Transformées en distance
- **Transformée L1** : basée sur une connexité à 4 voisins avec balayages successifs.
- **Transformée max (ou max-distance)** : basée sur une connexité à 8 voisins, plus douce visuellement.
- Comparaison des effets des deux approches sur des images simples et complexes.

### 3. Érosion morphologique à partir d’une transformée
- Implémentation de l’érosion en inversant l’image, calculant la distance, puis en binarisant selon un seuil.
- Visualisation de l’effet du seuil sur l’érosion.

### 4. Application pratique : Comptage de graines
- Binarisation d’une image en niveaux de gris selon un seuil déterminé empiriquement.
- Suppression du bruit éventuel (optionnelle).
- Squelettisation de l’image binaire.
- Détection de squelettes isolés éloignés des bords pour ne compter qu’un seul pixel par élément.
- Comptage des pixels restants comme estimation du nombre d’objets.

---

## 🧰 Technologies et bibliothèques

- **Langage** : C++
- **Bibliothèque** : [ViSP](https://visp.inria.fr/)
- **Compilation** : CMake

---

## 📝 Remarques

- Les algorithmes sont développés pour traiter des images binaires, mais peuvent être adaptés à d'autres types d'images.
- Le seuil de binarisation ainsi que le rayon utilisé dans la détection des squelettes sont choisis de manière empirique.
- L'algorithme de comptage est efficace sur une image donnée, mais reste peu généralisable sans adaptation.

---

## 👨‍💻 Auteurs

- **Axel Plessis**
- **Damien Vailland** – ESIR 2 Imagerie Numérique

---

## 📅 Date

TP réalisé le **14 novembre 2024**
