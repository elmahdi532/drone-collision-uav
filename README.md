# Dossier de Conception Technique
## Système de Détection de Collision — Essaim UAV
**École des Sciences de l'Information | Pr. HOUICHIME Tarik**

---

## 1. Présentation du Problème

Le système doit, chaque milliseconde, identifier la paire de drones la plus proche parmi N = 10 000 appareils volant dans un espace 3D. Une approche naïve (double boucle) génère ~50 millions de calculs par cycle, ce qui est inacceptable sur processeur embarqué.

---

## 2. Architecture Choisie : Sweep & Sort (Balayage par axe)

### 2.1 Principe

L'algorithme repose sur deux phases :

1. **Tri sur l'axe X** : on ordonne les drones par leur coordonnée x croissante (via `qsort` — O(n log n)).
2. **Fenêtre glissante** : pour chaque drone i, on ne compare qu'aux drones j dont l'écart sur X vérifie : `(xⱼ - xᵢ)² < δ²` (où δ est la meilleure distance courante). Dès que l'écart dépasse δ, on interrompt la boucle interne (le tableau étant trié, tous les drones suivants seront encore plus éloignés).

### 2.2 Justification du choix de l'axe X

Dans un espace 3D uniforme, les trois axes sont statistiquement équivalents. L'axe X est choisi par convention. Dans une version plus avancée, on pourrait choisir l'axe de plus grande variance (PCA), mais ce n'est pas nécessaire ici.

### 2.3 Pourquoi pas un k-d tree ?

Un k-d tree donne également O(n log n) mais sa mise en œuvre requiert une allocation dynamique récursive complexe, incompatible avec la contrainte de bloc mémoire unique et d'arithmétique de pointeurs. Le Sweep & Sort est plus simple, aussi efficace en pratique, et respecte toutes les contraintes.

---

## 3. Mécanique des Pointeurs — Respect de la Contrainte

Le cahier des charges interdit strictement l'opérateur `[]`. Voici comment toutes les opérations sont réalisées :

| Opération classique | Équivalent pointeur utilisé |
|---|---|
| `essaim[i]` | `*(essaim + i)` |
| `essaim[i].x` | `(essaim + i)->x` |
| Avancer i | `ptr_i++` |
| Index depuis pointeur | `(int)(ptr_i - essaim)` |
| Lire le résultat final | `(essaim + best_i)->id` |

La navigation dans le bloc mémoire est entièrement réalisée par :
- **Incrémentation** (`ptr++`) pour parcourir les drones séquentiellement.
- **Différence de pointeurs** (`ptr_j - ptr_i`) pour calculer les offsets.
- **Déréférencement** (`ptr->champ`) pour accéder aux membres.

---

## 4. Gestion Mémoire

```
malloc(10000 × sizeof(struct Drone))
       └─────────────────────────────────────────────┐
       [ Drone 0 | Drone 1 | Drone 2 | ... | Drone 9999 ]
         ↑                                            ↑
        essaim                                  essaim + N (sentinelle)
```

- **Un seul appel `malloc`** → bloc contigu, cohérent avec le modèle cache du processeur embarqué (localité spatiale).
- **Un seul appel `free`** à la fin → aucune fuite mémoire.
- **Vérification du retour de `malloc`** → robustesse industrielle.

---

## 5. Optimisation : distance au carré

Le calcul de `sqrt()` est coûteux. Pour toutes les **comparaisons intermédiaires**, on utilise `dist²` (carré de la distance). On n'appelle `sqrtf()` qu'une seule fois à la fin, pour l'affichage de la distance réelle. Cela économise ~N log N appels à une fonction transcendante.
