CC=@gcc
CFLAGS=-std=c99 -lm
SRC=./src
BUILD=genetico
FORMAT=.out.exe

all: 
	@# Compila printando a saída do GCC
	$(CC) $(CFLAGS) $(SRC)/*.c -o $(BUILD)$(FORMAT)
	@echo "Compilado!"

silence:
	@# Compila mutando a saída do GCC, rediricionando warnings para /dev/null - OBS: Só funciona em sistemas linux
	$(CC) $(CFLAGS) $(SRC)/*.c -o $(BUILD)$(FORMAT) 2>/dev/null
	@echo "Compilado!"

clean:
	@# Apaga todos os arquivos objeto e binários já compilados anteriormente neste diretório
	@rm -f *.o *.out.exe *.txt
	@echo "Ambiente limpo!"

test:
	@./$(BUILD)$(FORMAT)