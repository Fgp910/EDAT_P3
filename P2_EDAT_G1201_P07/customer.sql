select customer.customer_id, customer.first_name, customer.last_name, customer.create_date, address.address, city.city, country.country
from customer, address, city, country
where (first_name = 'John' or last_name = 'Hunt') and
	customer.address_id = address.address_id and
	address.city_id=city.city_id and
	city.country_id=country.country_id;
