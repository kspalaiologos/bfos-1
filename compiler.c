#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_SIZE  (65536 * 8)

FILE *src;
FILE *dest;

void transpile(int op, int count) {

    switch (op) {
        case '+':
            if (count > 1)
                fprintf(dest, "add byte [rbx], %d\n", count);
            else
                fprintf(dest, "inc byte [rbx]\n");
            break;
        case '-':
            if (count > 1)
                fprintf(dest, "sub byte [rbx], %d\n", count);
            else
                fprintf(dest, "dec byte [rbx]\n");
            break;
        case '<':
            if (count > 1)
                fprintf(dest, "sub rbx, %d\n", count);
            else
                fprintf(dest, "dec rbx\n");
            break;
        case '>':
            if (count > 1)
                fprintf(dest, "add rbx, %d\n", count);
            else
                fprintf(dest, "inc rbx\n");
            break;
        default:
            break;
    }

}

void usage(char *exec_name) {
    fprintf(stderr,
        "Usage: %s <source> <output>\n", exec_name
    );
    return;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "BFBoot: no source file.\n");
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    if (argc < 3) {
        fprintf(stderr, "BFBoot: no output filename.\n");
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (!(src = fopen(argv[1], "r"))) {
        fprintf(stderr, "BFBoot: Failed to open %s.\n", argv[1]);
        return EXIT_FAILURE;
    }
    if (!(dest = fopen(argv[2], "w"))) {
        fprintf(stderr, "BFBoot: Failed to open %s.\n", argv[2]);
        fclose(src);
        return EXIT_FAILURE;
    }

    int cur_label = 0;
    int *label_stack = malloc(MAX_STACK_SIZE);
    if (!label_stack) {
        fprintf(stderr, "BFBoot: malloc fail.\n");
        fclose(src);
        fclose(dest);
        return EXIT_FAILURE;
    }
    int label_sp = 0;
    int last_operand = 0;
    int operand_count = 0;

    fprintf(dest, "section .text\nglobal bfcode\nglobal bfcode_size\nbfcode:\n");

    /* get operands and repeat count */
    for (;;) {
        switch (fgetc(src)) {
            case EOF:
                transpile(last_operand, operand_count);
                fprintf(dest, "jmp $\n");
                fprintf(dest, "bfcode_end:\nbfcode_size: equ bfcode_end - bfcode\n");
                fclose(src);
                fclose(dest);
                free(label_stack);
                return 0;
            case '+':
                if (last_operand != '+') {
                    transpile(last_operand, operand_count);
                    operand_count = 1;
                    last_operand = '+';
                } else {
                    operand_count++;
                }
                break;
            case '-':
                if (last_operand != '-') {
                    transpile(last_operand, operand_count);
                    operand_count = 1;
                    last_operand = '-';
                } else {
                    operand_count++;
                }
                break;
            case '>':
                if (last_operand != '>') {
                    transpile(last_operand, operand_count);
                    operand_count = 1;
                    last_operand = '>';
                } else {
                    operand_count++;
                }
                break;
            case '<':
                if (last_operand != '<') {
                    transpile(last_operand, operand_count);
                    operand_count = 1;
                    last_operand = '<';
                } else {
                    operand_count++;
                }
                break;
            case '.':
            case ',':
                break;
            case '[':
                transpile(last_operand, operand_count);
                last_operand = 0;
                fprintf(dest, "label%d:\n", cur_label);
                fprintf(dest,
                    "cmp byte [rbx], 0\n"
                    "je label%d_out\n", cur_label
                );
                label_stack[label_sp++] = cur_label++;
                break;
            case ']':
                transpile(last_operand, operand_count);
                last_operand = 0;
                fprintf(dest,
                    "cmp byte [rbx], 0\n"
                    "jne label%d\n", label_stack[label_sp-1]
                );
                fprintf(dest, "label%d_out:\n", label_stack[label_sp-1]);
                label_sp--;
                break;
            default:
                break;
        }

    }

}
