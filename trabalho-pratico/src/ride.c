#include "ride.h"

#include <glib.h>
#include "struct_util.h"
#include "string_util.h"

/**
 * Struct that represents a ride.
 */
struct Ride {
    double price;
    double tip;
    Date date;
    Date user_account_creation_date;
    Date driver_account_creation_date;
    int32_t user_id;
    int32_t id;
    int32_t driver_id;
    u_int8_t distance;
    u_int8_t city_id;
    u_int8_t score_user;
    u_int8_t score_driver;
};

Ride *create_ride(int id, Date date, int driver_id, int city_id, int distance, int score_user, int score_driver, double tip) {
    Ride *ride = malloc(sizeof(Ride));

    ride->id = id;
    ride->date = date;
    ride->driver_id = driver_id;
    ride->city_id = city_id;
    ride->distance = distance;
    ride->score_user = score_user;
    ride->score_driver = score_driver;
    ride->tip = tip;

    return ride;
}

Ride *parse_line_ride(TokenIterator *line_iterator) {
    return parse_line_ride_detailed(line_iterator, NULL, NULL);
}

Ride *parse_line_ride_detailed(TokenIterator *line_iterator, char **parsed_city, char **parsed_user_username) {
    char *id_string = token_iterator_next(line_iterator);
    if (IS_EMPTY(id_string)) return NULL;
    int id = parse_int_unsafe(id_string);

    char *date_string = token_iterator_next(line_iterator);
    Date date = parse_date(date_string);
    if (!is_date_valid(date)) return NULL;

    char *driver_string = token_iterator_next(line_iterator);
    if (IS_EMPTY(driver_string)) return NULL;
    int driver_id = parse_int_unsafe(driver_string);

    char *user = token_iterator_next(line_iterator);
    if (IS_EMPTY(user)) return NULL;

    char *city = token_iterator_next(line_iterator);
    if (IS_EMPTY(city)) return NULL;
    int city_id = 0;

    int error = 0;

    char *distance_string = token_iterator_next(line_iterator);
    if (IS_EMPTY(distance_string)) return NULL;
    int distance = parse_int_safe(distance_string, &error);

    char *user_score_string = token_iterator_next(line_iterator);
    if (IS_EMPTY(user_score_string)) return NULL;
    int user_score = parse_int_safe(user_score_string, &error);

    char *driver_score_string = token_iterator_next(line_iterator);
    if (IS_EMPTY(driver_score_string)) return NULL;
    int driver_score = parse_int_safe(driver_score_string, &error);

    char *tip_string = token_iterator_next(line_iterator);
    if (IS_EMPTY(tip_string)) return NULL;
    double tip = parse_double_safe(tip_string, &error);

    if (error || distance < 1 || user_score < 1 || driver_score < 1 || tip < 0 || tip != tip) return NULL;

    if (parsed_city) *parsed_city = city;
    if (parsed_user_username) *parsed_user_username = user;

    return create_ride(id, date, driver_id, city_id, distance, user_score, driver_score, tip);
}

void free_ride(Ride *ride) {
    free(ride);
}

void ride_set_city_id(Ride *ride, int city_id) {
    ride->city_id = city_id;
}

int ride_get_driver_id(Ride *ride) {
    return ride->driver_id;
}

int ride_get_id(Ride *ride) {
    return ride->id;
}

Date ride_get_date(Ride *ride) {
    return ride->date;
}

int ride_get_user_id(Ride *ride) {
    return ride->user_id;
}

void ride_set_user_id(Ride *ride, int user_id) {
    ride->user_id = user_id;
}

int ride_get_distance(Ride *ride) {
    return ride->distance;
}

int ride_get_score_user(Ride *ride) {
    return ride->score_user;
}

int ride_get_score_driver(Ride *ride) {
    return ride->score_driver;
}

double ride_get_tip(Ride *ride) {
    return ride->tip;
}

int ride_get_city_id(Ride *ride) {
    return ride->city_id;
}

void ride_set_price(Ride *ride, double price) {
    ride->price = price;
}

double ride_get_price(Ride *ride) {
    return ride->price;
}

void ride_set_user_account_creation_date(Ride *ride, Date date) {
    ride->user_account_creation_date = date;
}

Date ride_get_user_account_creation_date(Ride *ride) {
    return ride->user_account_creation_date;
}

void ride_set_driver_account_creation_date(Ride *ride, Date date) {
    ride->driver_account_creation_date = date;
}

Date ride_get_driver_account_creation_date(Ride *ride) {
    return ride->driver_account_creation_date;
}

int compare_rides_by_date(const void *a, const void *b) {
    Ride *a_ride = *(Ride **) a;
    Ride *b_ride = *(Ride **) b;

    return date_compare(ride_get_date(a_ride), ride_get_date(b_ride));
}

int compare_rides_by_distance(const void *a, const void *b) {
    Ride *a_ride = *(Ride **) a;
    Ride *b_ride = *(Ride **) b;

    int by_distance = ride_get_distance(b_ride) - ride_get_distance(a_ride);
    if (by_distance != 0) {
        return by_distance;
    }
    int by_date = date_compare(ride_get_date(b_ride), ride_get_date(a_ride));
    if (by_date != 0) {
        return by_date;
    }

    return ride_get_id(b_ride) - ride_get_id(a_ride);
}

int compare_ride_by_driver_and_user_account_creation_date(const void *a, const void *b) {
    Ride *ride_a = *(Ride **) a;
    Ride *ride_b = *(Ride **) b;

    Date driver_a_account_creation_date = ride_get_driver_account_creation_date(ride_a);
    Date driver_b_account_creation_date = ride_get_driver_account_creation_date(ride_b);

    int by_account_creation_driver = date_compare(driver_a_account_creation_date, driver_b_account_creation_date);
    if (by_account_creation_driver != 0) {
        return by_account_creation_driver;
    }

    Date user_a_account_creation_date = ride_get_user_account_creation_date(ride_a);
    Date user_b_account_creation_date = ride_get_user_account_creation_date(ride_b);

    int by_account_creation_user = date_compare(user_a_account_creation_date, user_b_account_creation_date);
    if (by_account_creation_user != 0) {
        return by_account_creation_user;
    }

    return ride_get_id(ride_a) - ride_get_id(ride_b);
}
