#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "family.h"

/* Number of word pointers allocated for a new family.
   This is also the number of word pointers added to a family
   using realloc, when the family is full.
*/
static int family_increment = 0;


/* Set family_increment to size, and initialize random number generator.
   The random number generator is used to select a random word from a family.
   This function should be called exactly once, on startup.
*/
void init_family(int size) {
    family_increment = size;
    srand(time(0));
}


/* Given a pointer to the head of a linked list of Family nodes,
   print each family's signature and words.

   Do not modify this function. It will be used for marking.
*/
void print_families(Family *fam_list) {
    int i;
    Family *fam = fam_list;

    while (fam) {
        printf("***Family signature: %s Num words: %d\n",
               fam->signature, fam->num_words);
        for (i = 0; i < fam->num_words; i++) {
            printf("     %s\n", fam->word_ptrs[i]);
        }
        printf("\n");
        fam = fam->next;
    }
}


/* Return a pointer to a new family whose signature is 
   a copy of str. Initialize word_ptrs to point to 
   family_increment+1 pointers, numwords to 0, 
   maxwords to family_increment, and next to NULL.
*/
Family *new_family(char *str) {
    Family *fam = malloc(sizeof(Family));// 
    if (fam == NULL) {
        perror("malloc");
        exit(1);
    }

    fam->signature = malloc(sizeof(char) * (strlen(str) + 1));
    if (fam->signature == NULL) {
        perror("malloc");
        exit(1);
    }
    strcpy(fam->signature, str); // safe because we allocated enough space for the \0?

    fam->word_ptrs = malloc(sizeof(char *) * (family_increment + 1));
    if (fam->word_ptrs == NULL) {
        perror("malloc");
        exit(1);
    }

    fam->num_words = 0;

    fam->max_words = family_increment;

    fam->next = NULL;

    for (int i = 0; i <= fam->max_words; i++) {
        (fam->word_ptrs)[i] = NULL;
    }
    return fam;
}


/* Add word to the next free slot fam->word_ptrs.
   If fam->word_ptrs is full, first use realloc to allocate family_increment
   more pointers and then add the new pointer.
*/
void add_word_to_family(Family *fam, char *word) {
    if (fam == NULL) {
        return;
    }
    if (fam->num_words == fam->max_words) {
        fam->max_words += family_increment;
        (fam->word_ptrs) = realloc(fam->word_ptrs, sizeof(char *) * (fam->max_words + 1));
        if (fam->word_ptrs == NULL) {
            perror("realloc");
            exit(1);
        }
        for (int i = (fam->num_words) + 1; i <= fam->max_words; i++) {
            (fam->word_ptrs)[i] = NULL; // set empty values to NULL
        }
    }
    int i = fam->num_words;
    (fam->word_ptrs)[i] = word; // We assume word is a pointer to something in the dynamically allocated word_list
    (fam->num_words)++;
}


/* Return a pointer to the family whose signature is sig;
   if there is no such family, return NULL.
   fam_list is a pointer to the head of a list of Family nodes.
*/
Family *find_family(Family *fam_list, char *sig) { // fam_list is a linked list, last node has NULL .next
    Family *fam = fam_list;
    while (fam != NULL) {
        if (strcmp(fam->signature, sig) == 0) {
            return fam;
        }
        fam = fam->next;
    }
    return NULL;
}


/* Return a pointer to the family in the list with the most words;
   if the list is empty, return NULL. If multiple families have the most words,
   return a pointer to any of them.
   fam_list is a pointer to the head of a list of Family nodes.
*/
Family *find_biggest_family(Family *fam_list) {
    Family *fam = fam_list;
    Family *max_fam = fam;
    while (fam != NULL) {
        if (fam->num_words > max_fam->num_words) {
            max_fam = fam;
        }
        fam = fam->next;
    }
    return max_fam;
}


/* Deallocate all memory rooted in the List pointed to by fam_list. */
void deallocate_families(Family *fam_list) {
    Family *fam = fam_list;
    while (fam != NULL) {
        free(fam->signature); // Free the signature
        free(fam->word_ptrs); // words in word_ptrs shouldn't be deallocated here.
        Family *temp = fam;
        fam = fam->next;
        free(temp); // Free the family
    }
}


/* Generate and return a linked list of all families using words pointed to
   by word_list, using letter to partition the words.

   Implementation tips: To decide the family in which each word belongs, you
   will need to generate the signature of each word. Create only the families
   that have at least one word from the current word_list.
*/
Family *generate_families(char **word_list, char letter) {
    Family *fam_list = NULL;
    Family *end = fam_list;
    int size_fam_list = 0;
    int wrd_num = 0;
    if (word_list == NULL) {
        return NULL;
    }
    while (word_list[wrd_num] != NULL) {
        /* Get signature for word */
        char *word = word_list[wrd_num];
        unsigned int len = strlen(word);
        char sig[len + 1];
        sig[len] = '\0';
        for (int i = 0; i < len; i++) {
            if (word[i] == letter) {
                sig[i] = letter;
            } else {
                sig[i] = '_';
            }
        }

        /* Check if signature already exists */
        Family *fam;
        if (fam_list == NULL) { // list empty so initialize
            fam_list = new_family(sig);
            end = fam_list;
            size_fam_list++;
            fam = fam_list;
        } else {
            fam = find_family(fam_list, sig); // try and find existing family with required signature
            if (fam == NULL) { // no existing matching family, add new family to end of list
                fam = new_family(sig);
                end->next = fam;
                end = fam;
                size_fam_list++;
            }
        }
        add_word_to_family(fam, word);
        wrd_num++;
    }
    return fam_list;
}


/* Return the signature of the family pointed to by fam. */
char *get_family_signature(Family *fam) {
    return fam->signature;
}


/* Return a pointer to word pointers, each of which
   points to a word in fam. These pointers should not be the same
   as those used by fam->word_ptrs (i.e. they should be independently malloc'd),
   because fam->word_ptrs can move during a realloc.
   As with fam->word_ptrs, the final pointer should be NULL.
*/
char **get_new_word_list(Family *fam) {
    if (fam == NULL) {
        return NULL;
    }
    int length = fam->num_words;
    char **wrd_ptrs = fam->word_ptrs;
    char **word_pointers = malloc(sizeof(char *) * (length + 1));
    if (word_pointers == NULL) {
        perror("malloc");
        exit(1);
    }
    word_pointers[length] = NULL;
    for (int i = 0; i < length; i++) {
        word_pointers[i] = wrd_ptrs[i];
    }
    return word_pointers;


}


/* Return a pointer to a random word from fam. 
   Use rand (man 3 rand) to generate random integers.
*/
char *get_random_word_from_family(Family *fam) {
    return (fam->word_ptrs)[rand() % (fam->num_words)];
}
