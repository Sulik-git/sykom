#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <errno.h>
#include <string.h>

#define SYSFS_FILE_A1 "/sys/kernel/sykt/sjda1"
#define SYSFS_FILE_A2 "/sys/kernel/sykt/sjda2"
#define SYSFS_FILE_W  "/sys/kernel/sykt/sjdw"		//Definicja opdowiednich makr 
#define SYSFS_FILE_B  "/sys/kernel/sykt/sjdb"
#define SYSFS_FILE_L  "/sys/kernel/sykt/sjdl"
#define uint32_t unsigned long
#define MAX_BUFF 16 

void print_buffer(int i, char *buf)
{
	for(int z = 0; z<i;z++)						//Funkcja print_buffer służaca do prawidłowego wypisania 
	{											//zawartości bufora do konsoli.
		printf("%c",buf[z]);
	}
}

int test_module( uint32_t a1, uint32_t a2){ 
	char bufferW[MAX_BUFF];
	char bufferL[MAX_BUFF];						
	char bufferA1[MAX_BUFF];				//Deklaracja buforów
	char bufferA2[MAX_BUFF];				
	char bufferB[MAX_BUFF];					
	int wn,ln,a1n,a2n,bn;					//Deklaracja zmiennych służacych do przechowania liczby znaków odczytanych
											// za pomocą funkcji fread. 
	FILE *a1_file; 		
	FILE *a2_file; 							//Deklaracja wskaźników do plików	
	FILE *w_file; 		
	FILE *b_file; 		
    FILE *l_file;        
	
    a1_file = fopen(SYSFS_FILE_A1, "w");
	if (a1_file == NULL){					//obsługi wujatkow
		printf("Couldn't open a1 file.");
		return -1;
	}
	snprintf(bufferA1, MAX_BUFF, "%d", a1);
	fwrite(bufferA1, 1, strlen(bufferA1), a1_file);			//Zapis do pliku A1
	fclose(a1_file);	

    a2_file = fopen(SYSFS_FILE_A2, "w");
	if (a2_file == NULL){
		printf("Couldn't open a2 file.");
		return -1;
	}
	snprintf(bufferA2, MAX_BUFF, "%d", a2);
	fwrite(bufferA2, 1, strlen(bufferA2), a2_file);			//Zapis do pliku A2
    fclose(a2_file);


	for(;;){
		b_file = fopen(SYSFS_FILE_B, "r");
		if (b_file == NULL){
			printf("Couldn't open status file.");			//Pętla oczekująca na zakończenie wykonywanej operacji mnożenia
			return -1;										
		}
		fread(bufferB, 1, MAX_BUFF, b_file);
		if (bufferB[2] == '0' | bufferB[2] == '1') break;						//Warunek kończący pętlę - "Jeżeli stan rejestru B mówi o zakończeniu 
    	fclose(b_file);															//operacji mnożenia to przerwij pętlę"

        sleep(1);
	}
	
    
	w_file = fopen(SYSFS_FILE_W, "r");
	if (w_file == NULL){
		printf("Couldn't open result file.");
		return -1;
	}

    l_file = fopen(SYSFS_FILE_L, "r");
    if (l_file == NULL){
        printf("Couldn't open result file.");
        return -1;
    }

    a1_file = fopen(SYSFS_FILE_A1, "r");
	if (a1_file == NULL){					//obsługi wujatkow
		printf("Couldn't open a1 file.");
		return -1;
	}
    a2_file = fopen(SYSFS_FILE_A2, "r");
	if (a2_file == NULL){
		printf("Couldn't open a2 file.");
		return -1;
	}
	b_file = fopen(SYSFS_FILE_B, "r");
	if (b_file == NULL){
		printf("Couldn't open status file.");			
		return -1;										
	}
	wn = fread(bufferW, 1, MAX_BUFF, w_file);
	ln = fread(bufferL, 1, MAX_BUFF, l_file);
	bn = fread(bufferB, 1, MAX_BUFF, b_file);
	a1n = fread(bufferA1, 1, MAX_BUFF, a1_file);
	a2n = fread(bufferA2, 1, MAX_BUFF, a2_file);
	fclose(l_file);						
   	fclose(w_file);
   	fclose(b_file);
   	fclose(a1_file);
   	fclose(a2_file);

	
	printf("Wynik mnozenia liczb %d i %d wynosi: ", a1, a2);
	print_buffer(wn,bufferW);
	printf("Liczba jedynek w wyniku wynosi: ");
	print_buffer(ln,bufferL);
	printf("Rejestr statusu: ");
	print_buffer(bn,bufferB);
	printf("Rejestr A1: ");							//Wypisanie buforow przechowujacych odczytane wartości rejestrów
	print_buffer(a1n,bufferA1);
	printf("Rejestr A2: ");
	print_buffer(a2n,bufferA2);
    printf("\n");



	
	return 0;
} 

int main(void){
	printf("Compiled at %s %s\n", __DATE__, __TIME__);
	test_module(2,4);
   	test_module(19,27);
	test_module(211,123);    									//Testowanie modułu
    test_module(2222,1333);
    test_module(2222222,1333);
	return 0;
}

