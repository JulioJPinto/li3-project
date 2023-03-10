#include "driver.h"

#include <glib.h>
#include "string_util.h"

/**
 * Struct that represents a driver.
 */
struct Driver {
    char *name;
    // char *license_plate;
    double total_earned;
    Date birthdate;
    Date last_ride_date;
    Date account_creation_date;
    int32_t id;
    uint16_t accumulated_score;
    uint8_t city_id;
    uint8_t rides_amount;
    AccountStatus account_status;
    Gender gender;
    CarClass car_class;
};

Driver *create_driver(int id, char *name, Date birth_date, Gender gender, CarClass car_class, const char *license_plate,
                      Date account_creation_date, AccountStatus account_status) {
    Driver *driver = malloc(sizeof(Driver));
    driver->id = id;
    driver->name = g_strdup(name);
    driver->birthdate = birth_date;
    driver->gender = gender;
    driver->car_class = car_class;
    (void) license_plate; // driver->license_plate = g_strdup(license_plate);
    driver->account_creation_date = account_creation_date;
    driver->account_status = account_status;

    driver->rides_amount = 0;
    driver->accumulated_score = 0;
    driver->total_earned = 0;
    driver->last_ride_date = (Date){0};

    return driver;
}

Driver *parse_line_driver(TokenIterator *line_iterator) {
    return parse_line_driver_detailed(line_iterator, NULL);
}

Driver *parse_line_driver_detailed(TokenIterator *line_iterator, char **parsed_city) {
    char *id_string = token_iterator_next(line_iterator);
    if (IS_EMPTY(id_string)) return NULL;

    int id = parse_int_unsafe(id_string);

    char *name = token_iterator_next(line_iterator);
    if (IS_EMPTY(name)) return NULL;

    char *date_string = token_iterator_next(line_iterator);
    Date date = parse_date(date_string);
    if (!is_date_valid(date)) return NULL;

    char *gender_string = token_iterator_next(line_iterator);
    if (IS_EMPTY(gender_string)) return NULL;
    Gender gender = parse_gender(gender_string);

    char *car_class_string = token_iterator_next(line_iterator);
    CarClass car_class = parse_car_class(car_class_string);
    if (car_class == INVALID_CAR_CLASS) return NULL;

    char *license_plate = token_iterator_next(line_iterator);
    if (IS_EMPTY(license_plate)) return NULL;

    char *city = token_iterator_next(line_iterator);
    if (IS_EMPTY(city)) return NULL;

    char *creation_date_string = token_iterator_next(line_iterator);
    Date creation_date = parse_date(creation_date_string);
    if (!is_date_valid(creation_date)) return NULL;

    char *acc_status_string = token_iterator_next(line_iterator);

    AccountStatus acc_status = parse_acc_status(acc_status_string);
    if (acc_status == INVALID_ACCOUNT_STATUS) return NULL;

    if (parsed_city) *parsed_city = city;

    return create_driver(id, name, date, gender, car_class, license_plate, creation_date, acc_status);
}

void driver_set_city_id(Driver *driver, int city_id) {
    driver->city_id = city_id;
}

int driver_get_id(Driver *driver) {
    return driver->id;
}

char *driver_get_name(Driver *driver) {
    return g_strdup(driver->name);
}

int driver_get_city_id(Driver *driver) {
    return driver->city_id;
}

Date driver_get_birthdate(Driver *driver) {
    return driver->birthdate;
}

Gender driver_get_gender(Driver *driver) {
    return driver->gender;
}

CarClass driver_get_car_class(Driver *driver) {
    return driver->car_class;
}

Date driver_get_account_creation_date(Driver *driver) {
    return driver->account_creation_date;
}

AccountStatus driver_get_account_status(Driver *driver) {
    return driver->account_status;
}

Date driver_get_last_ride_date(Driver *driver) {
    return driver->last_ride_date;
}

void driver_increment_number_of_rides(Driver *driver) {
    driver->rides_amount++;
}

void driver_add_score(Driver *driver, int score) {
    driver->accumulated_score += score;
}

void driver_add_earned(Driver *driver, double earned) {
    driver->total_earned += earned;
}

int driver_get_number_of_rides(Driver *driver) {
    return driver->rides_amount;
}

double driver_get_average_score(Driver *driver) {
    return (double) driver->accumulated_score / (double) driver->rides_amount;
}

double driver_get_total_earned(Driver *driver) {
    return driver->total_earned;
}

void driver_register_ride_date(Driver *driver, Date date) {
    if (date_compare(date, driver->last_ride_date) > 0) {
        driver->last_ride_date = date;
    }
}

void free_driver(void *driver) {
    Driver *actual_driver = (Driver *) driver;
    free(actual_driver->name);
    // free(actual_driver->license_plate);
    free(actual_driver);
}

int compare_drivers_by_score(const void *a, const void *b) {
    Driver *a_driver = *((Driver **) a);
    Driver *b_driver = *((Driver **) b);

    int by_activeness = (int) driver_get_account_status(a_driver) - (int) driver_get_account_status(b_driver);
    if (by_activeness != 0) {
        return by_activeness;
    }

    double average_score_a = driver_get_average_score(a_driver);
    double average_score_b = driver_get_average_score(b_driver);

    int by_score = (average_score_b > average_score_a) - (average_score_b < average_score_a);
    if (by_score != 0) {
        return by_score;
    }

    int by_last_ride = date_compare(driver_get_last_ride_date(b_driver), driver_get_last_ride_date(a_driver));
    if (by_last_ride != 0) {
        return by_last_ride;
    }

    return driver_get_id(a_driver) - driver_get_id(b_driver);
}
