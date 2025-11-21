# Guilherme Teodoro de Oliveira RA: 10425362
# Luís Henrique Ribeiro Fernandes RA: 10420079
# Vinícius Brait Lorimier - 10420046

# Define o compilador
CC = gcc

# Flags de compilação
# -Wall para mostrar todos os warnings
CFLAGS = -Wall -O3 -fopenmp 

# Lista de dependências e objetos
DEPS = hash_table.h
OBJ_COMMON = hash_table.o
OBJ_SEQ = analyzer_seq.o

# Regra principal (por enquanto gera apenas o sequencial)
all: analyzer_seq

# Compila a hash_table (genérico)
hash_table.o: hash_table.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Compila o analyzer_seq
analyzer_seq.o: analyzer_seq.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Linka o executável final
analyzer_seq: $(OBJ_SEQ) $(OBJ_COMMON)
	$(CC) -o analyzer_seq $(OBJ_SEQ) $(OBJ_COMMON) $(CFLAGS)

# Limpeza dos arquivos compilados
clean:
	rm -f *.o analyzer_seq results.csv