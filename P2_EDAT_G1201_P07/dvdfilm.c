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
    char film_id[MAX_NAME];

    if (argc != 3 || strcmp(argv[1],"remove")) {
        printf("ERROR: Deberia ser %s remove <film_id>\n", argv[0]);
        return EXIT_FAILURE;
    }

    strncpy(film_id, argv[2], MAX_NAME);
    strncpy(query, "select 1 from film, film_actor, film_category, inventory, rental where film.film_id=? and film_actor.film_id=? and film_category.film_id=? and inventory.film_id=? and inventory.inventory_id=rental.rental_id", MAX_Q);

    if (sql_connect_alloc(&env, &dbc, &stmt, &ret) == EXIT_FAILURE) return EXIT_FAILURE;

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &film_id, 0, NULL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &film_id, 0, NULL);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &film_id, 0, NULL);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &film_id, 0, NULL);
    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &film_id, 0, NULL);
    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &film_id, 0, NULL);

    sql_execute(&stmt, query, &columns);

    for (i = 1; i <= columns; i++)
        SQLBindCol(stmt, i, SQL_C_CHAR, resultado[i - 1], sizeof(resultado[i]), NULL);

    if (!SQL_SUCCEEDED(ret=SQLFetch(stmt))) {
        printf("ERROR: Pelicula no encontrada\n");
        return EXIT_FAILURE;
    }

    SQLCloseCursor(stmt);

    strncpy(query, "DELETE FROM payment WHERE rental_id IN (select rental.rental_id from rental, inventory where rental.inventory_id=inventory.inventory_id and inventory.film_id=?); DELETE FROM rental WHERE inventory_id IN (select inventory_id from inventory where film_id=?); DELETE FROM inventory WHERE film_id=?; DELETE FROM film_actor WHERE film_id=?; DELETE FROM film_category WHERE film_id=?; DELETE FROM film WHERE film_id=?;", MAX_Q);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &film_id, 0, NULL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &film_id, 0, NULL);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &film_id, 0, NULL);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &film_id, 0, NULL);
    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &film_id, 0, NULL);

    sql_execute(&stmt, query, &columns);

    SQLCloseCursor(stmt);
    printf("\n");
    return free_all(&stmt,&env,&dbc,&ret);
}
