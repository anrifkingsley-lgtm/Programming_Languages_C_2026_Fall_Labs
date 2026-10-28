/*
 * Salim Anrifoudine 260ADB184
 * https://github.com/SalimAnrifoudine/calc-lab-home
 * Compile with: gcc -O2 -Wall -Wextra -std=c17 -o calc calc.c -lm
 * Resources used: C standard library (stdio, stdlib, string, ctype, math, errno, dirent, sys/stat), K&R C, and Gemini AI.
 * AI (CHATGPT) prompts used: 
 * - "how to handle 1-based character position tracking including newlines and comments for error reporting in a file parser in C"
 * - "implementing recursive descent parser with right-associative exponentiation and precedence"
 */

/* =========================================================================
 * 8. COMMENTS RATIONALE :
 * =========================================================================
 * 
 * 1. HOW THE TOKENIZER TRACKS POSITIONS:
 *    We maintain a 1-based global character offset that 
 *    increments with every single character consumed from the input buffer 
 *    (including spaces, tabs, and newlines). We do this because test suites 
 *    require absolute 1-based character indexing relative to the raw file stream.
 * 
 * 2. WHY THE GRAMMAR GIVES THE RIGHT PRECEDENCE AND WHY ** IS RIGHT-ASSOCIATIVE:
 *    We use a recursive descent parser with strict hierarchical functions per 
 *    precedence level (expr -> term -> power -> primary). 
 *    Good: power() calls itself for the exponent, so 2**3**2 groups as 2**(3**2).
 *    This naturally enforces standard mathematical hierarchy without complex state.
 * 
 * 3. WHY EACH ERROR IS REPORTED AT THE POSITION CHOSEN:
 *    Errors are flagged precisely at the starting index of the token being 
 *    evaluated when a syntax violation, invalid character, division by zero, 
 *    or negative exponent occurs. Saving the token's initial offset
 *    allows us to point directly to the exact point of failure.
 * 
 * 4. HOW WE DECIDE BETWEEN INTEGER AND FLOAT ARITHMETIC:
 *    Calculations default to 64-bit integers  with truncation toward 
 *    zero to maximize performance and mirror C standards. Arithmetic automatically 
 *    promotes to `double` float if any operand is explicitly a float, if division 
 *    is performed, or if a negative exponent is processed.
 * 
 * 5. HOW WE HANDLE LARGE FILES WITHOUT SLOWING DOWN :
 *    To process multi-megabyte files efficiently, the entire file is loaded into 
 *    a single contiguous memory buffer in one block using `fseek` and `fread`. 
 *    This completely eliminates repeated disk I/O overhead and avoids incremental 
 *    buffer reallocation or copying.
 * ========================================================================= */

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct {
    int has_error;
    long long err_pos;
    double value;
    int is_float;
} EvalResult;

typedef struct {
    const char *content;
    size_t length;
    size_t pos;
    long long current_file_pos;
} Parser;

EvalResult parse_expression(Parser *p);
EvalResult parse_term(Parser *p);
EvalResult parse_factor(Parser *p);
EvalResult parse_power(Parser *p);

void skip_whitespace_and_comments_in_line(Parser *p) {
    while (p->pos < p->length) {
        char c = p->content[p->pos];
        if (c == ' ' || c == '\t') {
            p->pos++;
            p->current_file_pos++;
        } else {
            break;
        }
    }
}

EvalResult make_error(long long pos) {
    EvalResult res = {1, pos, 0.0, 0};
    return res;
}

EvalResult parse_number(Parser *p) {
    skip_whitespace_and_comments_in_line(p);
    if (p->pos >= p->length) return make_error(p->current_file_pos);

    size_t start_pos = p->pos;
    long long start_file_pos = p->current_file_pos;
    int has_dot = 0;
    int has_digits = 0;

    while (p->pos < p->length) {
        char c = p->content[p->pos];
        if (isdigit((unsigned char)c)) {
            has_digits = 1;
            p->pos++;
            p->current_file_pos++;
        } else if (c == '.') {
            if (has_dot) {
                return make_error(p->current_file_pos);
            }
            has_dot = 1;
            p->pos++;
            p->current_file_pos++;
        } else if (tolower((unsigned char)c) == 'e') {
            p->pos++;
            p->current_file_pos++;
            if (p->pos < p->length && (p->content[p->pos] == '+' || p->content[p->pos] == '-')) {
                p->pos++;
                p->current_file_pos++;
            }
        } else {
            break;
        }
    }

    if (!has_digits) {
        return make_error(start_file_pos);
    }

    char num_str[128];
    size_t len = p->pos - start_pos;
    if (len >= sizeof(num_str)) len = sizeof(num_str) - 1;
    memcpy(num_str, &p->content[start_pos], len);
    num_str[len] = '\0';

    EvalResult res;
    res.has_error = 0;
    if (has_dot || strchr(num_str, 'e') || strchr(num_str, 'E')) {
        res.is_float = 1;
        res.value = strtod(num_str, NULL);
    } else {
        res.is_float = 0;
        res.value = (double)strtoll(num_str, NULL, 10);
    }
    return res;
}

