/************************************************************
Sistemas Operacionais I (SSC541)
Trabalho Problema dos Leitores e Escritores

ICMC 2020
Prof. Sarita

Grupo
Gabriel dos Reis Coutinho 9807124
Mônica Colacique 7143527
Jonas Wendel 10295461 


*************************************************************/

/************************************************************
No presente trabalho, um arquivo .txt é compartilhado entre
processos.

O processo leitor escreve 3 números (a,b,c) no arquivo. Cada
processo leitor acessa e lê o arquivo e realiza uma operação
aritmética diferente.

Foram implementados pontos de parada para os processos
leitores e escritores.
*************************************************************/



/************************************************************/




/* Bibliotecas para implementação de POSIX threads, operações de semáforos e E/S */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

/* Quantidade de escritores (1) e leitores (10) e do numero de numeros no arquivo */
#define N_WRITER 1
#define N_READER 11
#define MAX_READER 10
#define N_ARRAY 3


sem_t wrt, rdr; //semaforo para exclusao mutua e sinalização
pthread_mutex_t mutex; //exclusão mutua
pthread_cond_t cond;


int countReader = 0;

/* Intervalo dos numeros aleatorios */
int randRange(int min, int max){
   return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

/* Funcao para gerar array de numeros aleatorios */
int *getRandom(){

   static int numberArray[N_ARRAY];
   int i;

   srand((unsigned)time(NULL));

   for ( i = 0; i < N_ARRAY; ++i) {
      numberArray[i] = randRange(1,100);
   }

   return numberArray;
}

/* Ler conteudo de arquivo como array de inteiros */
void readFile(int sizeArray, int numbersArray[]){

    FILE *myFile;
    myFile = fopen("f.txt", "r");

    for (int i = 0; i < sizeArray; i++){
        fscanf(myFile, "%d", &numbersArray[i]);
    }

   for (int i = 0; i <sizeArray; i++){
        fprintf(myFile, "---- aqui %d\n", numbersArray[i]);
    }

    fclose(myFile);

}

void writeFile(int sizeArray, int numbersArray[]){

    FILE *myFile;
    myFile = fopen("f.txt", "w");


    for (int i = 0; i <sizeArray; i++){
        fprintf(myFile, "%d\n", numbersArray[i]);
    }

    fclose(myFile);

}

void *writer(void *nroWriter){

  int *p, i, data_write[N_ARRAY], c;
  int data = 0;
  int writer_flag = 30;

  while(1){

    p = getRandom();

   for ( i = 0; i < N_ARRAY; i++ ) {
      data_write[i] = *(p + i);
   }
    writeFile(3,data_write);

    sem_wait(&wrt);
    data++;

    //Escrevendo
    printf("\n\nEscritor %d modificou os dados do arquivo para \n  a = %d, b = %d, c = %d\n\n", (*((int *)nroWriter)), data_write[0],data_write[1],data_write[2]);
    sem_post(&wrt);
    sleep(2);

    writer_flag--;
              if(writer_flag < 0){
            printf("\n-----> Escritor %d saindo\n\n", (*((int *)nroWriter)));
            pthread_exit(0);}
  }
}

void *reader(void *nroReader){
  int c =0, operation, data_read[N_ARRAY]={0,0,0};
  int i = *(int *)nroReader;
  int data_r[N_ARRAY];

  while(1) {

    pthread_mutex_lock(&mutex);//Lock antes de modificar o countReader
    countReader++;
    //printf("\n countReader: %d", countReader);

    /* Primeiro Leitor bloqueia Escritor*/
    if (countReader == 1) {
     sem_wait(&wrt);
    }

    //caso exista um decimo primeiro reader, ele deve ser bloqueado e esperar outro reader terminar
    while(countReader == 11){
        pthread_cond_wait(&cond, &mutex);
    }

    //sem_post(&rdr);
    /* Ler o conteudo do arquivo como array de inteiros de tamanho N_ARRAY */
    c++;
    readFile(N_ARRAY, data_read);

    printf("\n\nOperacao Leitor %d: ", i);

    if (i == 1){

      operation = data_read[0]+data_read[1]+data_read[2];
      printf("a + b + c\n");
      printf("  %d + %d + %d = ", data_read[0],data_read[1],data_read[2]);
    }

    else if (i == 2){

      operation = data_read[0]*data_read[1]*data_read[2];
      printf("a * b * c\n");
      printf("  %d * %d * %d = ", data_read[0],data_read[1],data_read[2]);
    }

    else if (i == 3){

      operation = (data_read[0]+data_read[1]+data_read[2])/3;
      printf("media aritmetica\n");
      printf("  (%d + %d + %d)/3 = ", data_read[0],data_read[1],data_read[2]);
    }

    else if (i == 4){

      operation = data_read[0] - data_read[2];
      printf("a - c\n");
      printf("        %d - %d = ", data_read[0],data_read[2]);
    }

    else if (i == 5){

      operation = data_read[0] - data_read[1];
      printf("a - b\n");
      printf("        %d - %d = ", data_read[0],data_read[1]);
    }

    else if (i == 6){

      operation = data_read[0] * data_read[2];
      printf("a * c\n");
      printf("        %d * %d = ", data_read[0],data_read[2]);
    }

    else if (i == 7){

      operation = data_read[1] - data_read[2];
      printf("b - c\n");
      printf("        %d - %d = ", data_read[1],data_read[2]);
    }

    else if (i == 8){

      operation = data_read[1] + data_read[2];
      printf("b + c\n");
      printf("        %d + %d = ", data_read[1],data_read[2]);
    }

    else if (i == 9){

      operation = data_read[1] * data_read[2];
      printf("b * c\n");
      printf("        %d + %d = ", data_read[1],data_read[2]);
    }

    else if (i == 10){

      operation = data_read[0] + data_read[1];
      printf("a + b\n");
      printf("        %d + %d = ", data_read[0],data_read[1]);
    }

    else if (i == 11){
      operation = data_read[0] + data_read[0];
      printf("a + a\n");
      printf("        %d + %d = ", data_read[0],data_read[0]);
    }
    else
      printf("-");

    printf("%d",operation);



    pthread_mutex_unlock(&mutex);

    //se existirem 11 readers, dar o sinal para que o decimo primeiro possa continuar
    if(countReader == 11){
        countReader--;
        pthread_cond_signal(&cond);//sinal para o reader continuar
    }else{
        countReader--;
    }


  if (countReader == 0)
  {
  sem_post(&wrt);//se for o ultimo reader, vai acordar o writer
  }
  pthread_mutex_unlock(&mutex);
  sleep(5);
  c++;

   if(c % 10 == 0)
        {
          printf("\n\n---------- Leitor %d saindo ----------\n\n", (*((int *)nroReader)));
            pthread_exit(0);
        }
}
}

int main(){

  pthread_t read[N_READER],write[N_WRITER];
  pthread_mutex_init(&mutex, NULL);
  sem_init(&wrt, 0, 1);
  sem_init(&rdr, 0, 1);

  int return_code;

  /* Numerando escritores e leitores */
  int a[11] = {1,2,3,4,5,6,7,8,9,10,11};

  for (int i = 0; i < N_READER; ++i){

  //  printf("\n\nCriacao thread %d\n", i);
    return_code = pthread_create(&read[i], NULL, (void *)reader, (void*)&a[i]);
        if (return_code){
          printf("ERRO; mensagem de retorno %d\n", return_code);
          exit(-1);
        }
  }

  for (int i = 0; i < N_WRITER; ++i){
  //  printf("\n\nCriacao thread %d\n", i);
    return_code = pthread_create(&write[i], NULL, (void *)writer, (void *)&a[i]);
        if (return_code){
          printf("ERRO; mensagem de retorno %d\n", return_code);
          exit(-1);
       }
  }


  for (int i = 0; i < MAX_READER; ++i)
  {
    pthread_join(read[i], NULL);
  }

  for (int i = 0; i < N_WRITER; ++i)
  {
    pthread_join(write[i], NULL);
  }

  pthread_mutex_destroy(&mutex);
  sem_destroy(&wrt);

  return 0;
}
