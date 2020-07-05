#include<stdbool.h>
#define false 0
#define true 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<time.h>

char admin_id[50] = "admin007";
char admin_pass[50] = "bond";


//TYPES
typedef struct record{
	char name[10];
	int m1;
	int m2;
	int m3;
	float p;
}record;

typedef struct node {
	void ** pointers;
	int * keys;
	struct node * parent;
	bool is_leaf;
	int num_keys;
	struct node * next; // Used for queue.
}node;


// GLOBALS.
int order;

//The queue is used to print the tree in level order
node * queue = NULL;


// FUNCTION PROTOTYPES.
// Output and utility.

void enqueue(node * new_node);
node * dequeue(void);
int height(node * const root);
int path_to_root(node * const root, node * child);
void print_leaves(node * const root);
void print_tree(node * const root);
void find_and_print(node * const root, int key);
void find_and_print_range(node * const root, int range1, int range2);
int find_range(node * const root, int key_start, int key_end,
		int returned_keys[], void * returned_pointers[]);
node * find_leaf(node * const root, int key);
record * find(node * root, int key);
int cut(int length);

// Insertion.

record * make_record(char value[], int, int, int, float);
node * make_node(void);
node * make_leaf(void);
int get_left_index(node * parent, node * left);
node * insert_into_leaf(node * leaf, int key, record * pointer);
node * insert_into_leaf_after_splitting(node * root, node * leaf, int key,
                                        record * pointer);
node * insert_into_node(node * root, node * parent,
		int left_index, int key, node * right);
node * insert_into_node_after_splitting(node * root, node * parent,
                                        int left_index,
		int key, node * right);
node * insert_into_parent(node * root, node * left, int key, node * right);
node * insert_into_new_root(node * left, int key, node * right);
node * start_new_tree(int key, record * pointer);
node * insert(node * root, int key, char value[], int, int, int, float);



// FUNCTION DEFINITIONS.

// OUTPUT AND UTILITIES.------------------------------------------------------------------------------------------

void usage_2(void) {
	printf("Enter any of the following commands after the prompt > :\n"
	"\tf <k>  --      Find the name of the student with roll no<k>.\n"
	"\tr <k1> <k2> -- Print the roll nos and names found in the range[<k1>, <k2>]\n"
    "\tb --           Print the file data\n"
	"\tt -- Print the B+ tree Level wise.\n"
	"\tl -- Print the data of the leaves.\n"
	"\tq -- Log out\n"
	"\t? -- Print this help message.\n");
}

void usage_1(void)
{
    printf("Enter any of the following commands after the prompt > :\n"
    "\ti <k> <v> --   Insert the name <v> (string) as the name of student with roll no<k> (an integer).\n"
    "\tb --           Print the file data\n"
    "\td <k>  --      Delete roll no <k> and its associated name.\n"
	"\tx -- Delete the entire file\n"
	"\tq -- Log out\n"
	"\t? -- Print this help message.\n");
}


/*Helper function for printing the
 tree out.  See print_tree.
 */
void enqueue(node * new_node) {
	node * c;
	if (queue == NULL) {
		queue = new_node;
		queue->next = NULL;
	}
	else {
		c = queue;
		while(c->next != NULL) {
			c = c->next;
		}
		c->next = new_node;
		new_node->next = NULL;
	}
}


/*Helper function for printing the
tree out.  See print_tree.
 */
node * dequeue(void) {
	node * n = queue;
	queue = queue->next;
	n->next = NULL;
	return n;
}


/* Prints the bottom row of keys
 of the tree
 */
void print_leaves(node * const root) {
	if (root == NULL) {
		printf("Empty tree.\n");
		return;
	}
	printf("The leaf nodes are: \n");
	int i;
	node * c = root;
	while (!c->is_leaf)
		c = c->pointers[0];
	while (true) {
		for (i = 0; i < c->num_keys; i++) {
			printf("%d ", c->keys[i]);
		}
		if (c->pointers[order - 1] != NULL) {
			printf(" | ");
			c = c->pointers[order - 1];
		}
		else
			break;
	}
	printf("\n");
}


