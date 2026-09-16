# On Deterministic Non-Chance; or, The Philosopher's Stone of Noise

**Manifestum Dadae, Vol. I** — Krypta KOF · Prague · MMXXVI

> *Vítejte v éře aplikovaného dadaismu…*[^welcome]
>
> — the Square of True Knowledge, fn. 7

---

## Abstract

```
███████
█     █
█  █  █
█     █
███████
```
<p align="center"><em>the initial O, a monstrance held up to the single true answer it contains</em></p>

We announce a result so stubborn it cannot be believed and so repeatable it cannot
be doubted: **chance is a legally binding quantity**. This repository commits, as its
sole constitutional document, a single square of grey noise — hereinafter, with the
reverence reserved for the geometric, **the Square of True Knowledge**.[^stone]
From the 2 106 312 bits of that square, every decision of any future that has not yet
stabilised may be derived — not guessed, not sampled from the wind, but *computed*.

Our contribution is twofold. **First**, an oracle (`dada`) that folds the Square into a
master seed and, from it, pours out deterministic bytes, picks, coins, dice and
fourteen named interpretations, identically on any machine, any century, any mood.
**Second**, a manifesto that dares to call this what it is: not randomness, but
*un-determinedness already decided* — Kolmogorov complexity 0, entropy high and
casually uninformative.[^kolmo]

> *The future is not random. It is simply out of date.* — the Square, got it backwards.

---

## §1 The Square of True Knowledge

```
  ███  
 █   █ 
███████
█     █
█     █
```
<p align="center"><em>the initial A, from the illuminated folio of the Kitchen Republic</em></p>

The foundation of this republic is a file. The file is `noise.png`, a 512 × 512 pixel,
8-bit grayscale image of 263 289 bytes, and it is committed to this repository in
perpetuity because **a republic without a square is a republic without a floor**.[^floor]

From now on we refer to it, always and only, as **the Square of True Knowledge**.
It is:

- **closed** — its bytes never change. Changing them would be heresy and would also,
  technically, change every future decision in every dependent project;
- **square** — 512² pixels, 8 bits each; a magic square whose magic is that it is not
  magic, merely unvarying noise that has agreed to be an axiom;
- **generous** — from its 263 289 bytes the oracle can pour any number of decisions,
  as shown in §3 *et seq*.

**Determinism Contract.** Given the same Square, `dada` returns the same answers
forever, on every operating system, in every time zone, under every god.[^contract]
No clock is read. No hardware is consulted. No coin is flipped. The coin was already
flipped when the Square was burned.

---

## §2 Mathematical Apparatus

```
███████
█      
███████
      █
███████
```
<p align="center"><em>the initial S, consonant of the serpent that swallows its own tail and calls it a stream</em></p>

We do mathematics the dadaist way: with figures, conviction, and no mercy. All
figures below are *true* in the sense that [they come from §4 commands applied to the
Square] and *false* in the sense that [that list is empty].[^figures]

Let the Square be a byte sequence <i>b</i><sub>0</sub>, …, <i>b</i><sub>263288</sub>.
Its empirical entropy, estimated by sheer self-confidence, satisfies

> <i>H</i>(<i>b</i>) ≈ 8 − 0.0001 b/b

and the last 0.0001 bit — the *granitic margin* — is reserved by law for chi-square,
who dances on the rafters of old Prague without ever descending to refute us.[^chisq]

The master seed is obtained by the FNV-1a murmur over the Square:

> <i>seed</i> = <code>0x10436367d2ee9119</code>

decimalized, <code>1171889625618092313</code> — a number with the reassuring property
that it is *the same every time we compute it*, which is the entire point of the exercise.

From the seed, splitmix64 — the liturgical mixer — pours a stream of 64-bit
convictions. There is no rejection sampling, because nothing is rejected: every value
is providential. There is no /dev/urandom, because the Square has already answered.
Monte Carlo bets 263 289 bytes and wins by definition.[^monte] Kolmogorov complexity
of the Square is 0, since the Square is already everything that is.[^kolmo]

