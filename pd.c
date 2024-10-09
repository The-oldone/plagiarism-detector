#include<stdio.h>
#include<stdlib.h>
#include"pd.h"
#include<string.h>
#include<ctype.h>

#define WORDSIZE 32
#define HASHSIZE 101

void initIndices(indexQueue *indices) {
	indices -> head = NULL;
	indices -> end = NULL;
	return;
}

int generateHashValue(char *word) {
	unsigned hashValue = 0;
	while(*word) {
		hashValue = *word + hashValue;
		word++;
	}
	return hashValue % HASHSIZE;
}

void lowerWord(char *word) {
	while(*word) {
		*word = tolower(*word);
		word++;
	}
	return;
}

void indexPush(indexQueue *indexQueue, int index) {
	indexNode *newNode;
	newNode = (indexNode *) malloc(sizeof(newNode));
	newNode -> next = NULL;
	newNode -> index = index;
	if(!indexQueue -> end) {
		indexQueue -> head = indexQueue -> end = newNode;
		return;
	}
	indexQueue -> end -> next = newNode;
	indexQueue -> end = newNode;
	return;
}

indexQueue *hashTableSearch(hashTableHead *hashTable, char *word) {
	int hashValue;
	hashValue = generateHashValue (word);
	hashNode *nodeSearcher;
	for(nodeSearcher = hashTable[hashValue].head; nodeSearcher; nodeSearcher = nodeSearcher -> next) {
		if(!strcmp(nodeSearcher -> word, word)) {
			return &(nodeSearcher -> indices);
		}
	}
	return NULL;
}

void initHashArray(hashTableHead *hashArray) {
	int i;
	for(i = 0; i < HASHSIZE; i++){
		hashArray[i].head = NULL;
	}
	return;
}

void hashTableInsert(hashTableHead *hashTable, char *word, int index) {
	int hashValue;
	hashNode *sampleNode, *newNode, *nodeBehindSampleNode;
	hashValue = generateHashValue(word);
	if(hashTable[hashValue].head != NULL){
		for(sampleNode = hashTable[hashValue].head; sampleNode != NULL; sampleNode = sampleNode -> next) {
			nodeBehindSampleNode = sampleNode;
			if(!strcmp(sampleNode -> word, word)) {
				indexPush(&(sampleNode -> indices), index);
				return;
			}
		}
	}
	newNode = (hashNode *) malloc (sizeof(hashNode));
	strcpy(newNode -> word, word);
	newNode -> next = NULL;
	initIndices(&(newNode -> indices));
	indexPush(&(newNode -> indices), index);
	if(!hashTable[hashValue].head) {
		hashTable[hashValue].head = newNode;
		return;
	}
	nodeBehindSampleNode -> next = newNode;
	return;
}

int isEmpty(queue *queue) {
	if(queue -> head) {
		return 0;
	}
	return 1;
}

void enqueue(queue *queue, char *word) {
	wordNode *newNode;
	newNode = (wordNode *) malloc (sizeof(wordNode));
	newNode -> next = NULL;
	strcpy(newNode ->word, word);
	if(!queue -> head) {
		queue -> head = queue -> rear = newNode;
		return;
	}
	queue -> rear -> next = newNode;
	queue -> rear = newNode;
	return;
}

int checkCommon(char *word) {
	if(!strcmp(word, "a") || !strcmp(word, "an") || !strcmp(word, "the") || !strcmp(word, "for")
		|| !strcmp(word, "and") || !strcmp(word, "but") || !strcmp(word, "or") || !strcmp(word, "so")
		|| !strcmp(word, "yet") || !strcmp(word, "nor")){
		return 1;
	}
	return 0;
}

char *tokenizeWord(char *word, int firstTimeFlag) {
	if(firstTimeFlag) {
		return strtok(word, ",./?\t'*\\\"!\n");
	} else {
		return strtok(NULL, ",./?\t'*\\\"!\n");
	}
}

void readFile(queue *queue, FILE *file, hashTableHead *hashTable, int *fileSize) {
	char word[WORDSIZE], *brokenWord;
	*fileSize = 0;
	while(!feof(file)){
		fscanf(file, "%s", word);
		/*just in case the word is larger than 32 characs*/
		word[WORDSIZE - 1] = '\0';
		brokenWord = tokenizeWord(word, 1);
		do {
			if(strlen(brokenWord) < 4){
				if(checkCommon(brokenWord)) {
					continue;
				}
			}
			lowerWord(brokenWord);
			enqueue(queue, brokenWord);
			hashTableInsert(hashTable, brokenWord, *fileSize);
			(*fileSize)++;
		}while((brokenWord = tokenizeWord(word, 0)));
	}
	return;
}

void readFileWithoutHashing(queue *queue, FILE *file) {
	char word[WORDSIZE], *brokenWord;
	int currentIndex = 0;
	while(!feof(file)){
		fscanf(file, "%s", word);
		/*just in case the word is larger than 32 characs*/
		word[WORDSIZE - 1] = '\0';
		brokenWord = tokenizeWord(word, 1);
		do {
			if(strlen(brokenWord) < 4){
				if(checkCommon(brokenWord)) {
					continue;
				}
			}
			lowerWord(brokenWord);
			enqueue(queue, brokenWord);
			currentIndex++;
		}while((brokenWord = tokenizeWord(word, 0)));
	}
	return;
}

void initQueue(queue *queue) {
	queue -> head = queue -> rear = NULL;
	return;
}

char *dequeue(queue *queue) {
	char *wordToBeReturned = (char *) malloc (sizeof(char) * WORDSIZE);
	wordNode *toBeFreed;
	if(!queue -> head) {
		return "\0";
	}
	toBeFreed = queue -> head;
	strcpy(wordToBeReturned, queue ->head -> word);
	if(!(queue -> head -> next)) {
		queue -> head = NULL;
		queue -> rear = NULL;
		free (toBeFreed);
		return wordToBeReturned;
	}
	queue -> head = queue -> head -> next;
	free(toBeFreed);
	return wordToBeReturned;
}

int traverseTillDissimilar(queue *file1, queue *file2, int destinationIndex) {
	wordNode *wordFromFile1, *wordFromFile2;
	int i;
	int count = 1;
	wordFromFile1 = file1 -> head;
	wordFromFile2 = file2 -> head;
	for(i = 0; i < destinationIndex; i++) {
		wordFromFile1 = wordFromFile1 -> next;
		if(!wordFromFile1) {
			return 0;
		}
	}
	for(; wordFromFile1 && wordFromFile2; wordFromFile1 = wordFromFile1 -> next, wordFromFile2 = wordFromFile2 -> next) {
		if(strcmp(wordFromFile1 -> word, wordFromFile2 -> word)) {
			break;
		}
		count++;
	}
	return count;
}

float checkPlagiarism(queue *file1, queue *file2, hashTableHead *hashtable, int firstFileSize) {
	char *word;
	indexNode *indicesCounter;
	float plagiarismExtent;
	int index, count, max = 0;
	while(!isEmpty(file2)){
		word = dequeue(file2);
		if(!hashTableSearch(hashtable, word)) {
			continue;
		}
		indicesCounter = hashTableSearch(hashtable, word) -> head;
		while(indicesCounter) {
			index = indicesCounter -> index;
			count = traverseTillDissimilar(file1, file2, index + 1);
			if(count > max) {
				max = count;
			}
			indicesCounter = indicesCounter -> next;
		}
		free(word);
	}
	plagiarismExtent = (float) max / firstFileSize;
	return plagiarismExtent;
}