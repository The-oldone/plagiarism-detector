#include<stdio.h>
#include<stdlib.h>
#include"pd.h"
#include<string.h>

int main(int argc, char *argv[]) {
	FILE *f1 = fopen(argv[1], "r"), *f2 = fopen(argv[2], "r");
	char word[64];
	if(!f1) {
		printf("first file doesn't exist\n");
		return 1;
	} else if(!f2) {
		printf("second file doesn't exist\n");
		return 1;
	}
	while(!feof(f1)){
		fscanf(f1, "%s", word);
		queuePush(word);
	}
	printf("reading done\n");
	fclose(f1);
	fclose(f2);
	return 0;
}