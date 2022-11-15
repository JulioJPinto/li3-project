#include "catalog.h"
#include "price_util.h"

typedef struct Catalog {
    GPtrArray *users_array;
    GPtrArray *drivers_array;
    GPtrArray *rides_array;

    GHashTable *user_from_username_hashtable;
    GHashTable *driver_from_id_hashtable;
} Catalog;

void glib_wrapper_free_user(gpointer user) {
    free_user(user);
}

void glib_wrapper_free_driver(gpointer driver) {
    free_driver(driver);
}

void glib_wrapper_free_ride(gpointer ride) {
    free_ride(ride);
}

Catalog *create_catalog(void) {
    Catalog *catalog = malloc(sizeof(struct Catalog));

    catalog->users_array = g_ptr_array_new_with_free_func(glib_wrapper_free_user);
    catalog->drivers_array = g_ptr_array_new_with_free_func(glib_wrapper_free_driver);
    catalog->rides_array = g_ptr_array_new_with_free_func(glib_wrapper_free_ride);

    catalog->user_from_username_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
    catalog->driver_from_id_hashtable = g_hash_table_new_full(g_int_hash, g_int_equal, free, NULL);

    return catalog;
}

void register_user(Catalog *catalog, User *user) {
    g_ptr_array_add(catalog->users_array, user);
    g_hash_table_insert(catalog->user_from_username_hashtable, user_get_username(user), user);
}

void register_driver(Catalog *catalog, Driver *driver) {
    g_ptr_array_add(catalog->drivers_array, driver);

    int *key = malloc(sizeof(int));
    *key = driver_get_id(driver);
    // No need to free the key, it will be freed when the Driver is freed

    g_hash_table_insert(catalog->driver_from_id_hashtable, key, driver);
}

void register_ride(Catalog *catalog, Ride *ride) {
    g_ptr_array_add(catalog->rides_array, ride);

    Driver *driver = catalog_get_driver(catalog, ride_get_driver_id(ride));
    double total_price = ride_get_tip(ride) + compute_price(ride_get_distance(ride), driver_get_car_class(driver));

    driver_increment_number_of_rides(driver);
    driver_add_score(driver, ride_get_score_driver(ride));
    driver_add_earned(driver, total_price);
    driver_register_last_ride_date(driver, ride_get_date(ride));

    User *user = catalog_get_user(catalog, ride_get_user_username(ride));
    user_increment_number_of_rides(user);
    user_add_score(user, ride_get_score_user(ride));
    user_add_spent(user, total_price);
}

User *catalog_get_user(Catalog *catalog, char *username) {
    return g_hash_table_lookup(catalog->user_from_username_hashtable, username);
}

Driver *catalog_get_driver(Catalog *catalog, int id) {
    return g_hash_table_lookup(catalog->driver_from_id_hashtable, &id);
}

void catalog_get_top_n_drivers(Catalog *catalog, int n, GPtrArray *result) {
    for (int i = 0; i < n; i++) {
        g_ptr_array_add(result, g_ptr_array_index(catalog->drivers_array, i));
    }
}

int compare_driver_by_id(Driver *a, Driver *b) {
    return driver_get_id(a) - driver_get_id(b);
}

int compare_driver_by_last_ride(Driver *a, Driver *b) {
    return date_compare(driver_get_last_ride_date(a), driver_get_last_ride_date(b));
}

int compare_driver_by_score(Driver *a, Driver *b) {
    double average_score_a = driver_get_average_score(a);
    double average_score_b = driver_get_average_score(b);

    return (average_score_a > average_score_b) - (average_score_a < average_score_b);
}

int glib_wrapper_sort_drivers(gconstpointer a, gconstpointer b) {
    Driver *a_driver = *((Driver **) a);
    Driver *b_driver = *((Driver **) b);

    int by_score = compare_driver_by_score(b_driver, a_driver);
    if (by_score != 0) {
        return by_score;
    }

    int by_last_ride = compare_driver_by_last_ride(b_driver, a_driver);
    if (by_last_ride != 0) {
        return by_last_ride;
    }

    int by_id = compare_driver_by_id(b_driver, a_driver);
    return by_id;
}

void notify_stop_registering(Catalog *catalog) {
    g_ptr_array_sort(catalog->drivers_array, glib_wrapper_sort_drivers);
}

void free_catalog(Catalog *catalog) {
    g_ptr_array_free(catalog->users_array, TRUE);
    g_ptr_array_free(catalog->drivers_array, TRUE);
    g_ptr_array_free(catalog->rides_array, TRUE);

    g_hash_table_destroy(catalog->user_from_username_hashtable);
    g_hash_table_destroy(catalog->driver_from_id_hashtable);
    free(catalog);
}
