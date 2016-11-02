#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct node{
	int len, off;
	struct node *left, *right, *parent;
} node;

typedef struct info {
	int off, len;
} info;

void creattree(node **tree);
void insert(node **tree, unsigned char *win, int index, int lasize, int sbsize);
info find(node **tree, unsigned char *win, int index, int size);
void delete(node **tree, unsigned char *win, int index, int max);
node* minChild(node *p);
void printtree(node *p, char *win);

