# Guilherme Teodoro de Oliveira RA: 10425362
# Luís Henrique Ribeiro Fernandes RA: 10420079
# Vinícius Brait Lorimier - 10420046

# Define o compilador
CC = gcc

# Flags de compilação
# -Wall para mostrar todos os warnings
# -fopenmp para suporte a OpenMP
CFLAGS = -Wall -O3 -fopenmp 
# Flags de linkagem
LDFLAGS = -fopenmp

# Cabeçalhos
DEPS = hash_table.h

# Objeto comum da tabela hash
OBJ_COMMON = hash_table.o

# Nome dos arquivos fonte
SEQ_SOURCE = analyzer_seq.c

# Objetos dos executáveis
SEQ_OBJ = analyzer_seq.o

# Executáveis finais
SEQ_TARGET = analyzer_seq

# Regra para compilar as três versões
all: $(SEQ_TARGET) 

# Compilação da hash_table
$(OBJ_COMMON): hash_table.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Compilação e linkagem das versões sequencial e paralelas
$(SEQ_OBJ): $(SEQ_SOURCE) $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(SEQ_TARGET): $(SEQ_OBJ) $(OBJ_COMMON)
	$(CC) -o $@ $^ $(LDFLAGS)

# Limpeza dos arquivos compilados, incluindo o arquivo de resultados
clean:
	rm -f *.o analyzer_seq results.csv