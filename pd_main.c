#include<stdio.h>
#include<stdlib.h>
#include"pd.h"
#include<string.h>


/**
 * we can't not store the second file in a queue, because, we scan one word, then to check more words
 * we scan more words from the second file and the queue, then those words can't be checked again
 * without modifying the file pointer of the second file. This will be implemented if viewed as
 * providing sufficient gains.
 */

float pd_main_text_file(char **argv) {
	wordQueue q1, q2;
	FILE *f1 = fopen(argv[0], "r"), *f2 = fopen(argv[1], "r");
	int /*i, */firstFileSize;
	float plagiarismValue = 0.0, plagiarismPercentage;
	hashTableHead hashTable[HASHSIZE];
	// hashNode *testNode;
	// indexNode *testindex;
	if(!f1) {
		printf("first file doesn't exist\n");
		return -1.0;
	} else if(!f2) {
		printf("second file doesn't exist\n");
		return -1.0;
	}
	initQueue(&q1);
	initQueue(&q2);
	initHashArray(hashTable);
	readFile(&q1, f1, hashTable, &firstFileSize, 1);
	readFile(&q2, f2, hashTable, &firstFileSize, 0);
	// hash table test block
	/*for(i = 0; i < HASHSIZE; i++){
		for(testNode = hashTable[i].head; testNode; testNode = testNode -> next){
			printf("%s ", testNode -> word);
			for(testindex = testNode->indices.head; testindex; testindex = testindex -> next) {
				printf("%d ", testindex -> index);
			}
			printf("\n");
		}
	}*/
	plagiarismValue = checkPlagiarism(&q1, &q2, hashTable, firstFileSize);
	plagiarismPercentage = plagiarismValue * 100;
	printf("%.2f%%\n", plagiarismPercentage);
	freeHashTable(hashTable);
	freeQueue(&q1);
	freeQueue(&q2);
	// hash table test block to test if table has been freed
	/*for(i = 0; i < HASHSIZE; i++){
		for(testNode = hashTable[i].head; testNode; testNode = testNode -> next){
			printf("%s ", testNode -> word);
			for(testindex = testNode->indices.head; testindex; testindex = testindex -> next) {
				printf("%d ", testindex -> index);
			}
			printf("\n");
		}
	}*/
	fclose(f1);
	fclose(f2);
	return plagiarismPercentage;
}