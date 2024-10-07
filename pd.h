#include<stdio.h>
typedef struct wordNode{
	struct wordNode *next;
	char word[32];
}wordNode;

typedef struct queue{
	wordNode *head;
	wordNode *rear;
}queue;

typedef struct indexNode {
	int index;
	struct indexNode *next;
}indexNode;

typedef struct indexQueue {
	indexNode *head;
	indexNode *end;
}indexQueue;

typedef struct hashNode {
	// struct hashNode *hash;
	indexQueue indices;
	char word[32];
	struct hashNode *next;
}hashNode;

typedef struct hashTableHead{
	hashNode *head;
}hashTableHead;

void enqueue(queue *q, char *word);
int checkCommon(char *word);
void initQueue(queue *q);
indexQueue *hashTableSearch(hashTableHead *h, char *word);
void hashTableInsert(hashTableHead *h, char *word, int index);
void readFile(queue *q, FILE *file, hashTableHead *hashTable, int *fileSize);
void readFileWithoutHashing(queue *q, FILE *file);
char *dequeue(queue *q);
float checkPlagiarism(queue *, queue *, hashTableHead *, int);
int isEmpty(queue *);
int traverseTillDissimilar(queue *, queue *, int index);/*to be done*/
void initIndices(indexQueue *indices);
int generateHashValue(char *word);
void indexPush(indexQueue *, int);
void lowerWord(char *);
void initHashArray(hashTableHead *hashArray);