#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sha256.h"

#define HASH_LEN 65
#define MAX_BLOCKS 20

typedef struct {
    int index;
    char timestamp[32];
    char data[128];
    char prev_hash[HASH_LEN];
    unsigned long nonce;
    char hash[HASH_LEN];
} Block;

typedef struct {
    Block chain[MAX_BLOCKS];
    int size;
    int difficulty;
} Blockchain;

void get_time(char *buf, size_t sz) {
    time_t t = time(NULL);
    strftime(buf, sz, "%Y-%m-%d %H:%M:%S", localtime(&t));
}

void hash_block(Block *b, char out[HASH_LEN]) {
    char raw[512];
    snprintf(raw, sizeof(raw), "%d%s%s%s%lu", b->index, b->timestamp, b->data, b->prev_hash, b->nonce);
    sha256_string(raw, out);
}

void mine_block(Block *b, int difficulty) {
    b->nonce = 0;
    while (1) {
        hash_block(b, b->hash);
        int ok = 1;
        for (int i = 0; i < difficulty; i++) {
            if (b->hash[i] != '0') { ok = 0; break; }
        }
        if (ok) break;
        b->nonce++;
    }
}

void init_blockchain(Blockchain *bc, int diff) {
    bc->size = 0;
    bc->difficulty = diff;

    Block genesis;
    genesis.index = 0;
    get_time(genesis.timestamp, sizeof(genesis.timestamp));
    strcpy(genesis.data, "Bloc Genesis");
    memset(genesis.prev_hash, '0', 64);
    genesis.prev_hash[64] = '\0';

    mine_block(&genesis, bc->difficulty);
    bc->chain[bc->size++] = genesis;
}

void add_block(Blockchain *bc, const char *data) {
    if (bc->size >= MAX_BLOCKS) return;

    Block b;
    b.index = bc->size;
    get_time(b.timestamp, sizeof(b.timestamp));
    strncpy(b.data, data, sizeof(b.data) - 1);
    strcpy(b.prev_hash, bc->chain[bc->size - 1].hash);

    mine_block(&b, bc->difficulty);
    bc->chain[bc->size++] = b;
}

// Fonction de falsification
void tamper_block(Blockchain *bc, int index, const char *fake_data) {
    if (index < bc->size) {
        strncpy(bc->chain[index].data, fake_data, sizeof(bc->chain[index].data) - 1);
    }
}

// Audit complet de la blockchain
int verify_chain(const Blockchain *bc) {
    for (int i = 1; i < bc->size; i++) {
        const Block *curr = &bc->chain[i];
        const Block *prev = &bc->chain[i - 1];

        // 1. Vérification du hash recalculé
        char check_hash[HASH_LEN];
        hash_block((Block*)curr, check_hash);
        if (strcmp(curr->hash, check_hash) != 0) {
            printf("  [ECHEC] Falsification detectee dans les donnees du bloc #%d !\n", curr->index);
            return 0;
        }

        // 2. Vérification du lien avec le bloc précédent
        if (strcmp(curr->prev_hash, prev->hash) != 0) {
            printf("  [ECHEC] Rupture de chainage entre le bloc #%d et le bloc #%d !\n", prev->index, curr->index);
            return 0;
        }
    }
    return 1;
}

void print_chain(const Blockchain *bc) {
    for (int i = 0; i < bc->size; i++) {
        const Block *b = &bc->chain[i];
        printf("#%d [%s]\n", b->index, b->timestamp);
        printf("  Data      : %s\n", b->data);
        printf("  PrevHash  : %.16s...\n", b->prev_hash);
        printf("  Hash      : %.16s... | Nonce: %lu\n\n", b->hash, b->nonce);
    }
}

int main() {
    printf("---EXERCICE 4 : BLOCKCHAIN & TEST D'INTEGRITE---\n");

    Blockchain bc;
    init_blockchain(&bc, 3); // Difficulté 3 zéros

    printf("--> Ajout des transactions en cours...\n");
    add_block(&bc, "Alice->Bob:10 DH");
    add_block(&bc, "Bob->Charlie:5 DH");
    add_block(&bc, "Charlie->Dave:2 DH");

    printf("\n=== 1. AFFICHAGE DE LA CHAINE ORIGINALE ===\n");
    print_chain(&bc);

    printf("Verification de l'integrite : %s\n\n",
           verify_chain(&bc) ? "VALIDE (Aucune alteration)" : "CORROMPUE");

    printf("=== 2. ATTAQUE / FALSIFICATION DU BLOC #1 ===\n");
    printf("Modification de la donnee du bloc #1 : 'Alice->Bob:10 DH' -> 'HACKED (1 000 000 DH)'\n");
    tamper_block(&bc, 1, "HACKED (1 000 000 DH)");

    printf("\nVerification de l'integrite apres attaque :\n");
    if (!verify_chain(&bc)) {
        printf("Resultat : La blockchain a REJETE l'attaque avec succes !\n");
    }

    return 0;
}