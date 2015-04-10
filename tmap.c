#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tmap.h"

// don't forget to define your tmap_struct!!!


struct nodeptr{
	int num_NODE;
	int num_left_NODE;
	int num_right_NODE;
	NAME_VAL *data;
	struct nodeptr *left;
	struct nodeptr *right;	
};

typedef struct nodeptr *NODE;

struct tmap_struct{
	NODE root;
	int height;
	int left_height;
	int right_height;
};

static int k = 0;
static num_insertion = 0;
static num_rebalance = 0;

static void put_to_array(NODE curr_NODE, NODE *array)
{
	if(curr_NODE != NULL)
	 {
		put_to_array(curr_NODE->left,array);
		array[k] = curr_NODE;
		array[k]->num_NODE = 0;
		array[k]->num_left_NODE = 0;
		array[k]->num_right_NODE = 0;
		k++;
		put_to_array(curr_NODE->right,array);	
	 }
}

static NODE reconnect(NODE *array, int end)
{
	NODE curr_NODE;
	int m;
	if(end == 0)
	return NULL;
	
	m = end/2;
	curr_NODE = array[m];
	curr_NODE->left = reconnect(array,m);
	curr_NODE->right = reconnect(&(array[m+1]),end - (m+1));
	
	curr_NODE->num_NODE = end;        
        curr_NODE->num_left_NODE = m;
	if(end%2 == 0) // currently even number of node in the array          
          {
                curr_NODE->num_right_NODE = m - 1;
          }
        else // current array is odd
          {
                curr_NODE->num_right_NODE = m;    
          }

	return curr_NODE;
}
NODE rebalance(NODE curr_NODE)
{
	
	int n = curr_NODE->num_NODE;
	NODE *array = malloc(sizeof(struct nodeptr)*n);
	put_to_array(curr_NODE,array);
	k = 0; // the global veriable must be set back to zero for the next use
	curr_NODE = reconnect(array,n);
	free(array);

	return curr_NODE;
}
/**
* Creates and initializes a tmap.  Returns pointer to
*  created tmap.
* Parameter:  fname gives the name of a file containing
*  name/value pairs, one pair per line.  The tmap is
*  initialized with these values.
*
*  File format example:
*
     bob 2.911
     larry 1.8
     mike 2.9
*
*  If fname is NULL an empty tmap is initialized.
**/
TMAP_PTR tmap_create(char *fname){
FILE *src;
int violate = 0; // to indicate whether there is a violation
int curr_height;
int go_left = 0; // determine if the temp node start going to the left side or not;
char NEW_name[30];
double val;
TMAP_PTR t; 
	t =  malloc(sizeof(struct tmap_struct));
	t->root = NULL;
	t->height = -1;
	t->left_height = -1;
	t->right_height = -1;

	src = fopen(fname,"r");
		if(src == NULL)
			; // the empty tree is created
		else
	 	{
			while(fscanf(src,"%s %lf",NEW_name,&val) != EOF)
			  {
				NODE temp_NODE, curr_NODE, prev_NODE, root_violate, violate_prev_NODE, violate_NODE;
				root_violate = NULL;

				temp_NODE = malloc(sizeof(struct nodeptr)); // new node
			        temp_NODE->num_NODE = 1;		 // node counters
				temp_NODE->num_left_NODE = 0;		 // left node counters
				temp_NODE->num_right_NODE = 0;		 // right node counters
        			temp_NODE->data = malloc(sizeof(NAME_VAL)); // new data(the name and the income)
				temp_NODE->data->name = malloc(sizeof(char)*strlen(NEW_name));
				strcpy(temp_NODE->data->name,NEW_name);
				temp_NODE->data->value = val;
        			temp_NODE->left = NULL;
        			temp_NODE->right = NULL;
		
				curr_NODE = t->root;
				if(curr_NODE != NULL && curr_NODE->data->value > val)
					go_left = 1;

                                prev_NODE = NULL;
				curr_height = -1;
				while(curr_NODE != NULL)
				  {	
					curr_height++;
				      // found the rebalance part but not yet rebalance
                                       if(((t->root->num_left_NODE)*2 + 1) < t->root->num_right_NODE ||
                                           ((t->root->num_right_NODE)*2 + 1) < t->root->num_left_NODE)
                                          {
						violate = 1;
                                                root_violate = t->root;
                                          }
					// subtree violate
				      else if((((curr_NODE->num_left_NODE)*2 + 1) < curr_NODE->num_right_NODE ||
					 ((curr_NODE->num_right_NODE)*2 + 1) < curr_NODE->num_left_NODE) && violate == 0) 
					  {
						violate = 1;
						violate_prev_NODE = prev_NODE;
						violate_NODE = curr_NODE;
				 	  }

				         prev_NODE = curr_NODE;
                                         curr_NODE->num_NODE++;
				        if(curr_NODE->data->value > val)
			  	          {
				 	     curr_NODE->num_left_NODE++;
				  	     curr_NODE = curr_NODE->left;
					  }
				        else
					  {
				  	     curr_NODE->num_right_NODE++;
				 	     curr_NODE = curr_NODE->right;
				    	  }
  				  }
				if(prev_NODE == NULL)
				  {
					t->root = temp_NODE;
					t->height = 0;
					t->left_height = 0;
					t->right_height = 0;
				  }
				else
				  {
					if(t->height <= curr_height)
					  {
						t->height = curr_height + 1;
						if(go_left)
						t->left_height = curr_height + 1;
						else
						t->right_height = curr_height + 1;
					  }

					if(prev_NODE->data->value > val)
					   prev_NODE->left = temp_NODE;
					else
					   prev_NODE->right = temp_NODE;
				  }
			
				if(violate)// do the rebalance
				  {
					num_rebalance++;
					if(root_violate != NULL)// the root violate
					  {
					    root_violate = rebalance(root_violate);
					    t->root = root_violate;
					    curr_NODE = root_violate;
					    curr_height = -1;
					    if(curr_NODE != NULL)
						{
						    curr_NODE = curr_NODE->left;
						    curr_height++;
						}
					    t->height = curr_height;
					    t->left_height = curr_height;	
						if(t->root->num_NODE%2 == 0)
							t->right_height = curr_height - 1;
						else
							t->right_height = curr_height;

					    root_violate = NULL;
					  }
					else
					  {
						int i, j;
						i = curr_height;// before the doing rebalance
						curr_NODE = violate_NODE;
			
						while(curr_NODE->left != NULL || curr_NODE->right != NULL)
						  {
								if(curr_NODE->left != NULL)
								  {
									curr_height++;
									curr_NODE = curr_NODE->left;
								  }
								else if(curr_NODE->right != NULL)
							 	  {
									curr_height++;
									curr_NODE = curr_NODE->right;
								  }
						  }
						j = curr_height;// before the doing rebalance and at the current hieght;

						violate_NODE = rebalance(violate_NODE);
						
						curr_NODE = violate_NODE;
						
                                                while(curr_NODE->left != NULL)
                                                  {
                                                     i++;
                                                     curr_NODE = curr_NODE->left;
                                                  }

						if(go_left)
						  {
							if(t->left_height > j)
							    {
								j = j - i;
								t->left_height = t->left_height - j;
							    }
		 
						  }
						else
					   	  {
							if(t->right_height > j)
							  {
								j = j - i;
								t->right_height = t->right_height - j;
							  }
						  }
						
						if(t->height < t->right_height)
							t->height = t->right_height;
						else
							t->height = t->left_height;

						if(violate_prev_NODE->data->value > violate_NODE->data->value)
							violate_prev_NODE->left = violate_NODE;
						else
							violate_prev_NODE->right = violate_NODE;	
					  } 
				  }
				violate = 0;// set this value because there the next node is not violated
				go_left = 0;
			  }
		fclose(src);
	 	}
	return t;
}


