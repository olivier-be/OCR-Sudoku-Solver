#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "network.h"

double sigmoid_prime(double z)
{
	return sigmoid(z)*(1-sigmoid(z)) ;
}

double sigmoid_prime_o(double z)
{
	return z*(1-z) ;
}



void free2d(double **tab, size_t s)
{
	free (*(tab));
	free (*(tab + 1));
	if (s == 3)
	{
		free (*(tab+2));
	}
	free (tab);

}

double active_func_train(struct Neural neural,size_t j)
{
	//sigmoid
	double res = 0;
	res += neural.biases;
	for (size_t p = 0; p < e[j]; p++)
	{
		res +=(*(*((struct Neural**)(neural.previouslayer)) + p)).value * *(neural.weights + p);
	}
	return res;
}



void use_network_getz(struct Neural **network, double **z)
{

	size_t j = 1;
	size_t k = 0;
	while (k < e[0])
	{
		(*(*(z ) + k)) = (*(*(network ) + k)).value;
		k++;
	}
	while ( j< 3 )
	{
		k = 0;
		while (k < e[j])
		{
			(*(*(z + j) + k)) = active_func_train((*(*(network + j) + k)),j-1);
			(*(*(network + j) + k)).value = sigmoid((*(*(z + j) + k)));
			k++;

		}
		j++;
	}
	//printf("done \n");
	//double res[10];
	//return softmax(*(network + 2) , e[2], res);
}



void setgradient(double **gradientb, double **gradientw)
{
	size_t init =0;
	while (init < e[1] * e[0])
	{
		*(*(gradientb  )+init)=0;
		*(*(gradientw  )+init)=0;
		init ++;
	}
	init =0;
	while (init < e[1] * e[2])
	{
		*(*(gradientb +1 )+init)=0.0;
		*(*(gradientw +1 )+init)=0.0;
		init ++;
	}
}



double cost(struct Neural **network,size_t  exepted)
{

	struct Neural *output = *(network + 2);
	double cost =0;
	for (size_t i = 0 ; i < e[2]; i++ )
	{
		if (i == exepted)
		{
			cost +=0.5 *( (*(output+i)).value - 1)*( (*(output+i)).value - 1);
		}
		else
		{
			cost +=0.5* (0 - (*(output+i)).value )*(0 - (*(output+i)).value);
		}
	}
	return cost;
}



double error_pad(double predicted, double exepted)
{
	return -(exepted - predicted);
}
double error(double predicted, double exepted)
{
	return 0.5 *(predicted - exepted)*(predicted - exepted);
}


double somme_gradient_weight(size_t j,size_t pos,struct Neural **neural, double **gradientb)
{
	double wpg = 0;
	size_t k = 0;
	while (k < e[j + 1])
	{
		wpg += *(*(gradientb+1)+ k * e[j] + pos) * 
			*((*(*(neural +j +1) + k)).weights + pos);
		k++;
	}
	return wpg;

}


