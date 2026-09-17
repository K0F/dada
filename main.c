#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define DEFAULT_IMAGE "noise.png"
#define MAX_BYTES (1u << 20)
#define W 512

static uint64_t fnv64(const uint8_t *p, size_t n) {
    uint64_t h = 0xcbf29ce484222325ULL;
    for (size_t i = 0; i < n; i++) {
        h ^= (uint64_t)p[i];
        h *= 0x100000001b3ULL;
    }
    return h;
}

static uint64_t splitmix64(uint64_t *s) {
    uint64_t z = (*s += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

static void *xmalloc(size_t n) {
    void *p = malloc(n);
    if (!p) {
        fprintf(stderr, "dada: the orchards are full.\n");
        exit(1);
    }
    return p;
}

static uint8_t *load_file(const char *path, size_t *out_len) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long end = ftell(f);
    if (end <= 0) { fclose(f); return NULL; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return NULL; }
    uint8_t *buf = xmalloc((size_t)end);
    size_t got = fread(buf, 1, (size_t)end, f);
    fclose(f);
    if (got != (size_t)end) { free(buf); return NULL; }
    *out_len = (size_t)end;
    return buf;
}

static int realistic_bytes(uint8_t *p, size_t n) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) return -1;
    size_t got = 0;
    while (got < n) {
        ssize_t r = read(fd, p + got, n - got);
        if (r <= 0) break;
        got += (size_t)r;
    }
    close(fd);
    return got == n ? 0 : -1;
}

enum { K_RAW, K_PLANE, K_CHANNEL, K_DELTA, K_XOR };
#define N_INTERPS (1 + 8 + 3 + 1 + 1)

typedef struct { const char *name; int kind; int param; } Interp;

static char names[N_INTERPS][24];
static Interp interps[N_INTERPS];

static void build_registry(void) {
    int i = 0;
    interps[i++] = (Interp){ "raw", K_RAW, 0 };
    for (int p = 0; p < 8; p++) {
        snprintf(names[i], sizeof names[i], "bit-plane-%d", p);
        interps[i] = (Interp){ names[i], K_PLANE, p };
        i++;
    }
    for (int c = 0; c < 3; c++) {
        snprintf(names[i], sizeof names[i], "rgb-channel-%d", c);
        interps[i] = (Interp){ names[i], K_CHANNEL, c };
        i++;
    }
    interps[i++] = (Interp){ "delta", K_DELTA, 0 };
    interps[i++] = (Interp){ "xor", K_XOR, 0 };
}

static size_t run_interp(const Interp *in, const uint8_t *src, size_t n, uint8_t *dst) {
    switch (in->kind) {
    case K_RAW:
        memcpy(dst, src, n);
        return n;
    case K_PLANE: {
        size_t out = n / 8;
        for (size_t i = 0; i < out; i++) {
            uint8_t b = 0;
            for (int k = 0; k < 8; k++)
                b = (uint8_t)((b << 1) | ((src[i * 8 + k] >> in->param) & 1u));
            dst[i] = b;
        }
        return out;
    }
    case K_CHANNEL: {
        size_t out = n / 3;
        for (size_t i = 0; i < out; i++)
            dst[i] = src[i * 3 + in->param];
        return out;
    }
    case K_DELTA: {
        size_t out = n > 0 ? n - 1 : 0;
        for (size_t i = 0; i < out; i++)
            dst[i] = (uint8_t)(src[i + 1] - src[i]);
        return out;
    }
    case K_XOR: {
        size_t out = n > 0 ? n - 1 : 0;
        for (size_t i = 0; i < out; i++)
            dst[i] = (uint8_t)(src[i] ^ src[i + 1]);
        return out;
    }
    default:
        return 0;
    }
}

static const Interp *find_interp(const char *name) {
    for (int i = 0; i < N_INTERPS; i++)
        if (strcmp(interps[i].name, name) == 0) return &interps[i];
    return NULL;
}

static uint64_t geist;
static void egg(uint64_t key) { geist = key; }
static uint64_t hatch(void) { return splitmix64(&geist); }
static uint8_t hatch8(void) { return (uint8_t)(hatch() >> 56); }

static uint64_t season(uint64_t base, const char **args, int nargs) {
    for (int i = 0; i < nargs; i++)
        base ^= fnv64((const uint8_t *)args[i], strlen(args[i]));
    return base;
}

static void print_hex(const uint8_t *p, size_t n) {
    for (size_t i = 0; i < n; i++) printf("%02x", p[i]);
    putchar('\n');
}

