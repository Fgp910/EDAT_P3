/*
    Project: EDAT Lab 3 test program
    File:    table_test.c
    Author:  Leandro Garcia & Fabian Gutierrez (P07)
    Rev.     1.0
    Date:    30/11/2019

    It tests the table functionalities
*/

#include <stdio.h>
#include <string.h>
#include "table.h"
#include "type.h"

#define N_COLS 5

/*Private functions*/
void type_fill (type_t *tp);
void data_fill(void **data, char *f_name, char *l_name, int *age, long long int *imdb_id, double *height);
int table_test_clean_up(table_t* t, type_t* tp, void** data,int return_value);

/*Main function*/
int main(int argc, char** argv) {
    table_t *t = NULL;
    type_t *tp = NULL;
    int  n_cols = 0, i = 0;
    long pos = 0, last_pos = 0;
    void **data = NULL, *val = NULL;
    char path[] = "LaLaLand.txt";

    int age = 39;
    long long int imdb_id = 0331516;
    double height = 1.84;

    /*Creating table*/
    if ((tp = (type_t*)malloc(N_COLS*sizeof(type_t))) == NULL)
        return 1;
    type_fill(tp);

    if (!table_create(path, N_COLS, tp)) {
        fprintf(stdout, "ERROR: table_create failed.\n");
        return table_test_clean_up(NULL, tp, NULL, 1);
    }

    if ((t = table_open(path)) == NULL) {
        fprintf(stdout, "ERROR: table_open failed.\n");
        return table_test_clean_up(t, tp, NULL, 1);
    }

    /*Filling table*/
    if ((data = malloc(N_COLS*sizeof(void*))) == NULL)
        return table_test_clean_up(t, tp, data, 1);
    data_fill(data, "Ryan", "Gosling", &age, &imdb_id, &height);

    if (!table_insert_record(t, data)) {
        fprintf(stdout, "ERROR: table_insert_record failed (1).\n");
        return table_test_clean_up(t, tp, data, 1);
    }

    age = 31; imdb_id = 1297015; height = 1.68;
    data_fill(data, "Emma", "Stone", &age, &imdb_id, &height);

    if (!table_insert_record(t, data)) {
        fprintf(stdout, "ERROR: table_insert_record failed (2).\n");
        return table_test_clean_up(t, tp, data, 1);
    }

    /*Reading table*/
    free(tp);
    if ((tp = table_types(t)) == NULL) {
        fprintf(stdout, "ERROR: table_types failed.\n");
        return table_test_clean_up(t, NULL, data, 1);
    }
    if ((n_cols = table_ncols(t)) < 0) {
    	fprintf(stdout, "ERROR: table_ncols failed.\n");
        return table_test_clean_up(t, NULL, data, 1);
    }
    if ((pos = table_first_pos(t)) <= 0) {
        fprintf(stdout, "ERROR: table_first_pos failed.\n");
        return table_test_clean_up(t, tp, data, 1);
    }
    if ((last_pos = table_last_pos(t)) <= 0) {
    	fprintf(stdout, "ERROR: table_last_pos failed.\n");
        return table_test_clean_up(t, tp, data, 1);
    }

    while (pos > 0 && pos != last_pos) {
        pos = table_read_record(t, pos);
        for (i = 0; i < n_cols; i++) {
            val = table_get_col(t, i);
            print_value(stdout, tp[i], val);
            printf("  ");
        }
        printf("\n");
    }

    return table_test_clean_up(t, NULL, data, 0);
}

/*Private functions implementation*/
void type_fill (type_t *tp) {
    if (tp != NULL) {
        tp[0] = STR;
        tp[1] = STR;
        tp[2] = INT;
        tp[3] = LLNG;
        tp[4] = DBL;
    }
}

void data_fill(void **data, char *f_name, char *l_name, int *age, long long int *imdb_id, double *height) {
    if (data != NULL) {
        data[0] = f_name;
        data[1] = l_name;
        data[2] = age;
        data[3] = imdb_id;
        data[4] = height;
    }
}

int table_test_clean_up(table_t* t, type_t* tp, void** data,int return_value) {
    if (t != NULL)
        table_close(t);
    if (tp != NULL)
        free(tp);
    if (data != NULL)
        free(data);

    return return_value;
}
