# dada

A deterministic oracle. `dada` derives a master seed from a committed image file
(`noise.png`), then produces deterministic bytes, picks, coins and dice from that
seed — identical output on any machine, with no OS randomness involved.

## How it works

- `noise.png` is a 512×512, 8-bit grayscale PNG of 263,289 bytes.
- The master seed is the 64-bit FNV-1a hash of the file:
  `0x10436367d2ee9119` (decimal `1171889625618092313`).
- splitmix64, seeded with the master seed, produces the byte stream.
- `--img FILE` uses any other file as the stone, changing the seed.

All arithmetic is integer-only, so results are identical across operating systems
and platforms. `make test` verifies this by running commands twice and
byte-diffing the outputs.

## Build

Requires a C compiler. No other dependencies.

```console
$ make            # cc -O2 -std=c11 -Wall -Wextra -o dada main.c
$ make test       # determinism smoke test, prints DETERMINISM HOLDS
$ make check      # the full proof: semantics + immutability (45 assertions)
$ make clean
```

`make test` runs `seed`, `bytes 64`, `pick`, `stream xor 32` and
`stream bit-plane-3 16` twice each and byte-diffs the results. On success it
prints:

```console
DETERMINISM HOLDS — same stone, same world, forever.
```

`make check` (`tests.sh`) goes deeper and is the one to trust before a release:
it checks every command against ground truth it derives itself (for `cell`, the
stone's own bytes via `od`, byte-for-byte), verifies the seeded commands really
move and the unseeded ones stay frozen, double-runs the whole deterministic
battery, and pins everything to the committed golden transcript
(`tests/dada.golden`). Regenerate that transcript after an *intentional*
behavior change with `make gold`.

## Usage

```
dada [--img FILE] [--poem] [command [args...]]
```

Commands:

| Command             | Output                                                       |
|---------------------|--------------------------------------------------------------|
| *(no command)*      | the poem (identical every run)                               |
| `seed`              | master seed, hex and decimal                                 |
| `bytes <n> [s...]` | `n` hex bytes from the seed stream; seeds move the stream |
| `pick <a> <b> ...`  | one deterministic choice from the candidates                 |
| `coin [s...]`      | 0 or 1; add a seed to move it along                         |
| `roll [s...]`      | 1..6; add a seed to move it along                           |
| `stream <name> <n>` | `n` hex bytes from one of the fourteen interpretations       |
| `interps`           | list the fourteen interpretations                            |
| `cell <x> <y>`      | one raw byte 0..255 at board position, wrapping by W x H     |
| `feed`              | print a fixed dadaist recipe                                 |
| `freerun [sub]`     | answers from `/dev/urandom` instead of the stone (random)    |
| `help`              | usage summary                                                |

Flags:

- `--img FILE` — use a different stone (changes the seed).
- `--poem` — print only the poem.

The fourteen interpretations available to `stream`:

```
raw  bit-plane-0 bit-plane-1 bit-plane-2 bit-plane-3 bit-plane-4
bit-plane-5 bit-plane-6 bit-plane-7 rgb-channel-0 rgb-channel-1
rgb-channel-2 delta xor
```

## Animated answers

Unseeded `coin`, `roll` and `bytes` are *frozen*: for a given stone they return
the same value forever (`coin` is `1` for noise.png — that is the determinism,
not a bug). To move them, give them a seed; the seed is mixed into the master
seed (the same way the candidates of `pick` are), so same seed, same answer,
new seed, new answer:

```console
$ ./dada coin           # 1 — frozen
$ ./dada coin 1 && ./dada coin 2   # 0, 1 — moving
```

That *is* the animation register. Sweep a grid, or bind a seed to the board:

```console
$ for x in $(seq 0 511); do ./dada coin "$x" "$((x * 7 % 515))"; done
```

## raylift.sh

A second oracle in bash. `noise.png` is laid out as a chessboard 512 columns
wide; two knights patrol it forever. Each answer is the XOR of the two cells'
four-byte FNV squints.

```console
$ ./raylift.sh                 # one value 0..1023 per line, forever
$ ./raylift.sh | head -n 12    # first twelve answers:
93
57
953
752
60
505
65
680
384
799
977
931
$ ./raylift.sh --scene         # verbose trace: positions, values, verse
```

Deterministic: same stone, same stream, on any machine.

## scatter.py

Regenerates `README.md` from the raylift stream, permuting only list groups and
quotable blocks. Code blocks, tables, headings and footnotes are never shuffled.

```console
$ ./scatter.py
scatter: N true answers consumed; README re-scattered.
```

## Exit codes

| Code | Meaning                                            |
|------|----------------------------------------------------|
| 0    | succeeded                                          |
| 1    | `freerun` could not read `/dev/urandom`            |
| 2    | the stone is missing, or an argument is missing    |

## Security

Do not use the determined mode for anything secret. All output of `seed`,
`bytes`, `pick`, `coin`, `roll` and `stream` is reproducible by anyone with this
repository. It also uses modulo sampling and is not a cryptographic
construction. For secrets, tokens or nonces use `freerun`, which reads the
system's entropy, or a proper secret generator.

## Documents

- `GUIDE.md` — the full field manual (decisions, lotteries, freerun usage,
  seeding other projects).

## License

WTFPL. See `LICENSE`.