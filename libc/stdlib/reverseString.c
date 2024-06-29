#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

char* reverseString(char *string){
	char temp; 
	size_t stringLength = strlen(string) - 1;
	size_t n = stringLength; 
	for(size_t i = 0; i < stringLength; i++){ 
		temp = string[n]; 
		string[n] = string[i]; 
		string[i] = temp; 
		n--;
		if(n == (stringLength / 2)){
			break; 
		}
	}
	return string;
}
