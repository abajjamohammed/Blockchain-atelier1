#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sha256.h"

typedef struct {
    int index;
    char timestamp[32];
    char data[64];
    char prev_hash[65];
    unsigned long nonce;
    char hash[65];
} Block;

// Obtient la date/heure actuelle
void get_current_time(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Sérialisation du bloc pour le calcul du hash
void serialize_block(const Block *b, char *out, size_t size) {
    snprintf(out, size, "%d%s%s%s%lu", b->index, b->timestamp, b->data, b->prev_hash, b->nonce);
}

// Vérifie si le hash commence par 'difficulty' zéros
int check_difficulty(const char *hash, int difficulty) {
    for (int i = 0; i < difficulty; i++) {
        if (hash[i] != '0') return 0;
    }
    return 1;
}

// Minage PoW avec mesure du temps en millisecondes
double mine_block(Block *b, int difficulty) {
    char target_str[1024];
    b->nonce = 0;

    clock_t start = clock();
    while (1) {
        serialize_block(b, target_str, sizeof(target_str));
        sha256_string(target_str, b->hash);

        if (check_difficulty(b->hash, difficulty)) {
            break;
        }
        b->nonce++;
    }
    clock_t end = clock();
    return ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC; // temps en ms
}

int main() {
    printf("---EXERCICE 2 : PROOF OF WORK (BLOCS ET CHAINES)---\n");

    Block chain[6];
    char prev[65] = "0000000000000000000000000000000000000000000000000000000000000000";

    printf("%-10s | %-12s | %-14s | %-20s\n", "Difficulte", "Nonce teste", "Temps (ms)", "Hash (16 premiers car.)");
    printf("------------------------------------------------------------------------\n");

    for (int diff = 1; diff <= 5; diff++) {
        Block b;
        b.index = diff;
        get_current_time(b.timestamp, sizeof(b.timestamp));
        snprintf(b.data, sizeof(b.data), "Transactions du Bloc #%d", diff);
        strcpy(b.prev_hash, prev);

        double ms = mine_block(&b, diff);
        printf("%-10d | %-12lu | %10.2f ms | %.20s...\n", diff, b.nonce, ms, b.hash);

        strcpy(prev, b.hash);
        chain[diff] = b;
    }

    printf("\nConclusion : Le temps de minage augmente de maniere exponentielle avec la difficulte.\n");
    return 0;
}