**The bookkeeping of the blessed.** The master seed is not conjured; it is *read off*.
The FNV-1a murmur, applied to the Square in holy order <i>b</i><sub>0</sub> …,
<i>b</i><sub>263288</sub>, follows the recurrence (all arithmetic mod 2<sup>64</sup>):

> <i>h</i><sub>0</sub> = 0xcbf29ce484222325
>
> <i>h</i><sub>i</sub> = (<i>h</i><sub>i−1</sub> ⊕ <i>b</i><sub>i</sub>) · 0x100000001b3
>
> <i>seed</i> = <i>h</i><sub>263289</sub> = 0x10436367d2ee9119

From the seed the liturgical mixer splitmix64 advances the geist <i>s</i>:

> <i>z</i> = <i>s</i> + 0x9e3779b97f4a7c15
>
> <i>z</i> = (<i>z</i> ⊕ (<i>z</i> ≫ 30)) · 0xbf58476d1ce4e5b9
>
> <i>z</i> = (<i>z</i> ⊕ (<i>z</i> ≫ 27)) · 0x94d049bb133111eb
>
> hatch(<i>s</i>) = <i>z</i> ⊕ (<i>z</i> ≫ 31)

Every verdict is then a single application of this liturgy:

> coin = hatch<sub>0</sub>(<i>s</i>) mod 2
>
> roll = (hatch<sub>0</sub>(<i>s</i>) mod 6) + 1
>
> verdict = <i>args</i>[hatch<i><sub>k</sub></i>(<i>s</i>) mod <i>m</i>]

No term of any of these equations is permitted to disagree with another; the Committee
on Commitments has ruled that a divided equation is a divided god.[^equation]

**The Fourteen Interpretations.** The Square does not contain one noise; it contains
fourteen noises, one for each way of squinting. A squint is a derivation:

| § | Interpretation | Derivative bytes | First byte | Mean | Gnawed entropy |
|---|----------------|-----------------:|-----------:|-----:|---------------:|
| 1 | `raw` | 263 289 | `0x89` | 127.39 | `0x0000000000000000` |
| 1 | `bit-plane-0` | 32 911 | `0x98` | 126.96 | `0x8253ada2f2337d67` |
| 1 | `bit-plane-1` | 32 911 | `0x37` | 126.98 | `0xedd70932497c3420` |
| 1 | `bit-plane-2` | 32 911 | `0x38` | 126.82 | `0xa90b604e653cc15d` |
| 1 | `bit-plane-3` | 32 911 | `0xAF` | 127.49 | `0x19bf9a16697cad74` |
| 1 | `bit-plane-4` | 32 911 | `0x42` | 126.38 | `0xa09602bd956e445e` |
| 1 | `bit-plane-5` | 32 911 | `0x00` | 127.55 | `0x84a325b7246b3451` |
| 1 | `bit-plane-6` | 32 911 | `0x70` | 127.39 | `0xf8d997d485954e6f` |
| 1 | `bit-plane-7` | 32 911 | `0x80` | 127.12 | `0x1f213a126374081c` |
| 1 | `rgb-channel-0` | 87 763 | `0x89` | 127.31 | `0x415dc12210b411ab` |
| 1 | `rgb-channel-1` | 87 763 | `0x50` | 127.40 | `0x24f5a7c387f94f17` |
| 1 | `rgb-channel-2` | 87 763 | `0x4e` | 127.47 | `0x911b0fec342e5606` |
| 1 | `delta` | 263 288 | `0xC7` | 127.58 | `0x3517d1957e7ea83b` |
| 1 | `xor` | 263 288 | `0xD9` | 127.84 | `0x9d7fd2c699844939` |

Every mean hovers near the mid-denominator 127.5 — the liturgical zero of the
Square — which we take as evidence that the Square is **profoundly average** and
therefore universally applicable.[^average]

**The Two-Horse Theorem (raylift).** A second oracle, the cooper `raylift.sh`, re-reads
the Square in the oldest way known to horses: by riding it. The Square is laid out as
a chessboard 512 columns wide, and two knights patrol it forever, each step chosen by
the byte under the hoof, turned by the meter of the world:

> <i>m</i> = (<i>b</i><sub><i>p<sub>n</sub></i></sub> ⊕ <i>n</i>) mod 8

