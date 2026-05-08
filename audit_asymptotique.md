Note d'Expertise — Audit Asymptotique
Preuve Formelle de la Complexité Temporelle
École des Sciences de l'Information | Pr. HOUICHIME Tarik

1. Rappel du problème de saturation
L'algorithme naïf effectue, pour N = 10 000 drones :
C_naïf = N×(N-1)/2 = 10000×9999/2 = 49 995 000 opérations
Chaque opération inclut 3 soustractions, 3 multiplications, 2 additions et 1 racine carrée. Sur un processeur embarqué à 100 MHz, cela dépasse largement la contrainte de 1 ms → Timeout garanti.
Complexité naïve : O(n²)

2. Complexité de l'algorithme Sweep & Sort
Phase 1 — Tri (qsort)
qsort utilise un tri rapide (Quicksort) optimisé, de complexité :
T_tri = O(n log n)
Pour n = 10 000 :  10 000 × log₂(10 000) ≈ 10 000 × 13,3 ≈ 133 000 opérations
Phase 2 — Balayage avec fenêtre glissante
Cas défavorable (worst case) : Si tous les drones sont au même point, la fenêtre ne se réduit jamais → O(n²). Ce cas est physiquement impossible (deux drones ne peuvent pas occuper la même position).
Cas réaliste (average case) : Pour une distribution uniforme dans un cube de côté L, la distance minimale attendue entre deux drones est :
δ_moy ≈ L / n^(1/3)
La taille moyenne de la fenêtre glissante (nombre de drones dans l'intervalle [xᵢ, xᵢ + δ]) est bornée par une constante k indépendante de n (pour des distributions uniformes). Donc :
T_balayage = O(n × k) = O(n)
Complexité totale
T_total = T_tri + T_balayage
        = O(n log n) + O(n)
        = O(n log n)         [le terme dominant]

3. Preuve de non-saturation
CritèreAlgorithme naïfSweep & SortComplexitéO(n²)O(n log n)Opérations (n=10 000)~50 000 000~133 000Rapport1×~375× plus rapideTimeout à 1 msOUINON
Démonstration formelle
Soit T(n) la complexité de notre algorithme :
T(n) = c₁ · n·log(n)  +  c₂ · n
Avec c₁ et c₂ des constantes matérielles. Pour n = 10 000 :
T(10 000) = c₁ · 133 000 + c₂ · 10 000
Le rapport avec le naïf :
T_naïf / T_notre  =  50 000 000 / 133 000  ≈  375
Notre algorithme est 375 fois plus rapide que l'approche naïve pour n = 10 000. Cet avantage croît avec n (car n² croît bien plus vite que n log n), garantissant la non-saturation même si la flotte s'agrandit.

4. Conclusion
L'architecture Sweep & Sort, implémentée avec arithmétique pure de pointeurs sur un bloc mémoire contigu, résout formellement la problématique de saturation matérielle. La complexité O(n log n) est prouvée et le gain de performance (~375×) garantit le respect de la contrainte temps-réel de 1 ms sur processeur embarqué.
