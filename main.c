#include <stdio.h>
#include <stdlib.h>


typedef struct {
    char *buffer;
    int buffer_length;
    int buffer_size;
}  InputBuffer;

InputBuffer* new_input_buffer();

int main(void) {
    
    return 0; 
}

InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer *) malloc (sizeof(InputBuffer)); 
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->buffer_size = 0;

    return input_buffer;
}