/* Utility function to give the height
 * of the tree, which length in number of edges
 * of the path from the root to any leaf.
 */
int height(node * const root) {
	int h = 0;
	node * c = root;
	while (!c->is_leaf) {
		c = c->pointers[0];
		h++;
	}
	return h;
}


/* Utility function to give the length in edges
 * of the path from any node to the root.
 */
int path_to_root(node * const root, node * child) {
	int length = 0;
	node * c = child;
	while (c != root) {
		c = c->parent;
		length++;
	}
	return length;
}

void print_file_data()
{
    FILE *fp1;
    fp1 = fopen("Btreedata.txt","r");
    if(fp1==NULL)
    {
        printf("Error opening file :/ File doesn't exist!\n");
        return;
    }
    printf("ROLLNO\tNAME\tMARKS1\tMARKS2\tMARKS3\tPERCENTAGE\n");
    int kee,m1,m2,m3;
    float p;
    char valu[10];
	while(!feof(fp1))
	{
        fscanf(fp1,"%d %s %d %d %d %f\n",&kee,valu,&m1,&m2,&m3,&p);
        printf("%d\t%s\t%d\t%d\t%d\t%f\n",kee,valu,m1,m2,m3,p);
	}
	fclose(fp1);
}


void delete_key_from_file(int k)
{
    FILE *fp1, *fp2;
    fp1 = fopen("Btreedata.txt","r");
    fp2 = fopen("Btemp.txt","w");

    int kee,m1,m2,m3,flag=0;
    float p;
    char valu[10];
    while(!feof(fp1))
    {
        fscanf(fp1,"%d %s %d %d %d %f\n",&kee,valu,&m1,&m2,&m3,&p);
        if(kee!=k)
        {
            fprintf(fp2, "%d %s %d %d %d %f\n", kee, valu, m1,m2,m3,p);
        }
        if(kee==k) flag=1;
    }

    fclose(fp1);
    fclose(fp2);

    if(flag==0)
    {
        printf("\n The key does not exist. Deletion cannot be performed.\n");
        return;
    }

    fp2 = fopen("Btemp.txt","r");
    fp1 = fopen("Btreedata.txt", "w");
    if(feof(fp2))
    {
        printf("Deleted the record with value %d\n",k);
        remove("Btreedata.txt");
        return;
    }
    while(!feof(fp2))
    {
        fscanf(fp2,"%d %s %d %d %d %f\n",&kee,valu,&m1,&m2,&m3,&p);
        fprintf(fp1, "%d %s %d %d %d %f\n", kee, valu, m1,m2,m3,p);
    }
    printf("Deleted the record with value %d\n",k);
    fclose(fp1);
    fclose(fp2);
}

/* Prints the B+ tree
 */
void print_tree(node * const root) {

	node * n = NULL;
	int i = 0;
	int rank = 0;
	int new_rank = 0;

	if (root == NULL) {
		printf("Empty tree.\n");
		return;
	}
	printf("The tree looks like this: \n");
	queue = NULL;
	enqueue(root);
	while(queue != NULL) {
		n = dequeue();
		if (n->parent != NULL && n == n->parent->pointers[0]) {
			new_rank = path_to_root(root, n);
			if (new_rank != rank) {
				rank = new_rank;
				printf("\n");
			}
		}
		for (i = 0; i < n->num_keys; i++) {
			printf("%d ", n->keys[i]);
		}
		if (!n->is_leaf)
			for (i = 0; i <= n->num_keys; i++)
				enqueue(n->pointers[i]);

		printf("| ");
	}
	printf("\n");

}


/* Finds the record under a given key and prints an
 * appropriate message to stdout.
 */
void find_and_print(node * const root, int key) {
	record * r = find(root, key);
	if (r == NULL)
		printf("Record not found under key %d.\n", key);
	else
	{
		printf("Record at %p ---\n",r);
		printf("ROLLNO\tNAME\tSUB1\tSUB2\tSUB3\tPERCENTAGE\n");
		printf("%d\t%s\t%d\t%d\t%d\t%f\n",key, r->name, r->m1, r->m2, r->m3, r->p);
    }
}


