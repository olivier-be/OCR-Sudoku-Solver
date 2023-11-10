#include "network.h"


void save(struct Neural **network)
{
	FILE * inputFile;
	inputFile = fopen( "test2.txt", "w" );
	if ( inputFile == NULL )
	{
		fprintf( stderr, "Cannot open file \n" );
		exit( 0 );
	}
	size_t n = 0;
	while ( n< 3 )
	{
		fprintf(inputFile,"p\n");
		size_t i = 0;
		while (i < e[n])
		{
			//printf("%zu  \n",i);
			//printf("%f %zu %zu \n",(*(*(network + n) + i)).biases,n,i);
			fprintf(inputFile,"%f ",(*(*(network + n) + i)).biases);
			double *v= (*(*(network + n) + i)).weights;
			if (v != NULL)
			{
				for (size_t p =0 ; p < e[n - 1]; p++)
				{
					fprintf(inputFile,"%f ",*(v + p));
				}
			}
			fprintf(inputFile,"\n");
			i++;
		}
		//printf("%zu test 2\n", i);
		n++;
	}
	fclose( inputFile );

}


double findvalue(FILE * inputFile, int* endofline, char *word2)
{
	char word[50];
	size_t pw = 0;
	char ch = 0;
	if (!feof(inputFile))
	{
		ch = fgetc(inputFile);
	}
	//printf("first char is \"%c\" \n",ch);
	if (!feof(inputFile) && (ch == '\n' || ch == ' '))
	{
		ch = fgetc(inputFile);
	}
	while (!feof(inputFile) && ch != '\n' && ch != ' '  )
	{
		if (pw < 49)
		{
			word[pw] = ch;
			word2[pw] = ch;
		}
		pw ++;
		ch = fgetc(inputFile);
	}
	if (pw <49)
	{
		word[pw] = 0;
		word2[pw] = 0;
	}
	else
	{
		word[49] = 0;
		word2[49] = 0;
	}
	if (ch == '\n')
	{
		*endofline = 1;
	}
	//printf("word find is \"%s\" \n",word);
	double d;
	sscanf(word, "%lf", &d);
	//printf("value find %f \n", d);
	return d;
}



struct Neural** initfromfile()
{
	FILE * inputFile;
	int endofline= 0;
	inputFile = fopen( "test.txt", "r" );
	if ( inputFile == NULL )
	{
		fprintf( stderr, "Cannot open file \n" );
		exit( 0 );
	}
	struct Neural **network = malloc(sizeof(struct Neural*) * 3);
	size_t n = 0;
	char ch;
	char word[50];
	while ( n< 3 )
	{
		while (!feof(inputFile) && (ch = fgetc(inputFile)) != 'p'  )
		{
		}
		ch = fgetc(inputFile);
		*(network + n) = malloc( sizeof(struct Neural) * e[n]);
		size_t i = 0;
		while (i < e[n])
		{

			endofline = 0;
			//printf("%zu \n", i);
			if (n != 0)
			{
				(*(*(network + n) + i)).biases = findvalue(inputFile, &endofline,word) ;
				//printf("word is H%sH /n",word);
				//printf("value find %f \n", (*(*(network + n) + i)).biases);
				(*(*(network + n) + i)).previouslayer =(void *)(network +n - 1);
				(*(*(network + n) + i)).value = 1;

				//printf("%p %p  test\n", (*((struct Neural**)(network +n -1)) + i), (*(network + n - 1) + i));
				double *v= calloc(e[n - 1], sizeof(double));

				for (size_t p =0 ; p < e[n - 1]; p++)
				{

					if (endofline == 1 && p + 1 < e[n-1])
					{

						printf("error bad size pos : %zu %zu %f\n",n, p, findvalue(inputFile, &endofline, word));
						exit(1);
					}
					*(v + p) = findvalue(inputFile, &endofline,word);
					//printf("%f \n",*(v + p));
				}
				(*(*(network + n) + i)).weights = v;
			}
			else
			{
				(*(*(network + n) + i)).biases = findvalue(inputFile, &endofline, word);
				(*(*(network + n) + i)).previouslayer = NULL;
				(*(*(network + n) + i)).value = 0;
				(*(*(network + n) + i)).weights = NULL;
			}
			i++;
			//printf("%zu test 2\n", i);
		}
		n++;
	}
	fclose( inputFile );
	return network;

}

