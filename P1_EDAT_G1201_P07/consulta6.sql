CREATE VIEW num_rentals 
AS 
  SELECT customer_id, 
         Count(*) AS num 
  FROM   rental 
  GROUP  BY customer_id; 

CREATE VIEW max_customer 
AS 
  SELECT customer_id, 
         first_name, 
         last_name, 
         num 
  FROM   num_rentals 
         NATURAL JOIN customer 
  WHERE  num = (SELECT Max(num) 
                FROM   num_rentals); 

CREATE VIEW count_category 
AS 
  SELECT customer.customer_id, 
         category.category_id, 
         Count(*) AS total 
  FROM   max_customer 
         NATURAL JOIN customer, 
         rental, 
         inventory, 
         film, 
         film_category, 
         category 
  WHERE  customer.customer_id = rental.customer_id 
         AND rental.inventory_id = inventory.inventory_id 
         AND inventory.film_id = film.film_id 
         AND film.film_id = film_category.film_id 
         AND film_category.category_id = category.category_id 
  GROUP  BY customer.customer_id, 
            category.category_id; 

SELECT category_id, 
       name 
FROM   category 
       NATURAL JOIN count_category c1 
WHERE  total = (SELECT Max(total) 
                FROM   count_category c2 
                WHERE  c1.customer_id = c2.customer_id); 

DROP VIEW count_category; 

DROP VIEW max_customer; 

DROP VIEW num_rentals;
