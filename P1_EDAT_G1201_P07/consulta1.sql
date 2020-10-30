SELECT Extract (year FROM rental_date) AS year, 
       Count(*) 
FROM   rental 
GROUP  BY year;
