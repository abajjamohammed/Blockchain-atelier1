#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sha256.h"

typedef struct {
    char name[32];
    int stake;
    int win_count;
} Validator;

// Sélection du validateur pondérée par la mise en jeu (loterie PoS)
int select_validator(Validator vs[], int count, int total_stake) {
    int pick = (rand() % total_stake) + 1;
    int cum = 0;
    for (int i = 0; i < count; i++) {
        cum += vs[i].stake;
        if (pick <= cum) return i;
    }
    return count - 1;
}

int main() {
    srand(time(NULL));

    printf("---EXERCICE 3 : PROOF OF STAKE vs PROOF OF WORK---\n");

    Validator vs[] = {
        {"Alice", 32, 0},
        {"Bob", 64, 0},
        {"Charlie", 128, 0},
        {"Dave", 16, 0}
    };
    int count = 4;
    int total_stake = 32 + 64 + 128 + 16; // 240

    // 1. Simulation statistique (10 000 tirages)
    int iterations = 10000;
    clock_t start_pos = clock();
    for (int i = 0; i < iterations; i++) {
        int winner = select_validator(vs, count, total_stake);
        vs[winner].win_count++;
    }
    clock_t end_pos = clock();
    double time_pos = ((double)(end_pos - start_pos) * 1000.0) / CLOCKS_PER_SEC;

    printf("[1] Simulation PoS (10 000 blocs valides) :\n");
    for (int i = 0; i < count; i++) {
        double pct = (100.0 * vs[i].win_count) / iterations;
        double theoretical = (100.0 * vs[i].stake) / total_stake;
        printf("  - %-7s (Stake: %3d) : %5d victoires (%5.2f%% | Theorie: %5.2f%%)\n",
               vs[i].name, vs[i].stake, vs[i].win_count, pct, theoretical);
    }
    printf("  -> Temps total pour 10 000 blocs PoS : %.2f ms (%.6f ms par bloc)\n\n",
           time_pos, time_pos / iterations);

    // 2. Comparaison avec la validation d'un seul bloc en PoW (difficulté 5)
    printf("[2] Minage d'un SEUL bloc avec Proof of Work (Difficulte 5) :\n");
    clock_t start_pow = clock();
    unsigned long nonce = 0;
    char pow_hash[65];
    char dummy_block[] = "Bloc transaction data Alice->Bob";

    while (1) {
        char test[128];
        snprintf(test, sizeof(test), "%s%lu", dummy_block, nonce);
        sha256_string(test, pow_hash);
        if (pow_hash[0] == '0' && pow_hash[1] == '0' && pow_hash[2] == '0' &&
            pow_hash[3] == '0' && pow_hash[4] == '0') {
            break;
        }
        nonce++;
    }
    clock_t end_pow = clock();
    double time_pow = ((double)(end_pow - start_pow) * 1000.0) / CLOCKS_PER_SEC;

    printf("  -> Nonce trouve : %lu\n", nonce);
    printf("  -> Hash valide  : %s\n", pow_hash);
    printf("  -> Temps total pour 1 SEUL bloc PoW : %.2f ms\n\n", time_pow);

    printf("============================= BILAN ====================================\n");
    printf("Pour valider un bloc :\n");
    printf("  - Proof of Stake : Instantane (~0.0001 ms, zero gaspillage electrique)\n");
    printf("  - Proof of Work  : Lent (%.2f ms, calculs intensifs)\n", time_pow);
    printf("=> PoS est radicalement plus rapide et ecologique que PoW.\n");

    return 0;
}