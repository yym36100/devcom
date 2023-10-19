// parsegen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

typedef enum{sname=0,sid,stx_field, stx_filed_len,srx_filed,srx_field_len}estate;
class cparser{
	estate state;

public:
	char *get_element(char *p, char* delimiter,int *i){
		static char buff[50];
		int j=0;
		buff[0]=0;

		while(1){
			if(*p<0x20) break;
			char *del = delimiter;
				while(*del){
					if(*p==*del++) goto outerloop;
				}
				buff[j++]=*p++;
		}
		outerloop:
		buff[j++]=0;
		if(i)*i=j;
		return buff;
	}
	void parse_line(char *line){
		state = sname;		
		char *p=line;
		int j=0;
		static char *temp;
		while(*p){
			if(*p == '/' && *(p+1)=='/'){
				printf("comment line\n");
				break;
			}
			
			temp=get_element(p,":",&j);p+=j;
			if(temp[0]==0)break;
			printf("name=%s ",temp);

			temp=get_element(p,":",&j);p+=j;
			printf("I%s ",temp);

			for(int ti=0;ti<20;ti++){
			temp=get_element(p,"[",&j);p+=j;
			printf("-%s ",temp);

			temp=get_element(p,"]",&j);p+=j;
			printf("{%s} ",temp);
			if(*p=='<')break;
			}
			printf("\n");
			break;
			
		}
	}

};


int main(int argc, char* argv[])
{
	FILE *in = fopen(argv[1],"rt");
	cparser parse;
	if(!in)return -1;

	char inpline[256];
	while(!feof(in)){
		//fscanf(in,"%s\n",&inpline);
		fgets(inpline,256,in);

		//printf(">>%s",inpline);
		parse.parse_line(inpline);
	}

	printf("ok");
	fclose(in);

	return 0;
}

