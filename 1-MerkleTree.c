#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha256.h"

#define MAX_NODES 32
#define HASH_LEN 65

typedef struct {
    char nodes[MAX_NODES][HASH_LEN];
    int count;
} MerkleLevel;

int main() {
    // 5 transactions pour prouver la gestion d'un nombre impair !
    const char *txs[] = {
        "Alice->Bob:10",
        "Bob->Charlie:5",
        "Charlie->Dave:2",
        "Dave->Eve:1",
        "Eve->Alice:0.5"
    };
    int n_tx = 5;

    printf("---EXERCICE 1 : ARBRE DE MERKLE---\n");

    MerkleLevel tree[10];
    int total_levels = 0;

    // Niveau 0 : Feuilles (Hachage des transactions)
    tree[0].count = n_tx;
    printf("--- Niveau 0 (Feuilles : %d transactions) ---\n", n_tx);
    for (int i = 0; i < n_tx; i++) {
        sha256_string(txs[i], tree[0].nodes[i]);
        printf("Tx[%d] \"%s\" -> Hash: %.16s...\n", i, txs[i], tree[0].nodes[i]);
    }
    total_levels++;

    // Construction ascendante de l'arbre
    while (tree[total_levels - 1].count > 1) {
        MerkleLevel *prev = &tree[total_levels - 1];
        MerkleLevel *curr = &tree[total_levels];
        curr->count = 0;

        printf("\n--- Niveau %d (%d noeuds) ---\n", total_levels, (prev->count + 1) / 2);
        for (int i = 0; i < prev->count; i += 2) {
            char combined[HASH_LEN * 2];
            if (i + 1 < prev->count) {
                // Deux nœuds disponibles
                snprintf(combined, sizeof(combined), "%s%s", prev->nodes[i], prev->nodes[i + 1]);
            } else {
                // Nombre impair : duplication du dernier nœud (règle Bitcoin)
                snprintf(combined, sizeof(combined), "%s%s", prev->nodes[i], prev->nodes[i]);
                printf("  [Note] Noeud impair duplique pour equilibrer l'arbre.\n");
            }
            sha256_string(combined, curr->nodes[curr->count]);
            printf("Combinaison -> %.16s...\n", curr->nodes[curr->count]);
            curr->count++;
        }
        total_levels++;
    }

    printf("MERKLE ROOT FINALE :\n%s\n", tree[total_levels - 1].nodes[0]);

    return 0;
}