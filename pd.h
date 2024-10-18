#include<stdio.h>
typedef struct wordNode{
	struct wordNode *next;
	char word[32];
}wordNode;

typedef struct wordQueue {
	wordNode *head;
	wordNode *rear;
}wordQueue;

typedef struct indexNode {
	int index;
	struct indexNode *next;
}indexNode;

typedef struct indexQueue {
	indexNode *head;
	indexNode *end;
}indexQueue;

typedef struct hashNode {
	indexQueue indices;
	char word[32];
	struct hashNode *next;
}hashNode;

typedef struct hashTableHead{
	hashNode *head;
}hashTableHead;

void enqueue(wordQueue *q, char *word);
int checkCommon(char *word);
void initQueue(wordQueue *q);
indexQueue *hashTableSearch(hashTableHead *h, char *word);
void hashTableInsert(hashTableHead *h, char *word, int index);
void readFile(wordQueue *q, FILE *file, hashTableHead *hashTable, int *fileSize, int);
char *dequeue(wordQueue *q);
float checkPlagiarism(wordQueue *, wordQueue *, hashTableHead *, int);
int isEmpty(wordQueue *);
int traverseTillDissimilar(wordQueue *, wordQueue *, int index);/*to be done*/
void initIndices(indexQueue *indices);
int generateHashValue(char *word);
void indexPush(indexQueue *, int);
void lowerWord(char *);
void initHashArray(hashTableHead *hashArray);
char *tokenizeWord(char *, int);
void freeHashTable(hashTableHead *);
void freeQueue(wordQueue *);
void freeIndices(indexQueue *);