/**
* Inserts given name-value pair into tmap.
*
* returns 1 if succeeds; 0 for failure.
*
* For partial credit:  you may assume that every insert
*   is for a name not currently in the tmap.
*
* For full credit:  if there is an entry with the same
*   name already in the tmap you change the associated
*   value to be the parameter val (and modify the tree
*   accordingly).
*
* Runtime O(log n) (ammortized)
**/
int tmap_insert(TMAP_PTR t, char * name, double val){
		NODE temp_NODE, curr_NODE, prev_NODE, root_violate, violate_prev_NODE, violate_NODE;
		int go_left, curr_height,violate;	
		violate = 0;
		go_left = 0;
		curr_height = -1;
		violate_NODE = NULL;
		root_violate = NULL;
			temp_NODE = malloc(sizeof(struct nodeptr)); // new node
		        temp_NODE->num_NODE = 1;		 // node counters
			temp_NODE->num_left_NODE = 0;		 // left node counters
			temp_NODE->num_right_NODE = 0;		 // right node counters
        		temp_NODE->data = malloc(sizeof(NAME_VAL)); // new data(the name and the income)
			temp_NODE->data->name = malloc(sizeof(char)*strlen(name));
			strcpy(temp_NODE->data->name,name);
			temp_NODE->data->value = val;
        		temp_NODE->left = NULL;
        		temp_NODE->right = NULL;
			
				curr_NODE = t->root;
				if(curr_NODE != NULL && curr_NODE->data->value > val)
					go_left = 1;

                                prev_NODE = NULL;
				curr_height = -1;
				while(curr_NODE != NULL)
				  {	
					curr_height++;
				      // found the rebalance part but not yet rebalance
                                       if(((t->root->num_left_NODE)*2 + 1) < t->root->num_right_NODE ||
                                           ((t->root->num_right_NODE)*2 + 1) < t->root->num_left_NODE)
                                          {
						violate = 1;
                                                root_violate = t->root;
                                          }
					// subtree violate
				      else if((((curr_NODE->num_left_NODE)*2 + 1) < curr_NODE->num_right_NODE ||
					 ((curr_NODE->num_right_NODE)*2 + 1) < curr_NODE->num_left_NODE) && violate == 0) 
					  {
						violate = 1;
						violate_prev_NODE = prev_NODE;
						violate_NODE = curr_NODE;
				 	  }

				         prev_NODE = curr_NODE;
                                         curr_NODE->num_NODE++;
				        if(curr_NODE->data->value > val)
			  	          {
				 	     curr_NODE->num_left_NODE++;
				  	     curr_NODE = curr_NODE->left;
					  }
				        else
					  {
				  	     curr_NODE->num_right_NODE++;
				 	     curr_NODE = curr_NODE->right;
				    	  }
  				  }
				if(prev_NODE == NULL)
				  {
					t->root = temp_NODE;
					t->height = 0;
					t->left_height = 0;
					t->right_height = 0;
				  }
				else
				  {
					if(t->height <= curr_height)
					  {
						t->height = curr_height + 1;
						if(go_left)
						t->left_height = curr_height + 1;
						else
						t->right_height = curr_height + 1;
					  }

					if(prev_NODE->data->value > val)
					   prev_NODE->left = temp_NODE;
					else
					   prev_NODE->right = temp_NODE;
				  }

				if(violate)// do the rebalance
				  {
					num_rebalance++;
					if(root_violate != NULL)// the root violate
					  {
					    root_violate = rebalance(root_violate);
					    t->root = root_violate;
					    curr_NODE = root_violate;
					    curr_height = -1;
					    if(curr_NODE != NULL)
						{
						    curr_NODE = curr_NODE->left;
						    curr_height++;
						}
					    t->height = curr_height;
					    t->left_height = curr_height;
	
						if(t->root->num_NODE%2 == 0)
							t->right_height = curr_height - 1;
						else
							t->right_height = curr_height;

					    root_violate = NULL;
					  }
					else
					  {
						int i, j;
						i = curr_height;// before the doing rebalance
						curr_NODE = violate_NODE;
						while(curr_NODE->left != NULL || curr_NODE->right != NULL)
						  {
								if(curr_NODE->left != NULL)
								  {
									curr_height++;
									curr_NODE = curr_NODE->left;
								  }
								else if(curr_NODE->right != NULL)
							 	  {
									curr_height++;
									curr_NODE = curr_NODE->right;
								  }
						  }
						j = curr_height;// before the doing rebalance and at the current hieght;

						violate_NODE = rebalance(violate_NODE);						
						curr_NODE = violate_NODE;

                                                while(curr_NODE->left != NULL)
                                                  {
                                                     i++;
                                                     curr_NODE = curr_NODE->left;
                                                  }

						if(go_left)
						  {
							if(t->left_height > j)
							    {
								j = j - i;
								t->left_height = t->left_height - j;
							    }
						  }
						else
					   	  {
							if(t->right_height > j)
							  {
								j = j - i;
								t->right_height = t->right_height - j;
							  }
						  }

						if(t->height < t->right_height)
							t->height = t->right_height;
						else
							t->height = t->left_height;

						if(violate_prev_NODE->data->value > violate_NODE->data->value)
							violate_prev_NODE->left = violate_NODE;
						else
							violate_prev_NODE->right = violate_NODE;	
					  } 
				  }
	num_insertion++;
	return 1; // always success
}

