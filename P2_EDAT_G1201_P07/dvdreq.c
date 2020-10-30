#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

#define MAX_Q 2048
#define MAX_COLS 20
#define MAX_WORD 512
#define MAX_NAME 30
#define MAX_ID 10
#define MAX_DATE 11 /*yyyy-mm-dd*/

/* Funciones utiles */
int sql_connect_alloc(SQLHENV *env, SQLHDBC *dbc, SQLHSTMT *stmt, SQLRETURN *ret);
void sql_execute(SQLHSTMT *stmt, char* query, SQLSMALLINT *columns);
int free_all(SQLHSTMT *stmt, SQLHENV *env, SQLHDBC *dbc, SQLRETURN *ret);

/* Funciones de consulta */
void dvdreq_customer(SQLHSTMT *stmt, SQLRETURN *ret, char* fname, char* lname);
void dvdreq_film(SQLHSTMT *stmt, SQLRETURN *ret, SQLHDBC *dbc, char* ftitle);
void dvdreq_rent(SQLHSTMT *stmt, SQLRETURN *ret, char* cid, char* initdate, char* enddate);
void dvdreq_recommend(SQLHSTMT *stmt, SQLRETURN *ret, char* cid);

/* Programa Principal */
int main(int argc, char** argv) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret;  /*ODBC API return status */

    if (argc == 1) {
        printf("ERROR: No hay parametros de entrada (parametros validos: customer, film, rent, recommend).\n");
        return EXIT_FAILURE;
    }

    if (sql_connect_alloc(&env, &dbc, &stmt, &ret) == EXIT_FAILURE) return EXIT_FAILURE;

    /*---customer---*/
    if (!strcmp(argv[1],"customer")) {
        if (argc != 6 || strcmp(argv[2],"-n") || strcmp(argv[4],"-a")) {
            printf("ERROR: Deberia ser %s customer -n <first_name> -a <last_name>.\n",argv[0]);
            return EXIT_FAILURE;
        }
        dvdreq_customer(&stmt, &ret, argv[3], argv[5]);
    }
    /*---film---*/
    else if (!strcmp(argv[1],"film")) {
        if (argc != 3) {
            printf("ERROR: Debería ser %s film <title>\n", argv[0]);
            return EXIT_FAILURE;
        }
        dvdreq_film(&stmt, &ret, &dbc, argv[2]);
    }
    /*---rent---*/
    else if (!strcmp(argv[1],"rent")) {
        if (argc != 5) {
            printf("ERROR: Debería ser %s rent <customer_id> <init_date> <end_date> (las fechas: yyyy-mm-dd)\n", argv[0]);
            return EXIT_FAILURE;
        }
        dvdreq_rent(&stmt, &ret, argv[2], argv[3], argv[4]);
    }
    /*---recommend---*/
    else if (!strcmp(argv[1],"recommend")) {
        if (argc != 3) {
            printf("ERROR: Debería ser %s recommend <customer_id>\n", argv[0]);
            return EXIT_FAILURE;
        }
        dvdreq_recommend(&stmt, &ret, argv[2]);
    }

    else {
        printf("ERROR: Parametro de entrada no reconocido (parametros validos: customer, film, rent, recommend).\n");
        SQLCloseCursor(stmt);
        printf("\n");
        free_all(&stmt,&env,&dbc,&ret);
        return EXIT_FAILURE;
    }
    
    /*exit*/
    SQLCloseCursor(stmt);
    printf("\n");
    return free_all(&stmt,&env,&dbc,&ret);
}

/* Implementacion de funciones */
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

void dvdreq_customer(SQLHSTMT *stmt, SQLRETURN *ret, char* fname, char* lname) {
    char f_name[MAX_NAME], l_name[MAX_NAME];
    char query[MAX_Q];
    SQLUSMALLINT i = 0;
    SQLSMALLINT columns;
    SQLCHAR buf[MAX_Q];
    SQLCHAR resultado[MAX_COLS][MAX_WORD]; /*Aqui se guardan las filas resultantes*/

    strncpy(f_name, fname, MAX_NAME - 1);
    strncpy(l_name, lname, MAX_NAME - 1);
    sprintf(query, "select customer.customer_id, customer.first_name, customer.last_name, customer.create_date, address.address, city.city, country.country from customer, address, city, country where (first_name = ? or last_name = ?) and customer.address_id = address.address_id and address.city_id = city.city_id and city.country_id = country.country_id;");
    #ifdef DEBUG
        printf("%s, %s, %s\n", f_name, l_name, query);
    #endif

    SQLBindParameter(*stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, &f_name, 0, NULL);
    SQLBindParameter(*stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, &l_name, 0, NULL);

    sql_execute(stmt, query, &columns);

    /* print the name of each column */
    for (i = 1; i <= columns - 3; i++) {
        SQLDescribeCol(*stmt, i, buf, sizeof(buf), NULL, NULL, NULL, NULL, NULL);
        printf("%15s|", buf);
    }
    SQLDescribeCol(*stmt, columns - 2, buf, sizeof(buf), NULL, NULL, NULL, NULL, NULL);  /*Aprovechamos que la primera parte de la direccion se llama address*/
    printf("%40s|", buf);
    printf("\n");

    for (i = 1; i <= columns; i++)
        SQLBindCol(*stmt, i, SQL_C_CHAR, resultado[i - 1], sizeof(resultado[i - 1]), NULL);

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(*ret = SQLFetch(*stmt))) {
        for (i = 1; i <= columns - 3; i++) {
            printf("%15s|", (char*)resultado[i - 1]);
        }
        sprintf((char*)resultado[columns - 3] + strlen((char*)resultado[columns - 3]),", %s, %s",resultado[columns - 2],resultado[columns - 1]);
        printf("%40s|\n", (char*)resultado[columns - 3]);   /*anadimos ciudad y pais a address*/
    }
}