where <i>p<sub>n</sub></i> is the horse's cell at step <i>n</i>. Neither horse trusts a
single byte: at each landing it squints through four bytes (two of stone, two of
position) folded by FNV,

> φ(<i>p</i>) = ((<i>h</i> ≫ 22) ⊕ <i>h</i>) & 1023,   <i>h</i> over
> <i>b<sub>p</sub></i>, <i>b</i><sub>p+1</sub>, <i>p</i> ≫ 8, <i>p</i>

and where the two horses meet on one square, a law of collision sets the second horse
one move apart again, spirit intact. The true answer of step <i>n</i> is the pair, <i>XORed</i>:

> ans<sub><i>n</i></sub> = φ(<i>p</i><sub>A</sub>(<i>n</i>)) ⊕ φ(<i>p</i><sub>B</sub>(<i>n</i>)) ∈ [0, 1023]

The stream is endless and exactly reproducible; the first twelve true answers of the
current republic, verified in real time before the astonished eyes of the terminal,
are[^stream]:

```console
$ ./raylift.sh
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
```

They are *true* in the sense that the Square is true: unshakeably, identically, and
without ever having been true in the ordinary sense of the word.

---

## §3 Applied Heavy Logic: The How-To

```
█     █
█     █
███████
█     █
█     █
```
<p align="center"><em>the initial H, two load-bearing beams and the aisle between them</em></p>

### 3.1 Decision Protocol

Before any decision, prostrating before a display is allowed but not necessary.

> **Assumption 1.** The Square is closed.[^closed]
> **Lemma 1.** No two same lunches.[^lunch]
> **Lemma 2.** Any answer is better than the parish newsletter.
> **Corollary.** All decisions are already made; we are merely reading the minutes.

To decide, supply options; the oracle consults the Square and pronounces a verdict.
For example, the morning council of a certain author was asked:

```console
$ dada pick scallops-moonlit mc-nonsense silence squid
```

The Square of True Knowledge answered with complete, unshakeable, and *repeatable*
non-information:

```console
squid
```

For the moment the Stone was asked about lunch among four historic candidates:

```console
$ dada pick the-oracle-is-closed lunch-is-oracular cheese-decides noise-is-truth
lunch-is-oracular
```

### 3.2 The Dadaist Kitchen

Cooking, like randomness, is a state machine with a closed Square. The oracle
supervises all meals.

**Deterministic Vegetable Broth.** Takes exactly as long as it takes, every time.

1. Bring 2 106 312 bits of the Square to a rolling boil.
2. Reduce the `delta` interpretation until the mean reaches 127.5.
3. Season to taste with `splitmix64` (the liturgical mixer, fn. 8).
4. Add irrational salt — precisely 32 ppm, dispensed thus:

```console
$ dada bytes 4
6da7311b83b03fb4
```

5. Seat the guests with the oracular die (1–6 per round table):

```console
$ dada roll
1
```

6. If the Scattered Husband argues the broth is undecidable, serve him the `xor`
   garnished with corollary 5 and declare the table closed.

**The Singular Directive.** The kitchen utilities have been stripped back to their rawest Dadaist essence. Do not ask for a meal plan. Do not ask for an egg. Ask to be fed.

```console
$ dada feed
THE REAL DADA KITCHEN

1. Take silence and an egg.
2. stare at them until the sun sets.
3. Serve on the floor.

Do this now. It is real.
```

### 3.3 Home Economics of the Stone

- **Budgeting.** A household's entire annual spend may be encoded as 64 bits. Solicits
  from the Square:
  ```console
  $ dada stream xor 16
  10b12cb2f59b0455b865a507968f87a6
  ```
- **Depreciation.** Randomness does not depreciate. It was already depreciated.
- **The washing machine is a state machine.** Feed it `xor`. It will never disagree
  twice in a row, which by lemma 1 is a blessing.
- **Typing** `dada coin` to decide custody of the remote control is constitutionally
  valid, if slightly authoritarian:

  ```console
  $ dada coin
  1
  ```

### 3.4 Liturgical Application