void backpropagation(struct Neural** network,size_t exepted, double *set, double **w, double **b, double tvs)
{
	double **gradientw = malloc(sizeof(double *) * 2);
	*(gradientw  ) = malloc(sizeof(double ) * e[1] * e[0]);
	*(gradientw + 1) = malloc(sizeof(double ) * e[2]* e[1]);

	double **gradientb = malloc(sizeof(double *) * 2);
	*(gradientb  ) = malloc(sizeof(double ) * e[1] * e[0]);
	*(gradientb + 1 ) = malloc(sizeof(double ) * e[2] * e[1]);

	double **z = malloc(sizeof(double *) * 3);
	*(z  ) = malloc(sizeof(double ) * e[0]);
	*(z + 1 ) = malloc(sizeof(double ) * e[1]);
	*(z + 2) = malloc(sizeof(double ) * e[2]);

	setgradient(gradientb,gradientw);
	set_network (network, set ,tvs);
	use_network_getz(network,z);
	//printf("exepted is %zu \n",exepted);
	//printf("cost is : %f \n",cost(network,exepted));
	//double coste = cost (network , exepted);
	size_t j = 2,k = 0, p=0;
	double error;
	while (k < e[j])
	{
		p = 0;
		double find=0.0 ;
		if (k == exepted)
		{
			//printf("found tttttttttttttttttttttttttt\n");
			find = 1; //0.99;
		}
		//sp = sigmoid_prime_o((*(*(network +j) +k )).value);
		//printf("val out 1 %f \n",(*(*(network +j) +k )).value);
		error = error_pad( (*(*(network +j) +k )).value ,find ) * sigmoid_prime_o((*(*(network +j) +k )).value);
		while (p < e[j -1])
		{
			/*
			   else
			   {
			   find = 0.01/(e[2]-1);
			   }
			   */

			//printf("error is %9f \n",error);
			//printf("net0  %9f \n",(*(*(network + j -1 ) + p )).value);
			//printf("sp is %9f \n",sp);
			//printf("error is  %f \n",sp * error );
			*(*(gradientb +j -1)+ p + k * e[ j - 1]) = error;
			*(*(gradientw +j -1 )+ p + k * e[ j - 1]) = error
				* (*(*(network + j -1 ) + p )).value;
			//printf("value of w gradient is : %f \n",*(*(gradientw +j -1 )+ p + k * e[ j - 1]  ));
			//printf("weight is %f \n", *((*(*(network + j ) + k )).weights+p));
			//printf("res for weight : %f \n", *((*(*(network + j ) + k )).weights+p)  - 0.9 * *(*(gradientw +j -1 )+ p + k * e[ j - 1]));
			//printf("value of b gradient is : %f \n",*(*(gradientb +j -1 )+ p + k*e[j]  ));
			//printf("value of w gradient is : %f \n", sp *error* (*(*(network + j -1 ) + p )).value* 0.9);
			/*
			   if (pos == exepted)
			   {
			   printf("value is : %f \n", *(*(gradientb +n -1)+p + pos * e[ n-1]));
			   find = 1;
			   }
			   */
			//printf("\n");
			p++;
		}
		k++;
	}
	j --;
	k = 0;
	p = 0;
	while (j > 0)
	{
		k =0;
		while (k < e[j ])
		{
			p=0;
			error = somme_gradient_weight(j,k,network,gradientb) 
				* sigmoid_prime(*(*(z +j) +k )); 
			;
			// check and it corrected to that point
			//printf("pos k is %zu \n",k);
			while (p < e[j -1 ])
			{
				*(*(gradientb +j -1)+ k * e[j-1] + p) = error;
				*(*(gradientw +j -1)+ k * e[j-1] + p) =  error
					* (*(*(network+ j-1 ) + p )).value;
				p++;
			}
			k++;
		}
		j--;
	}
	//printf("\n");
	free2d(z,3);
	k = 0;
	j = 2;
	while (j > 0)
	{
		k =0;
		while (k < e[j ])
		{
			p=0;
			while (p < e[j -1 ])
			{
				*(*(b +j-1 )+k* e[j-1 ] + p) += *(*(gradientb +j -1)+ k * e[j-1] + p);
				*(*(w +j-1 )+k* e[j-1 ] + p) += *(*(gradientw +j -1)+ k * e[j-1] + p);
				p++;
			}
			k++;
		}
		j--;
	}
	//printf("backpropagaton done\n");
	free2d(gradientb,2);
	free2d(gradientw,2);

}



void gradient_descent (struct Neural **network,double **gradientw, double **gradientb)
{
	size_t  k=0, p=0,j = 2;
	double learning_rate = 0.5;
	while (j > 0)
	{
		k =0;
		while (k < e[j ])
		{
			p=0;
			(*(*(network +j)+k)).biases -=
				learning_rate * *(*(gradientb +j-1 )+k* e[j-1 ] + p) ;
			while (p < e[j -1 ])
			{
				*((*(*(network +j)+k)).weights+p) -=
					learning_rate *  *(*(gradientw +j-1 )+k* e[j-1 ] + p) ;
				p++;
			}
			k++;
		}
		j--;
	}
	free2d(gradientw,2);
	free2d(gradientb,2);


}