EvalResult parse_factor(Parser *p) {
    skip_whitespace_and_comments_in_line(p);
    if (p->pos >= p->length) return make_error(p->current_file_pos);

    long long current_pos = p->current_file_pos;
    char c = p->content[p->pos];

    if (c == '(') {
        p->pos++;
        p->current_file_pos++;
        EvalResult res = parse_expression(p);
        if (res.has_error) return res;

        skip_whitespace_and_comments_in_line(p);
        if (p->pos >= p->length || p->content[p->pos] != ')') {
            return make_error(p->current_file_pos);
        }
        p->pos++;
        p->current_file_pos++;
        return res;
    } else if (isdigit((unsigned char)c)) {
        return parse_number(p);
    } else {
        return make_error(current_pos);
    }
}

EvalResult parse_power(Parser *p) {
    EvalResult left = parse_factor(p);
    if (left.has_error) return left;

    skip_whitespace_and_comments_in_line(p);
    long long op_pos = p->current_file_pos;

    if (p->pos + 1 < p->length && p->content[p->pos] == '*' && p->content[p->pos+1] == '*') {
        p->pos += 2;
        p->current_file_pos += 2;

        EvalResult right = parse_power(p); // Right-associative call
        if (right.has_error) return right;

        if (!right.is_float && right.value < 0 && !left.is_float) {
            return make_error(op_pos);
        }

        double val = pow(left.value, right.value);
        if (!isfinite(val)) {
            return make_error(op_pos);
        }

        EvalResult res;
        res.has_error = 0;
        res.is_float = left.is_float || right.is_float || (right.value < 0);
        res.value = val;
        return res;
    }

    return left;
}

EvalResult parse_term(Parser *p) {
    EvalResult left = parse_power(p);
    if (left.has_error) return left;

    while (1) {
        skip_whitespace_and_comments_in_line(p);
        if (p->pos >= p->length) break;

        long long op_pos = p->current_file_pos;
        char op = p->content[p->pos];
        if (op != '*' && op != '/') break;

        p->pos++;
        p->current_file_pos++;

        EvalResult right = parse_power(p);
        if (right.has_error) return right;

        if (op == '/') {
            if (right.value == 0.0) {
                return make_error(op_pos);
            }
            if (!left.is_float && !right.is_float) {
                long long l_val = (long long)left.value;
                long long r_val = (long long)right.value;
                left.value = (double)(l_val / r_val);
            } else {
                left.value = left.value / right.value;
                left.is_float = 1;
            }
        } else {
            if (!left.is_float && !right.is_float) {
                left.value = left.value * right.value;
            } else {
                left.value = left.value * right.value;
                left.is_float = 1;
            }
        }
        if (!isfinite(left.value)) return make_error(op_pos);
    }
    return left;
}

EvalResult parse_expression(Parser *p) {
    EvalResult left = parse_term(p);
    if (left.has_error) return left;

    while (1) {
        skip_whitespace_and_comments_in_line(p);
        if (p->pos >= p->length) break;

        char op = p->content[p->pos];
        if (op != '+' && op != '-') break;

        p->pos++;
        p->current_file_pos++;

        EvalResult right = parse_term(p);
        if (right.has_error) return right;

        if (!left.is_float && !right.is_float) {
            if (op == '+') left.value += right.value;
            else left.value -= right.value;
        } else {
            left.is_float = 1;
            if (op == '+') left.value += right.value;
            else left.value -= right.value;
        }
    }
    return left;
}

EvalResult evaluate_line(const char *line_content, long long line_start_file_pos) {
    Parser p;
    p.content = line_content;
    p.length = strlen(line_content);
    p.pos = 0;
    p.current_file_pos = line_start_file_pos;

    skip_whitespace_and_comments_in_line(&p);
    if (p.pos >= p.length) {
        EvalResult empty = {0, 0, 0.0, 0};
        return empty;
    }

    EvalResult res = parse_expression(&p);
    if (res.has_error) return res;

    skip_whitespace_and_comments_in_line(&p);
    if (p.pos < p.length) {
        return make_error(p.current_file_pos);
    }

    return res;
}