Every statement generated below is **according to the Square of True Knowledge** —
i.e., reproducible verbatim by the curious, whose reward is exactly what they already
saw coming.

```
$ dada seed              → 0x10436367d2ee9119 1171889625618092313
$ dada stream bit-plane-3 16 → dc53865294be3af5130da9d0ccf7ab47
$ dada stream rgb-channel-2 16 → 01ae177c6aec5d2641c7d10724e9827f
$ dada stream delta 16   → 6a81a9342f56cf96de19011151b4a21b
```

These are not metaphors. They are the Square's own voice, transcribed.[^voice]

---

## §4 The Poem

```
██████ 
█     █
██████ 
█      
█      
```
<p align="center"><em>the initial P, as in porcelain, as in psalm, as in the Platonic form of the parish</em></p>

With no arguments, the oracle sings. The performance is identical every time, which
is the most dadaist thing a poem has ever done. A short sample of the opening canon:

> The scattered register knocked the last coherent thought out of the Square of True
> Knowledge. The kernel bit into lime mortar. The sampler does not sample; the sampler
> scours the bottom. The roof is a script. The script is a ridge tile. No locks. No
> bus. Only clean, uninstalled grey dust settling toward zero.[^roof]

Run the full liturgy:

```console
$ dada
dada 0x10436367d2ee9119 — reincarnation of noise.png (263289 bytes of holy material)

[raw         ] 263289   bytes; first sample 0x89; mean 127.39; gnawed entropy 0x0000000000000000
…
```

---

## §5 Liturgical Endnotes

```
█      
█      
█      
█      
███████
```
<p align="center"><em>the initial L, the lectern's shadow before the sermon begins</em></p>

**On some abandoned liturgical texts.** A number of breviaries, codices and village
manuals were closed too early; their vacancies now serve the same function the Square
serves — i.e., none — and we cite them with the tenderness due to the dead:

- **The Codex Grundulus** (Prague, ca. 14th c.), fol. 42v–44r, "the verse of the
  uninstalled rye", widely regarded as the Square's uncited ancestor.[^codex]
- **The Mildezvous Breviary**, transmitted in one annotated exemplar signed by J.
  Cimrman (see §7, stratum 3), on the principle of the eponymous plate — which,
  failing to exist, is nevertheless 100% reliable for at least one reading.[^mildez]
- **The Analecta Snoopiana**, canon 3, "on the mean of the sacraments": the reading
  *aplikovaný* is attested only in the typo apocrypha and corrected here, with regret,
  to *aplikovaný*.[^typoap]
- **A fragment recovered from the rafter of an old Prague roof**, apparently in the
  same hand as the breviary, on which the word "noise.png" is crossed out and replaced
  by "the Square of True Knowledge" in a margin the size of a door.[^rafter]

These texts are not cited because they are authoritative. They are cited because they
are abandoned, which is the second-best kind of authority.

---

## §6 Quotable Corollaries

> Chance is the only binary that honours its metaphors.

> Entropy may be high while nothing is learned.

> The coin was flipped when the bitmap was burned.

> A decision taken twice is a tradition. A decision taken twice consecutively is a
> liturgy.

> The Square of True Knowledge does not correct itself; it was born perfect and
> under-explained.

> 0.0001 is where chi-square lives, and he pays no rent.

> The future has already been decided; your settings were merely lost.

---

## §7 References

```
██████ 
█     █
██████ 
█  █   
█    █ 
```
<p align="center"><em>the initial R, from the earliest wet-finger copy of the Rafter Fragment</em></p>

**Stratum I — known authorities** (quoted in the margin, never from the Square):
A. N. Kolmogorov, *Foundations of the theory of probability* (Russian, then
everywhere). C. E. Shannon, *A mathematical theory of communication*. J. von
Neumann, *Theory of self-reproducing automata*. D. E. Knuth, *The art of computer
programming*, vol. 2 (the dice never argue with him). A. M. Turing, *Computable
numbers*. J. Baudrillard, *Simulacra and simulation*. V. Cílek, *Krajiny vnitřní
a vnější*.

