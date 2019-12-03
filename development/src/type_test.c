/*
    Project: EDAT Lab 3 test program
    File:    type_test.c
    Author:  Leandro Garcia & Fabian Gutierrez (P07)
    Rev.     1.0
    Date:    30/11/2019

    It tests the table functionalities
*/

#include <stdio.h>
#include <string.h>
#include "type.h"

int main(int argc, char** argv) {
    int i = 4;
    char str[] = "prueba";
    long long int lli = 1213456789;
    double dbl = 1.002;
    void *aux = NULL;

    /*value_length*/
    if (value_length(INT, &i) != sizeof(int) ||
        value_length(STR, str) != (strlen(str) + 1)*sizeof(char) ||
        value_length(LLNG, &lli) != sizeof(long long int) ||
        value_length(DBL, &dbl) != sizeof(double))
    {
        printf("Error en value_length.\n");
        return 1;
    }

    /*print_value*/
    print_value(stdout, INT, &i);       printf("\n");
    print_value(stdout, STR, str);      printf("\n");
    print_value(stdout, LLNG, &lli);    printf("\n");
    print_value(stdout, DBL, &dbl);     printf("\n");
    if (ferror(stdout)) {
        printf("Error en print_value.\n");
        return 1;
    }

    /*value_cmp*/
    if (value_cmp(INT, &i, &i) ||
        value_cmp(STR, str, str) ||
        value_cmp(LLNG, &lli, &lli) ||
        value_cmp(DBL, &dbl, &dbl))
    {
        printf("Error en value_cmp.\n");
        return 1;
    }

    /*type_parse*/
    if (type_parse("INT")  != INT ||
        type_parse("STR")  != STR ||
        type_parse("LLNG") != LLNG||
        type_parse("DBL")  != DBL)
    {
        printf("Error en type_parse.\n");
        return 1;
    }

    /*value_parse*/
    while (1) {
        aux = value_parse(INT, "2");
        if (*((int*)aux) != 2) break;
        free(aux);

        aux = value_parse(STR, "otro_test");
        if (strcmp((char*)aux, "otro_test")) break;
        free(aux);

        aux = value_parse(LLNG, "987654321");
        if (*((long long int*)aux) != 987654321) break;
        free(aux);

        aux = value_parse(DBL, "9.87654321");
        if (*((double*)aux) != 9.87654321) break;
        free(aux);

        aux = NULL; break;
    }
    if (aux != NULL) {
        free(aux);
        printf("Error en value_parse.\n");
        return 1;
    }

    /*type_to_str*/
    while (1) {
        aux = type_to_str(INT);
        if (strcmp((char*)aux, "INT")) break;
        free(aux);

        aux = type_to_str(STR);
        if (strcmp((char*)aux, "STR")) break;
        free(aux);

        aux = type_to_str(LLNG);
        if (strcmp((char*)aux, "LLNG")) break;
        free(aux);

        aux = type_to_str(DBL);
        if (strcmp((char*)aux, "DBL")) break;
        free(aux);

        aux = NULL; break;
    }
    if (aux != NULL) {
        free(aux);
        printf("Error en type_to_str.\n");
        return 1;
    }

    /*exit*/
    printf("Salida correcta.\n");
    return 0;
}
