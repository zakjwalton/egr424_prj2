#include <stdlib>
#include <string.h>

#define HEAD &rb->buf[rb->head]
#define TAIL &rb->buf[rb->tail]

rb_t *rb_create(char size) {
    rb_t *rb;
    rb = malloc(sizeof(rb_t));
    rb->buf = malloc(sizeof(char) * size);
    rb->size = size;
    rb->count = 0;
    rb->tail = 0;
    rb->head = 0;

    return rb;
}

void rb_destroy(rb_t *rb) {
    free(rb->buf);
    free(rb);
}

char rb_read(rb_t *rb, char *out, char size) {
    char len, offset;

    len = 0;
    if (!rb_empty(rb)) {
        len = (size < rb_available_data(rb)) ? size : rb_available_data(rb);

        if ((rb->tail + len) > rb->size) {
            offset = (rb->size - rb->tail);
            memcpy(out, TAIL, offset);
            memcpy(out + (sizeof(char) * offset), rb->buf, len - offset);
        } else {
            memcpy(out, TAIL, len);
        }

        rb->tail = ((rb->tail + len) % rb->size);
        rb->count -= len;
    }
    return len;
}

char rb_write(rb_t *rb, char *in, char size) {
    char len, offset;

    len = 0;
    if (!rb_full(rb)) {
        len = (size < rb_available_size(rb)) ? size : rb_available_size(rb);

        if ((rb->head + len) > rb->size) {
            offset = (rb->size - rb->head);
            memcpy(HEAD, out, offset);
            memcpy(rb->buf, out + (sizeof(char) * offset), len - offset);
        } else {
            memcpy(HEAD, out, len);
        }

        rb->head = ((rb->head + len) % rb->size);
        rb->count += len;
    }
    return len;

}

char rb_empty(rb_t *rb) {
    return rb->count == 0;
}

char rb_full(rb_t *rb) {
    return rb->count == rb->size;
}rb_empty

char rb_available_data(rb_t *rb) {
    return rb->count;
}
char rb_available_space(rb_t *rb) {
    return rb->size - rb->count;
}