/**
* returns the number of name-value pairs in the map
*
* Runtime:  O(1)
*/
int tmap_size(TMAP_PTR t){
	if(t->root == NULL)
	return 0;
	return t->root->num_NODE;
}


/**
* returns the height of the underlying tree
*
* Runtime:  O(1)
*/
int tmap_height(TMAP_PTR t){
	if(t == NULL)
	  return (-1);
	return t->height;
}


/**
* returns pointer to NAME_VAL pair which is the
*   i'th entry in the sorted sequence.
*
* if i=1, you return the min entry
* if i=n, you return the max entry
* if n=n/2 you return the median entry (or close)
* if(i < 1 || i > n) return NULL;
*
* Runtime:  O(log n)
*/
NAME_VAL * tmap_get_ith(TMAP_PTR t, int i){
	if(t == NULL)
	return NULL;

	NODE curr_NODE, prev_NODE;
	curr_NODE = t->root;
	int n = curr_NODE->num_NODE;
	NAME_VAL *data;
	if(curr_NODE != NULL)
	  {	
	  if(i == 1) // min
	     {
		while(curr_NODE != NULL)
                  {
                        prev_NODE = curr_NODE;
                        curr_NODE = curr_NODE->left;
                  }
                data = prev_NODE->data;
                return data;
	     } 
	  else if(i == curr_NODE->num_NODE) // max
	     { 
		 while(curr_NODE != NULL)
                  {
                        prev_NODE = curr_NODE;
                        curr_NODE = curr_NODE->right;
                  }
                data = prev_NODE->data;
                return data;
	     }
	 else if(i == curr_NODE->num_NODE/2)// middle
             {
                NODE *array = malloc(sizeof(struct nodeptr)*n);
                put_to_array(curr_NODE,array);
                k = 0;
                data = array[i - 1]->data;
		curr_NODE = reconnect(array, curr_NODE->num_NODE);
		t->root = curr_NODE;
                free (array);
                return data;
             }
	 else if(i > 1 && i < n)// in between
	     {
		i = i - 1;
		NODE *array = malloc(sizeof(struct nodeptr)*n);
                put_to_array(curr_NODE,array);
                k = 0;
                data = array[i - 1]->data;
		curr_NODE = reconnect(array, curr_NODE->num_NODE);
		t->root = curr_NODE;
                free (array);
                return data;
	     }
	  else
		return NULL;
	  }
	else
	  return NULL;
}

