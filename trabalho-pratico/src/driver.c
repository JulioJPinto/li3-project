#include <glib.h>

#include "struct_util.h"
#include "driver.h"

struct driver {
    int id;
    GString name;
    Date birth_date;
    Gender gender;
    CarClass car_class;
    GString license_plate;
    GString city;
    Date account_creation_date;
    AccountStatus account_status;
};

Driver *create_driver(int id, GString name, Date birth_date, Gender gender, CarClass car_class, GString license_plate,
                      GString city, Date account_creation_date, AccountStatus account_status) {
    Driver *driver = malloc(sizeof(Driver));
    driver->id = id;
    driver->name = name;
    driver->birth_date = birth_date;
    driver->gender = gender;
    driver->car_class = car_class;
    driver->license_plate = license_plate;
    driver->city = city;
    driver->account_creation_date = account_creation_date;
    driver->account_status = account_status;

    return NULL;
}

int driver_get_id(Driver *driver) {
    return driver->id;
}

void free_driver(Driver *driver) {
    g_string_free(&driver->name, TRUE);
    g_string_free(&driver->license_plate, TRUE);
    g_string_free(&driver->city, TRUE);
    free(driver);
}
