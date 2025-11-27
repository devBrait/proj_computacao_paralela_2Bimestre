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

// Estrutura para armazenar o log na memória
typedef struct {
    char** lines;
    long count;
} logStruct;

// Função de Limpeza Simples do Manifest, para remover espaços em branco no final
void clean_manifest(char* str) {
    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len-1])) {
        str[len-1] = '\0';
        len--;
    }
}

// Função para extrair a URL do log
int get_url(const char* line, char* url_out, size_t buffer_size) {
    
    const char* start = strstr(line, "GET "); // Procura a string "GET "
    if (!start) return 0; // Se não encontrar, retorna 0
    start += 4; 

    const char* end = strchr(start, ' ');
    size_t len = end ? (size_t)(end - start) : strlen(start);
    if (len == 0 || len >= buffer_size) return 0;

    memcpy(url_out, start, len);
    url_out[len] = '\0';
    return 1;
}

// Função de Carregamento do Arquivo Manifest.txt
void load_manifest(HashTable* ht, const char* filename) {

    FILE* file = fopen(filename, "r");
    if (!file) { 
        printf("Erro ao abrir o arquivo manifest.txt!\n");
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

// Função para carregar o log na memória
logStruct load_log_memory(const char* filename) {

    FILE* file = fopen(filename, "r");
    if (!file) { 
        printf("Erro ao abrir o arquivo de log!\n");
        exit(1); 
    }

    // Inicializa a capacidade e aloca memória
    long capacity = 1024 * 16; 
    char** lines = malloc(capacity * sizeof(char*));
    if (!lines) exit(1);

    char buffer[BUFFER_SIZE];
    long count = 0;

    // Lê cada linha do arquivo de log
    while (fgets(buffer, sizeof(buffer), file)) {
        if (count == capacity) {
            long new_capacity = capacity * 2;
            char** tmp = realloc(lines, new_capacity * sizeof(char*)); // Realoca memória para armazenar mais linhas

            if (!tmp) exit(1); // Em caso de falha, termina o programa
            lines = tmp;
            capacity = new_capacity;
        }

        // Duplica a linha e armazena no array
        lines[count] = strdup(buffer);
        count++;
    }

    // Fecha o arquivo de log
    fclose(file);
    if (count < capacity) {
        char** tmp = realloc(lines, count * sizeof(char*)); // Ajusta o tamanho do array para a contagem real
        if (tmp) lines = tmp;
    }
    return (logStruct){lines, count};
}

// Libera a memória do log carregado
void clean_log_memory(logStruct log) {
    if (!log.lines) return;
    for (long i = 0; i < log.count; i++) {
        free(log.lines[i]);
    }
    free(log.lines);
}

int main(int argc, char* argv[]) {

    // Verifica se o arquivo de log foi fornecido como argumento
    const char* log_file = (argc > 1) ? argv[1] : NULL;
    if(log_file == NULL) {
        printf("Uso correto: %s <arquivo_log>\n", argv[0]);
        return 1;
    }

    HashTable* ht = ht_create(TABLE_SIZE); // Cria a tabela hash
    load_manifest(ht, MANIFEST_FILE); // Carrega o arquivo manifest.txt
    
    logStruct log = load_log_memory(log_file); // Carrega o log na memória
    printf("Log carregado na memória: %ld linhas.\n", log.count);

    double start = omp_get_wtime(); // Início da medição de tempo
    for (long i = 0; i < log.count; i++) {

        char url_buf[BUFFER_SIZE]; // buffer temporário para a URL
        if (!get_url(log.lines[i], url_buf, sizeof(url_buf))) continue;// Caso não extraia a URL vai para próxima linha

        CacheNode* node = ht_get(ht, url_buf); // Procura na hash table
        if (node) node->hit_count++; 
    }   
    double end = omp_get_wtime();

    // Exibe resultados finais e salva o result.csv
    printf("Tempo Sequencial: %f segundos\n", end - start);
    ht_save_results(ht, "results.csv");
    clean_log_memory(log);
    ht_destroy(ht);

    return 0;
}