void dvdreq_film(SQLHSTMT *stmt, SQLRETURN *ret, SQLHDBC *dbc, char* ftitle) {
    char title[MAX_WORD], cast[MAX_WORD];
    char film_id[MAX_ID];
    char query[MAX_Q];
    SQLUSMALLINT i = 0;
    SQLSMALLINT columns;
    SQLCHAR buf[MAX_Q];
    SQLCHAR resultado[MAX_COLS][MAX_WORD]; /*Aqui se guardan las filas resultantes*/
    /*Variables para una segunda consulta*/
    SQLHSTMT stmt2;
    SQLSMALLINT columns2;
    SQLCHAR resultado2[MAX_COLS][MAX_WORD];

    sprintf(title, "%%%s%%", ftitle);
    strncpy(query, "select film.film_id, film.title, film.release_year, film.length, language.name, film.description from film, language where film.title like ? and language.language_id=film.language_id;", MAX_Q -1);
    #ifdef DEBUG
        printf("%s, %s\n", title, query);
    #endif

    SQLAllocHandle(SQL_HANDLE_STMT, *dbc, &stmt2);

    SQLBindParameter(*stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, &title, 0, NULL);

    sql_execute(stmt, query, &columns);

    /* print the name of each column */
    for (i = 1; i <= columns; i++) {
        SQLDescribeCol(*stmt, i, buf, sizeof(buf), NULL, NULL, NULL, NULL, NULL);
        if (i == 2)     /*title*/
            printf("%-25s|", buf);
        else if (i == 6)  /*description*/
            printf("%-115s|", buf);
        else
            printf("%-20s|", buf);
    }
    printf("%-150s|\n", "cast");

    for (i = 1; i <= columns; i++)
        SQLBindCol(*stmt, i, SQL_C_CHAR, resultado[i - 1], sizeof(resultado[i - 1]), NULL);

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(*ret = SQLFetch(*stmt))) {
        for (i = 1; i <= columns; i++) {
            if (i == 2)     /*title*/
                printf("%-25s|", resultado[i - 1]);
            else if (i == 6)  /*description*/
                printf("%-115s|", resultado[i - 1]);
            else
                printf("%-20s|", resultado[i - 1]);
        }

        /*Cast*/
        cast[0] = '\0';
        sprintf(film_id, "%s", (char*)resultado[0]);
        strncpy(query, "select actor.first_name, actor.last_name from actor, film_actor where film_actor.actor_id=actor.actor_id and film_actor.film_id=?;", MAX_Q - 1);
        SQLBindParameter(stmt2, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &film_id, 0, NULL);
        sql_execute(&stmt2, query, &columns2);

        SQLBindCol(stmt2, 1, SQL_C_CHAR, resultado2[0], sizeof(resultado2[0]), NULL);
        SQLBindCol(stmt2, 2, SQL_C_CHAR, resultado2[1], sizeof(resultado2[1]), NULL);

        while (SQL_SUCCEEDED(*ret = SQLFetch(stmt2))) {
            sprintf(cast + strlen(cast), "%s %s, ", (char*)resultado2[0], (char*)resultado2[1]);
        }
        cast[strlen(cast) - 3] = '\0';
        SQLCloseCursor(stmt2);

        printf("%-150s|\n", cast);
    }
    /*exit*/
    SQLFreeHandle(SQL_HANDLE_STMT, stmt2);
}

