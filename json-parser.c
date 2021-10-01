#include "json-parser.h"
#include <string.h>

#include <math.h>

int i,j;
char char_res[100];
int int_res;
char* delimiter = ":\"";
char* pos;
int length;


int json_parser_get_int(char* json_string, char* key){
    
    
    i = 0;
	pos =  strstr(json_string, key);
	pos = strstr(pos,delimiter)+2;


	while(pos[i]!='\"'){
		char_res[i]=pos[i];
		i++;
	}
	length = i;
	int_res =0;
	i=0;
	while(i<length){
		int_res += (char_res[i] - '0') * pow(10, length-i-1);
		i++;
		}	
		
		
	return int_res;
	}
	
	
	
	
char* json_parser_get_string(char* json_string, char* key){
    i = 0;
    bzero(char_res,strlen(char_res));
	pos = strstr(json_string, key);
	pos = strstr(pos,delimiter)+2;
	while(pos[i]!='\"'){
		char_res[i]=pos[i];
		i++;
	}
	
	pos = strstr(json_string,"}");
	return char_res;
	}