/* Finds and prints the keys, pointers, and values within a range
 * of keys between key_start and key_end, including both bounds.
 */
void find_and_print_range(node * const root, int key_start, int key_end)
{
	int i;
	int array_size = key_end - key_start + 1;
	int returned_keys[array_size];
	void * returned_pointers[array_size];
	int num_found = find_range(root, key_start, key_end,
			returned_keys, returned_pointers);
	if (!num_found)
		printf("None found.\n");
	else {
        printf("ROLLNO\tLOCATION\tNAME\tSUB1\tSUB2\tSUB3\tPERCENTAGE\n");
		for (i = 0; i < num_found; i++)
			printf("%d\t%p\t%s\t%d\t%d\t%d\t%f\n",returned_keys[i],returned_pointers[i],((record *)returned_pointers[i])->name,((record *)returned_pointers[i])->m1,((record *)returned_pointers[i])->m2,((record *)returned_pointers[i])->m2,((record *)returned_pointers[i])->p);
	}
}


/* Finds keys and their pointers, if present, in the range specified
 by key_start and key_end, inclusive.  Places these in the arrays
 returned_keys and returned_pointers, and returns the number of entries found.
 */
int find_range(node * const root, int key_start, int key_end,
		int returned_keys[], void * returned_pointers[]) {
	int i, num_found;
	num_found = 0;
	node * n = find_leaf(root, key_start);
	if (n == NULL) return 0;
	for (i = 0; i < n->num_keys && n->keys[i] < key_start; i++) ;
	if (i == n->num_keys) return 0;
	while (n != NULL) {
		for (; i < n->num_keys && n->keys[i] <= key_end; i++) {
			returned_keys[num_found] = n->keys[i];
			returned_pointers[num_found] = n->pointers[i];
			num_found++;
		}
		n = n->pointers[order - 1];
		i = 0;
	}
	return num_found;
}


/* Traces the path from the root to a leaf, searching
 * by key.
 * Returns the leaf containing the given key.
 */
node * find_leaf(node * const root, int key) {
	if (root == NULL) {
		return root;
	}
	int i = 0;
	node * c = root;
	while (!c->is_leaf) {
		i = 0;
		while (i < c->num_keys) {
			if (key >= c->keys[i]) i++;
			else break;
		}
		c = (node *)c->pointers[i];
	}
	return c;
}


/* Finds and returns the record to which
 * a key refers.
 */
record * find(node * root, int key) {
    if (root == NULL) {
        return NULL;
    }

	int i = 0;
    node * leaf = NULL;

	leaf = find_leaf(root, key);

    /* If root != NULL, leaf must have a value, even
     * if it does not contain the desired key.
     * (The leaf holds the range of keys that would
     * include the desired key.)
     */

	for (i = 0; i < leaf->num_keys; i++)
		if (leaf->keys[i] == key) break;

	if (i == leaf->num_keys)
		return NULL;
	else
		return (record *)leaf->pointers[i];
}

/* Finds the appropriate place to
 * split a node that is too big into two.
 */
int cut(int length) {
	if (length % 2 == 0)
		return length/2;
	else
		return length/2 + 1;
}


// INSERTION------------------------------------------------------------------------------------------------------------------

/* Creates a new record to hold the value
 * to which a key refers.
 */
record * make_record(char value[], int m1, int m2, int m3, float p) {
	record * new_record = (record *)malloc(sizeof(record));
	if (new_record == NULL) {
		perror("Record creation.");
		exit(EXIT_FAILURE);
	}
	else {
		strcpy(new_record->name,value);
		new_record->m1 = m1;
		new_record->m2 = m2;
		new_record->m3 = m3;
		new_record->p = p;
	}
	return new_record;
}


/* Creates a new general node, which can be adapted
 * to serve as either a leaf or an internal node.
 */
