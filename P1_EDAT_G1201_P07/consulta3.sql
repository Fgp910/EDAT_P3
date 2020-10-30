CREATE VIEW bf_films 
AS 
  SELECT film_id 
  FROM   film_actor, 
         actor 
  WHERE  film_actor.actor_id = actor.actor_id 
         AND actor.first_name = 'Bob' 
         AND actor.last_name = 'Fawcett'; 

CREATE VIEW bf_customers 
AS 
  SELECT rental.customer_id 
  FROM   bf_films 
         NATURAL JOIN inventory, 
         rental 
  WHERE  rental.inventory_id = inventory.inventory_id; 

CREATE VIEW bf_cities 
AS 
  SELECT address.city_id 
  FROM   bf_customers 
         NATURAL JOIN customer, 
         address 
  WHERE  customer.address_id = address.address_id 
  GROUP  BY city_id; 

SELECT city_id, 
       city 
FROM   bf_cities 
       NATURAL JOIN city; 

DROP VIEW bf_cities; 

DROP VIEW bf_customers; 

DROP VIEW bf_films;
