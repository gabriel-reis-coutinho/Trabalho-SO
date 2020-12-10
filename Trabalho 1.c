/*
Gabriel dos Reis Coutinho 9807124


*/

/*
Algoritimo apresentado em sala de aula

typedef int semaphore;
semaphore mutex = 1
semaphore dp = 1;
int rc = 0;

void reader(void){
	while(TRUE){
		down(&mutex);
		rc = rc + 1;
		if(rc == 1)	down(&db);
		up(&mutex);
		read_data_base();
		down(&mutex);
		rc = rc - 1;
		if(rc == 0)	up(&db);
		up(&mutex);
		use_data_read();
	}
}

void writer(void){
	while(TRUE){
		think_up_data;
		down(&db);
		write_data_base();
		up(&db);
	}
}
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>


sem_t wrt; //semaforo binário para exclusao mutua e sinalização
pthread_mutex_t mutex; //exclusão mutua enquanto o qtdReader está sendo modificado
int data = 1;
int qtdReader = 0; //quantidade de readers

void *writer(void *nroWriter){
	sem_wait(&wrt);
	data++;

	//Escrevendo
	printf("Escritor %d modificou data para %d\n", (*((int *)nroWriter)), data);
	/*
	função de escrita em um arquivo real?
	*/
	sem_post(&wrt);
}

void *reader(void *nroReader){
	pthread_mutex_lock(&mutex);//Lock antes de modificar o qtdReader
	qtdReader++;
	if (qtdReader == 1)
	{
		sem_wait(&wrt);//Se for o primeiro reader, vai bloquear o writer
	}
	pthread_mutex_unlock(&mutex);

	//Lendo
	printf("Reader %d: leu data como %d\n",*((int *)nroReader), data);
	/*
	função de leitura de um arquivo real?
	*/

	pthread_mutex_lock(&mutex);
	qtdReader--;
	if (qtdReader == 0)
	{
		sem_post(&wrt);//se for o ultimo reader, vai acordar o writer
	}
	pthread_mutex_unlock(&mutex);
}

int main(){

	pthread_t read[10],write[10];//conforme a especificação, 10 leitores.
	pthread_mutex_init(&mutex, NULL);
	sem_init(&wrt, 0, 1);

	int a[10] = {1,2,3,4,5,6,7,8,9,10};//nomeando os consumidores e produtores

	for (int i = 0; i < 10; ++i)
	{
		pthread_create(&read[i], NULL, (void *)reader, (void*)&a[i]);
	}

	for (int i = 0; i < 10; ++i)
	{
		pthread_create(&write[i], NULL, (void *)writer, (void *)&a[i]);
	}


	for (int i = 0; i < 10; ++i)
	{
		pthread_join(read[i], NULL);
	}

	for (int i = 0; i < 6; ++i)
	{
		pthread_join(write[i], NULL);
	}

	pthread_mutex_destroy(&mutex);
	sem_destroy(&wrt);

	return 0;
}