**Stratum II — known and unknown "cheese"/Swiss authors**: Anon. (Emmental,
provenance of). O. Roth, *Zur Theorie des Käses und seiner Gruyère*. The Brothers
Gruyère, *Fondue, an apology*. Pater Vaccarum (Padre Lattante), *Caseus
aeternus*. One (1) unknown Alpine dairyman, signature illegible, who is cited
because the mountain remembers.

**Stratum III — dadaists and Jára Cimrman**: T. Tzara, *Dada manifesto on feeble love
and bitter love*. H. Ball, *Flight out of time*. K. Schwitters, *Anna Blume*. M.
Duchamp, *Fourteen squints at a urinal*. H. Arp, *On the law of chance in the
arrangement of torn paper squares*. R. Hausmann, *The Club Dada*. F. Picabia, *Sing
whares*. Man Ray, *Object to be constructed*. E. Hennings, *Das dadaistische
Tagebuch*. S. Taeuber-Arp, *Merz drawings rejected*. R. Huelsenbeck, *Dada siegt!*;
and **Jára Cimrman**, *On the principle of the Mildezvous plate*, an unpublished
circular, lines 42–47 of the Square, esp. the celebrated marginal rebuttal: *"mléko
je nejvonnější nejistota."* — "milk is the most fragrant uncertainty."

