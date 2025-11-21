/*
    Guilherme Teodoro de Oliveira RA: 10425362
    Luís Henrique Ribeiro Fernandes RA: 10420079
    Vinícius Brait Lorimier - 10420046
*/

// Import das libs necessárias
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>
#include "hash_table.h"

// Declarações de constantes
#define TABLE_SIZE 150000 
#define BUFFER_SIZE 4096

#define MANIFEST_FILE "manifest.txt"

// Função de Limpeza Simples do Manifest, para remover espaços em branco no final
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

    // Procura o próximo espaço após a URL
    char* end = strchr(start, ' ');
    if (end) {
        *end = '\0'; // Corta a string aqui
    }

    return start; // Retorna a URL extraída
}

// Função de Carregamento do Arquivo Manifest.txt
void load_manifest(HashTable* ht, const char* filename) {

    FILE* file = fopen(filename, "r");
    if (!file) { 
        printf("Erro ao abrir o arquivo manifest.txt\n");
        exit(1);
    }

    char line[BUFFER_SIZE];
    // Percorre cada linha do arquivo manifest.txt
    while (fgets(line, sizeof(line), file)) {
        clean_manifest(line); // Limpa espaços em branco no final
        if (strlen(line) > 0) {
            ht_put(ht, line);
        }
    }

    fclose(file);
    printf("Arquivo manifest.txt carregado com sucesso!!!\n");
}

// Função de Processamento do Arquivo de Log
void process_logs(HashTable* ht, const char* filename) {

    FILE* file = fopen(filename, "r");
    if (!file) { 
        printf("Erro ao abrir o arquivo de log\n");
        exit(1);
    }

    // Declara variáveis para contagem de linhas e hits
    char line[BUFFER_SIZE];
    long total = 0;
    long hits = 0;

    // Percorre cada linha do arquivo de log
    while (fgets(line, sizeof(line), file)) {
        // Extrai apenas a URL da linha de log complexa
        char* url = get_url(line);

        // Verifica se a URL está na tabela hash
        if (url && strlen(url) > 0) {
            CacheNode* node = ht_get(ht, url);
            if (node) {
                node->hit_count++; 
                hits++;
            }
        }
        total++;
    }

    // Fecha o arquivo de log após o processamento
    fclose(file);
    printf("Processamento executado com sucesso. Linhas: %ld | Hits Confirmados: %ld\n", total, hits);
}

int main(int argc, char* argv[]) {

    // Verifica se o arquivo de log foi fornecido como argumento
    const char* log_file = (argc > 1) ? argv[1] : NULL;
    if(log_file == NULL) {
        fprintf(stderr, "Uso correto para execução do programa: %s <arquivo_log>\n", argv[0]);
        return 1;
    }

    HashTable* ht = ht_create(TABLE_SIZE); // Cria a tabela hash
    load_manifest(ht, MANIFEST_FILE); // Carrega o arquivo manifest.txt

    double start = omp_get_wtime(); // Início da medição de tempo
    process_logs(ht, log_file); // Processa o arquivo de log
    double end = omp_get_wtime(); // Fim da medição de tempo

    // Exibe resultados finais e salva o result.csv
    printf("Tempo Sequencial: %f s \n", end - start);
    ht_save_results(ht, "results.csv");
    ht_destroy(ht);
    
    return 0;
}
