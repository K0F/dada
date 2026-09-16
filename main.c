#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define DEFAULT_IMAGE "noise.png"
#define MAX_BYTES (1u << 20)

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
        "boil", "spice", "chore", NULL
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
    printf("  bytes <n>         n hex bytes from the stone\n");
    printf("  pick <a> <b> ...  one deterministic choice\n");
    printf("  coin              0 or 1\n");
    printf("  roll              1..6\n");
    printf("  stream <name> <n> hex bytes from one interpretation\n");
    printf("  interps           list the fourteen interpretations\n");
    printf("  boil              seconds to boil an egg (deterministic)\n");
    printf("  spice             which spice to add (deterministic)\n");
    printf("  chore <a> <b> ... who does the dishes\n");
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
        egg(master);
        printf("%u\n", (unsigned)(hatch8() & 1u));
    } else if (strcmp(cmd, "roll") == 0) {
        egg(master ^ 0x0badc0dedeaf0d00ULL);
        printf("%u\n", (unsigned)((hatch8() % 6u) + 1u));
    } else if (strcmp(cmd, "bytes") == 0) {
        size_t want = parse_n(nargs > 0 ? args[0] : NULL, 16);
        egg(master);
        uint8_t *buf = xmalloc(want);
        for (size_t i = 0; i < want; i++) buf[i] = hatch8();
        print_hex(buf, want);
        free(buf);
    } else if (strcmp(cmd, "interps") == 0) {
        for (int i = 0; i < N_INTERPS; i++) printf("%s\n", interps[i].name);
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
    } else if (strcmp(cmd, "chore") == 0) {
        if (nargs < 1) {
            fprintf(stderr, "dada: chore requires candidates.\n");
            return 2;
        }
        uint64_t acc = master ^ 0x0C404E0000000000ULL;
        for (int i = 0; i < nargs; i++)
            acc ^= fnv64((const uint8_t *)args[i], strlen(args[i]));
        egg(acc);
        
        const char *loser = args[hatch() % (size_t)nargs];
        static const char *reasons[] = {
            "as dictated by the strict geometry of the Square",
            "because their personal entropy is suspiciously low",
            "as penance for disturbing the master seed",
            "since they are the only one who truly understands the sponge",
            "because the 14th interpretation demands a wet sacrifice",
            "as a purely deterministic consequence of their birth",
            "because the noise deemed their hands the most expendable"
        };
        size_t n_reasons = sizeof(reasons) / sizeof(reasons[0]);
        printf("%s must do the dishes, %s.\n", loser, reasons[hatch() % n_reasons]);
    } else if (strcmp(cmd, "boil") == 0) {
        egg(master ^ 0xB011000000000000ULL);
        unsigned seconds = hatch() % 10000;
        static const char *methods[] = {
            "on a rolling boil while maintaining direct eye contact with the pot",
            "in water drawn entirely from a weeping willow",
            "over a flame fueled only by overdue utility bills",
            "using ambient room temperature, however long that takes",
            "in a broth of your own regrets",
            "while explaining the concept of zero to a dog",
            "in a pan that has never known heat",
            "under the cold, indifferent gaze of the moon"
        };
        size_t n_methods = sizeof(methods) / sizeof(methods[0]);
        printf("%u seconds, %s.\n", seconds, methods[hatch() % n_methods]);
    } else if (strcmp(cmd, "spice") == 0) {
        static const char *spices[] = {
            "a single grain of salt, cleanly bisected",
            "the memory of garlic",
            "cinnamon, inhaled deeply before looking at the pan",
            "MSG, applied with the reverence of a sacrament",
            "paprika, but only on alternating Tuesdays",
            "nothing. The dish is already too loud.",
            "a bay leaf that you must later deny ever seeing",
            "sugar, applied with aggressive pessimism",
            "thyme, which you have run out of",
            "an uncracked peppercorn, hidden as a trap for the unwary",
            "the mathematical concept of heat (do not actually use chili)"
        };
        size_t n_spices = sizeof(spices) / sizeof(spices[0]);
        egg(master ^ 0x591CE00000000000ULL);
        printf("%s\n", spices[hatch() % n_spices]);
    } else {
        print_poem(img, raw, n, master, only);
    }

    free(raw);
    return 0;
}