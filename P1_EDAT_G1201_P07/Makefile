# Por favor especifique su grupo y numero de pareja en las dos
# líneas siguientes.
GRUPO=1201
PAREJA=07

# Por lo general no se requiere modificar el fichero
# a parir de esta línea
# export PGDATABASE:=$(GRUPO)_$(PAREJA)
export PGDATABASE:=dvdrental
export PGUSER :=alumnodb
export PGPASSWORD :=alumnodb
export PGCLIENTENCODING:=LATIN9
export PGHOST:=localhost

DBNAME =$(PGDATABASE)
PSQL = psql
CREATEDB = createdb
DROPDB = dropdb --if-exists
PG_DUMP = pg_dump
PG_RESTORE = pg_restore

all: dropdb createdb restore
nuevabase: dropdb createdb
	@echo restore nuevabase data base
	@cat nuevabase.sql | $(PSQL)
createdb:
	@echo Creando BBDD
	@$(CREATEDB)
dropdb:
	@echo Eliminando BBDD
	@$(DROPDB) $(DBNAME)
	rm -f *.log
dump:
	@echo creando dumpfile
	@$(PG_DUMP) > $(DBNAME).sql
restore:
	@echo restore data base
	@cat $(DBNAME).sql | $(PSQL)
shell:
	@echo create psql shell
	@$(PSQL)

consulta1:
	@echo consulta-1: "alquileres por agno"
	@cat consulta1.sql | $(PSQL) | tee consulta1.log
consulta2:
	@echo consulta-2: "cliente que alquila mas peliculas"
	@cat consulta2.sql | $(PSQL) | tee consulta2.log
consulta3:
	@echo consulta3 "ciudades alquiler Bob Fawcett"
	@cat consulta3.sql | $(PSQL) | tee consulta3.log
consulta4:
	@echo consulta4: "lenguage en el que se producen mas peliculas"
	@cat consulta4.sql | $(PSQL) | tee consulta4.log
consulta5:
	@echo consulta5: "lenguage en el que se alquilan mas peliculas"
	@cat consulta5.sql | $(PSQL) | tee consulta5.log
consulta6:
	@echo consulta6: "categoria favorita del cliente que alquila mas  peliculas"
	@cat consulta6.sql | $(PSQL) | tee consulta6.log