static void print_poem(const char *img, const uint8_t *raw, size_t n, uint64_t master, int only) {
    if (!only) {
        printf("dada 0x%016llx — reincarnation of %s (%zu bytes of holy material)\n\n",
               (unsigned long long)master, img, n);
        uint8_t *work = xmalloc(n);
        for (int i = 0; i < N_INTERPS; i++) {
            size_t len = run_interp(&interps[i], raw, n, work);
            if (len == 0) continue;
            uint64_t gnawed = fnv64(work, len) ^ master;
            double sum = 0.0;
            for (size_t j = 0; j < len; j++) sum += work[j];
            printf("[%-12s] %-8zu bytes; first sample 0x%02X; mean %.2f; gnawed entropy 0x%016llx\n",
                   interps[i].name, len, work[0], sum / (double)len,
                   (unsigned long long)gnawed);
        }
        free(work);
    }
    printf("\nNoise.\n\n");
    printf("The scattered register knocked the last coherent thought out of %s.\n", img);
    printf("The kernel bit into lime mortar. The sampler does not sample; the sampler scours the bottom.\n");
    printf("The roof is a script. The script is a ridge tile.\n");
    printf("No locks. No bus. Only clean, uninstalled grey dust settling toward zero.\n");
}

static int is_command(const char *s) {
    static const char *cmds[] = {
        "help", "poem", "seed", "bytes", "pick", "coin", "roll", "stream", "interps",
        "cell", "feed", "freerun", NULL
    };
    for (int i = 0; cmds[i]; i++)
        if (strcmp(s, cmds[i]) == 0) return 1;
    return 0;
}

static void print_help(const char *prog) {
    printf("dada — deterministic oracle, philosopher's stone edition\n");
    printf("usage: %s [--img stone] [--poem] [command] [args...]\n\n", prog);
    printf("  (no command)      the poem\n");
    printf("  seed              master seed (hex decimal)\n");
    printf("  bytes <n> [s...] n hex bytes from the stone; seeds move the stream\n");
    printf("  pick <a> <b> ...  one deterministic choice\n");
    printf("  coin [s...]       0 or 1; seeds move it along\n");
    printf("  roll [s...]       1..6; seeds move it along\n");
    printf("  stream <name> <n> hex bytes from one interpretation\n");
    printf("  interps           list the fourteen interpretations\n");
    printf("  cell <x> <y>      one raw byte 0..255 at board x,y (wraps W x H)\n");
    printf("  feed              a real, dead simple, dadaist recipe\n");
    printf("  freerun [sub]     /dev/urandom — the breaking of the contract\n");
    printf("                      freerun [n | bytes <n>]  real hex bytes\n");
    printf("                      freerun seed | coin | roll | pick <a> <b>...\n");
    printf("  help              this\n");
}

static size_t parse_n(const char *s, size_t def) {
    if (!s) return def;
    unsigned long long v = strtoull(s, NULL, 10);
    if (v == 0 || v > MAX_BYTES) return def;
    return (size_t)v;
}

