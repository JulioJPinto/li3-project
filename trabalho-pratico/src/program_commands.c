#include "program_commands.h"

#include "program.h"
#include "logger.h"
#include "output_writer.h"
#include "terminal_controller.h"
#include "file_util.h"
#include "struct_util.h"
#include "string_util.h"

/**
 * Struct that holds information about a program command.
 */
typedef struct ProgramCommand {
    char *name;
    char *description;
    void (*function)(Program *program, char **args, int arg_size);
} ProgramCommand;

/**
 * `file` command implementation.
 * Runs all the queries from the given file.
 */
void program_run_queries_from_file_command(Program *program, char **args, int arg_size) {
    if (arg_size < 2) {
        LOG_WARNING("Usage: 'file <file_path>'");
        return;
    }

    char *input_file_path = args[1];

    if (!program_run_queries_from_file(program, input_file_path)) {
        LOG_WARNING_VA("Failed to run queries from file '%s'", input_file_path);
    }
}

void program_run_help_command(Program *program, char **args, int arg_size);

/**
 * `exit` command implementation.
 * Exits the program.
 */
void program_exit_command(Program *program, char **args, int arg_size) {
    (void) args;
    (void) arg_size;

    program_set_state(program, PROGRAM_STATE_EXITING);
}

/**
 * `restart` command implementation.
 * Restarts the program (asks for a new catalog).
 */
void program_restart_command(Program *program, char **args, int arg_size) {
    (void) args;
    (void) arg_size;

    program_set_state(program, PROGRAM_STATE_EXITING);
    program_set_should_exit(program, FALSE);
}

/**
 * `clear` command implementation.
 * Clears the terminal.
 */
void program_clear_command(Program *program, char **args, int arg_size) {
    (void) args;
    (void) arg_size;
    (void) program;

    int r = system("clear");
    (void) r;
}

/**
 * `list-output` command implementation.
 * Lists the output files in the output folder.
 */
void program_list_output_command(Program *program, char **args, int arg_size) {
    (void) args;
    (void) arg_size;
    (void) program;

    int r = system("ls " OUTPUT_FOLDER PATH_SEPARATOR);
    (void) r;
}

/**
 * `cat` command implementation.
 * Prints the content of the given file.
 * The file can be a path to a file, an output file or an output file id.
 */
void program_cat_files_command(Program *program, char **args, int arg_size) {
    (void) program;

    if (arg_size < 2) {
        LOG_WARNING("Usage: 'cat <file|output_file|output_file_id>'");
        return;
    }

    char *input_file_path = args[1];

    int is_not_int = 0;     //value to check if the input_file_path is an ID
    parse_int_safe(input_file_path, &is_not_int);

    if(is_not_int && !string_ends_with(input_file_path, ".txt")) {
        LOG_WARNING_VA("Can't open '%s' file, can only open .txt files", input_file_path);
        return;
    } 

    GPtrArray *output = g_ptr_array_new_with_free_func(free);
    OutputWriter *output_writer = create_array_of_semicolon_strings_output_writer(output);

    FILE *input_file = fopen(input_file_path, "r");
    if (input_file == NULL) {
        char *file_path = g_strdup_printf(OUTPUT_FOLDER PATH_SEPARATOR "%s", input_file_path);
        input_file = fopen(file_path, "r");
        free(file_path);
    }
    if (input_file == NULL) {
        char *file_path = g_strdup_printf(OUTPUT_FOLDER PATH_SEPARATOR OUTPUT_FILE_NAME_S, input_file_path);
        input_file = fopen(file_path, "r");
        free(file_path);
    }
    if (input_file == NULL) {
        LOG_WARNING_VA("Could not find file, output_writer file or output_writer file id with name '%s'", input_file_path);
        return;
    }

    char line_buffer[1024]; // We want to limit the line size to 1024 characters, otherwise we might run out of memory
    while (fgets(line_buffer, 1024, input_file) != NULL) {
        writer_write_output_token_end(output_writer, line_buffer);
    }

    print_content(output);

    close_output_writer(output_writer);
    g_ptr_array_free(output, TRUE);
    fclose(input_file);
}

/**
 * Array that holds all the available program commands.
 */
const ProgramCommand program_commands[] = {
        {"cat", "Prints the output from a result or input file", program_cat_files_command},
        {"clear", "Cleans everything in the terminal", program_clear_command},
        {"exit", "Exits the program", program_exit_command},
        {"file", "Runs all the queries from a file", program_run_queries_from_file_command},
        {"help", "Shows this help message", program_run_help_command},
        {"list-output", "Shows the output files available", program_list_output_command},
        {"restart", "Restarts the program", program_restart_command},
};

/**
 * Size of the `program_commands` array.
 */
const int program_commands_size = sizeof(program_commands) / sizeof(ProgramCommand);

/**
 * `help` command implementation.
 * Prints all the available commands and their description.
 */
void program_run_help_command(Program *program, char **args, int arg_size) {
    (void) program;
    (void) args;
    (void) arg_size;

    log_info(TERMINAL_YELLOW_BOLD "Available commands:\n");
    log_info(TERMINAL_YELLOW "  <query_id> <values>" TERMINAL_RESET " - Runs a query\n");

    for (int i = 0; i < program_commands_size; i++) {
        log_info(TERMINAL_YELLOW "  %s" TERMINAL_RESET " - %s\n", program_commands[i].name, program_commands[i].description);
    }
}

const ProgramCommand *get_program_command(char *command_name) {
    for (int i = 0; i < program_commands_size; i++) {
        if (strcmp(program_commands[i].name, command_name) == 0) {
            return &program_commands[i];
        }
    }

    return NULL;
}

void run_program_command(const ProgramCommand *command, Program *program, char **args, int arg_size) {
    command->function(program, args, arg_size);
}
