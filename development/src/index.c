#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "index.h"

typedef struct {
    int key;                /*It only works with integers by now*/
    int n_offsets;
    long *offsets;
} entry;

struct index_ {
    FILE *path;
    type_t type;
    int n_keys;
    entry **entries;
};

/*Private fuctions prototypes*/
int binary_search(index_t *idx, int key);

/*
   Function: int index_create(char *path, int type)

   Creates a file for saving an empty index. The index is initialized
   to be of the specific tpe (in the basic version this is always INT)
   and to contain 0 entries.

   Parameters:
   path:  the file where the index is to be created
   type:  the type of the index (always INT in this version)

   Returns:
   1:   index created
   0:   parameter error or file creation problem. Index not created.
 */
int index_create(char *path, type_t type) {
    FILE *f;

    if (path == NULL) {
        return 0;
    }

    f = fopen(path, "wb");
    if (f == NULL) {
        return 0;
    }

    fwrite(&type, sizeof(type_t), 1, f);
    fwrite(0, sizeof(int), 1, f);

    return 1;
}



/*
   Opens a previously created index: reads the contents of the index
   in an index_t structure that it allocates, and returns a pointer to
   it (or NULL if the files doesn't exist or there is an error).

   NOTE: the index is stored in memory, so you can open and close the
   file in this function. However, when you are asked to save the
   index, you will not be given the path name again, so you must store
   in the structure either the FILE * (and in this case you must keep
   the file open) or the path (and in this case you will open the file
   again).

   Parameters:
   path:  the file where the index is

   Returns:
   pt:   index opened
   NULL: parameter error or file opening problem. Index not opened.

 */
index_t* index_open(char* path) {
    FILE *f;
    int n, i, j, key, n_offsets;
    type_t type;
    index_t *index;
    entry *ent;

    f=fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }

    if (fread(&type, sizeof(type_t), 1, f) < 0) {
        fclose(f);
        return NULL;
    }
    if (fread(&n, sizeof(int), 1, f) < 0) {
        fclose(f);
        return NULL;
    }

    index = (index_t*)malloc(sizeof(index_t));
    if (index == NULL) {
        fclose(f);
        return NULL;
    }

    index->path = path;
    index->type = type;
    index->n_keys = n;

    index->entries = (entry**)malloc(n*sizeof(entry*));
    if (index->entries == NULL) {
        fclose(f);
        index_close(index);
        return NULL;
    }

    for (i = 0; i < n; i++) {
        index->entries[i] = (entry*)malloc(sizeof(entry));
        if (index->entries[i] == NULL) {
            index_close(index);
            return NULL;
        }

        if(fread(&(index->entries[i]->key), sizeof(int), 1, f) < 0) {
            index_close(index);
            return NULL;
        }
        if(fread(&(index->entries[i]->n_offsets), sizeof(int), 1, f) < 0) {
            index_close(index);
            return NULL;
        }
        for (j = 0; j < n_offsets; j++) {
            if(fread(&(index->entries[i]->offsets[j]), sizeof(long), 1, f) < 0) {
                index_close(index);
                return NULL;
            }
        }

        for (j = i; j > 0 && index->entries[j]->key < index->entries[j-1]->key; j--) {
            ent = index->entries[j];
            index->entries[j] = index->entries[j-1];
            index->entries[j-1] = ent;
        }
    }
    fclose(f);
    return index;
}


/*
   int index_save(index_t* index);

   Saves the current state of index in the file it came from. Note
   that the name of the file in which the index is to be saved is not
   given.  See the NOTE to index_open.

   Parameters:
   index:  the index the function operates upon

   Returns:
   1:  index saved
   0:  error saving the index (cound not open file)

*/
int index_save(index_t* idx) {
    FILE *f;
    int i, j;

    if (idx == NULL) {
        return 0;
    }

    f = fopen(idx->path, "wb");
    if (f == NULL) {
        return 0;
    }

    fwrite(&(idx->type), sizeof(type_t), 1, f);
    fwrite(&(idx->n_keys), sizeof(int), 1, f);

    for (i = 0; i < idx->n_keys; i++) {
        fwrite(&(idx->entries[i]->key), sizeof(int), 1, f);
        fwrite(&(idx->entries[i]->n_offsets), sizeof(int), 1, f);
        for (j = 0; j < idx->entries[i]->n_offsets; j++) {
            fwrite(&(idx->entries[i]->offsets[j]), sizeof(long), 1, f);
        }
    }

    return 1;
}


