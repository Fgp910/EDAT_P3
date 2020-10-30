select actor.first_name, actor.last_name
from actor, film_actor
where film_actor.actor_id=actor.actor_id and
	film_actor.film_id=;