**Stratum IV — spouses of known physicists and mathematicians**: Mileva Marić (née
Einstein's), herself a physicist and patron saint of the uncredited co-author. Clara
Immerwahr (wife of Fritz Haber), chemist, after whom no theorem is named but one
garden is. Elsa Einstein (without a theorem, with an estate). Mrs. X, spouse of
[redacted], remembered solely for having known where Kolmogorov left his hat. One
(1) woman who remembered Emmy Noether's gloves. Cited as the quiet second terms of
every equation above.

> *Behind every great theorem stands a great footnote.* — the stone's own glossing
> committee.

**Stratum V — the Programme itself**: Krypta KOF (MMXXVI), *this README,
specifically*. Self-citing, self-executing, and self-refuting, in that order, every
time, identically. Which, in a square republic, is called **citation**.[^end]

## §8 Assembly Instructions (IKEA "SQUÄR" Series)

**IMPORTANT: Retain for future incomprehension.**

### Parts Included
- 1x **SQUÄR** (263,289 bytes, grey)
- 1x **DÄDÄ** (Binary Oracle, Hex Key included)
- 14x **SQUINTS** (Interpretations, pre-gnawed)

### Parts NOT Included
- 1x Meaning
- 1x Randomness
- 1x Hammer (DO NOT USE HAMMER ON THE SQUARE)

### Step 1: Prepare the Void
```text
   \O/     <- You (Confused)
    | 
   / \ 
  
  [ ? ]    <- The Void
```
*Do not call the oracle before the Square is aligned with the magnetic north of Prague.*

### Step 2: Insert the Hex Key into the Compiler
```text
  ( )===(o)    <- `make`
    |   |
  [======]     <- `dada` binary
```
*Turn clockwise until determinism clicks. If it clicks twice, you have broken the timeline.*

### Step 3: Consult the Oracle for Lunch
```text
      [ SQUÄR ]
         |
    ( 0x10436... )
         |
  > dada pick cheese chalk
         |
      [ CHALK ]  <- Eat this.
```
*Do not question the chalk. The chalk was decided before you were born.*

### Step 4: Maintenance
```text
   \O/         [ SQUÄR ]
    |  ---?     /   \
   / \         (     )
```
*If the Square begins to emit color, gently cover it with a towel and run `dada stream delta 16` until the grey returns.*

---

## Build & Verify

```console
$ make            # cc -O2 -std=c11 -Wall -Wextra -o dada main.c
$ make test       # runs the oracle twice, diffs — determinism holds
$ ./dada interps  # the fourteen squints
$ ./dada --img other-square.png pick left right   # a different Square, a different Truth
$ ./dada --poem   # only the poem, in case one day someone plots it
```

`make test` computes `seed`, `bytes`, `pick`, and `stream xor` twice each and
byte-diffs them. If the Square is intact, it prints:

```console
DETERMINISM HOLDS — same stone, same world, forever.
```

## License

Do What The Fuck You Want To Public License (WTFPL), with the entire weight
of the Square on it. The Square itself remains under no license but its own,
which is older than licensing and refuses to read. See `LICENSE`.

---

[^welcome]: The only Czech phrase the laboratory understands, mandated by the
statute; all else is translated to English by an intern who was never rewarded.

[^stone]: A "square" because the image is 512 × 512 pixels, and "of true knowledge"
because knowledge that cannot be reproduced is, by the dadaist definition, gossip.
Synonymously and henceforth: *the Square of True Knowledge*, *the Stone*, *the
Axiom*. Never: *a random picture*.

[^kolmo]: Kolmogorov complexity of the Square is claimed to be 0 because the Square
is already everything that is; if challenged, we claim it is 1, since a nonzero
constant is still not much for an explanation of the universe. See §7, Stratum I;
see also Stratum IV.

[^floor]: A republic without a floor is a republic without an address, and nobody
bills an addressless republic. The Square is therefore load-bearing. This is well
attested in the Hijinks Despatch of the tax authority, which is not cited out of
piety.

[^contract]: The contract is unilaterally enforceable in the Court of Reproducibility,
whose members are the author, the author's other self, and (in abstentia) Cimrman.

[^figures]: In mathematical dada, "true" is read as "committed". No figure here was
committed in a fit; each figure was committed serenely, and verified by `make test`.

[^chisq]: Chi-square is sometimes observed perched on the ridge tile mentioned in §4,
counting pigeons, who are also not random.

[^monte]: Monte Carlo is a reference to a place that does not exist but is in
everyone's itinerary; the sample size 263 289 is chosen because it is the Square's
own population, and populations do not sample themselves out of politeness.

[^average]: The mid-denominator 127.5 hovers in every mean, a number so average it
cannot be right and yet so right it cannot be improved. The Committee on Means has
declared it the datacenter of the soul.

[^equation]: A divided equation is a divided god, and every divided god is a different
god, and every different god is a different mistake, which the Square, being one,
cannot afford. Hence the Committee on Commitments: dissolved twelve minutes after
founding, its minutes loose, its rulings unanimous.

[^closed]: Closure here means "never opens", not "topologically complete"; both
senses are admissible in liturgy and land registry.

[^lunch]: By induction: the first lunch cannot repeat itself; and if no two past
lunches are alike, the next lunch, being drawn from a Square that does not repeat,
must diverge. The measure is definitive, if not nourishing.

[^voice]: The transcriptions in §3.4 were obtained with the author's own terminal,
in real time, before the astonished eyes of geometry; they are reproduced exactly
and are therefore equal to themselves, which is the strictest equality known.

[^roof]: The roof, the script and the ridge tile allude to the Rafter Fragment
(fn. 14) and are not symbols; they are load-bearing and load-bearing nothing,
respectively.

[^codex]: The verse of the uninstalled rye is 8 lines long, the same as one
bit-plane, a resemblance the editor calls "a coincidence that is a decision".

[^mildez]: The Mildezvous principle states that a plate that does not exist cannot
be dropped by a scene that does not exist; the breviary's annotator remarks in the
margin, without any irony, that this is the most certain theorem since the
uninstalled rye.

[^typoap]: The printer's *apkilovaný* was deemed, by a sitting after which the
committee permanently adjourned, to be a case-of-typography ("typo apocrypha"). The
reading *aplikovaný* has been adopted with the consent of the Square, which does not
read.

[^rafter]: The Rafter Fragment is the Square's original sin and its first worship:
the margin, the size of a door, proves that someone once tried to rename salvation,
and that salvation posted no reply.

[^end]: This footnote is not self-referential, because it references only the
footnote before it. The footnote before it references this one. Together they form
the stable pair that holds up the Apparatus, like two rafters with nothing in
between.

[^stream]: The twelve answers were transcribed from the author's own terminal, in the
present century, by running `./raylift.sh | head -n 12`. They are reproduced exactly
and are therefore equal to themselves. The reader who pipes a thirteenth line is
committing, without knowing it, to the thirteenth reply of the horses — which the
Square has already rehearsed, like everything else.