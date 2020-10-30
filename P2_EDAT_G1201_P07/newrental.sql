select  distinct inventory.inventory_id
from customer, film, staff, store, inventory, rental
where customer.customer_id=1 and
	film.film_id=1 and
	staff.staff_id=1 and
	store.store_id=1 and
	store.store_id=inventory.store_id and
	inventory.film_id=1 and
	inventory.inventory_id not in (select inventory_id
					from rental
					where return_date>CURRENT_DATE)
order by inventory.inventory_id limit 1;
	
