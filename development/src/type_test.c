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

    if (value_length(INT, &i) != sizeof(int) ||
        value_length(STR, str) != (strlen(str) + 1)*sizeof(char) ||
        value_length(LLNG, &lli) != sizeof(long long int) ||
        value_length(DBL, &dbl) != sizeof(double))
    {
        printf("Error en value_length.\n");
        return 1;
    }

    print_value(stdout, INT, &i);       printf("\n");
    print_value(stdout, STR, str);      printf("\n");
    print_value(stdout, LLNG, &lli);    printf("\n");
    print_value(stdout, DBL, &dbl);     printf("\n");
    if (ferror(stdout)) {
        printf("Error en print_value.\n");
        return 1;
    }

    if (value_cmp(INT, &i, &i) ||
        value_cmp(STR, str, str) ||
        value_cmp(LLNG, &lli, &lli) ||
        value_cmp(DBL, &dbl, &dbl))
    {
        printf("Error en value_cmp.\n");
        return 1;
    }

    if (type_parse("INT")  != INT ||
        type_parse("STR")  != STR ||
        type_parse("LLNG") != LLNG||
        type_parse("DBL")  != DBL)
    {
        printf("Error en type_parse.\n");
        return 1;
    }

    if (*((int*)value_parse(INT, "2"))  != 2 ||
        strcmp((char*)value_parse(STR, "otro_test"), "otro_test") ||
        *((long long int*)value_parse(LLNG, "987654321")) != 987654321||
        *((double*)value_parse(DBL, "9.87654321"))  != 9.87654321)
    {
        printf("Error en value_parse.\n");
        return 1;
    }

    if (type_to_str(INT)  != "INT" ||
        type_to_str(STR)  != "STR" ||
        type_to_str(LLNG) != "LLNG"||
        type_to_str(DBL)  != "DBL")
    {
        printf("Error en type_to_str.\n");
        return 1;
    }

    return 0;
}
