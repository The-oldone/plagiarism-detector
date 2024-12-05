#include<stdio.h>
#include<stdlib.h>
#include"pd.h"
#include<string.h>
#include<ctype.h>

#define WORDSIZE 32

/*need to fix current bug*/
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

int isEmpty(wordQueue *queue) {
	if(queue -> head) {
		return 0;
	}
	return 1;
}

void enqueue(wordQueue *queue, char *word) {
	wordNode *newNode;
	newNode = (wordNode *) malloc (sizeof(wordNode));
	newNode -> next = NULL;
	strcpy(newNode -> word, word);
	newNode -> visited = 0;
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

void readFile(wordQueue *queue,
		FILE *file,
		hashTableHead *hashTable,
		int *fileSize,
		int firstFile) {
	char word[WORDSIZE], *brokenWord;
	if(firstFile)
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
			if(firstFile) {
				hashTableInsert(hashTable, brokenWord, *fileSize);
				(*fileSize)++;
			}
		}while((brokenWord = tokenizeWord(word, 0)));
	}
	return;
}

void initQueue(wordQueue *queue) {
	queue -> head = queue -> rear = NULL;
	return;
}

char *peekqueue(wordQueue *queue) {
	return queue -> head -> word;
}

char *dequeue(wordQueue *queue) {
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

int traverseTillDissimilar(wordQueue *file1, wordQueue *file2, int destinationIndex) {
	wordNode *wordFromFile1, *wordFromFile2;
	int i;
	/**count is initiated to 1 because we dequeued the first word,
	 * and so would need to skip the first word compared in second file,
	 * and begin counted from the second
	 */
	int count = 0;
	wordFromFile1 = file1 -> head;
	wordFromFile2 = file2 -> head;
	for(i = 0; i < destinationIndex; i++) {
		wordFromFile1 = wordFromFile1 -> next;
		if(!wordFromFile1) {
			return 0;
		}
	}
	for(; wordFromFile1 && wordFromFile2; wordFromFile1 = wordFromFile1 -> next, wordFromFile2 = wordFromFile2 -> next) {
		if(wordFromFile1 -> visited == 1 || strcmp(wordFromFile1 -> word, wordFromFile2 -> word)) {
			break;
		}
		wordFromFile1 -> visited = 1;
		count++;
	}
	return count;
}

float checkPlagiarism(wordQueue *file1,
		wordQueue *file2,
		hashTableHead *hashtable,
		int firstFileSize,
		int wordInRowThreshold) {
	char *word;
	indexNode *indicesCounter;
	float plagiarismExtent;
	int index, count, total = 0;
	while(!isEmpty(file2)){
		word = peekqueue(file2);
		if(!hashTableSearch(hashtable, word)) {
			word = dequeue(file2);
			free(word);
			continue;
		}
		indicesCounter = hashTableSearch(hashtable, word) -> head;
		while(indicesCounter) {
			index = indicesCounter -> index;
			count = traverseTillDissimilar(file1, file2, index);
			if(count >= wordInRowThreshold) {
				total += count;
			}
			indicesCounter = indicesCounter -> next;
		}
		count = 0;
		word = dequeue(file2);
		free(word);
	}
	plagiarismExtent = (float) total / firstFileSize;
	return plagiarismExtent;
}

/*a linked list of wordnodes which has to be freed*/
void freeQueue(wordQueue *wordQueue) {
	wordNode *toBeFreed, *currentNode;
	toBeFreed = currentNode = wordQueue -> head;
	while(currentNode) {
		toBeFreed = currentNode;
		currentNode = currentNode -> next;
		free(toBeFreed);
	}
	wordQueue -> head = NULL;
	wordQueue -> rear = NULL;
	return;
}

/*a linked list of hashNodes each of which contains a linked list of indexNodes which has to be freed*/
void freeHashTable(hashTableHead *hashTable) {
	hashNode *toBeFreed, *currentNode;
	int i;
	for (i = 0; i < HASHSIZE; i++) {
		toBeFreed = currentNode = hashTable -> head;
		while(currentNode) {
			toBeFreed = currentNode;
			currentNode = currentNode -> next;
			freeIndices(&(toBeFreed -> indices));
			free(toBeFreed);
		}
		hashTable -> head = NULL;
		hashTable++;
	}
	return;
}

void freeIndices(indexQueue *indexQueue) {
	indexNode *toBeFreed, *currentNode;
	toBeFreed = currentNode = indexQueue -> head;
	while(currentNode) {
		toBeFreed = currentNode;
		currentNode = currentNode -> next;
		free(toBeFreed);
	}
	indexQueue -> head = NULL;
	return;
}