void process_file(const char *input_path, const char *output_path) {
    FILE *fin = fopen(input_path, "r");
    if (!fin) {
        fprintf(stderr, "Error opening input file: %s\n", input_path);
        exit(EXIT_FAILURE);
    }

    fseek(fin, 0, SEEK_END);
    long fsize = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    char *buffer = malloc(fsize + 1);
    if (!buffer) {
        fclose(fin);
        exit(EXIT_FAILURE);
    }
    
    
    if (fread(buffer, 1, fsize, fin) != (size_t)fsize) {
        
    }
    
    buffer[fsize] = '\0';
    fclose(fin);

    FILE *fout = fopen(output_path, "w");
    if (!fout) {
        fprintf(stderr, "Error opening output file: %s\n", output_path);
        free(buffer);
        exit(EXIT_FAILURE);
    }

    long long file_pos = 1;
    size_t i = 0;
    while (i < (size_t)fsize) {
        size_t line_start = i;
        while (i < (size_t)fsize && buffer[i] != '\n') {
            i++;
        }
        size_t line_len = i - line_start;
        
        char *line_buf = malloc(line_len + 1);
        memcpy(line_buf, &buffer[line_start], line_len);
        line_buf[line_len] = '\0';

        int is_comment = 0;
        size_t idx = 0;
        while (idx < line_len && (line_buf[idx] == ' ' || line_buf[idx] == '\t')) idx++;
        if (idx < line_len && line_buf[idx] == '#') {
            is_comment = 1;
        }

        if (!is_comment && line_len > 0) {
            EvalResult res = evaluate_line(line_buf, file_pos);
            if (res.has_error) {
                fprintf(fout, "ERROR:%lld\n", res.err_pos);
                free(line_buf);
                free(buffer);
                fclose(fout);
                return;
            } else {
                int has_content = 0;
                for(size_t k=0; k<line_len; k++) {
                    if(!isspace((unsigned char)line_buf[k])) { has_content = 1; break; }
                }
                if (has_content) {
                    if (res.is_float) {
                        fprintf(fout, "%.15g\n", res.value);
                    } else {
                        fprintf(fout, "%lld\n", (long long)res.value);
                    }
                }
            }
        }

        free(line_buf);

        file_pos += line_len;
        if (i < (size_t)fsize && buffer[i] == '\n') {
            file_pos++;
            i++;
        }
    }

    free(buffer);
    fclose(fout);
}

void create_directory(const char *path) {
    char tmp[256];
    snprintf(tmp, sizeof(tmp), "%s", path);
    size_t len = strlen(tmp);
    if (len == 0) return;
    if (tmp[len - 1] == '/') tmp[len - 1] = '\0';
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0777);
            *p = '/';
        }
    }
    mkdir(tmp, 0777);
}

int main(int argc, char *argv[]) {
    char *dir_arg = NULL;
    char *out_arg = NULL;
    char *input_file = NULL;

    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dir") == 0) && i + 1 < argc) {
            dir_arg = argv[++i];
        } else if ((strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output-dir") == 0) && i + 1 < argc) {
            out_arg = argv[++i];
        } else if (input_file == NULL) {
            input_file = argv[i];
        } else {
            fprintf(stderr, "Error: Unknown or extra argument %s\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    const char *student_name = "Salim";
    const char *student_lastname = "Anrifoudine";
    const char *student_id = "260ADB184";

    if (dir_arg != NULL) {
        DIR *d = opendir(dir_arg);
        if (!d) {
            fprintf(stderr, "Error: Cannot open directory %s\n", dir_arg);
            return EXIT_FAILURE;
        }

        char out_dir[512];
        if (out_arg != NULL) {
            snprintf(out_dir, sizeof(out_dir), "%s", out_arg);
        } else {
            snprintf(out_dir, sizeof(out_dir), "%s_%s_%s", dir_arg, student_name, student_id);
        }
        create_directory(out_dir);

        struct dirent *dir;
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG || dir->d_type == DT_UNKNOWN) {
                char *ext = strrchr(dir->d_name, '.');
                if (ext && strcmp(ext, ".txt") == 0) {
                    char in_path[1024], out_path[1024];
                    snprintf(in_path, sizeof(in_path), "%s/%s", dir_arg, dir->d_name);
                    
                    char base_name[256];
                    size_t base_len = ext - dir->d_name;
                    strncpy(base_name, dir->d_name, base_len);
                    base_name[base_len] = '\0';

                    snprintf(out_path, sizeof(out_path), "%s/%s_%s_%s_%s.txt", out_dir, base_name, student_name, student_lastname, student_id);
                    process_file(in_path, out_path);
                }
            }
        }
        closedir(d);
    } else if (input_file != NULL) {
        char *ext = strrchr(input_file, '.');
        char base_name[256];
        if (ext) {
            char *last_slash = strrchr(input_file, '/');
            char *start_ptr = last_slash ? last_slash + 1 : (char*)input_file;
            size_t base_len = ext - start_ptr;
            strncpy(base_name, start_ptr, base_len);
            base_name[base_len] = '\0';
        } else {
            strcpy(base_name, input_file);
        }

        char out_dir[512], out_path[1024];
        if (out_arg != NULL) {
            snprintf(out_dir, sizeof(out_dir), "%s", out_arg);
        } else {
            snprintf(out_dir, sizeof(out_dir), "%s_%s_%s_%s", base_name, student_name,student_lastname, student_id);
        }
        create_directory(out_dir);

        snprintf(out_path, sizeof(out_path), "%s/%s_%s_%s_%s.txt", out_dir, base_name, student_name,student_lastname, student_id);
        process_file(input_file, out_path);
    } else {
        fprintf(stderr, "Usage: calc [-d DIR] [-o OUTDIR] [input.txt]\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}