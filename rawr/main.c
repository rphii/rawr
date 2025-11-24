#include <stdint.h>
#include <rlarg.h>
#include <rlso.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

static int g_seed;

// Used to seed the generator.           
void fast_srand(int seed);
inline void fast_srand(int seed) {
    g_seed = seed;
}

// Compute a pseudorandom integer.
// Output value in range [0, 32767]
inline int fast_rand(void) {
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16)&0x7FFF;
}
int fast_rand(void);

typedef enum {
    MODE_NONE,
    MODE_ENCODE,
    MODE_DECODE,
} Mode;

void encode(So path_out, So content) {
    uint64_t len_total = content.len + 12; // 4 bytes for endian/signature, 8 bytes for length
    int square = (int)ceil(sqrt((double)len_total / 4));
    size_t len_pad = square * square * 4;

    if(!content.len) {
        printf(F("[E] nothing to encode\n", FG_RD));
        return;
    }

    printf(F("[I] final length of bytes: %zu\n", ), len_pad);

    /* signature */
    So final = SO;
    so_extend(&final, so("rawr"));

    /* encode length */
    for(int i = 0; i < sizeof(uint64_t); ++i) {
        uint8_t byte = (len_total >> (i * 8)) & 0xff;
        so_push(&final, byte);
    }

    /* encode data */
    so_extend(&final, content);

#if 1
    /* encode padding with 0 */
    for(size_t i = final.len; i < len_pad; ++i) {
        so_push(&final, 0);
    }
#else
    /* encode padding with random data */
    for(size_t i = final.len; i < len_pad; ++i) {
        so_push(&final, fast_rand());
    }
#endif

    ASSERT(final.len == len_pad, "lengths do not match");

    So extension = so_get_ext(so_clone(path_out));
    if(so_cmp(extension, so(".png"))) {
        printf(F("[W] extension does not match .png: %.*s (contents will be png)\n", FG_YL), SO_F(extension));
    }

    printf(F("[I] creating PNG image: %.*s (%ux%ux4)\n", ), SO_F(path_out), square, square);
    char *cpath = so_dup(path_out);
    stbi_write_png(cpath, square, square, 4, so_it0(final), 0);
    free(cpath);

    so_free(&path_out);
    so_free(&final);

}

void decode(So *content, So path_in) {
    ASSERT_ARG(content);

    int x = 0, y = 0, ch = 0;
    char *cpath = so_dup(path_in);
    unsigned char *data = stbi_load(cpath, &x, &y, &ch, 0);
    free(cpath);

    size_t len_all = x * y * ch;
    So encoded = so_ll(data, len_all);

    if(!data) {
        printf(F("[E] error reading: %.*s\n", FG_RD), SO_F(path_in));
        goto defer;
    }

    if(!so_cmp(path_in, so("rawr"))) {
        printf(F("[E] signature mismatch: %.*s\n", FG_RD), SO_F(path_in));
        goto defer;
    }

    printf(F("[I] decoding %.*s\n", ), SO_F(path_in));

    /* reconstruct total length */
    uint64_t len_expected = 0;
    for(size_t i = 0; i < sizeof(uint64_t); ++i) {
        uint8_t byte = so_at(encoded, 4 + i);
        len_expected |= (uint64_t)byte << (8 * i);
    }

    /* check for total length */
    if(len_expected >= len_all || len_expected < 12) {
        printf(F("[E] have %zu bytes, but expect more: %zu\n", FG_RD), len_all, len_expected);
        goto defer;
    }

    printf(F("[I] encoded data length: %zu\n", ), SO_F(path_in));

    /* finally copy bytes 1:1 */
    size_t len_copy = len_expected - 12;
    so_extend(content, so_sub(encoded, 12, 12 + len_copy));

defer:
    free(data);
}

int main(int argc, const char **argv) {

    int result = 0;
    Mode mode = MODE_NONE;
    VSo files_input = 0;

    So data_in = SO;
    So data_out = SO;
    So path_out = SO;

    struct Arg *arg = arg_new();
    arg_init(arg, so_l(argv[0]), so("encode/decode data as images"), SO);
    arg_init_rest(arg, so("files"), &files_input);

    struct ArgXGroup *g = argx_group(arg, so("Core"), false), *h;
    struct ArgX *x;
    argx_builtin_opt_help(g);
    /* mode selection */
    x=argx_pos(arg, so("mode"), so("encode/decode"));
      h=argx_opt(x, (int *)&mode, 0);
        x=argx_init(h, 0, so("encode"), so("perform encoding"));
          argx_opt_enum(x, MODE_ENCODE);
        x=argx_init(h, 0, so("decode"), so("perform decoding"));
          argx_opt_enum(x, MODE_DECODE);
    /* output handling */
    x=argx_init(g, 'o', so("output"), so("path to the output file"));
      argx_str(x, &path_out, 0);

    /* argument parsing */
    bool quit_early = false;
    arg_parse(arg, argc, argv, &quit_early);
    if(quit_early) {
        goto defer;
    }

    /* act on mode */
    switch(mode) {
        case MODE_ENCODE: {

            if(!so_len(path_out)) ABORT("no output specified");

            /* gather input text */
            So content = SO;
            for(size_t i = 0; i < array_len(files_input); ++i) {
                So filename = array_at(files_input, i);
                if(so_file_read(filename, &content)) {
                    printf(F("[W] error reading: %.*s\n", FG_YL), SO_F(filename));
                } else {
                    printf(F("[I] concatenating: %.*s\n", ), SO_F(filename));
                    so_extend(&data_in, content);
                }
            }
            so_free(&content);

            /* encode */
            encode(path_out, data_in);

        } break;
        case MODE_DECODE: {
            So data_out = SO;

            for(size_t i = 0; i < array_len(files_input); ++i) {
                So filename = array_at(files_input, i);
                decode(&data_out, filename);
            }

            if(!so_len(path_out)) {
                printf("%.*s", SO_F(data_out));
            } else {
                so_file_write(path_out, data_out);
            }
            so_free(&data_out);

        } break;
        default: THROW("invalid mode");
    }

defer:
    so_free(&data_in);
    so_free(&data_out);
    arg_free(&arg);
    return result;

error:
    result = -1;
    goto defer;
}

