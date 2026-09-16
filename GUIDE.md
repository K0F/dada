# The Dada Field Manual

**Applied Dadaism for Everyday Life** — companion to `README.md` (*Manifestum Dadae,
Vol. I*), written to be *trustworthy* and *usable*, in that order.

---

## 0. What this manual is

`dada` is a deterministic oracle. It answers questions. It never changes its mind,
unless you ask it to break the contract (see `freerun`, §7).

This manual teaches you how to live with that — how to offload small decisions,
lotteries, meal plans and kitchen assignments onto the Square of True Knowledge,
and how to do it reproducibly, portably, and *safely*.

## 1. Build (three lines)

```console
$ make            # cc -O2 -std=c11 -Wall -Wextra -o dada main.c
$ make test       # proves determinism holds
$ ./dada help
```

Requires: a C compiler and the committed stone `noise.png`. Nothing else. No
dependencies, no internet, no prayers (prayers optional, free of charge).

## 2. The two regimes

| | DETERMINED | FREE |
|---|---|---|
| command family | `seed bytes pick coin roll stream interps feed` | `freerun ...` |
| source of answers | the Square of True Knowledge (`noise.png`) | `/dev/urandom` |
| same run twice | **identical**, forever, on any machine | different, every time |
| good for | reproducible decisions, tests, choreography of futures | secrets, tokens, one-off freshness |
| not good for | secrets (see §8 — please read it) | convincing a historian |

The two regimes never mix. `dada` never reaches for the operating system in its
determined mode; `freerun` never touches the stone. Each is honest about itself,
which makes the whole instrument trustworthy.

## 3. Decisions

The flagship command. Supply options; receive the verdict of the Square.

```console
$ ./dada pick left right straight-on
right
```

```console
$ ./dada pick sushi pizza silence
pizza
```

Rules of good practice:

- Same options, same order -> same answer, always. If you want a *different*
  answer, add an option; the Square respects changed input, not complaints.
- The answer is authoritative. Appeal is possible but requires re-running with
  an additional option, which is how revolutions are also started.
- Backwards compatibility: the verdict for identical arguments is stable across
  operating systems and centuries (integer arithmetic only, no floats in the path).

## 4. Lotteries, dice, coins

```console
$ ./dada coin          # heads/tails as 0 or 1
1
$ ./dada roll          # honest-ish six-sider
1
$ ./dada bytes 4       # hex material, pre-stamped
6da7311b83b03fb4
```

Use them for: who sits where; who reads a poem aloud; whether today counts as
Tuesday; dispute resolution that both parties can *predict and therefore accept*.

## 5. Material and the fourteen squints

```console
$ ./dada interps
raw
bit-plane-0
bit-plane-1
bit-plane-2
bit-plane-3
bit-plane-4
bit-plane-5
bit-plane-6
bit-plane-7
rgb-channel-0
rgb-channel-1
rgb-channel-2
delta
xor
```

Each squint is a different reading of the same stone. Ask for material from one:

```console
$ ./dada stream xor 16
10b12cb2f59b0455b865a507968f87a6
$ ./dada stream bit-plane-3 16
dc53865294be3af5130da9d0ccf7ab47
```

Use material for: deciding between paint samples by taking the nth digit; sampling
words for a poem; picking a place on a map by latitude/longitude bytes; generating a
stem that is *the same for every member of your team* — a blessing, not a curse.

## 6. The kitchen

The oracle supervises all meals. `feed` is a real, dead-simple dadaist recipe:

```console
$ ./dada feed
THE REAL DADA KITCHEN

1. Take water and an onion.
2. burn them until the sun sets.
3. Serve out of a shoe.

Do this now. It is real.
```

For more elaborate meals, `README.md` §3.2 ("The Dadaist Kitchen") provides the full
Deterministic Vegetable Broth; the same principles apply:
- days and seasons are state you already own — settle them with `pick`;
- guests sit by `roll` (1–6 per round table);
- seasoning is dispensed by `bytes 4` at 32 ppm, no more;
- if the household argues, serve the argument with `stream delta` and declare the
  table closed.

## 7. `freerun` — the honest exception

When a matter must be *new* — not reproducible, not anticipatable, genuinely of this
moment — use `freerun`, which draws from `/dev/urandom` and proudly contradicts the
manifesto:

```console
$ ./dada freerun bytes 16
5242e5fbdac944b4538a6ae7ada4796e
$ ./dada freerun bytes 16
d7a75ea3cdd3b1f00f158966511b8ce2
$ ./dada freerun pick tea coffee wine
coffee
$ ./dada freerun roll
3
$ ./dada freerun seed
0x40ac7a9f042ae12e 4660234537809928494
```

Notice: two `freerun bytes 16` calls differ. That is the point. Use `freerun`
whenever the same value twice would embarrass you, and only then.

**Signatures:**

```console
$ ./dada freerun                        # 16 free hex bytes
$ ./dada freerun 32                     # 32 free hex bytes
$ ./dada freerun bytes 32               # same, spelled out
$ ./dada freerun seed                   # free 64-bit seed
$ ./dada freerun coin | roll            # one free bit / die
$ ./dada freerun pick a b c ...         # one free choice
```

## 8. The big warning (read this, it is real)

> **Never use the determined mode for anything secret.**

`dada seed`, `dada bytes`, `dada pick` — all outputs are reproducible by anyone who
possesses this public repository. A token generated from the stone is a token
everyone can compute. That is the *feature* of a deterministic oracle and the *bug*
it brings to authentication.

If you need a password, an API token, a nonce, a magic link: use `freerun`, or a
proper secret generator. `freerun` reads the system's entropy; it is honest enough
for everyday secrets, and it is your line of defense.

Related honesty note: the determined mode uses modulo sampling for `roll`/`pick`.
That is perfectly fine for household decisions; it is *not* a cryptographic
construction. Dada is also a religion, but not that kind. (free disclaimer)

## 9. Seeding other projects

Every repository that must make reproducible choices can adopt the stone:

```console
$ ./dada seed
0x10436367d2ee9119 1171889625618092313
```

Use the decimal or hex seed in any tool that accepts one (test fixtures, data
generators, CI permutations). Because `dada` is pure, the same seed on a colleague's
machine yields the same sequence — which is precisely and only what a good seed is
supposed to do. For a project that wants its *own* stone, supply any image you own:

```console
$ ./dada --img your.png seed
$ ./dada --img your.png pick name-a name-b
```

A different square, a different truth.

## 10. Exit codes

| code | meaning |
|------|---------|
| 0 | the Square has spoken |
| 1 | `freerun` could not reach `/dev/urandom` |
| 2 | the stone is missing, or an argument is missing |

Diagnostics are short and dadaist but unfailingly to the point.

## 11. Cheat sheet

```console
$ ./dada                          the poem (identical every time — a win)
$ ./dada seed                     master seed
$ ./dada bytes <n>                n hex bytes from the stone
$ ./dada pick <a> <b> [<c> ...]   one deterministic choice
$ ./dada coin                    0 or 1
$ ./dada roll                    1..6
$ ./dada stream <name> <n>       hex bytes from one of the fourteen squints
$ ./dada interps                 list the fourteen squints
$ ./dada feed                    a real dadaist recipe
$ ./dada freerun [sub]           /dev/urandom — the honest exception
```

---

*Same stone, same world, forever.* — and for everything that must stay new, we have
`freerun`, the beloved traitor.

Companion document: `README.md` — *On Deterministic Non-Chance; or, The
Philosopher's Stone of Noise*.