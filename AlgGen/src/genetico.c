#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define PRINT_DEC 0       // 1 para printar os indivíduos em decimal || 0 para printar os indivíduos em binário
#define PI 3.14159265359
#define GERACOES 500
#define TORNEIO_LEN 30
#define GEN_LEN 9
#define CROSSOVER 0.6
#define MUTACAO 0.25
#define NPOP 30
#define SOLUCAO 2.85 // Resultado buscado, definido para critério de parada

void bigbang(FILE* output);
void gerapop(int* popul);
int avaliaInd(int* individuo);
void avaliaPop(int* popul);
int melhor(int* popul);
void cruzamento(int* pai, int* mae);
void mutar(int* individuo);
int* decbin(int num, int bits);
void showbin(int bits, int *vec, FILE* out);
int bindec(int* vec, int bits);

int fitness[NPOP];  // Vetor que armazena o fitness de todos os individuos, com indice identico ao vetor populacao, declarado no main


int main(){
	// unsigned long seed = 1529614466;    // Seed de rand para que => X = 1.85 e Y = 2.85 no indivíduo 15
	// unsigned long seed = 1529648723;    // Seed de rand que acha a solução na 44ª geração
	// unsigned long seed = 1529693143;    // Seed de rand que acha a solução na 56ª geração
	unsigned long seed = time(NULL);       // Seed de rand que usa a hora atual, em segundos, contando desde ás 00h de 01/01/1970
	srand(seed);                           // Usa a seed para configurar o rand

	FILE* saida;                           // Arquivo de saída
	if((saida = fopen("Mundo.txt","w+")) == NULL){
		printf("Não consegui abrir o arquivo!\nEncerrando...\n");
		return 1;
	}

	fprintf(saida, "[Configuração inicial]\nSeed desta execução: %d\nNúmero de indivíduos da população: %d\nCritério de Parada: Fitness do melhor indivíduo é igual a %d\n\nExecução:\n", seed,((int)NPOP),((int)(100*SOLUCAO)));
	bigbang(saida);
	fclose(saida);
	return 0;
}

void bigbang(FILE* output){
	int* populacao = (int *) calloc(NPOP,sizeof(int));
	int g = 0;
	int mel = 0, decmel = 0;
	gerapop(populacao);

	for(g = 1; g <= GERACOES; g++){
		avaliaPop(populacao);
		mel = melhor(populacao);
		decmel = bindec(populacao[mel], GEN_LEN);
		// printf("Geração [%d] -- Erro: %d -- Melhor Fitness: %d\n", g, ((int)(100*SOLUCAO)-fitness[mel]), fitness[mel]);
		fprintf(output, "Geração %d\n", g);
		fprintf(output, "    Melhor indivíduo (x): ", g);

		#if PRINT_BIN == 0
			fprintf(output, " %d    ||    Erro deste indivíduo: %d\n", decmel, ((int)(100*SOLUCAO)-fitness[mel]));
		#else
			showbin(GEN_LEN, populacao[mel], output);
			fprintf(output, "    ||    Erro deste indivíduo: %d\n", ((int)(100*SOLUCAO)-fitness[mel]));
		#endif

		if(fitness[mel] == (100*SOLUCAO))
			break;

		for(int t = 0; t < TORNEIO_LEN; t++){
			float chance_cruz = (float)(rand()%101)/100;
			if(chance_cruz < CROSSOVER){
				int pai = rand()%NPOP;
				int mae;
				do{
					mae = rand()%NPOP;
				}while(mae == pai);
				if(avaliaInd(populacao[mae]) > avaliaInd(populacao[pai])){
					cruzamento(populacao[pai], populacao[mae]);
				}
				else{
					cruzamento(populacao[mae], populacao[pai]);
				}
			}
		}
	}
	// int scoreMel = avaliaInd(populacao[mel]);
	// float xMel = bindec(populacao[mel], GEN_LEN)/100.0;
	// float yMel = scoreMel/100.0;
	// printf("\n\n==Resultado==\nGeração [%d]:\n", g);
	// printf("Fitness do melhor indivíduo: %d\n", scoreMel);
	// printf("X [Melhor]: %.2f\t", xMel);
	// printf("Y [Melhor]: %.2f\n", yMel);

	fprintf(output, "\n[Resultado]\n");
	fprintf(output, "Número de Gerações: %d\n", g);
	#if PRINT_BIN == 0
		fprintf(output, "Melhor indivíduo: %d\n", decmel);
	#else
		fprintf(output, "Melhor indivíduo: ");
		showbin(GEN_LEN, populacao[mel], output);
	#endif
	fprintf(output, "Erro do melhor indivíduo: %d\n", ((int)(100*SOLUCAO)-fitness[mel]));
	for(int i = 0; i < NPOP; i++)
		free(populacao[i]);
	free(populacao);
}