void dvdreq_rent(SQLHSTMT *stmt, SQLRETURN *ret, char* cid, char* initdate, char* enddate) {
    char customer_id[MAX_ID];
    char init_date[MAX_DATE], end_date[MAX_DATE];
    char query[MAX_Q];
    SQLUSMALLINT i = 0;
    SQLSMALLINT columns;
    SQLCHAR buf[MAX_Q];
    SQLCHAR resultado[MAX_COLS][MAX_WORD]; /*Aqui se guardan las filas resultantes*/

    strncpy(customer_id, cid, MAX_ID - 1);
    strncpy(init_date, initdate, MAX_DATE - 1);
    strncpy(end_date, enddate, MAX_DATE - 1);
    sprintf(query, "Select rental.rental_id, rental.rental_date, film.film_id, film.title, staff.staff_id, staff.first_name, staff.last_name, staff.store_id, payment.amount From rental, inventory, film, payment, staff Where rental.customer_id = ? and rental.inventory_id = inventory.inventory_id and inventory.film_id = film.film_id and rental.staff_id = staff.staff_id and payment.rental_id = rental.rental_id and rental.rental_date >= ? and rental.rental_date <= ? Order by rental.rental_date;");
    #ifdef DEBUG
        printf("%s, %s, %s, %s\n", customer_id, init_date, end_date, query);  /*Para depurar*/
    #endif

    SQLBindParameter(*stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &customer_id, 0, NULL);
    SQLBindParameter(*stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TIMESTAMP, 0, 0, &init_date, 0, NULL);
    SQLBindParameter(*stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TIMESTAMP, 0, 0, &end_date, 0, NULL);

    sql_execute(stmt, query, &columns);

    /* print the name of each column */
    for (i = 1; i <= columns; i++) {
        SQLDescribeCol(*stmt, i, buf, sizeof(buf), NULL, NULL, NULL, NULL, NULL);
        if (i == 2)   /*date*/
            printf("%20s|", buf);
        else if (i == 4)    /*title*/
            printf("%25s|", buf);
        else
            printf("%10s|", buf);
    }
    printf("\n");
    if (i == 1)     /*No hizo bien la consulta*/
        printf("ERROR: No pudo realizarse la consulta.\nSugerencia: La fecha ha de ser yyyy-mm-dd\n");

    for (i = 1; i <= columns; i++)
        SQLBindCol(*stmt, i, SQL_C_CHAR, resultado[i - 1], sizeof(resultado[i - 1]), NULL);

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(*ret = SQLFetch(*stmt))) {
        for (i = 1; i <= columns; i++) {
            if (i == 2)   /*date & title*/
                printf("%20s|", resultado[i - 1]);
            else if (i == 4)    /*title*/
                printf("%25s|", resultado[i - 1]);
            else
                printf("%10s|", resultado[i - 1]);
        }
        printf("\n");
    }
}

void dvdreq_recommend(SQLHSTMT *stmt, SQLRETURN *ret, char* cid) {
    char customer_id[MAX_ID];
    char query[MAX_Q];
    SQLUSMALLINT i = 0;
    SQLSMALLINT columns;
    SQLCHAR buf[MAX_Q];
    SQLCHAR resultado[MAX_COLS][MAX_WORD]; /*Aqui se guardan las filas resultantes*/

    strncpy(customer_id, cid, MAX_ID - 1);
    strncpy(query, "WITH count_category AS ( SELECT category.category_id, Count(*) AS total FROM rental, inventory, film, film_category, category WHERE rental.customer_id = ? AND rental.inventory_id = inventory.inventory_id AND inventory.film_id = film.film_id AND film.film_id = film_category.film_id AND film_category.category_id = category.category_id GROUP BY category.category_id ), fav_category AS ( SELECT category_id FROM count_category ORDER BY total DESC LIMIT 1 ), count_film AS ( SELECT film_category.film_id, Count(*) AS total FROM fav_category NATURAL JOIN film_category, inventory, rental WHERE film_category.film_id = inventory.film_id AND inventory.inventory_id = rental.inventory_id GROUP BY film_category.film_id ), customer_films AS ( SELECT film_category.film_id FROM fav_category NATURAL JOIN film_category, inventory, rental WHERE rental.customer_id = ? AND film_category.film_id = inventory.film_id AND inventory.inventory_id = rental.inventory_id GROUP BY film_category.film_id ) SELECT film.title FROM film NATURAL JOIN count_film WHERE film.film_id NOT IN (SELECT * FROM customer_films) ORDER BY count_film.total DESC LIMIT 3;", MAX_Q - 1);
    #ifdef DEBUG
        printf("%s, %s\n", customer_id, query);  /*Para depurar*/
    #endif

    SQLBindParameter(*stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &customer_id, 0, NULL);
    SQLBindParameter(*stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, &customer_id, 0, NULL);

    sql_execute(stmt, query, &columns);

    /* print the name of each column */
    for (i = 1; i <= columns; i++) {
        SQLDescribeCol(*stmt, i, buf, sizeof(buf), NULL, NULL, NULL, NULL, NULL);
        printf("%30s|", buf);
    }
    printf("\n");

    for (i = 1; i <= columns; i++)
        SQLBindCol(*stmt, i, SQL_C_CHAR, resultado[i], sizeof(resultado[i]), NULL);

    /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(*ret = SQLFetch(*stmt))) {
        for (i = 1; i <= columns; i++)
            printf("%30s|", resultado[i]);
        printf("\n");
    }
}
