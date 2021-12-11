#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reverse(char s[])
 {
     int i, j;
     char c;

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
}

void itoa(int n, char s[])
 {
     int i, sign;

     if ((sign = n) < 0)
         n = -n;
     i = 0;
     do {
         s[i++] = n % 10 + '0';
     } while ((n /= 10) > 0);
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }


int** calculation(int** arr, int height, int width) {

    int** temp = (int**)malloc(sizeof(int*) * height);
    for (int i = 0; i < height; i++)
        temp[i] = (int*)malloc(sizeof(int) * width);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            int neighbours = 0, i_plus, i_minus, j_plus, j_minus;

            if (i + 1 > height - 1)
                i_plus = i + 1 - height;
            else
                i_plus = i + 1;

            if (i - 1 < 0)
                i_minus = i - 1 + height;
            else
                i_minus = i - 1;

            if (j + 1 > width - 1)
                j_plus = j + 1 - width;
            else
                j_plus = j + 1;

            if (j - 1 < 0)
                j_minus = j - 1 + width;
            else
                j_minus = j - 1;

            neighbours = arr[i_minus][j] + arr[i_minus][j_minus] + arr[i_minus][j_plus] + arr[i_plus][j]
                         + arr[i_plus][j_minus] + arr[i_plus][j_plus]
                         + arr[i][j_plus] + arr[i][j_minus];

            switch (neighbours) {
                case 0: temp[i][j] = 0; break;
                case 1: temp[i][j] = 0; break;
                case 2: temp[i][j] = arr[i][j]; break;
                case 3: temp[i][j] = 1; break;
                default: temp[i][j] = 0; break;
            }
        }
    }
    return temp;
}

int main(int argc, char* argv[]) {
    int height, width, size, generations, freq = 1;
    unsigned char bmp_header[54];
    char* directory = NULL;
    FILE* f = NULL;

    for (int i = 0; i < argc; i++) {
        if (!strcmp("--input", argv[i]))
            f = fopen(argv[i + 1], "rb");

        if (!strcmp("--output", argv[i])) {
            directory = argv[i + 1];
        }
        if (!strcmp("--max_iter", argv[i]))
            generations = strtol(argv[i + 1], 0, 10);

        if (!strcmp("--dump_freq", argv[i]))
            freq = strtol(argv[i + 1], 0, 10);
    }

    fread(bmp_header, sizeof(unsigned char), 54, f);

    width = bmp_header[21] * 256 * 256 * 256 + bmp_header[20] * 256 * 256 + bmp_header[19] * 256 + bmp_header[18];
    height = bmp_header[25] * 256 * 256 * 256 + bmp_header[24] * 256 * 256 + bmp_header[23] * 256 + bmp_header[22];
    size = bmp_header[5] * 256 * 256 * 256 + bmp_header[4] * 256 * 256 + bmp_header[3] * 256 + bmp_header[2];

    printf("bmp size = %d\n", size);
    printf("bmp width = %d\n", width);
    printf("bmp height = %d\n", height);

    unsigned char* buff = (unsigned char*) malloc((size - 54) * sizeof(unsigned char));
    fread(buff, sizeof(unsigned char), size, f);

    int** a = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        a[i] = (int*)malloc(width * sizeof(int));
    }

    int factor_4 = -(width % 4);
    for (int i = height - 1; i >= 0; i--) {
        factor_4 += width % 4;
        for (int j = 0; j < width; j++) {
            if (buff[factor_4] == 255)
                a[i][j] = 0;
            else
                a[i][j] = 1;
            factor_4 += 3;
        }
    }



    for (int i = 1; i <= generations; i++) {
        a = calculation(a, height, width);
        if (i % freq != 0)
            continue;

        char file_name[256], generation_value[10], name[30] = "gen";
        strcpy(file_name, directory);
        strcat(file_name, "\\");
        itoa(i, generation_value);
        strcat(name, generation_value);
        strcat(name, ".bmp");
        strcat(file_name, name);
        FILE* out = fopen(file_name, "wb");
        fwrite(bmp_header, 1, 54, out);
        factor_4 = 0;

        for (int i = height - 1; i >= 0; i--) {
            for (int j = 0; j < width; j++) {
                for (int k = 0; k < 3; k++) {
                    if (a[i][j] == 1)
                        buff[factor_4] = 0;
                    else
                        buff[factor_4] = 255;
                    factor_4++;
                }
            }
            while (factor_4 % 4 != 0) {
                buff[factor_4] = 0;
                factor_4++;
            }
        }
        fwrite(buff, sizeof(unsigned char), size, out);
        printf("next gen %d was successfully created\n" , i);
        fclose(out);
    }

    for (int i = 0; i < height - 1; i++) {
        free(a[i]);
    }
    free(a);
    free(buff);
    return 0;

}

