#pragma once
#ifndef LI3_QUERIES_H
#define LI3_QUERIES_H

#include <stdio.h>
#include "catalog.h"

/**
 * Query function format for queries to be saved and called in the query manager.
 * Catalog*: The catalog to run the query on.
 * FILE*: The output stream to write the result to.
 * char**: The arguments (split by spaces) to the query.
 */
typedef void (*QueryFunction)(Catalog *, FILE *, char **);

/**
 * Executes the query number 1.
 */
void execute_query_find_user_or_driver_by_name_or_id(Catalog *catalog, FILE *output, char **args);

/**
 * Executes the query number 2.
 */
void execute_query_top_n_drivers(Catalog *catalog, FILE *output, char **args);

/**
 * Executes the query number 3.
 */
void execute_query_longest_n_total_distance(Catalog *catalog, FILE *output, char **args);

/**
 * Executes the query number 4.
 */
void execute_query_average_price_in_city(Catalog *catalog, FILE *output, char **args);

/**
 * Executes the query number 5.
 */
void execute_query_average_price_in_date_range(Catalog *catalog, FILE *output, char **args);

#endif //LI3_QUERIES_H