void train(struct Neural **network,double **vectorin,int *vectorout,size_t nbtest,double tvs )
{
	double **gradientw = malloc(sizeof(double *) * 2);
	*(gradientw  ) = malloc(sizeof(double ) * e[1] * e[0]);
	*(gradientw + 1) = malloc(sizeof(double ) * e[2]* e[1]);

	double **gradientb = malloc(sizeof(double *) * 2);
	*(gradientb  ) = malloc(sizeof(double ) * e[1] * e[0]);
	*(gradientb + 1 ) = malloc(sizeof(double ) * e[2] * e[1]);

	setgradient(gradientb,gradientw);
	size_t nbtestc=0;
	while (nbtestc <nbtest)
	{
		//printf("test %zu \n",nbtestc);
		backpropagation (network,(double)*(vectorout+nbtestc) ,*(vectorin + nbtestc ) ,gradientw , gradientb,tvs);
		//printf("backpropagation pass\n");
		//printf("pass 1\n");
		//printf("new 2\n");
		//gradient_descent(network,vectorout[nbtest - 1]);
		nbtestc ++;
	}
	size_t n = 0;
	while (n < 2)
	{
		//printf("try add \n")
		size_t pos = 0;
		while (pos < e[n])
		{
			size_t p = 0;
			while (p < e[n+1])
			{
				//printf("before %f  layer %zu \n",*(*(gradientb+n )+p + pos * e[ n + 1] ),n);
				*(*(gradientb+n )+p + pos * e[ n + 1] ) /= nbtest;

				*(*(gradientw+n )+p + pos * e[ n + 1] ) /= nbtest;

				//printf("grad %f at pos %zu neurone nb %zu layer  %zu \n",*(*(gradientw+n )+p + pos * e[ n + 1] ),p,pos,n);
				p++;
			}
			pos++;
		}
		n++;
	}
	gradient_descent(network,gradientw,gradientb);
	//printf("%zu value for the test \n", use_network(network));
	//printf("train pass\n\n");

}