int main(int argc, char **argv) {
    const char *img = DEFAULT_IMAGE;
    const char *cmd = "poem";
    int only = 0;
    int ai = 1;

    for (; ai < argc; ai++) {
        const char *a = argv[ai];
        if (strcmp(a, "--poem") == 0) { only = 1; continue; }
        if (strcmp(a, "--img") == 0 && ai + 1 < argc) { img = argv[++ai]; continue; }
        if (is_command(a)) { cmd = a; ai++; break; }
        img = a;
    }

    if (strcmp(cmd, "help") == 0) { print_help(argv[0]); return 0; }

    build_registry();

    size_t n = 0;
    uint8_t *raw = load_file(img, &n);
    if (!raw || n == 0) {
        fprintf(stderr, "dada: the stone %s is not baked into this repository.\n", img);
        return 2;
    }
    uint64_t master = fnv64(raw, n);
    const char **args = (const char **)(argv + ai);
    int nargs = argc - ai;

    if (strcmp(cmd, "seed") == 0) {
        printf("0x%016llx %llu\n", (unsigned long long)master, (unsigned long long)master);
    } else if (strcmp(cmd, "coin") == 0) {
        egg(season(master, args, nargs));
        printf("%u\n", (unsigned)(hatch8() & 1u));
    } else if (strcmp(cmd, "roll") == 0) {
        egg(season(master ^ 0x0badc0dedeaf0d00ULL, args, nargs));
        printf("%u\n", (unsigned)((hatch8() % 6u) + 1u));
    } else if (strcmp(cmd, "bytes") == 0) {
        size_t want = parse_n(nargs > 0 ? args[0] : NULL, 16);
        egg(season(master, args + 1, nargs > 0 ? nargs - 1 : 0));
        uint8_t *buf = xmalloc(want);
        for (size_t i = 0; i < want; i++) buf[i] = hatch8();
        print_hex(buf, want);
        free(buf);
    } else if (strcmp(cmd, "interps") == 0) {
        for (int i = 0; i < N_INTERPS; i++) printf("%s\n", interps[i].name);
    } else if (strcmp(cmd, "cell") == 0) {
        if (nargs < 2) {
            fprintf(stderr, "dada: cell requires x and y.\n");
            return 2;
        }
        unsigned long long x = strtoull(args[0], NULL, 10);
        unsigned long long y = strtoull(args[1], NULL, 10);
        unsigned long long h = (n + W - 1) / W;
        size_t idx = (size_t)((y % h) * W + (x % W)) % n;
        printf("%u\n", (unsigned)raw[idx]);
    } else if (strcmp(cmd, "pick") == 0) {
        if (nargs < 1) {
            fprintf(stderr, "dada: pick requires candidates.\n");
            return 2;
        }
        uint64_t acc = master;
        for (int i = 0; i < nargs; i++)
            acc ^= fnv64((const uint8_t *)args[i], strlen(args[i]));
        egg(acc);
        printf("%s\n", args[hatch() % (size_t)nargs]);
    } else if (strcmp(cmd, "stream") == 0) {
        const char *name = nargs > 0 ? args[0] : "raw";
        size_t want = parse_n(nargs > 1 ? args[1] : NULL, 32);
        const Interp *in = find_interp(name);
        if (!in) {
            fprintf(stderr, "dada: interpretation '%s' does not exist; available: ", name);
            for (int i = 0; i < N_INTERPS; i++)
                fprintf(stderr, "%s%s", i ? " " : "", interps[i].name);
            fputc('\n', stderr);
            return 2;
        }
        uint8_t *work = xmalloc(n);
        size_t len = run_interp(in, raw, n, work);
        egg(fnv64(work, len) ^ master);
        uint8_t *buf = xmalloc(want);
        for (size_t i = 0; i < want; i++) buf[i] = hatch8();
        print_hex(buf, want);
        free(buf);
        free(work);
    } else if (strcmp(cmd, "feed") == 0) {
        static const char *ingreds[] = {"water", "flour", "salt", "an egg", "a potato", "silence", "an onion", "oil", "a single bean"};
        static const char *actions[] = {"boil", "burn", "stare at", "crush", "ignore", "whisper to", "hold"};
        static const char *durations[] = {"for 300 seconds", "until it changes color", "until the sun sets", "for one breath", "forever"};
        static const char *serving[] = {"on the floor", "to the nearest stranger", "in complete darkness", "without breaking eye contact", "out of a shoe"};
        
        egg(master ^ 0x6B17C4E1ULL);
        printf("THE REAL DADA KITCHEN\n\n");
        printf("1. Take %s and %s.\n", ingreds[hatch() % 9], ingreds[hatch() % 9]);
        printf("2. %s them %s.\n", actions[hatch() % 7], durations[hatch() % 5]);
        printf("3. Serve %s.\n", serving[hatch() % 5]);
        printf("\nDo this now. It is real.\n");
    } else if (strcmp(cmd, "freerun") == 0) {
        if (nargs > 0 && strcmp(args[0], "pick") == 0) {
            int count = nargs - 1;
            if (count < 1) {
                fprintf(stderr, "dada freerun: pick requires candidates.\n");
                return 2;
            }
            uint64_t r;
            if (realistic_bytes((uint8_t *)&r, sizeof r) != 0) {
                fprintf(stderr, "dada freerun: /dev/urandom is silent.\n");
                return 1;
            }
            printf("%s\n", args[1 + (size_t)(r % (uint64_t)count)]);
        } else if (nargs > 0 && strcmp(args[0], "seed") == 0) {
            uint64_t r;
            if (realistic_bytes((uint8_t *)&r, sizeof r) != 0) {
                fprintf(stderr, "dada freerun: /dev/urandom is silent.\n");
                return 1;
            }
            printf("0x%016llx %llu\n", (unsigned long long)r,
                   (unsigned long long)r);
        } else if (nargs > 0 && strcmp(args[0], "coin") == 0) {
            uint8_t b;
            if (realistic_bytes(&b, 1) != 0) {
                fprintf(stderr, "dada freerun: /dev/urandom is silent.\n");
                return 1;
            }
            printf("%u\n", (unsigned)(b & 1u));
        } else if (nargs > 0 && strcmp(args[0], "roll") == 0) {
            uint8_t b;
            if (realistic_bytes(&b, 1) != 0) {
                fprintf(stderr, "dada freerun: /dev/urandom is silent.\n");
                return 1;
            }
            printf("%u\n", (unsigned)((b % 6u) + 1u));
        } else {
            size_t want = 16;
            const char *a0 = nargs > 0 ? args[0] : NULL;
            if (a0) {
                if (strcmp(a0, "bytes") == 0)
                    want = parse_n(nargs > 1 ? args[1] : NULL, 16);
                else
                    want = parse_n(a0, 16);
            }
            uint8_t *buf = xmalloc(want);
            if (realistic_bytes(buf, want) != 0) {
                fprintf(stderr, "dada freerun: /dev/urandom is silent.\n");
                free(buf);
                return 1;
            }
            print_hex(buf, want);
            free(buf);
        }
    } else {
        print_poem(img, raw, n, master, only);
    }

    free(raw);
    return 0;
}