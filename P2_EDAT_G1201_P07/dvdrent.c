#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

#define MAX_Q 512
#define MAX_COLS 20
#define MAX_WORD 512
#define MAX_NAME 50

int sql_connect_alloc(SQLHENV *env, SQLHDBC *dbc, SQLHSTMT *stmt, SQLRETURN *ret) {
    /* CONNECT */
    *ret = odbc_connect(env, dbc);
    if (!SQL_SUCCEEDED(*ret)) {
        return EXIT_FAILURE;
    }
    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, *dbc, stmt);
    return EXIT_SUCCESS;
}

void sql_execute(SQLHSTMT *stmt, char* query, SQLSMALLINT *columns) {
    SQLPrepare(*stmt, (SQLCHAR*) query, SQL_NTS);
    SQLExecute(*stmt);
    /* How many columns are there */
    SQLNumResultCols(*stmt, columns);
}

int free_all(SQLHSTMT *stmt, SQLHENV *env, SQLHDBC *dbc, SQLRETURN *ret) {
    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, *stmt);
    /* DISCONNECT */
    *ret = odbc_disconnect(*env, *dbc);
    if (!SQL_SUCCEEDED(*ret)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int main (int argc, char** argv) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    SQLUSMALLINT i = 0;
    SQLSMALLINT columns;

    SQLCHAR resultado[MAX_COLS][MAX_WORD]; /*Aqui se guardan las filas resultantes*/
    char query[MAX_Q];

    char customer_id[MAX_NAME], film_id[MAX_NAME], staff_id[MAX_NAME], store_id[MAX_NAME], inventory_id[MAX_NAME], rental_id[MAX_NAME], amount[MAX_NAME];

    if (argc == 1) {
        printf("ERROR: No hay parametros de entrada.\n");
        return EXIT_FAILURE;
    }

    if (!strcmp(argv[1], "new")) {
        if (argc != 7) {
            printf("ERROR: Deberia ser %s new <customer Id> <film id> <staff id> <store id> <amount>.\n",argv[0]);
            return EXIT_FAILURE;
        }

        strncpy(customer_id, argv[2],MAX_NAME);
        strncpy(film_id, argv[3],MAX_NAME);
        strncpy(staff_id, argv[4],MAX_NAME);
        strncpy(store_id, argv[5],MAX_NAME);
        strncpy(amount, argv[6],MAX_NAME);
        strncpy(query, "select  distinct inventory.inventory_id from customer, film, staff, store, inventory, rental where customer.customer_id=? and film.film_id=? and staff.staff_id=? and store.store_id=? and store.store_id=inventory.store_id and inventory.film_id=? and inventory.inventory_id not in (select inventory_id from rental where return_date>CURRENT_DATE) order by inventory.inventory_id limit 1;",MAX_Q);

        #ifdef DEBUG
            printf("%s, %s, %s\n", f_name, l_name, query);
        #endif

        if (sql_connect_alloc(&env, &dbc, &stmt, &ret) == EXIT_FAILURE) return EXIT_FAILURE;

        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &customer_id, 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &film_id, 0, NULL);
        SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &staff_id, 0, NULL);
        SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &store_id, 0, NULL);
        SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &film_id, 0, NULL);

        sql_execute(&stmt, query, &columns);

        for (i = 1; i <= columns; i++)
            SQLBindCol(stmt, i, SQL_C_CHAR, resultado[i - 1], sizeof(resultado[i]), NULL);

        if (SQL_SUCCEEDED(ret=SQLFetch(stmt))) {
            strncpy(inventory_id,(char*)resultado[0],MAX_NAME);
        }
        else {
            printf("ERROR: No se puede crear alquiler, datos invalidos.\n");
            return EXIT_FAILURE;
        }
        SQLCloseCursor(stmt);

        strncpy(query, "INSERT INTO rental (rental_id, rental_date, inventory_id, customer_id, return_date, staff_id, last_update) VALUES (DEFAULT, CURRENT_DATE, ?, ?, CURRENT_DATE+INTERVAL '1 month', ?, DEFAULT)", MAX_Q);

        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &inventory_id, 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &customer_id, 0, NULL);
        SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &staff_id, 0, NULL);

        sql_execute(&stmt, query, &columns);

        SQLCloseCursor(stmt);

        strncpy(query, "INSERT INTO payment (payment_id, customer_id, staff_id, rental_id, amount, payment_date) VALUES (DEFAULT, ?, ?, (select rental_id from rental order by last_update desc limit 1), ?, CURRENT_DATE); ",MAX_Q);

        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &customer_id, 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &staff_id, 0, NULL);
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_NUMERIC, 0, 0, &inventory_id, 0, NULL);

        sql_execute(&stmt, query, &columns);

        SQLCloseCursor(stmt);
        printf("\n");
        return free_all(&stmt,&env,&dbc,&ret);
    }

    if (!strcmp(argv[1], "remove")) {
        if (argc != 3) {
            printf("ERROR: Deberia ser %s remove <rental_id>.\n",argv[0]);
        }

        strncpy(rental_id, argv[2], MAX_NAME);
        strncpy(query, "select 1 from rental, payment where rental.rental_id=? and payment.rental_id=?", MAX_Q);

        if (sql_connect_alloc(&env, &dbc, &stmt, &ret) == EXIT_FAILURE) return EXIT_FAILURE;

        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &rental_id, 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &rental_id, 0, NULL);

        sql_execute(&stmt, query, &columns);

        for (i = 1; i <= columns; i++)
            SQLBindCol(stmt, i, SQL_C_CHAR, resultado[i - 1], sizeof(resultado[i]), NULL);

        if (!SQL_SUCCEEDED(ret=SQLFetch(stmt))) {
            printf("ERROR: Alquiler no encontrado.\n");
            return EXIT_FAILURE;
        }

        SQLCloseCursor(stmt);

        strncpy(query, "DELETE FROM payment WHERE rental_id=?; DELETE FROM rental WHERE rental_id=?;", MAX_Q);

        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &rental_id, 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &rental_id, 0, NULL);

        sql_execute(&stmt, query, &columns);

        SQLCloseCursor(stmt);
        printf("\n");
        return free_all(&stmt,&env,&dbc,&ret);
    }

    printf("ERROR: Instruccion no valida.\n");

    return EXIT_FAILURE;
}
