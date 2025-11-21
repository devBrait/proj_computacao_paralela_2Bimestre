/*
    Guilherme Teodoro de Oliveira RA: 10425362
    Luís Henrique Ribeiro Fernandes RA: 10420079
    Vinícius Brait Lorimier - 10420046
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>
#include "hash_table.h"

#define TABLE_SIZE 150000 
#define BUFFER_SIZE 4096

#define MANIFEST_FILE "manifest.txt"
#define LOG_FILE "log_distribuido.txt"

// Função de Limpeza Simples do Manifest (Remove Espaços em Branco no Final)
void clean_manifest(char* str) {

    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len-1])) {
        str[len-1] = '\0';
        len--;
    }
}

// Função de Extração de URL do Log
char* get_url(char* line) {
    // Procura onde começa o "GET "
    char* start = strstr(line, "GET ");

    if (!start) return NULL; // Não achou GET, ignora a linha
    start += 4;      // Pula os 4 caracteres do "GET "

    // Procura o próximo espaço (que separa a URL do HTTP/1.1)
    char* end = strchr(start, ' ');
    if (end) {
        *end = '\0'; // Corta a string aqui
    }

    return start;
}

// Função de Carregamento do Arquivo Manifest.txt
void load_manifest(HashTable* ht, const char* filename) {

    FILE* file = fopen(filename, "r");
    if (!file) { perror("Erro manifesto"); exit(1); }

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file)) {
        clean_manifest(line);
        if (strlen(line) > 0) {
            ht_put(ht, line);
        }
    }

    fclose(file);
    printf("Arquivo manifest.txt carregado com sucesso!!!\n");
}

void process_logs(HashTable* ht, const char* filename) {

    FILE* file = fopen(filename, "r");
    if (!file) { perror("Erro log"); exit(1); }

    char line[BUFFER_SIZE];
    long total = 0;
    long hits = 0;

    while (fgets(line, sizeof(line), file)) {
        // Extrai apenas a URL da linha de log complexa
        char* url = get_url(line);

        if (url && strlen(url) > 0) {
            CacheNode* node = ht_get(ht, url);
            if (node) {
                node->hit_count++; 
                hits++;
            }
        }
        total++;
    }

    fclose(file);
    printf("Processamento executado com sucesso. Linhas: %ld | Hits Confirmados: %ld\n", total, hits);
}

int main(int argc, char* argv[]) {

    const char* log_file = (argc > 1) ? argv[1] : LOG_FILE;
    HashTable* ht = ht_create(TABLE_SIZE);
    load_manifest(ht, MANIFEST_FILE); // Carrega o arquivo manifest.txt

    double start = omp_get_wtime();
    process_logs(ht, log_file);
    double end = omp_get_wtime();

    printf("Tempo Sequencial: %f s \n", end - start);
    ht_save_results(ht, "results.csv");
    ht_destroy(ht);
    
    return 0;
}
