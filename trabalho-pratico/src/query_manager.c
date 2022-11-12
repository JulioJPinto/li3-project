#include "query_manager.h"

#include <stdlib.h>

typedef void (*QueryFunction)(Catalog *, FILE *, char **);

static const QueryFunction query_functions[] = {
        // TODO: Add query functions here
};

static const size_t query_functions_size = sizeof(query_functions) / sizeof(QueryFunction);

void execute_query(Catalog *catalog, FILE *output, char *query) {
    char **args = g_strsplit(query, " ", 0);

    char *query_id_str_left;
    int query_id = (int) strtol(args[0], &query_id_str_left, 10);

    if (*query_id_str_left != '\0') {
        fprintf(output, "Invalid query id: %s\n", args[0]);
        return;
    }

    run_query(catalog, output, query_id, args + 1);

    g_strfreev(args);
}

void run_query(Catalog *catalog, FILE *output, int query_id, char **args) {
    if (query_id <= 0 || query_id > (int) query_functions_size) {
        fprintf(output, "Query %d not implemented\n", query_id);
        return;
    }
    query_functions[query_id - 1](catalog, output, args);
}