/*
   Function: int index_put(index_t *index, int key, long pos);

   Puts a pair key-position in the index. Note that the key may be
   present in the index or not... you must manage both situation. Also
   remember that the index must be kept ordered at all times.

   Parameters:
   index:  the index the function operates upon
   key: the key of the record to be indexed (may or may not be already
        present in the index)
   pos: the position of the corresponding record in the table
        file. This is the datum that we will want to recover when we
        search for the key.

   Return:
   n>0:  after insertion the file now contains n unique keys
   0:    error inserting the key

*/
int index_put(index_t *idx, int key, long pos) {
    int m, i;

    if (idx == NULL || pos < 0) {
        return 0;
    }

    m = binary_search(idx, key);
    if (m >= 0) {
        idx->entries[m]->n_offsets++;
        realloc(idx->entries[m]->offsets, idx->entries[m]->n_offsets);
        idx->entries[m]->offsets[idx->entries[m]->n_offsets-1] = pos; 
    }
    else {
        idx->n_keys++;
        realloc(idx->entries, idx->n_keys);
        for (i = idx->n_keys-1; i > -m-1; i--) {
            idx->entries[i]= idx->entries[i-1];
        }

        idx->entries[-m-1] = (entry*)malloc(sizeof(entry));
        if (idx->entries[-m-1]== NULL) {
            return 0;
        }
        idx->entries[-m-1]->key = key;
        idx->entries[-m-1]->n_offsets = 1;
        idx->entries[-m-1]->offsets = (long*)malloc(sizeof(long));
        if (idx->entries[-m-1]->offsets == NULL) {
            return 0;
        }
        idx->entries[-m-1]->offsets[0] = pos;
    }
}


/*
   Function: long *index_get(index_t *index, int key, int* nposs);

   Retrieves all the positions associated with the key in the index.

   Parameters:
   index:  the index the function operates upon
   key: the key of the record to be searched
   nposs: output paramters: the number of positions associated to this key

   Returns:

   pos: an array of *nposs long integers with the positions associated
        to this key
   NULL: the key was not found

   NOTE: the parameter nposs is not an array of integers: it is
   actually an integer variable that is passed by reference. In it you
   must store the number of elements in the array that you return,
   that is, the number of positions associated to the key. The call
   will be something like this:

   int n
   long **poss = index_get(index, key, &n);

   for (int i=0; i<n; i++) {
       Do something with poss[i]
   }

   ANOTHER NOTE: remember that the search for the key MUST BE DONE
   using binary search.

   FURTHER NOTE: the pointer returned belongs to this module. The
   caller guarantees that the values returned will not be changed.

*/
long* index_get(index_t *idx, int key, int* nposs) {
    int m;
    
    if (idx == NULL) {
        return NULL;
    }

    m= binary_search(idx, key);
    if (m < 0 || m >= idx->n_keys) {
        return NULL;
    }
    *(nposs) = idx->entries[m]->n_offsets;
    return idx->entries[m]->offsets;
}


/*
   Closes the index by freeing the allocated resources. No operation
   on the index will be possible after calling this function.

   Parameters:
   index:  the index the function operates upon

   Returns:
   Nothing

   NOTE: This function does NOT save the index on the file: you will
   have to call the function index_save for this.
*/
void index_close(index_t *idx) {
    int i;

    if (idx == NULL) return;

    if (idx->entries != NULL) {
        for (i = 0; i < idx->n_keys; i++) {
            if (idx->entries[i])
                free(idx->entries[i]);
        }
    }
    free(idx);
    return;
}


/*
  Function: long **index_get_order(index_t *index, int n, int* nposs);

  Function useful for debugging but that should not be used otherwise:
  returns the nth record in the index. DO NOT USE EXCEPT FOR
  DEBUGGING. The test program uses it.

   Parameters:
   index:  the index the function operates upon
   n: number of the record to be returned
   key: output parameter: the key of the record
   nposs: output parameter: the number of positions associated to this key

   Returns:

   pos: an array of *nposs long integers with the positions associated
        to this key
   NULL: the key was not found


   See index_get for explanation on nposs and pos: they are the same stuff
*/
long *index_get_order(index_t *index, int n, int *key, int* nposs) {
    if (index == NULL  || n < 0 || n >= index->n_keys) {
        return NULL;
    }

    *(key) = index->entries[n]->key;
    *(nposs) = index->entries[n]->n_offsets;
    return index->entries[n]->offsets;
}

int binary_search(index_t *idx, int key) {
    int low;
    int high;
    int middle;

    if (idx == NULL) {
        return NULL;
    }

    low = 0;
    high = idx->n_keys-1;

    while(low <= high) {
        middle = (low+high)/2;

        if (idx->entries[middle]->key == key)
            return middle; // clave encontrada
        if (idx->entries[middle]->key < key) {
            low = middle+1;
        }
        else {
            high = middle-1;
        }   
    }
    return -(middle+1); // clave no encontrada
} 
