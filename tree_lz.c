#include "tree_lz.h"

void creattree(node **tree) {
	*tree = NULL;
}

void insert(node **tree, unsigned char *win, int index, int lasize, int sbsize) {
	node *p, *n;
	n = (node*) malloc(sizeof(node) * 1);
	n->off = index;
	n->len = lasize;
	n->left = n->right = n->parent = NULL;

	/* if first node = root */
	if (*tree == NULL) {
		*tree = n;
	}
	else {
		p = *tree;

		while (1) {
			if ((memcmp(&(win[index]), &(win[p->off]), lasize) < 0)) {

				/* go to the left child */
				if (p->left == NULL) {
					p->left = n;
					n->parent = p;
					break;
				}
				p = p->left;
			}
			else {
				/* go to the right child */
				if (p->right == NULL) {
					p->right = n;
					n->parent = p;
					break;
				}
				p = p->right;
			}
		}
	}
}

info find(node **tree, unsigned char *win, int index, int size) {
	int i;
	info z;
	node *p;
	p = *tree;

	/* initialize as non-match values */
	z.off = 0;
	z.len = 0;

	if (*tree == NULL)
		return z;

	/* traverse the tree searching for the longest match node */
	while (1) {

		/* look for how many characters are equal between the lookahead and the node */
		for (i = 0; (win[index + i] == win[p->off + i]) && (i < size - 1); i++);

		/* if the new match is better than the previous one */
		if (i > z.len){
			z.off = index - p->off;
			z.len = i;
		}

		if ((win[index + i] < win[p->off + i]) && (p->left != NULL))
			p = p->left;
		else if ((win[index + i] > win[p->off + i]) && (p->right != NULL))
			p = p->right;
		else 
			break;
	}   
	return z;
}

node* minChild(node *p) {
	node *min = p;
	while (min->left != NULL)
		min = min->left;
	return min;
}

void delete(node **tree, unsigned char *win, int index, int max) {
	node *p, *q, *tmp = NULL;
	p = *tree;
	while (1) {
		if ((memcmp(&(win[index]), &(win[p->off]), max) < 0))
			p = p->left;
		else if ((memcmp(&(win[index]), &(win[p->off]), max) > 0))
			p = p->right;
		else break;
	}

	/* the node to be deleted doesnt have a left child */
	if (p->left == NULL) {
		q = p->right;
		if (q != NULL)
			q->parent = p->parent;
		tmp = p->parent;
	}

	/* the node to be deleted doesnt have a right child */
	else if (p->right == NULL) {
		q = p->left; 
		tmp = q->parent = p->parent;
	}

	/* the node to be deleted has both the children */
	else {
		/*Look for the child that will replace the node to be deleted */
		q = minChild(p->right);

		/* just the left child has to be updated */
		if (q->parent == p)
			tmp = q->parent = p->parent;
		else {
		/* Also the Right child needs to be updated */
			tmp = q->parent;
			tmp->left = q->right;
			if(q->right != NULL)
				q->right->parent = tmp;

			q->right = p->right;
			q->parent = p->parent;

			if (q->right != NULL)
				q->right->parent = q;
			tmp = q->parent;
		}
		
		q->left = p->left;
		if (q->left != NULL)
			q->left->parent = q;
	}	

	/* Establish Parent->Child relation else set Child as root*/
	if (tmp != NULL) {
		if (tmp->right == p) {
			tmp->right = q;
		}
		else
			tmp->left = q;
	}
	else
		*tree = q;

	free(p);
}

void printtree(node *p, char *win) {
	if (p == NULL)
		return;
	if (p->left != NULL)
		printtree(p->left, win);
	printf("%c ", win[p->off]);
	if (p->right != NULL)
		printtree(p->right, win);
}
