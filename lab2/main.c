#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define base 1000000000


typedef struct uint1024_t {
    uint32_t *chunk;
    size_t count;
    size_t size;
} uint1024_t;

void str_reverse(uint8_t *arr, size_t size) {
    uint8_t temp;
    for (size_t i = 0, j = size-1; i < size / 2; i++, j--) {
        temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void print_uint1024_t(uint1024_t *x) {
    if (x->count == 0) {
        printf("null\n");
        return;
    }
    ssize_t i = x->count-1;
    for (; i > 0; --i) {
        if (x->chunk[i] != 0) {
            break;
        }
    }
    printf("%u", x->chunk[i]);
    for (--i; i != -1; --i) {
        printf("%.9u", x->chunk[i]);
    }
    putchar('\n');
}

uint1024_t *add_op(uint1024_t *x, uint1024_t *y) {

    size_t max_count;

    if (x->count > y->count)
        max_count = x->count;
    else
        max_count = y->count;

    uint8_t carry = 0;

    uint1024_t *result;
    result = (uint1024_t*)malloc(sizeof(uint1024_t));
    memcpy(result, x, sizeof(uint1024_t));

    for (size_t i = 0; i < max_count; ++i) {
        uint64_t temp = carry + x->chunk[i] + y->chunk[i];
        result->chunk[i] = temp % base;
        carry = temp / base;
    }

    if (carry) {
        result->chunk[result->count] = carry;
        result->count++;
    }


    return result;
}

uint1024_t *init(uint8_t *str) {

    const size_t buff_size = 9;

    uint1024_t *new_uint = (uint1024_t*)malloc(sizeof(uint1024_t));
    new_uint->size = 32;
    new_uint->count = 0;
    new_uint->chunk = (uint32_t*)malloc(new_uint->size * sizeof(uint32_t));


    uint8_t num[buff_size + 1];
    memset(num, 0, (buff_size + 1) * sizeof(uint8_t));

    size_t next = 0;
    size_t index = 0;
    size_t length = strlen(str);

    for (int i = length - 1; i >= 0; i--) {
        num[index++] = str[i];
        if (index == buff_size) {
            index = 0;
            str_reverse(num, buff_size);
            new_uint->chunk[next++] = atoi(num);
            new_uint->count++;
        }
    }
    if (index != 0) {
        num[index] = '\0';
        str_reverse(num, index);
        new_uint->chunk[next++] = atoi(num);
        new_uint->count++;
    }

    return new_uint;
}

void _free(uint1024_t *x) {
    for (int i = 0; i < 32; i++){
        free(x->chunk[i]);
    }
    free(x->chunk);
    free(x);
}

int main() {

    char str1[309];
    char str2[309];

    printf ("enter first uint1024_t num ");
    scanf("%309s", str1);

    printf ("enter second uint1024_t num ");
    scanf("%309s", str2);

    uint1024_t *x = init(str1);
    uint1024_t *y = init(str2);

    printf ("\nfirst uint1024_t num ");
    print_uint1024_t(x);
    printf ("\nsecond uint1024_t num ");
    print_uint1024_t(y);

    printf ("\nsum uint1024_t num ");
    print_uint1024_t(add_op(x, y));


    _free(x);
    _free(y);



    return 0;
}


