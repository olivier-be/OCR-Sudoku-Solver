#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "network.h"
#include "save_network.c"
#include "train_network.c"
#include "mnist.h"

double randfrom(double min, double max) 
{
	double range = (max - min); 
	double div = RAND_MAX / range;
	return min + (rand() / div);
}




struct Neural** initnetwork()
{

	struct Neural **network = malloc(sizeof(struct Neural*) * 3);

	size_t n = 0;

	while ( n< 3 )
	{
		*(network + n) = malloc( sizeof(struct Neural) * e[n]);
		size_t i = 0;
		while (i < e[n])
		{
			if (n != 0)
			{
				(*(*(network + n) + i)).biases = 1;
				(*(*(network + n) + i)).previouslayer =(void *)(network +n - 1);
				(*(*(network + n) + i)).value = 1;

				double *v= calloc(e[n - 1], sizeof(double));
				for (size_t p =0 ; p < e[n - 1]; p++)
				{
					*(v + p) = randfrom(-1,1);
				}
				(*(*(network + n) + i)).weights = v;
			}
			else 
			{
				(*(*(network + n) + i)).biases = 0;
				(*(*(network + n) + i)).previouslayer = NULL;
				(*(*(network + n) + i)).value = 0;
				(*(*(network + n) + i)).weights = NULL;
			}
			i++;
		}
		n++;
	}
	return network;
}

double sigmoid(double z)
{
	return 1/(1+exp(-z));
}


double active_func(struct Neural neural,size_t size)
{
	//sigmoid
	double res = 0;
	res += neural.biases;
	for (size_t p = 0; p < size; p++)
	{
		res +=(*(*((struct Neural**)(neural.previouslayer)) + p)).value * *(neural.weights + p);
	}
	return res;
}


size_t softmax(struct Neural *neural,size_t size, double *res)
{
	double down =0;
	for (size_t i = 0 ; i < size; i++ )
	{
		*(res + i )= exp((*(neural+i)).value);
		down += *(res + i);
	}

	for (size_t i = 0 ; i < size; i++ )
	{
		*(res + i ) /= down;
		(*(neural+i)).value= *(res + i );
	}
	double max = res[0];
	size_t p = 0;
	for (size_t i = 0 ; i< 10 ; i++)
	{
		if (max < res[i])
		{
			p = i;
			max = res[i];
		}
	}
	return p;
}



size_t use_network(struct Neural **network)
{

	size_t n = 1;
	size_t i = 0;
	while ( n< 3 )
	{
		i = 0;
		while (i < e[n])
		{
			(*(*(network + n) + i)).value = sigmoid (active_func((*(*(network + n) + i)),e[n-1])) ; 
			i++;

		}
		n++;
	}
	double res[10];
	return softmax(*(network + 2) , e[2], res);
}

void free_network(struct Neural **network)
{
	size_t n = 0, i ;
	while ( n < 3 )
	{
		i = 0;
		while (i < e[n] )
		{
			free((*(*(network + n) + i)).weights);
			i++;
		}
		free(*(network + n));
		n++;
	}
	free(network);
}

void set_network( struct Neural **network , double *array ,size_t size)
{
	size_t p = 0;
	while (p < size && p < e[0])
	{
		//printf("%zu %f value\n",p, *(array+ p));
		(*(*network + p)).value= *(array + p);
		//printf("%zu %f value\n",p, (*(*network + p)).value);
		p++;
	}
	while (p < e[0] )
	{
		(*(*network + p)).value = 0;
		p++;
	}
}

int testnetwork ()
{
	srand(time(NULL));

	//double array[5] = {2, 2,2,2,2};
	double array[784];
	for (size_t p = 0 ; p < 784; p++)
	{
		*(array + p) = randfrom(0,255);
		//printf("%f \n",*(v + p));
	}


	struct Neural **network = initnetwork();
	set_network (network, array ,784);



	//printf("pow %i \n",pow2(9,6));
	/*size_t p = 0;
	  while (p < 784)
	  {
	  if ((*(*network + p)).value != 0)
	  printf("%f value\n",(*(*network + p)).value); //(*(*network + p)).value);
	  p++;
	  }*/

	printf("%zu value\n", use_network(network));
	save(network);
	printf("save done\n");
	//free_network(network); 
	struct Neural **network2= initfromfile(e);
	set_network (network2, array ,784);
	printf("%zu value\n", use_network(network2));

	// test for  findvalue in text file
	/*
	   FILE * inputFile;
	   int endofline= 0;
	   inputFile = fopen( "test.txt", "r" );
	   char word[50];
	   int x =0;
	   while (!feof(inputFile))
	   {
	   x++;
	   printf("value find is : %f \n",findvalue(inputFile, &endofline,word));
	   }
	   fclose( inputFile ); 
	   */

	free_network(network);
	free_network(network2);
	return 0 ;
}

void trainmnist()
{
	load_mnist();
	e[0]=784;
	srand(time(NULL));
	struct Neural **network = initnetwork();
	size_t z=0;
	double **train2 = malloc(sizeof(double **)* NUM_TRAIN);
	while (z < NUM_TRAIN)
	{
		*(train2+z) = malloc(sizeof(double *)*e[0]);
		size_t y=0;
		while (y < e[0])
		{
			*(*(train2+z)+y) = train_image[z][y];
			y++;
		}
		z++;
	}

	z=0;
	while (z<1000)
	{
		printf("the actual test is : %zu\n\n\n",z);
		train (network,train2,train_label,60000,e[0] );
		z++;
	}
	/*
	   z = 0;
	   while (z<10)
	   {
	   printf("the actual test is : %zu\n",z);
	   train (network,arrayin +1 ,arrayout +1,1,4 );
	   z++;
	   }
	   */
	int i =0, found = 0;
	while (i<60000)
	{
		set_network (network, train2[i] ,e[0]);
		int res = use_network(network);
		printf("get %i  exepted %i value is egual = %i \n",res,train_label[i],(int)res == train_label[i] );
		if (train_label[i] ==(int) res)
		{
			found++;
		}
		i++;
	}
	printf("nb good is %i \n",found);
	save(network);
	free_network(network);



}

int main ()
{
	//testnetwork();
	//testtrain();
	//xor();
	trainmnist();
	return 0;
}