node * make_node(void) {
	node * new_node;
	new_node = malloc(sizeof(node));
	if (new_node == NULL) {
		perror("Node creation.");
		exit(EXIT_FAILURE);
	}
	new_node->keys = malloc((order - 1) * sizeof(int));
	if (new_node->keys == NULL) {
		perror("New node keys array.");
		exit(EXIT_FAILURE);
	}
	new_node->pointers = malloc(order * sizeof(void *));
	if (new_node->pointers == NULL) {
		perror("New node pointers array.");
		exit(EXIT_FAILURE);
	}
	new_node->is_leaf = false;
	new_node->num_keys = 0;
	new_node->parent = NULL;
	new_node->next = NULL;
	return new_node;
}

/* Creates a new leaf by creating a node
 * and then adapting it appropriately.
 */
node * make_leaf(void) {
	node * leaf = make_node();
	leaf->is_leaf = true;
	return leaf;
}


/* Helper function used in insert_into_parent
 * to find the index of the parent's pointer to
 * the node to the left of the key to be inserted.
 */
int get_left_index(node * parent, node * left) {

	int left_index = 0;
	while (left_index <= parent->num_keys &&
			parent->pointers[left_index] != left)
		left_index++;
	return left_index;
}

/* Inserts a new pointer to a record and its corresponding
 * key into a leaf.
 * Returns the altered leaf.
 */
node * insert_into_leaf(node * leaf, int key, record * pointer) {

	int i, insertion_point;

	insertion_point = 0;
	while (insertion_point < leaf->num_keys && leaf->keys[insertion_point] < key)
		insertion_point++;

	for (i = leaf->num_keys; i > insertion_point; i--) {
		leaf->keys[i] = leaf->keys[i - 1];
		leaf->pointers[i] = leaf->pointers[i - 1];
	}
	leaf->keys[insertion_point] = key;
	leaf->pointers[insertion_point] = pointer;
	leaf->num_keys++;
	return leaf;
}


/* Inserts a new key and pointer
 * to a new record into a leaf so as to exceed
 * the tree's order, causing the leaf to be split
 * in half.
 */
node * insert_into_leaf_after_splitting(node * root, node * leaf, int key, record * pointer) {

	node * new_leaf;
	int * temp_keys;
	void ** temp_pointers;
	int insertion_index, split, new_key, i, j;

	new_leaf = make_leaf();

	temp_keys = malloc(order * sizeof(int));
	if (temp_keys == NULL) {
		perror("Temporary keys array.");
		exit(EXIT_FAILURE);
	}

	temp_pointers = malloc(order * sizeof(void *));
	if (temp_pointers == NULL) {
		perror("Temporary pointers array.");
		exit(EXIT_FAILURE);
	}

	insertion_index = 0;
	while (insertion_index < order - 1 && leaf->keys[insertion_index] < key)
		insertion_index++;

	for (i = 0, j = 0; i < leaf->num_keys; i++, j++) {
		if (j == insertion_index) j++;
		temp_keys[j] = leaf->keys[i];
		temp_pointers[j] = leaf->pointers[i];
	}

	temp_keys[insertion_index] = key;
	temp_pointers[insertion_index] = pointer;

	leaf->num_keys = 0;

	split = cut(order - 1);

	for (i = 0; i < split; i++) {
		leaf->pointers[i] = temp_pointers[i];
		leaf->keys[i] = temp_keys[i];
		leaf->num_keys++;
	}

	for (i = split, j = 0; i < order; i++, j++) {
		new_leaf->pointers[j] = temp_pointers[i];
		new_leaf->keys[j] = temp_keys[i];
		new_leaf->num_keys++;
	}

	free(temp_pointers);
	free(temp_keys);

	new_leaf->pointers[order - 1] = leaf->pointers[order - 1];
	leaf->pointers[order - 1] = new_leaf;

	for (i = leaf->num_keys; i < order - 1; i++)
		leaf->pointers[i] = NULL;
	for (i = new_leaf->num_keys; i < order - 1; i++)
		new_leaf->pointers[i] = NULL;

	new_leaf->parent = leaf->parent;
	new_key = new_leaf->keys[0];

	return insert_into_parent(root, leaf, new_key, new_leaf);
}


