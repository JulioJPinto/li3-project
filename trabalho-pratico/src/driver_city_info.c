#include "driver_city_info.h"

#include <glib.h>

/**
 * Struct that holds the information of a driver in a city.
 */
struct DriverCityInfo {
    int id;
    u_int16_t accumulated_score;
    u_int16_t amount_rides;
};

DriverCityInfo *create_driver_city_info(int id) {
    DriverCityInfo *driver_by_city = malloc(sizeof(struct DriverCityInfo));
    driver_by_city->id = id;
    driver_by_city->accumulated_score = 0;
    driver_by_city->amount_rides = 0;
    return driver_by_city;
}

int driver_city_info_get_id(DriverCityInfo *driver_city_info) {
    return driver_city_info->id;
}

void driver_city_info_register_ride_score(DriverCityInfo *driver_city_info, int score) {
    driver_city_info->accumulated_score += score;
    driver_city_info->amount_rides++;
}

double driver_city_info_get_average_score(DriverCityInfo *driver_city_info) {
    return (double) driver_city_info->accumulated_score / (double) driver_city_info->amount_rides;
}

void free_driver_city_info_voidp(void *driver_city_info) {
    free(driver_city_info);
}

int compare_driver_city_infos_by_average_score(const void *a_driver_city_info, const void *b_driver_city_info) {
    DriverCityInfo *a_dci = *(DriverCityInfo **) a_driver_city_info;
    DriverCityInfo *b_dci = *(DriverCityInfo **) b_driver_city_info;

    double score_a = driver_city_info_get_average_score(a_dci);
    double score_b = driver_city_info_get_average_score(b_dci);

    int by_score = (score_b > score_a) - (score_b < score_a);
    if (by_score != 0) {
        return by_score;
    }

    return driver_city_info_get_id(b_dci) - driver_city_info_get_id(a_dci);
}
