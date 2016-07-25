#include	"stdio.h"
#include<malloc.h>

static unsigned char basicword[94][94]; 

void s_fun(void * source, void *key, void *result)
{
	unsigned char *p, *q, *r;
	p = (unsigned char *)source;
	q = (unsigned char *)key;
	r = (unsigned char *)result;
	
	for(int i=0; ;i++)
	{
		if(*p != 0)
		{
			if(*q == 0)
				q = (unsigned char *)key;
			
			*r++ = 	basicword[*q - 33][*p - 33];
			q++;
			p++;
		}
		else
			break;
	}
}
void dis_fun(void * source, void *key, void *result)
{
	unsigned char *p, *q, *r;
	p = (unsigned char *)source;
	q = (unsigned char *)key;
	r = (unsigned char *)result;
	unsigned char *temp;
	
	for(int i=0; ;i++)
	{
		if(*p != 0)
		{
			if(*q == 0)
				q = (unsigned char *)key;
			
			temp =  &basicword[*q - 33][0];
			for(int j=0; j<94; j++){
				if(*temp == *p)
				{
					*r++ = j+33;
					break;	
				}	
				temp++;
			}
			q++;
			p++;
		}
		else
			break;
	}
}

int main(void)
{
	int i,j,k;
	char * source = {
		"\\2345678901234567890"
	};
	char * key = {
		"wuhannewcando2013"
	};
	char target[20];
	char target2[20];
		
	for(i=0; i<94; i++)
	{
		k = 33+i;
		for(j=0; j<94; j++)	
		{
			basicword[i][j] = k;
			k++;
			if(k > 126)
			k -= 94;
			printf("%c ", basicword[i][j]);
		}
		printf("\r\n");
	}

	s_fun(source, key, target);
	
	printf("\r\n\r\n\r\n密文位：");
	for(i=0; i<20; i++)
		printf("%c ", target[i]);
	
	dis_fun(target, key, target2); 
	printf("\r\n明文位：");
	for(i=0; i<20; i++)
		printf("%c ", target2[i]);
}