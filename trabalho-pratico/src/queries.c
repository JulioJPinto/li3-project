#include "queries.h"

#include "logger.h"

#define UNUSED(x) (void) (x)

/**
 * Same as fprintf but only print to stream if DEBUG macro is defined.
 */
void write_output_debug(OutputWriter *stream, const char *format, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, format);
    writer_write_output_token_end(stream, format, args);
    va_end(args);
#else
    UNUSED(stream);
    UNUSED(format);
#endif
}

/**
 * Query 1 for users
 */
void execute_query_find_user_by_name(Catalog *catalog, OutputWriter *output, char *username) {
    User *user = catalog_get_user_by_username(catalog, username);

    if (user == NULL) {
        write_output_debug(output, "User %s not found", username);
        return;
    }

    if (user_get_account_status(user) == INACTIVE) {
        write_output_debug(output, "User %s is inactive", username);
        return;
    }

    char *name = user_get_name(user);
    const char *gender = convert_gender_to_string(user_get_gender(user));
    int age = get_age(user_get_birthdate(user));
    double average_score = user_get_average_score(user);
    int number_of_rides = user_get_number_of_rides(user);
    double total_spent = user_get_total_spent(user);

    writer_write_output_token(output, "%s", name);
    writer_write_output_token(output, "%s", gender);
    writer_write_output_token(output, "%d", age);
    writer_write_output_token(output, "%.3f", average_score);
    writer_write_output_token(output, "%d", number_of_rides);
    writer_write_output_token_end(output, "%.3f", total_spent);

    free(name);
}

/**
 * Query 1 for driver
 */
void execute_query_find_driver_by_id(Catalog *catalog, OutputWriter *output, int id) {
    Driver *driver = catalog_get_driver(catalog, id);

    if (driver == NULL) {
        write_output_debug(output, "Driver %d not found", id);
        return;
    }

    if (driver_get_account_status(driver) == INACTIVE) {
        write_output_debug(output, "Driver %d is inactive", id);
        return;
    }

    char *name = driver_get_name(driver);
    const char *gender = convert_gender_to_string(driver_get_gender(driver));
    int age = get_age(driver_get_birthdate(driver));
    double average_score = driver_get_average_score(driver);
    int number_of_rides = driver_get_number_of_rides(driver);
    double total_spent = driver_get_total_earned(driver);

    writer_write_output_token(output, "%s", name);
    writer_write_output_token(output, "%s", gender);
    writer_write_output_token(output, "%d", age);
    writer_write_output_token(output, "%.3f", average_score);
    writer_write_output_token(output, "%d", number_of_rides);
    writer_write_output_token_end(output, "%.3f", total_spent);

    free(name);
}

/**
 * Query 1
 */
void execute_query_find_user_or_driver_by_name_or_id(Catalog *catalog, OutputWriter *output, char **args) {
    char *id_or_username = args[0];

    int error = 0;
    int id = parse_int_safe(id_or_username, &error);
    if (error) {
        execute_query_find_user_by_name(catalog, output, id_or_username);
    } else {
        execute_query_find_driver_by_id(catalog, output, id);
    }
}

/**
 * Query 2
 */
void execute_query_top_n_drivers(Catalog *catalog, OutputWriter *output, char **args) {
    int error = 0;
    int n = parse_int_safe(args[0], &error);
    if (error) {
        LOG_WARNING_VA("Couldn't parse number of size of top drivers '%s'", args[0]);
        return;
    }

    GPtrArray *result = g_ptr_array_sized_new(n);

    int result_size = query_2_catalog_get_top_drivers_with_best_score(catalog, n, result);

    for (int i = 0; i < result_size; i++) {
        Driver *driver = g_ptr_array_index(result, i);

        int id = driver_get_id(driver);
        char *name = driver_get_name(driver);
        double average_score = driver_get_average_score(driver);

        writer_write_output_token(output, "%012d", id);
        writer_write_output_token(output, "%s", name);
        writer_write_output_token_end(output, "%.3f", average_score);

        free(name);
    }

    g_ptr_array_free(result, TRUE);
}

