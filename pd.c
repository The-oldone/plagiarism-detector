#include<stdio.h>
#include<stdlib.h>
#include"pd.h"
#include<string.h>
#include<ctype.h>

#define WORDSIZE 32

/*Initialize an indexqueue*/
void initIndices(indexQueue *indices) {
	indices -> head = NULL;
	indices -> end = NULL;
	return;
}

/*Generate a hash value for a word*/
int generateHashValue(char *word) {
	unsigned hashValue = 0;
	while(*word) {
		hashValue = *word + hashValue;
		word++;
	}
	return hashValue % HASHSIZE;
}

/*Makes the whole word lowercase*/
void lowerWord(char *word) {
	while(*word) {
		*word = tolower(*word);
		word++;
	}
	return;
}

/*Adds a index holder node at the end of the index queue*/
void indexPush(indexQueue *indexQueue, int index) {
	indexNode *newNode;
	newNode = (indexNode *) malloc(sizeof(newNode));
	newNode -> next = NULL;
	newNode -> index = index;
	/*Handles empty queue*/
	if(!indexQueue -> end) {
		indexQueue -> head = indexQueue -> end = newNode;
		return;
	}
	indexQueue -> end -> next = newNode;
	indexQueue -> end = newNode;
	return;
}

/*Searches for a matching index queue node in the hash table*/
indexQueue *hashTableSearch(hashTableHead *hashTable, char *word) {
	/**
	 * Finds the hash value of the word to be searched to determine
	 * its position in the hash table
	 */
	int hashValue = generateHashValue (word);
	hashNode *nodeSearcher;
	for
			(nodeSearcher = hashTable[hashValue].head;
			nodeSearcher;
			nodeSearcher = nodeSearcher -> next) {
		/*If it's a match, return the word*/
		if(!strcmp(nodeSearcher -> word, word)) {
			return &(nodeSearcher -> indices);
		}
	}
	/*Handle word not found*/
	return NULL;
}

/*Initializes the hash table*/
void initHashArray(hashTableHead *hashArray) {
	int i;
	for(i = 0; i < HASHSIZE; i++){
		hashArray[i].head = NULL;
	}
	return;
}

/*Inserts a word and its index into the hash table*/
void hashTableInsert(hashTableHead *hashTable, char *word, int index) {
	int hashValue;
	hashNode *sampleNode, *newNode, *nodeBehindSampleNode;
	hashValue = generateHashValue(word);
	/**
	 * If word is found
	 */
	if(hashTable[hashValue].head != NULL){
		/**
		 * Loop through all words with the same hash value
		 * till either the required word is found or the list runs out
		 */
		for
				(sampleNode = hashTable[hashValue].head;
				sampleNode != NULL;
				sampleNode = sampleNode -> next) {
			/*store address of previous element*/
			nodeBehindSampleNode = sampleNode;
			/*if already in the hash table, add the index*/
			if(!strcmp(sampleNode -> word, word)) {
				indexPush(&(sampleNode -> indices), index);
				return;
			}
		}
	}
	/**
	 * create a new node if not already present
	 */
	newNode = (hashNode *) malloc (sizeof(hashNode));
	strcpy(newNode -> word, word);
	newNode -> next = NULL;
	initIndices(&(newNode -> indices));
	indexPush(&(newNode -> indices), index);
	/*handle empty hashtable node*/
	if(!hashTable[hashValue].head) {
		hashTable[hashValue].head = newNode;
		return;
	}
	nodeBehindSampleNode -> next = newNode;
	return;
}

/*checks if the word queue is empty*/
int isEmpty(wordQueue *queue) {
	if(queue -> head) {
		return 0;
	}
	return 1;
}

