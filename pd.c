#include<stdio.h>
#include<stdlib.h>
#include"pd.h"

void queuePush(queue *q, char *word) {
	wordNode *nn;
	
}

void readFile(queue *q, char *file, hashT **hashTable) {
	char word[32];
	while(!feof(file)){
		fscanf(file, "%s", word);
		if(checkCommon(word)) {
			continue;
		}
		enqueue(q, word);
		hashInsert(hashTable, word);
	}
	return;
}