/* Inserts a new key and pointer to a node
 * into a node into which these can fit
 * without violating the B+ tree properties.
 */
node * insert_into_node(node * root, node * n,
		int left_index, int key, node * right) {
	int i;

	for (i = n->num_keys; i > left_index; i--) {
		n->pointers[i + 1] = n->pointers[i];
		n->keys[i] = n->keys[i - 1];
	}
	n->pointers[left_index + 1] = right;
	n->keys[left_index] = key;
	n->num_keys++;
	return root;
}


/* Inserts a new key and pointer to a node
 * into a node, causing the node's size to exceed
 * the order, and causing the node to split into two.
 */
node * insert_into_node_after_splitting(node * root, node * old_node, int left_index,
		int key, node * right) {

	int i, j, split, k_prime;
	node * new_node, * child;
	int * temp_keys;
	node ** temp_pointers;

	/* First create a temporary set of keys and pointers
	 * to hold everything in order, including
	 * the new key and pointer, inserted in their
	 * correct places.
	 * Then create a new node and copy half of the
	 * keys and pointers to the old node and
	 * the other half to the new.
	 */

	temp_pointers = malloc((order + 1) * sizeof(node *));
	if (temp_pointers == NULL) {
		perror("Temporary pointers array for splitting nodes.");
		exit(EXIT_FAILURE);
	}
	temp_keys = malloc(order * sizeof(int));
	if (temp_keys == NULL) {
		perror("Temporary keys array for splitting nodes.");
		exit(EXIT_FAILURE);
	}

	for (i = 0, j = 0; i < old_node->num_keys + 1; i++, j++) {
		if (j == left_index + 1) j++;
		temp_pointers[j] = old_node->pointers[i];
	}

	for (i = 0, j = 0; i < old_node->num_keys; i++, j++) {
		if (j == left_index) j++;
		temp_keys[j] = old_node->keys[i];
	}

	temp_pointers[left_index + 1] = right;
	temp_keys[left_index] = key;

	/* Create the new node and copy
	 * half the keys and pointers to the
	 * old and half to the new.
	 */
	split = cut(order);
	new_node = make_node();
	old_node->num_keys = 0;
	for (i = 0; i < split - 1; i++) {
		old_node->pointers[i] = temp_pointers[i];
		old_node->keys[i] = temp_keys[i];
		old_node->num_keys++;
	}
	old_node->pointers[i] = temp_pointers[i];
	k_prime = temp_keys[split - 1];
	for (++i, j = 0; i < order; i++, j++) {
		new_node->pointers[j] = temp_pointers[i];
		new_node->keys[j] = temp_keys[i];
		new_node->num_keys++;
	}
	new_node->pointers[j] = temp_pointers[i];
	free(temp_pointers);
	free(temp_keys);
	new_node->parent = old_node->parent;
	for (i = 0; i <= new_node->num_keys; i++) {
		child = new_node->pointers[i];
		child->parent = new_node;
	}

	/* Insert a new key into the parent of the two
	 * nodes resulting from the split, with
	 * the old node to the left and the new to the right.
	 */

	return insert_into_parent(root, old_node, k_prime, new_node);
}



/* Inserts a new node (leaf or internal node) into the B+ tree.
 * Returns the root of the tree after insertion.
 */
node * insert_into_parent(node * root, node * left, int key, node * right) {

	int left_index;
	node * parent;

	parent = left->parent;

	/* Case: new root. */

	if (parent == NULL)
		return insert_into_new_root(left, key, right);

	/* Case: leaf or node. (Remainder of
	 * function body.)
	 */

	/* Find the parent's pointer to the left
	 * node.
	 */

	left_index = get_left_index(parent, left);


	/* Simple case: the new key fits into the node.
	 */

	if (parent->num_keys < order - 1)
		return insert_into_node(root, parent, left_index, key, right);

	/* Harder case:  split a node in order
	 * to preserve the B+ tree properties.
	 */

	return insert_into_node_after_splitting(root, parent, left_index, key, right);
}


