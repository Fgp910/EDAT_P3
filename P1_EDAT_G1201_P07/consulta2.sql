CREATE VIEW num_rentals 
AS 
  SELECT customer_id, 
         Count(*) AS num 
  FROM   rental 
  GROUP  BY customer_id; 

SELECT customer_id, 
       first_name, 
       last_name, 
       num 
FROM   num_rentals 
       NATURAL JOIN customer 
WHERE  num = (SELECT Max(num) 
              FROM   num_rentals); 

DROP VIEW num_rentals; 