/**
* returns the number of entries in the map with values
*   between min and max inclusive -- i.e., the number of
*   entries where min <= val <= max
*
* Runtime:  O(log n)
*/
int tmap_range_size(TMAP_PTR t, double min, double max){

	if(t == NULL || min >= max)
	return 0; // there is no such a range

	int left_counter, right_counter, go_left, go_right, num_range;
	NODE curr_NODE, prev_NODE;

	curr_NODE = t->root;
	prev_NODE = NULL;
	if(curr_NODE != NULL)
	  {
		left_counter = curr_NODE->num_left_NODE;
		right_counter = curr_NODE->num_right_NODE;
		go_left = 0;
		go_right = 0;
		if(curr_NODE->data->value > min )
		  {
			go_left = 1;
		  }
		
		if(curr_NODE->data->value < max)
		  {
			go_right = 1;
		  }
		while(curr_NODE != NULL) // find the min
		   {
			prev_NODE = curr_NODE;
			if(curr_NODE->data->value > min && go_left)
				curr_NODE = curr_NODE->left;
			else if(curr_NODE->data->value < min && !go_left)
				curr_NODE = curr_NODE->right;
			else
				break;
		   }
		if(curr_NODE != NULL)
		   {
			if(go_left)
			num_range = left_counter - prev_NODE->num_left_NODE; // min inclusive
			else
			num_range = prev_NODE->num_right_NODE + 1;
		   }
		else
		   {
			if(go_left)
			num_range = t->root->num_NODE;
			else
			{
			  return 0;// the minimun is greater the the max number of the tree
			}
		   }

		if(t->root->data->value > max) // going left side
		  {
			curr_NODE = t->root;
		  }
		else
		  {
			curr_NODE = prev_NODE;// going right side
		  }

		while(curr_NODE != NULL) // find max
		   {
			prev_NODE = curr_NODE;
			if(curr_NODE->data->value < max)
			  curr_NODE = curr_NODE->right;
			else
			{
				num_range = num_range - curr_NODE->num_right_NODE - 1;
				curr_NODE = curr_NODE->left;
			}
		   }
		return num_range;
	  }

	return 0;
}


