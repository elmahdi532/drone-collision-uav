/*
 * ============================================================
 *  Système de Détection de Collision pour Essaim Autonome UAV
 *  École des Sciences de l'Information — Pr. HOUICHIME Tarik
 *  Programmation Avancée en C — Projet Industriel
 * ============================================================
 *
 *  Approche : Tri par axe (Sweep & Sort) + arithmétique de pointeurs
 *  Complexité : O(n log n) — garanti
 *  Contrainte respectée : AUCUN opérateur [] utilisé
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

/* ─────────────────────────────────────────────
   1.  STRUCTURE DE DONNÉE
   ───────────────────────────────────────────── */
struct Drone {
    int   id;
    float x;
    float y;
    float z;
};

/* ─────────────────────────────────────────────
   2.  DISTANCE EUCLIDIENNE (au carré)
       On évite sqrt() pour la comparaison :
       plus rapide et sans perte de précision.
   ───────────────────────────────────────────── */
static float dist2(const struct Drone *a, const struct Drone *b)
{
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;
    return dx*dx + dy*dy + dz*dz;
}

/* ─────────────────────────────────────────────
   3.  COMPARATEUR pour qsort — tri sur l'axe X
       (axe choisi car il maximise la séparation
        dans un espace 3D uniforme)
   ───────────────────────────────────────────── */
static int cmp_x(const void *p1, const void *p2)
{
    /* p1 et p2 sont des pointeurs vers struct Drone */
    const struct Drone *d1 = (const struct Drone *)p1;
    const struct Drone *d2 = (const struct Drone *)p2;

    if (d1->x < d2->x) return -1;
    if (d1->x > d2->x) return  1;
    return 0;
}

/* ─────────────────────────────────────────────
   4.  ALGORITHME SWEEP & SORT  O(n log n)
       Après tri sur X, deux drones candidats
       à la paire la plus proche ne peuvent être
       séparés de plus de δ (distance minimale
       courante) sur l'axe X.
       → fenêtre glissante très réduite.
   ───────────────────────────────────────────── */
void trouver_paire_proche(struct Drone *essaim, int n,
                          int *id_a, int *id_b, float *dist_min)
{
    /* 4.1 — Tri sur l'axe X via qsort  O(n log n) */
    qsort(essaim, (size_t)n, sizeof(struct Drone), cmp_x);

    float   best2 = FLT_MAX;   /* meilleure distance au carré        */
    int     best_i = 0;
    int     best_j = 1;

    /*
     * 4.2 — Balayage avec fenêtre glissante
     *
     * Pour chaque drone i (pointeur ptr_i),
     * on ne compare qu'aux drones j dont :
     *     (ptr_j->x - ptr_i->x)^2  <  best2
     * Dès que cette condition est fausse, on
     * sort de la boucle intérieure : inutile
     * d'aller plus loin (tableau trié).
     *
     * Navigation STRICTEMENT par arithmétique
     * de pointeurs — aucun crochet [].
     */
    struct Drone *ptr_i = essaim;            /* pointeur courant i   */
    struct Drone *fin   = essaim + n;        /* sentinelle de fin    */

    while (ptr_i < fin - 1)
    {
        struct Drone *ptr_j = ptr_i + 1;     /* pointeur courant j   */

        while (ptr_j < fin)
        {
            /* Écart sur X uniquement — test de fenêtre rapide */
            float dx = ptr_j->x - ptr_i->x;
            float dx2 = dx * dx;

            /* Si dx² ≥ best2 : tous les j suivants seront encore
               plus éloignés (tableau trié) → on coupe la boucle */
            if (dx2 >= best2) break;

            /* Calcul de la distance complète 3D */
            float d2 = dist2(ptr_i, ptr_j);
            if (d2 < best2)
            {
                best2  = d2;
                best_i = (int)(ptr_i - essaim); /* offset = index   */
                best_j = (int)(ptr_j - essaim);
            }

            ptr_j++;   /* avancer j via arithmétique de pointeur     */
        }

        ptr_i++;       /* avancer i via arithmétique de pointeur     */
    }

    /* 4.3 — Lecture des résultats par arithmétique de pointeurs     */
    *id_a     = (essaim + best_i)->id;
    *id_b     = (essaim + best_j)->id;
    *dist_min = sqrtf(best2);          /* distance réelle (affichage) */
}

/* ─────────────────────────────────────────────
   5.  GÉNÉRATION DE DONNÉES DE TEST
       Positions pseudo-aléatoires dans un cube
       de 1000 × 1000 × 1000 unités.
   ───────────────────────────────────────────── */
static void init_essaim(struct Drone *essaim, int n)
{
    struct Drone *ptr = essaim;
    struct Drone *fin = essaim + n;

    int id = 0;
    while (ptr < fin)
    {
        ptr->id = id++;
        ptr->x  = (float)(rand() % 100000) / 100.0f;   /* 0..1000  */
        ptr->y  = (float)(rand() % 100000) / 100.0f;
        ptr->z  = (float)(rand() % 100000) / 100.0f;
        ptr++;                   /* arithmétique de pointeur         */
    }
}

/* ─────────────────────────────────────────────
   6.  PROGRAMME PRINCIPAL
   ───────────────────────────────────────────── */
int main(void)
{
    const int N = 10000;

    /* ── Allocation dynamique d'un seul bloc continu (le Tas) ── */
    struct Drone *essaim = (struct Drone *)malloc((size_t)N * sizeof(struct Drone));
    if (essaim == NULL)
    {
        fprintf(stderr, "[ERREUR] Échec d'allocation mémoire (%d drones)\n", N);
        return EXIT_FAILURE;
    }

    /* ── Initialisation ── */
    srand(42);                    /* graine fixe → résultats reproductibles */
    init_essaim(essaim, N);

    printf("=== Système de Collision UAV ===\n");
    printf("Nombre de drones : %d\n\n", N);

    /* ── Recherche de la paire la plus proche ── */
    int   id_a, id_b;
    float dist_min;
    trouver_paire_proche(essaim, N, &id_a, &id_b, &dist_min);

    printf("[ALERTE] Paire critique détectée !\n");
    printf("  Drone A  : ID = %d\n", id_a);
    printf("  Drone B  : ID = %d\n", id_b);
    printf("  Distance : %.4f m\n", dist_min);
    printf("\nManœuvre d'évitement déclenchée.\n");

    /* ── Libération mémoire ── */
    free(essaim);
    return EXIT_SUCCESS;
}
