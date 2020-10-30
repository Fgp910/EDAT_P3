CREATE VIEW cuenta 
AS 
  SELECT film.language_id, 
         Count(*) AS total 
  FROM   rental, 
         inventory, 
         film, 
         language 
  WHERE  rental.inventory_id = inventory.inventory_id 
         AND inventory.film_id = film.film_id 
         AND film.language_id = language.language_id 
  GROUP  BY film.language_id; 

SELECT language.name 
FROM   language 
       NATURAL JOIN cuenta 
WHERE  total = (SELECT Max(total) 
                FROM   cuenta); 

DROP VIEW cuenta; 