/*adds a word node onto the word queue*/
void enqueueWord(wordQueue *queue, char *word) {
	wordNode *newNode;
	newNode = (wordNode *) malloc (sizeof(wordNode));
	newNode -> next = NULL;
	strcpy(newNode -> word, word);
	newNode -> visited = 0;
	/*handle empty queue*/
	if(!queue -> head) {
		queue -> head = queue -> rear = newNode;
		return;
	}
	queue -> rear -> next = newNode;
	queue -> rear = newNode;
	return;
}

/*eliminates commonly used words*/
int checkCommon(char *word) {
	if(!strcmp(word, "a") || !strcmp(word, "an") || !strcmp(word, "the") || !strcmp(word, "for")
		|| !strcmp(word, "and") || !strcmp(word, "but") || !strcmp(word, "or") || !strcmp(word, "so")
		|| !strcmp(word, "yet") || !strcmp(word, "nor")){
		return 1;
	}
	return 0;
}

/*separate function to not duplicate the list of separators*/
char *tokenizeWord(char *word, int firstTimeFlag) {
	if(firstTimeFlag) {
		return strtok(word, ",./?\t'*\\\"!\n");
	} else {
		return strtok(NULL, ",./?\t'*\\\"!\n");
	}
}

/*reads a file into a word queue*/
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
			enqueueWord(queue, brokenWord);
			/**
			 * insert into hash table and increment file size counter
			 * only if it's the first file, as hash table doesn't
			 * need duplicate words and we count the common words
			 * for one file at a time only
			 */
			if(firstFile) {
				hashTableInsert(hashTable, brokenWord, *fileSize);
				(*fileSize)++;
			}
		}while((brokenWord = tokenizeWord(word, 0)));
	}
	return;
}

/*initializes the word queue*/
void initQueue(wordQueue *queue) {
	queue -> head = queue -> rear = NULL;
	return;
}

/*returns a pointer to the first element in said queue without freeing it*/
char *peekqueue(wordQueue *queue) {
	return queue -> head -> word;
}

/*removes the first element from the queue and returns a pointer
to its malloced version*/
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

/**
 * traverses a queue till an already visited word is found or a
 * dissimilarity occurs
 */
int traverseTillDissimilar(wordQueue *file1, wordQueue *file2, int destinationIndex) {
	wordNode *wordFromFile1, *wordFromFile2;
	/*number of common words in a row*/
	int count = 0;
	wordFromFile1 = file1 -> head;
	wordFromFile2 = file2 -> head;
	/**reach index of word found to be common in first file
	 * there's no need to do this in second file as
	 * due to the dequeue, we are already at the current word
	*/
	for(int i = 0; i < destinationIndex; i++) {
		wordFromFile1 = wordFromFile1 -> next;
		if(!wordFromFile1) {
			return 0;
		}
	}
	for(; wordFromFile1 && wordFromFile2; wordFromFile1 = wordFromFile1 -> next, wordFromFile2 = wordFromFile2 -> next) {
		/**if the words are different or the word has already been 
		 * visited in the first file, stop iterating
		*/
		if(wordFromFile1 -> visited == 1 || strcmp(wordFromFile1 -> word, wordFromFile2 -> word)) {
			break;
		}
		wordFromFile1 -> visited = 1;
		count++;
	}
	return count;
}

/**
 * the main function to check for plagiarism
 */
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
		/*peek to ensure non destructive checking*/
		word = peekqueue(file2);
		if(!hashTableSearch(hashtable, word)) {
			/**
			 * dequeue to move to next word(as we're only checking
			 * for plagiarism in file 1 wrt file 2, we can destroy
			 * file2's queue)
			 */
			word = dequeue(file2);
			free(word);
			continue;
		}
		indicesCounter = hashTableSearch(hashtable, word) -> head;
		while(indicesCounter) {
			index = indicesCounter -> index;
			count = traverseTillDissimilar(file1, file2, index);
			/**
			 * add total words found common in a row
			 * to aggregate if the amount found is greater
			 * than the threshold
			 */
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

/**
 * frees the set of indices in each hash node
 */
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