/**
* returns an array populated with all entries in the given range.
*   number of such entries is reported through the integer pointer
*   parameter m (so the caller knows the length of the array).
*
* Runtime requirement is "output dependent" in that it depends on
*   the map size AND the number of elements in the range (m).
*
* Runtime:  O(m + log n)
*/
NAME_VAL * tmap_extract_range(TMAP_PTR t, double min, double max, int *m){
		*m =  tmap_range_size(t,min,max);
		NAME_VAL *array = malloc(sizeof(struct name_val)*(*m));
}

/**
* Prints the following
*
*   The current size of the map.
*   The current height of the tree.
*   The total number of successful insertions since creation.
*   The total number of successful deletions since creation (if supported)
*   The total number of rebalancing operations performed since creation
*   The total "work" done over all rebalancing operations; an individual
*      rebalancing operation does work equal to the size of the subtree
*      being rebalanced since the operation is linear in the size of the
*      subtree being rebalanced.
*/
void tmap_stats(TMAP_PTR t){
	if(t!= NULL)
	printf("The number of NODES in the tree %d\n",t->root->num_NODE);
	printf("The number of Insertion is %d\n",num_insertion);
	printf("THe number of Rebalance is %d\n",num_rebalance);
}


/**
* deallocates all memory associated with the map.
*
*/
void tmap_destroy(TMAP_PTR t){
	NODE curr_NODE;
	if(t->root != NULL)
	  {
		curr_NODE = t->root;
		t->root = curr_NODE->left;
		tmap_destroy(t);
		t->root = curr_NODE->right;
		tmap_destroy(t);
		free(curr_NODE->data);
		free(curr_NODE);
		t->root = NULL;
	  }
}




/**  BONUS FEATURES ****/

/**
* If an entry has the given name, a pointer to that NAME_VAL
*   is returned.
* If no such entry, NULL is returned.
*
* Runtime:  O(1) for full bonus points.
*           O(log n) for partial bonus points
*           Theta(n):  no bonus points, but it might be handy for debugging.
*/
NAME_VAL * tmap_name2value(TMAP_PTR t, char *name){
	NODE curr_NODE;
	NAME_VAL *the_data;

	curr_NODE = t->root;
	if(curr_NODE == NULL)
	  return NULL;
	else if(strncmp(curr_NODE->data->name,name,strlen(name)) == 0)
	  return curr_NODE->data;

	t->root = curr_NODE->left;
	the_data =  tmap_name2value(t,name);
	if(the_data != NULL) // if foudn the data is return
	return the_data;

	t->root = curr_NODE->right;
	the_data =  tmap_name2value(t,name); // either the data or null is returned
	return the_data;	
}


/**
* removes entry corresponding to the given name (if such an entry
*   exists).  On success 1 is returned; if no such entry exists, 0
*   is returned.
*
* Runtime:  O(log n) amortized for bonus credit.
*/
int tmap_remove_by_name(TMAP_PTR t, char *name){
}