void xor1()
{
	//https://www.youtube.com/watch?v=n2L1J5JYgUk
	srand(time(NULL));
	e[0]=3;
	e[1]=2;
	e[2]=1;
	struct Neural **network = initnetwork();
	double array[1][3] =
	{
		{1,0,1}
	};
	//printf ("%f \n",**array);
	double **arrayin = (double **)malloc(1 * sizeof(double *));
	for (size_t i = 0; i < 1; i++) {
		arrayin[i] = (double *)malloc(e[0] * sizeof(double));
		for (size_t j = 0; j < e[0]; j++) {
			if (j < e[0])
				arrayin[i][j] = array[i][j];
			else
				arrayin[i][j] =0;
		}
	}

	set_network (network, array[0] ,e[0]);
	free_network(network);
	network =initfromfile();

	//printf("%zu value\n", use_network(network));


	int arrayout[8] =
	{
		0,
		2,
		0,
		0,
		3,
		3,
		3,
		3,
	};
	size_t z=0;
	while (z<1)
	{
		printf("the actual test is : %zu\n",z);
		train (network,arrayin,arrayout,1,4 );
		printf("res las neural is :%f \n",(*(*(network +2))).value );
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
	int i =0;
	while (i<1)
	{
		set_network (network, array[i] ,e[0]);
		printf("get %zu  exepted %i value\n",use_network(network),(int)arrayout[i]);
		printf("res las neural is :%f \n",(*(*(network +2))).value );
		i++;
	}
	printf("res las neural is :%f \n",(*(*(network +2))).value );

	for (int i = 0; i < 1; i++) {
		free(arrayin[i]);
	}
	free(arrayin);

	save(network);
	free_network(network);


}


void xor()
{
	srand(time(NULL));
	e[0]=4;
	e[1]=8;
	e[2]=4;
	struct Neural **network = initnetwork();
	double array[10][4] =
	{
		{0,1,0,0},
		{1,0,0,0},
		{0,0,0,1},
		{0,0,1,0},

		{1,1,1,1},
		{0,0,0,0},

		{1,0,0,1},
		{0,1,1,0},
		{1,1,0,0},
		{0,0,1,1},

	};
	//printf ("%f \n",**array);
	size_t s = 10;
	double **arrayin = (double **)malloc(s * sizeof(double *));
	for (size_t i = 0; i < s; i++) {
		arrayin[i] = (double *)malloc(e[0] * sizeof(double));
		for (size_t j = 0; j < e[0]; j++) {
			if (j < 4)
				arrayin[i][j] = array[i][j];
			else
				arrayin[i][j] =0;
		}
	}

	//set_network (network, array[0] ,8);
	//free_network(network);
	//network =initfromfile();

	//printf("%zu value\n", use_network(network));


	int arrayout[10] =
	{
		1,
		2,
		1,
		2,
		0,
		0,
		3,
		3,
		3,
		3
	};
	size_t z=0, i =0;
	while (i<2)
	{
		set_network (network, array[i] ,4);
		printf("get %zu  exepted %i value\n",use_network(network),(int)arrayout[i]);
		i++;
	}


	while (z<20000)
	{
		printf("the actual test is : %zu\n",z);
		train (network,arrayin ,arrayout ,2,4 );
		z++;
	}
	/*
	   while (z<1000)
	   {
	   printf("the actual test is : %zu\n",z);
	   train (network,arrayin + 1,arrayout+ 1,1,4 );
	   z++;
	   }
	   z = 0;
	   while (z<10)
	   {
	   printf("the actual test is : %zu\n",z);
	   train (network,arrayin +1 ,arrayout +1,1,4 );
	   z++;
	   }
	   */
	i =0;
	while (i<2)
	{
		set_network (network, array[i] ,4);
		printf("get %zu  exepted %i value\n",use_network(network),(int)arrayout[i]);
		i++;
	}

	for (size_t i = 0; i < s; i++) {
		free(arrayin[i]);
	}
	free(arrayin);

	save(network);
	free_network(network);


}


void xor3()
{
	srand(time(NULL));
	e[0]=4;
	e[1]=8;
	e[2]=4;
	struct Neural **network = initnetwork();
	double array[12][4] =
	{
		{0,1,0,0},
		{1,0,0,0},
		{0,0,0,1},
		{0,0,1,0},

		{1,1,1,1},
		{0,0,0,0},
		{1,0,0,1},
		{0,1,1,0},

		{1,0,0,1},
		{0,1,1,0},
		{1,1,0,0},
		{0,0,1,1},

	};
	//printf ("%f \n",**array);
	double **arrayin = (double **)malloc(12 * sizeof(double *));
	for (size_t i = 0; i < 12; i++) {
		arrayin[i] = (double *)malloc(e[0] * sizeof(double));
		for (size_t j = 0; j < e[0]; j++) {
			if (j < 4)
				arrayin[i][j] = array[i][j];
			else
				arrayin[i][j] =0;
		}
	}

	//set_network (network, array[0] ,8);
	//free_network(network);
	//network =initfromfile(e);

	//printf("%zu value\n", use_network(network));


	int arrayout[12] =
	{
		1,
		2,
		1,
		2,
		0,
		0,
		0,
		0,
		0,
		3,
		3,
		3,
	};
	size_t z=0;
	while (z<10000)
	{
		printf("the actual test is : %zu\n",z);
		train (network,arrayin,arrayout,2,4 );
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
	int i =0;
	while (i<8)
	{
		set_network (network, array[i] ,4);
		printf("get %zu  exepted %i value\n",use_network(network),(int)arrayout[i]);
		i++;
	}

	for (int i = 0; i < 8; i++) {
		free(arrayin[i]);
	}
	free(arrayin);

	save(network);
	free_network(network);


}


void xor2()
{
	srand(time(NULL));
	e[0]=2;
	e[1]=2;
	e[2]=2;
	struct Neural **network = initnetwork();
	double array[1][2] =
	{
		{0.05,0.10}

	};
	//printf ("%f \n",**array);
	double **arrayin = (double **)malloc(1 * sizeof(double *));
	for (size_t i = 0; i < 1; i++) {
		arrayin[i] = (double *)malloc(e[0] * sizeof(double));
		for (size_t j = 0; j < e[0]; j++) {
			if (j < 4)
				arrayin[i][j] = array[i][j];
			else
				arrayin[i][j] =0;
		}
	}

	set_network (network, array[0] ,e[0]);
	free_network(network);
	network =initfromfile();

	//printf("%zu value\n", use_network(network));


	int arrayout[8] =
	{
		1,
		2,
		0,
		0,
		3,
		3,
		3,
		3,
	};
	size_t z=0;
	while (z<1)
	{
		printf("the actual test is : %zu\n",z);
		train (network,arrayin,arrayout,1,4 );
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
	int i =0;
	while (i<1)
	{
		set_network (network, array[i] ,e[0]);
		printf("get %zu  exepted %i value\n",use_network(network),(int)arrayout[i]);
		i++;
	}

	for (int i = 0; i < 1; i++) {
		free(arrayin[i]);
	}
	free(arrayin);

	save(network);
	free_network(network);


}