/**
 * Query 3
 */
void execute_query_longest_n_total_distance(Catalog *catalog, OutputWriter *output, char **args) {
    int error = 0;
    int n = parse_int_safe(args[0], &error);

    if (error) {
        LOG_WARNING_VA("Couldn't parse number of size of top users '%s'", args[0]);
        return;
    }

    GPtrArray *result = g_ptr_array_sized_new(n);

    int result_size = query_3_catalog_get_top_users_with_longest_total_distance(catalog, n, result);

    for (int i = 0; i < result_size; i++) {
        User *user = g_ptr_array_index(result, i);

        char *username = user_get_username(user);
        char *name = user_get_name(user);
        int total_distance = user_get_total_distance(user);

        writer_write_output_token(output, "%s", username);
        writer_write_output_token(output, "%s", name);
        writer_write_output_token_end(output, "%d", total_distance);

        free(username);
        free(name);
    }

    g_ptr_array_free(result, TRUE);
}

/**
 * Query 4
 */
void execute_query_average_price_in_city(Catalog *catalog, OutputWriter *output, char **args) {
    char *city = args[0];

    int city_id = catalog_get_city_id(catalog, city);
    if (city_id == -1) {
        write_output_debug(output, "City %s not found", city);
        return;
    }

    double average_price = query_4_catalog_get_average_price_in_city(catalog, city_id);

    writer_write_output_token_end(output, "%.3f", average_price);
}

/**
 * Query 5
 */
void execute_query_average_price_in_date_range(Catalog *catalog, OutputWriter *output, char **args) {
    char *start_date_string = args[0];
    char *end_date_string = args[1];

    Date start_date = parse_date(start_date_string);
    Date end_date = parse_date(end_date_string);

    if (!is_date_valid(start_date)) {
        LOG_WARNING_VA("Couldn't parse start date '%s'. Use XX/XX/XXXX format.", start_date_string);
        return;
    }

    if (!is_date_valid(end_date)) {
        LOG_WARNING_VA("Couldn't parse end date '%s'. Use XX/XX/XXXX format.", end_date_string);
        return;
    }

    double average_price = query_5_catalog_get_average_price_in_date_range(catalog, start_date, end_date);

    if (average_price == -1) {
        write_output_debug(output, "No rides in date range");
        return;
    }

    writer_write_output_token_end(output, "%.3f", average_price);
}

/**
 * Query 6
 */
void execute_query_average_distance_in_city_in_date_range(Catalog *catalog, OutputWriter *output, char **args) {
    char *city = args[0];

    char *start_date_string = args[1];
    char *end_date_string = args[2];

    Date start_date = parse_date(start_date_string);
    Date end_date = parse_date(end_date_string);

    if (!is_date_valid(start_date)) {
        LOG_WARNING_VA("Couldn't parse start date '%s'. Use XX/XX/XXXX format.", start_date_string);
        return;
    }

    if (!is_date_valid(end_date)) {
        LOG_WARNING_VA("Couldn't parse end date '%s'. Use XX/XX/XXXX format.", end_date_string);
        return;
    }

    int city_id = catalog_get_city_id(catalog, city);
    if (city_id == -1) {
        write_output_debug(output, "City %s not found", city);
        return;
    }

    double average_distance = query_6_catalog_get_average_distance_in_city_by_date(catalog, start_date, end_date, city_id);

    if (average_distance == -1) {
        write_output_debug(output, "No rides in date range");
        return;
    }

    writer_write_output_token_end(output, "%.3f", average_distance);
}

/**
  * Query 7
  */