/* Creates a new root for two subtrees
 * and inserts the appropriate key into
 * the new root.
 */
node * insert_into_new_root(node * left, int key, node * right) {

	node * root = make_node();
	root->keys[0] = key;
	root->pointers[0] = left;
	root->pointers[1] = right;
	root->num_keys++;
	root->parent = NULL;
	left->parent = root;
	right->parent = root;
	return root;
}



/* First insertion:
 * start a new tree.
 */
node * start_new_tree(int key, record * pointer) {

	node * root = make_leaf();
	root->keys[0] = key;
	root->pointers[0] = pointer;
	root->pointers[order - 1] = NULL;
	root->parent = NULL;
	root->num_keys++;
	return root;
}



/* Master insertion function.
 * Inserts a key and an associated value into
 * the B+ tree, causing the tree to be adjusted
 * however necessary to maintain the B+ tree
 * properties.
 */
node * insert(node * root, int key, char value[], int m1, int m2, int m3, float p) {

	record * record_pointer = NULL;
	node * leaf = NULL;

	record_pointer = find(root, key);
    if (record_pointer != NULL) {

        /* If the key already exists in this tree, update
         * the value and return the tree.
         */

        strcpy(record_pointer->name,value);
        return root;
    }

	/* Create a new record for the
	 * value.
	 */
	record_pointer = make_record(value, m1, m2, m3, p);


	/* Case: the tree does not exist yet.
	 * Start a new tree.
	 */

	if (root == NULL)
		return start_new_tree(key, record_pointer);


	/* Case: the tree already exists
	 */

	leaf = find_leaf(root, key);

	/* Case: leaf has room for key and record_pointer.
	 */

	if (leaf->num_keys < order - 1) {
		leaf = insert_into_leaf(leaf, key, record_pointer);
		return root;
	}


	/* Case:  leaf must be split.
	 */

	return insert_into_leaf_after_splitting(root, leaf, key, record_pointer);
}


// MAIN

