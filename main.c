#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>


typedef struct {
    char *buffer;
    int buffer_length;
    int buffer_size;
}  InputBuffer;

typedef enum {
    COMMAND_SUCCESS,
    COMMAND_FAILURE,
} CommandResult;

typedef enum{ 
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct {
    StatementType type;
} Statement;

InputBuffer* new_input_buffer();
void print_prompt();
void read_input(InputBuffer* input_buffer);
void close_input_buffer(InputBuffer *input_buffer);
CommandResult do_command(InputBuffer* input_buffer);
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);
CommandResult do_command(InputBuffer* input_buffer);
void execute_statement(InputBuffer* input_buffer, Statement* statement);


int main(void) {
    InputBuffer* input_buffer = new_input_buffer();

    while (true)
    {
        print_prompt();
        read_input(input_buffer);

        if(input_buffer->buffer[0] == '.') {
            switch (do_command(input_buffer))
            {
            case COMMAND_SUCCESS:
                continue;
            case COMMAND_FAILURE:
                printf("Unrecognized command %s\n", input_buffer->buffer);
                continue;
            }
        }

        Statement statement;

        switch (prepare_statement(input_buffer, &statement))
        {
        case PREPARE_SUCCESS:
            break;
        case PREPARE_UNRECOGNIZED_STATEMENT:
            printf("Unrecognized keyword at the start of %s\n", input_buffer->buffer);
            continue;
        }

        execute_statement(input_buffer, &statement);

        printf("\nEXECUTED\n");
    }
    

    return 0; 
}

InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer *) malloc (sizeof(InputBuffer)); 
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->buffer_size = 0;

    return input_buffer;
}

void print_prompt() {
    printf("db > ");
}

void read_input(InputBuffer* input_buffer) { 
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_size), stdin);

    if(bytes_read <= 0) {
        printf("Error reading input\n");
        exit(1);
    }

    input_buffer->buffer_length = bytes_read - 1; // for the null terminator
    input_buffer->buffer[bytes_read - 1] = 0; // replacing the /n at the char before end (/0/0 at the end)

}

void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

CommandResult do_command(InputBuffer* input_buffer) {
    if(strncmp(input_buffer->buffer, ".exit", 5) == 0) {        
        close_input_buffer(input_buffer);
        exit(0);
    } else {
        return COMMAND_FAILURE;
    }
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if(strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }

    if(strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(InputBuffer* input_buffer, Statement* statement) {
    switch (statement->type)
    {
        case (STATEMENT_INSERT):
            printf("insert statement");
            break;
        case(STATEMENT_SELECT):
            printf("select statement");
            break;
        default:
            break;
    }
}