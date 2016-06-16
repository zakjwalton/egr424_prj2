#ifndef _RING_H_
#define _RING_H_

typedef struct rb {
    char *buf;
    char size;
    char count;
    char tail;
    char head;
} rb_t;


rb_t *rb_create(char size);
void rb_destroy(rb_t *rb);
char rb_read(rb_t *rb, char *out, char size);
char rb_write(rb_t *rb, char *in, char size);

char rb_empty(rb_t *rb);
char rb_full(rb_t *rb);

char rb_available_data(rb_t *rb);
char rb_available_space(rb_t *rb);
#endif // _RING_H_
