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

InputBuffer* new_input_buffer();
void print_prompt();
void read_input(InputBuffer* input_buffer);
void close_input_buffer(InputBuffer *input_buffer);

int main(void) {
    InputBuffer* input_buffer = new_input_buffer();

    while (true)
    {
        print_prompt();
        read_input(input_buffer);

        if(strcmp(input_buffer->buffer, ".exit") == 0) {
            close_input_buffer(input_buffer);
            exit(0);
        } else {
            printf("Unrecognized command %s \n", input_buffer->buffer);
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
    input_buffer->buffer[bytes_read - 1] = 0; // replacing the /n at the end (/0/0 at the end)

}

void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}