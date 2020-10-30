CREATE VIEW max_language 
AS 
  SELECT language_id, 
         Count(*) AS total 
  FROM   film 
  GROUP  BY language_id; 

SELECT name 
FROM   language 
       NATURAL JOIN max_language 
WHERE  total = (SELECT Max(total) 
                FROM   max_language); 

DROP VIEW max_language; 