int main() {

	FILE * fp;
	node * root;
	int input_key, input_key_2;
	char val[10];
	char instruction;
    int marks1, marks2, marks3;
    float percent;
	root = NULL;
	int op,log=1;
	int current=0,elapsed=0;

	while(log!=3)
	{
	printf("********************* B-TREE IMPLEMENTATION FOR STUDENT DATABASE MANAGEMENT *********************\n");
	printf("\n------------WELCOME TO THE LOGIN PAGE------------\n\n");
	printf("1->ADMIN \n2->STUDENT(Guest) \n3->EXIT \n");
	scanf("%d",&op);

    switch(op)
    {
        case 1:
        {
            log=1;
            int c=1,flag=0;
            char id[100], pass[100];
            printf("Admin ID:\t");
            scanf("%s",id);
            printf("Password:\t");
            scanf("%s",pass);
            if(!strcmp(id, admin_id))
            {
                if(!strcmp(pass,admin_pass))
                {
                    printf("\nLogin Successful!\n\n");
                    flag = 1;
                }
            }
            if(flag!=1)
            {
                printf("\nINVALID CREDENTIALS!!!\nPLEASE LOGIN AGAIN\n\n");
                current = time(NULL);
                elapsed = 0;
                do
                {
                   elapsed = time(NULL);

                }while((elapsed -  current)<1.5);
                system("cls");
                break;
            }
            system("cls");
            printf("\n-----------WELCOME TO ADMIN SECTION------------\n\n");
            usage_1();
            printf("> ");
            while (scanf("%c", &instruction) != EOF)
            {
                switch(instruction)
                {
                    case 'd':
                        scanf("%d", &input_key);
                        delete_key_from_file(input_key);
                        //print_tree(root);
                        break;
                    case 'i':
                    {   scanf("%d %s", &input_key, val);
                        printf("Enter the marks for subject1 out of 100: ");
                        while(1)
                        {
                            scanf("%d",&marks1);
                            if(marks1<=100) break;
                            printf("Cannot accept more than 100. Enter again: ");
                        }
                        printf("\nEnter the marks for subject2 out of 100: ");
                        while(1)
                        {
                            scanf("%d",&marks2);
                            if(marks2<=100) break;
                            printf("Cannot accept more than 100. Enter again: ");
                        }
                        printf("\nEnter the marks for subject3 out of 100: ");
                        while(1)
                        {
                            scanf("%d",&marks3);
                            if(marks3<=100) break;
                            printf("Cannot accept more than 100. Enter again: ");
                        }
                        percent = ((marks1+marks2+marks3)/300.0)*100;

                        fp = fopen("Btreedata.txt", "a");
                        if(fp!=NULL)
                        {
                            fprintf(fp, "%d %s %d %d %d %f\n", input_key, val, marks1, marks2, marks3, percent);
                        }
                        fclose(fp);
                        //print_tree(root);
                        break;
                    }
                    case 'b':
                        print_file_data();
                        break;
                    case 'q':
                        log=9;
                        break;
                    case 'x':
                        remove("Btemp.txt");
                        if(remove("Btreedata.txt")!=0)
                            perror("Error deleting file");
                        else
                            puts("File Successfully Deleted");
                        break;
                    case '?':
                        usage_1();
                    default:
                        break;
                }
                printf("\n> ");
                if(log==9) break; // log=9 means process of loggin out of the admin side
            }
            printf("PRESS 0 TO LOG OUT SECURELY!\n");
            scanf("%d",&c);
            if(c==0)
            {
                printf("\nLogging out...\n");
                current = time(NULL);
                elapsed = 0;
                do
                {
                   elapsed = time(NULL);

                }while((elapsed -  current)<2);
                printf("*************YOU HAVE LOGGED OUT SUCCESSFULLY**********\n");
                system("cls");
            }
        break;
        }
        case 2:
        {
            system("cls");
            printf("\n-----------WELCOME TO STUDENT SECTION------------\n\n");
            log=2;
            do
            {
                printf("\nEnter the order of the tree: (minimum order is 3 and maximum order is 20) ");
                scanf("%d",&order);
                if(order<3 || order>20) printf("\nEnter again please.");
            }while(order<3 || order>20);

            usage_2();
            //The tree formation part ----------------------------------------------------------------------------------
            fp = fopen("Btreedata.txt","r");
            if(fp==NULL)
            {
                printf("\nBtree can't be created\nError opening file :/ File doesn't exist!\n\n");

            }
            while(fp!=NULL && !feof(fp))
            {
                fscanf(fp,"%d %s %d %d %d %f\n",&input_key, val, &marks1, &marks2, &marks3, &percent);
                root = insert(root, input_key, val, marks1, marks2, marks3, percent);
            }
            fclose(fp);
            print_tree(root);
            //-----------------------------------------------------------------------------------------------------------

            printf("> ");
            while (scanf("%c", &instruction) != EOF) {
                switch (instruction) {
                case 'f':
                    scanf("%d", &input_key);
                    find_and_print(root, input_key);
                    break;
                case 'b':
                    print_file_data();
                    break;
                case 'r':
                    scanf("%d %d", &input_key, &input_key_2);
                    if (input_key > input_key_2) {
                        int tmp = input_key_2;
                        input_key_2 = input_key;
                        input_key = tmp;
                    }
                    find_and_print_range(root, input_key, input_key_2);
                    break;
                case 'l':
                    print_leaves(root);
                    break;
                case 'q':
                    log=9;
                    break;
                case 't':
                    print_tree(root);
                    break;
                case '?':
                    usage_2();
                default:
                    break;
                }
                printf("\n> ");
                if(log==9)
                    break;
            }
            printf("\n");
            printf("\nLogging out...\n");
            current = time(NULL);
            elapsed = 0;
            do
            {
                elapsed = time(NULL);
            }while((elapsed -  current)<2);
            system("cls");
            break;
        }
        case 3:
            log=3;
            break;
        default:
            break;
    }

    }
    printf("\n*************PROCESS TERMINATED*************\n");
	return EXIT_SUCCESS;
}
