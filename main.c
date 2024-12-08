#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include "simple_table.h"
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute) 


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
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR
} PrepareResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_FAILURE, //TODO: remove
    EXECUTE_TABLE_FULL
} ExecuteResult;

typedef struct {
    StatementType type;
    Row row_to_insert;
} Statement;

InputBuffer* new_input_buffer();
void print_prompt();
void read_input(InputBuffer* input_buffer);
void close_input_buffer(InputBuffer *input_buffer);
CommandResult do_command(InputBuffer* input_buffer);
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);
CommandResult do_command(InputBuffer* input_buffer);
ExecuteResult execute_insert(Statement* statement, Table* table);
ExecuteResult execute_select(Statement* statement, Table* table);
ExecuteResult execute_statement(Statement* statement, Table* table);
void serialize_row(Row* source, void* destination);
void deserialize_row(void* source, Row* destination);
void* row_slot(Table* table, int row_num);
Table* new_table();
void free_table(Table* table);
void print_row(Row* row);

const size_t ID_SIZE = size_of_attribute(Row, id);
const size_t USERNAME_SIZE = size_of_attribute(Row, username);
const size_t EMAIL_SIZE = size_of_attribute(Row, email); 
const size_t ID_OFFSET = 0;
const size_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const size_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const size_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const size_t PAGE_SIZE = 4096;
const size_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const size_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

int main(void) {
    InputBuffer* input_buffer = new_input_buffer();
    Table* table = new_table();

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
        case PREPARE_SYNTAX_ERROR:
            printf("Syntax error. Could not pare statement.\n");
            continue;
        case PREPARE_UNRECOGNIZED_STATEMENT:
            printf("Unrecognized keyword at the start of %s\n", input_buffer->buffer);
            continue;
        }

        switch (execute_statement(&statement, table))
        {
        case EXECUTE_SUCCESS:
            printf("Executed. \n");
            break;
        case EXECUTE_TABLE_FULL:
            printf("Error: Table is full. \n");
            break;
        default:
            break;
        }
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
        int args_assigned = sscanf(input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id), statement->row_to_insert.username, statement->row_to_insert.email);

        if(args_assigned < 3) {
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }

    if(strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
    if(table->num_rows == TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }

    Row* row_to_insert = &(statement->row_to_insert);

    serialize_row(row_to_insert, row_slot(table, table->num_rows));

    table->num_rows += 1;

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* statement, Table* table) {
    Row row;
    for(int i = 0; i < table->num_rows; i++) {
        deserialize_row(row_slot(table, i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement* statement, Table* table) { 
    switch (statement->type)
    {
        case (STATEMENT_INSERT):
            
            return execute_insert(statement, table);
        case(STATEMENT_SELECT):
            return execute_select(statement, table);
        default:
            break;
    }
}

void serialize_row(Row* source, void* destination) { 
    memcpy(destination + ID_OFFSET , &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void deserialize_row(void* source, Row* destination) {
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(destination->username, source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(destination->email, source + EMAIL_OFFSET, EMAIL_SIZE);

    destination->username[USERNAME_SIZE - 1] = '\0';
    destination->email[EMAIL_SIZE - 1] = '\0';
}


void* row_slot(Table* table, int row_num) { 
    int page_num = row_num / ROWS_PER_PAGE;
    void* page = table->pages[page_num];

    if(page == NULL) {
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }

    size_t row_offset = row_num % ROWS_PER_PAGE;
    size_t byte_offset = row_offset * ROW_SIZE;

    return page + byte_offset;
}

Table* new_table() {
    Table* table = (Table*)malloc(sizeof(Table));
    table->num_rows = 0;
    for(int i = 0; i < TABLE_MAX_PAGES; i++) {
        table->pages[i] = NULL;
    }

    return table;
}

void free_table(Table* table) {
    for(int i = 0; table->pages[i]; i++) {
        free(table->pages[i]);
    }

    free(table);
 }

void print_row(Row* row) {
    printf("(%d %s %s)\n", row->id, row->username, row->email);
 }