void gerapop(int* popul){
    for(int i = 0 ; i < NPOP; i++){
		popul[i] = (int *) calloc(GEN_LEN,sizeof(int));
        int n = rand()%301-100;
        popul[i] = decbin(n, GEN_LEN);
    }
}

int avaliaInd(int* individuo){
	float x = bindec(individuo, GEN_LEN)/100.0;
	float y = x*sin(10*PI*x)+1;
	return (y*100);
}

void avaliaPop(int* popul){
	for(int i = 0; i < NPOP; i++){
		fitness[i] = avaliaInd(popul[i]);
	}
}

int melhor(int* popul){
	int superman = 0;
	for(int i = 1; i < NPOP; i++){
		if(fitness[i] > fitness[superman])
			superman = i;
	}
	return superman;
}

void cruzamento(int* pai, int* mae){
	int *filho = (int *) calloc(GEN_LEN,sizeof(int));
	int ponto;
	float chance_xman = (float)(rand()%101)/100;
	do{
		ponto = rand()%(GEN_LEN+1);
	}while(ponto <= 0 || ponto > GEN_LEN);

	for(int i = 0; i < ponto; i++)
		filho[i] = pai[i];
	for(int j = ponto; j < GEN_LEN; j++)
		filho[j] = mae[j];

	if(chance_xman < MUTACAO)
		mutar(filho);
	
	int scoreFilho = avaliaInd(filho);
	int scorePai   = avaliaInd(pai);
	int decfilho   = bindec(filho, GEN_LEN); // Unico jeito de limitar os filhos ao intervalo fechado de [-1,2] com números binários

	if(scoreFilho > scorePai && filho && decfilho < 200 && decfilho > -100){
		for(int i = 0; i < GEN_LEN; i++){
			pai[i] = filho[i];
		}
	}
	free(filho);
}


void mutar(int* individuo){
	int ponto = rand()%GEN_LEN;
	if(individuo[ponto] == 0)
		individuo[ponto] = 1;
	else
		individuo[ponto] = 0;
}

/////// Funções para converter binários e decimais e mostrar o binário
int* decbin(int num, int bits){
	int x = num, i = 0;
	int *bin = (int *) calloc(bits,sizeof(int));
    if(num < 0){
    	x = -num;
    	bin[(bits-1)] = 1;
    }
	while(x != 0){
		bin[i] = (int)x%2;
		x = (int)(x/2);
		i++;
	}
	return bin;
}

void showbin(int bits, int *vec, FILE* out){
    int len = bits-1;
	int i =0;
	// printf("Binario  normal: ");
	for(i = len; i >= 0; i--)
		fprintf(out,"%i", vec[i]);
	// fprintf(out, "\n");
/*
	printf("Binario inverso: ");
	for(i = 0; i < len+1; i++)
		printf("%i", vec[i]);
	printf("\n");
*/
}

int bindec(int *vec, int bits){
	int result = 0;
	int len = bits-1;
	int i = 0;
	for (i = 0; i < len; i++)
		result += vec[i]*pow(2,i);
	if(vec[(bits-1)] == 1)
		return -result;
	return result;
}
