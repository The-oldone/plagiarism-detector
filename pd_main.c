#include<stdio.h>
#include<stdlib.h>
#include"pd.h"
#include<string.h>

#define HASHSIZE 101

/**
 * example of plagiarized doc
 * mary had a little lamb its feet was white as snow, then one day that lamb decided it was mary's time to go
 * 
 */
/**
 * we can't not store the second file in a queue, because, we scan one word, then to check more words
 * we scan more words from the second file and the queue, then those words can't be checked again
 * without modifying the file pointer of the second file. This will be implemented if viewed as
 * providing sufficient gains.
 */

int main(int argc, char *argv[]) {
	queue q1, q2;
	FILE *f1 = fopen(argv[1], "r"), *f2 = fopen(argv[2], "r");
	hashTableHead hashTable[HASHSIZE];
	if(!f1) {
		printf("first file doesn't exist\n");
		return 1;
	} else if(!f2) {
		printf("second file doesn't exist\n");
		return 1;
	}
	initQueue(&q1);
	initQueue(&q2);
	initHashArray(hashTable);
	readFile(&q1, f1, hashTable);
	/*readFileWithoutHashing(&q2, f2);
	checkPlagiarism(&q1, &q2, &hashTable);*/
	fclose(f1);
	fclose(f2);
	return 0;
}