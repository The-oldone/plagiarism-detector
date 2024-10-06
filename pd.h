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
	indexQueue *indices;
	char *word;
	struct hashNode *next;
}hashNode;

typedef struct hashTableHead{
	hashNode *head;
}hashTableHead;

void enqueue(queue *q, char *word);
int checkCommon(char *word);
void initQueue(queue *q);
indexQueue *hashSearch(hashTableHead *h, char *word);
void hashInsert(hashTableHead *h, char *word, int index);
void readFile(queue *q, FILE *file, hashTableHead *hashTable);
void readFileWithoutHashing(queue *q, FILE *file);
char *dequeue(queue *q);
int checkPlagiarism(queue *, queue *, hashTableHead *);
int isEmpty(queue *);
int traverseTillDissimilar(queue *, queue *, int index);/*to be done*/
void initIndices(indexQueue *indices);
int generateHash(char *word);
void indexPush(indexQueue *, int);
void lowerWord(char *);
void initHashArray(hashTableHead *hashArray);