void execute_query_top_drivers_in_city_by_average_score(Catalog *catalog, OutputWriter *output, char **args) {
    int error = 0;
    int n = parse_int_safe(args[0], &error);
    if (error) {
        LOG_WARNING_VA("Couldn't parse number of drivers in city '%s'", args[0]);
        return;
    }

    char *city = args[1];

    int city_id = catalog_get_city_id(catalog, city);
    if (city_id == -1) {
        write_output_debug(output, "City %s not found", city);
        return;
    }

    GPtrArray *result = g_ptr_array_sized_new(n);
    int size = query_7_catalog_get_top_n_drivers_in_city(catalog, n, city_id, result);

    for (int i = 0; i < size; i++) {
        DriverCityInfo *driver_city_info = g_ptr_array_index(result, i);
        int id = driver_city_info_get_id(driver_city_info);
        Driver *driver = catalog_get_driver(catalog, id);

        char *name = driver_get_name(driver);
        double average_score = driver_city_info_get_average_score(driver_city_info);

        writer_write_output_token(output, "%012d", id);
        writer_write_output_token(output, "%s", name);
        writer_write_output_token_end(output, "%.3f", average_score);

        free(name);
    }

    g_ptr_array_free(result, TRUE);
}

/**
  * Query 8 
  */
void execute_query_rides_with_users_and_drivers_same_gender_by_account_creation_age(Catalog *catalog, OutputWriter *output, char **args) {
    char *gender_string = args[0];
    Gender gender = parse_gender(gender_string);

    int error = 0;
    int min_account_age = parse_int_safe(args[1], &error);
    if (error) {
        LOG_WARNING_VA("Couldn't parse number of minimum age '%s'", args[1]);
        return;
    }

    GPtrArray *result = g_ptr_array_new();
    query_8_catalog_get_rides_with_user_and_driver_with_same_gender_above_acc_age(catalog, result, gender, min_account_age);

    for (size_t i = 0; i < result->len; i++) {
        Ride *ride = g_ptr_array_index(result, i);

        int driver_id = ride_get_driver_id(ride);
        Driver *driver = catalog_get_driver(catalog, driver_id);
        char *driver_name = driver_get_name(driver);

        int user_id = ride_get_user_id(ride);
        User *user = catalog_get_user_by_user_id(catalog, user_id);
        char *user_username = user_get_username(user);
        char *user_name = user_get_name(user);

        writer_write_output_token(output, "%012d", driver_id);
        writer_write_output_token(output, "%s", driver_name);
        writer_write_output_token(output, "%s", user_username);
        writer_write_output_token_end(output, "%s", user_name);

        free(driver_name);
        free(user_username);
        free(user_name);
    }

    g_ptr_array_free(result, TRUE);
}

/**
 * Query 9
 */
void execute_query_passenger_that_gave_tip(Catalog *catalog, OutputWriter *output, char **args) {
    char *start_date_string = args[0];
    char *end_date_string = args[1];

    Date start_date = parse_date(start_date_string);
    Date end_date = parse_date(end_date_string);

    if (is_date_valid(start_date) == FALSE) {
        LOG_WARNING_VA("Couldn't parse start date '%s'. Use XX/XX/XXXX format.", start_date_string);
        return;
    }

    if (is_date_valid(end_date) == FALSE) {
        LOG_WARNING_VA("Couldn't parse end date '%s'. Use XX/XX/XXXX format.", end_date_string);
        return;
    }

    GPtrArray *result = g_ptr_array_new();
    query_9_catalog_get_passengers_that_gave_tip_in_date_range(catalog, result, start_date, end_date);

    for (size_t i = 0; i < result->len; i++) {
        Ride *ride = g_ptr_array_index(result, i);
        int id = ride_get_id(ride);
        Date date = ride_get_date(ride);
        int distance = ride_get_distance(ride);
        int city_id = ride_get_city_id(ride);
        char *city = catalog_get_city_name(catalog, city_id);
        double tip = ride_get_tip(ride);

        char *date_string = convert_date_to_string(date);

        writer_write_output_token(output, "%012d", id);
        writer_write_output_token(output, "%s", date_string);
        writer_write_output_token(output, "%d", distance);
        writer_write_output_token(output, "%s", city);
        writer_write_output_token_end(output, "%.3f", tip);

        free(date_string);
        free(city);
    }

    g_ptr_array_free(result, TRUE);
}
