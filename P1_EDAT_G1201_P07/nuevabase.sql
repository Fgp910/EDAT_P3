CREATE TYPE public.mpaa_rating AS ENUM (
    'G',
    'PG',
    'PG-13',
    'R',
    'NC-17'
);

CREATE TABLE public.category (
    category_id serial PRIMARY KEY,
    name character varying(25) NOT NULL,
    last_update timestamp without time zone DEFAULT now() NOT NULL
);

CREATE TABLE public.actor (
    actor_id serial PRIMARY KEY,
    first_name character varying(45) NOT NULL,
    last_name character varying(45) NOT NULL,
    last_update timestamp without time zone DEFAULT now() NOT NULL
);

CREATE TABLE public.language (
    language_id serial PRIMARY KEY,
    name character(20) NOT NULL,
    last_update timestamp without time zone DEFAULT now() NOT NULL
);

CREATE TABLE public.country (
    country_id serial PRIMARY KEY,
    country character varying(50) NOT NULL,
    last_update timestamp without time zone DEFAULT now() NOT NULL
);

CREATE TABLE public.city (
    city_id serial PRIMARY KEY,
    city character varying(50) NOT NULL,
    country_id serial references public.country(country_id),
    last_update timestamp without time zone DEFAULT now() NOT NULL
);

CREATE TABLE public.film (
    film_id serial PRIMARY KEY,
    title character varying(255) NOT NULL,
    description text,
    release_year integer CHECK (((release_year >= 1901) AND (release_year <= 2155))),
    language_id serial references public.language(language_id),
    rental_duration smallint DEFAULT 3 NOT NULL,
    rental_rate numeric(4,2) DEFAULT 4.99 NOT NULL,
    length smallint,
    replacement_cost numeric(5,2) DEFAULT 19.99 NOT NULL,
    rating public.mpaa_rating DEFAULT 'G'::public.mpaa_rating,
    last_update timestamp without time zone DEFAULT now() NOT NULL,
    special_features text[],
    fulltext tsvector NOT NULL
);

CREATE TABLE public.film_category (
    film_id serial references public.film(film_id),
    category_id serial references public.category(category_id),
    PRIMARY KEY (film_id, category_id),
    last_update timestamp without time zone DEFAULT now() NOT NULL
);

CREATE TABLE public.film_actor (
    film_id serial references public.film(film_id),
    actor_id serial references public.actor(actor_id),
    PRIMARY KEY (film_id, actor_id),
    last_update timestamp without time zone DEFAULT now() NOT NULL
);

CREATE TABLE public.inventory (
    inventory_id serial PRIMARY KEY,
    film_id smallint NOT NULL,
    store_id smallint NOT NULL,
    last_update timestamp without time zone DEFAULT now() NOT NULL
);

CREATE TABLE public.address (
    address_id serial PRIMARY KEY,
    address character varying(50) NOT NULL,
    address2 character varying(50),
    district character varying(20) NOT NULL,
    city_id serial references public.city(city_id),
    postal_code character varying(10),
    phone character varying(20) NOT NULL,
    last_update timestamp without time zone DEFAULT now() NOT NULL
);

CREATE TABLE public.customer (
    customer_id serial PRIMARY KEY,
    store_id smallint NOT NULL,
    first_name character varying(45) NOT NULL,
    last_name character varying(45) NOT NULL,
    email character varying(50),
    address_id serial references public.address(address_id),
    activebool boolean DEFAULT true NOT NULL,
    create_date date DEFAULT ('now'::text)::date NOT NULL,
    last_update timestamp without time zone DEFAULT now(),
    active integer
);

CREATE TABLE public.staff (
    staff_id serial PRIMARY KEY,
    first_name character varying(45) NOT NULL,
    last_name character varying(45) NOT NULL,
    address_id serial references public.address(address_id),
    email character varying(50),
    store_id smallint NOT NULL,
    active boolean DEFAULT true NOT NULL,
    username character varying(16) NOT NULL,
    password character varying(40),
    last_update timestamp without time zone DEFAULT now() NOT NULL,
    picture bytea
);

CREATE TABLE public.rental (
    rental_id serial PRIMARY KEY,
    rental_date timestamp without time zone NOT NULL,
    inventory_id serial references public.inventory(inventory_id),
    customer_id serial references public.customer(customer_id),
    return_date timestamp without time zone,
    staff_id serial references public.staff(staff_id),
    last_update timestamp without time zone DEFAULT now() NOT NULL
);

CREATE TABLE public.store (
    store_id serial PRIMARY KEY,
    manager_staff_id serial references public.staff(staff_id),
    address_id serial references public.address(address_id),
    last_update timestamp without time zone DEFAULT now() NOT NULL
);

CREATE TABLE public.payment (
    payment_id serial PRIMARY KEY,
    customer_id serial references public.customer(customer_id),
    staff_id serial references public.staff(staff_id),
    rental_id serial references public.rental(rental_id),
    amount numeric(5,2) NOT NULL,
    payment_date timestamp without time zone NOT NULL
);

CREATE TABLE public.staff_store (
    staff_id serial references public.staff(staff_id),
    store_id serial references public.store(store_id),
    PRIMARY KEY (staff_id, store_id)
);
