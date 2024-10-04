#include<stdio.h>
typedef struct wordNode{
	struct wordNode *next;
	char word[32];
	int frequency;
	int *indexList;
}wordNode;

typedef struct queue{
	wordNode *head;
	wordNode *rear;
}queue;

typedef struct indNode {
	int index;
	struct indNode *next;
}indNode;

typedef struct indQueue {
	indNode *head;
	indNode *end;
}indQueue;

typedef struct hashNode {
	struct hashNode *hash;
	indQueue *indices;
	char *word;
	struct hashNode *next;
}hashT;

void enqueue(queue *q, char *word);
int checkCommon(char *word);
void initQueue(queue *q);
int hashSearch(hashT *h, char *word);
void hashInsert(hashT *h, char *word);
void readFile(queue *q, FILE *file, hashT *hashTable);
void readFileWithoutHashing(queue *q, FILE *file);

