select film.film_id, film.title, film.release_year, film.length, language.name, film.description
from film, language
where film.title like '' and
	language.language_id=film.language_id;
