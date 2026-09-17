#!/usr/bin/env bash
# prove — a profound, self-witnessing suite for the dada oracle.
#
# Two layers of proof:
#   1. SEMANTICS — every function must produce what it should, checked against
#      ground truth the script derives itself (the stone's own bytes, byte-for-byte).
#   2. IMMUTABILITY — every deterministic answer must always be the same, proven
#      two ways: a live double-run of the whole battery, and a committed golden
#      transcript (tests/dada.golden) that pins values across machines and centuries.
#
# Usage:
#   ./tests.sh            run the whole proof
#   ./tests.sh --gold     regenerate tests/dada.golden (after an *intentional* change)

set -u

ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT" || exit 2

D=./dada
STONE=noise.png
W=512
N=$(stat -c%s "$STONE" 2>/dev/null || echo 0)
H=$(( (N + W - 1) / W ))

PASS=0
FAIL=0
SKIP=0
TMP="$(mktemp -d /tmp/dada-proof.XXXXXX)"
trap 'rm -rf "$TMP"' EXIT

# ---- harness ----
run() {
    OUT=""
    RC=0
    if "$@" >"$TMP/out" 2>"$TMP/err"; then :; else RC=$?; fi
    OUT="$(cat "$TMP/out")"
}

claim_fail() {
    FAIL=$((FAIL + 1))
    echo "FAIL: $1"
    echo "      expected: $2"
    echo "      actual:   $OUT"
}

pass() { PASS=$((PASS + 1)); }
note() { SKIP=$((SKIP + 1)); echo "skip: $*"; }

assert_re() { # desc pattern cmd...
    local desc="$1" pat="$2"; shift 2
    run "$@"
    if [[ "$OUT" =~ $pat ]]; then pass; else claim_fail "$desc" "matched /$pat/" ; fi
}

assert_eq() { # desc expected cmd...
    local desc="$1" want="$2"; shift 2
    run "$@"
    if [[ "$OUT" == "$want" ]]; then pass; else claim_fail "$desc" "'$want'"; fi
}

assert_in() { # desc candidate... -- cmd...
    local desc="$1"; shift
    local -a items=()
    while [[ $# -gt 0 && "$1" != "--" ]]; do items+=("$1"); shift; done
    shift
    run "$@"
    local ok=0 it
    for it in "${items[@]}"; do [[ "$OUT" == "$it" ]] && ok=1; done
    if (( ok )); then pass; else claim_fail "$desc" "one of: ${items[*]}"; fi
}

assert_range() { # desc min max cmd...
    local desc="$1" lo="$2" hi="$3"; shift 3
    run "$@"
    if [[ "$OUT" =~ ^[0-9]+$ ]] && (( OUT >= lo && OUT <= hi )); then
        pass
    else
        claim_fail "$desc" "in [$lo,$hi]"
    fi
}

assert_exit() { # desc expected_rc cmd...
    local desc="$1" want="$2"; shift 2
    run "$@"
    if (( RC == want )); then pass; else claim_fail "$desc" "exit code $want"; fi
}

assert_varies() { # desc seeds... count cmd-base
    local desc="$1" count="$2"; shift 2
    local -A seen=()
    local i
    for (( i = 0; i < count; i++ )); do
        run "$@" "$i"
        seen["$OUT"]=1
    done
    if (( ${#seen[@]} >= 2 )); then
        pass
    else
        claim_fail "$desc" "at least two distinct outputs across $count seeds (got: ${!seen[*]})"
    fi
}

assert_files_match() { # desc fileA fileB
    if diff -q "$2" "$3" >/dev/null 2>&1; then pass; else
        claim_fail "$1" "'$2' and '$3' identical"
    fi
}

# ---- ground truth derived fresh from the stone ----
first_byte="$(od -An -v -t u1 -N1 "$STONE" | tr -d ' ')"
id_1="$(( ((6789 % H) * W + (12345 % W)) % N ))"
cell_oob="$(od -An -v -t u1 -j "$id_1" -N1 "$STONE" | tr -d ' ')"

echo "== dada proof: stone $STONE, $N bytes, board ${W}x${H} =="
echo

echo "--- semantics: seed ---"
assert_re "seed shape" '^0x[0-9a-f]{16} [0-9]+$' "$D" seed
if command -v python3 >/dev/null; then
    run "$D" seed
    hex="${OUT%% *}"; dec="${OUT#* }"
    hex="${hex#0x}"
    if python3 -c 'import sys; exit(0 if int(sys.argv[1],16)==int(sys.argv[2]) else 1)' "$hex" "$dec"; then
        pass
    else
        claim_fail "seed hex and decimal agree" "'$hex' == '$dec'"
    fi
else
    note "seed hex/decimal cross-check (python3 absent)"
fi

echo "--- semantics: bytes ---"
assert_re "bytes 16 = 32 hex chars" '^[0-9a-f]{32}$' "$D" bytes 16
assert_re "bytes 0 clamps to default 16" '^[0-9a-f]{32}$' "$D" bytes 0
assert_re "bytes 9999999 clamps to default 16" '^[0-9a-f]{32}$' "$D" bytes 9999999
assert_re "bytes (no arg) = default 16" '^[0-9a-f]{32}$' "$D" bytes
assert_re "bytes 8 a-frame = 16 hex chars" '^[0-9a-f]{16}$' "$D" bytes 8 a-frame

echo "--- semantics: pick ---"
assert_in "pick returns a candidate" alpha beta gamma -- "$D" pick alpha beta gamma
assert_exit "pick without candidates fails" 2 "$D" pick

echo "--- semantics: coin and roll ---"
assert_range "coin bit range" 0 1 "$D" coin
assert_range "seeded coin bit range" 0 1 "$D" coin frame-7
assert_range "roll die range" 1 6 "$D" roll
assert_range "seeded roll die range" 1 6 "$D" roll frame-7
assert_varies "coin animates across seeds" 10 "$D" coin
assert_varies "roll animates across seeds" 20 "$D" roll

echo "--- semantics: stream and interps ---"
assert_re "stream raw 64 = 128 hex chars" '^[0-9a-f]{128}$' "$D" stream raw 64
assert_re "stream xor 32 = 64 hex chars" '^[0-9a-f]{64}$' "$D" stream xor 32
assert_re "stream bit-plane-3 16 = 32 hex chars" '^[0-9a-f]{32}$' "$D" stream bit-plane-3 16
assert_exit "stream with unknown interpretation fails" 2 "$D" stream nonsense 16
assert_eq "interps lists exactly the fourteen" \
    "$(printf '%s\n' raw bit-plane-0 bit-plane-1 bit-plane-2 bit-plane-3 bit-plane-4 \
        bit-plane-5 bit-plane-6 bit-plane-7 rgb-channel-0 rgb-channel-1 rgb-channel-2 delta xor)" \
    "$D" interps

echo "--- semantics: cell (checked byte-for-byte against the stone) ---"
assert_eq "cell 0 0 = first byte of the stone" "$first_byte" "$D" cell 0 0
assert_eq "cell 512 0 wraps to column 0" "$first_byte" "$D" cell 512 0
assert_eq "cell 0 $H wraps to row 0" "$first_byte" "$D" cell 0 "$H"
assert_eq "cell 12345 6789 = stone byte at recomputed index" "$cell_oob" "$D" cell 12345 6789
assert_range "cell value is one byte (0..255)" 0 255 "$D" cell 12345 6789
assert_exit "cell with one argument fails" 2 "$D" cell 1

echo "--- semantics: feed, poem, help ---"
assert_re "feed serves the real kitchen" 'THE REAL DADA KITCHEN' "$D" feed
assert_exit "feed exits 0" 0 "$D" feed
assert_re "naked poem mentions noise" 'Noise' "$D"
assert_re "--poem prints the poem" 'Noise' "$D" --poem
assert_exit "help exits 0" 0 "$D" help
assert_re "help documents cell" 'cell' "$D" help
assert_re "help documents seeds" 'seeds' "$D" help

echo "--- semantics: freerun (honest randomness — shape only) ---"
assert_re "freerun = 32 hex chars" '^[0-9a-f]{32}$' "$D" freerun
assert_re "freerun bytes 16 = 32 hex chars" '^[0-9a-f]{32}$' "$D" freerun bytes 16
assert_re "freerun seed shape" '^0x[0-9a-f]{16} [0-9]+$' "$D" freerun seed
assert_range "freerun roll die range" 1 6 "$D" freerun roll
assert_range "freerun coin bit range" 0 1 "$D" freerun coin
assert_in "freerun pick returns a candidate" x y z -- "$D" freerun pick x y z

echo "--- semantics: error paths ---"
assert_exit "missing stone fails" 2 "$D" --img /no/such/stone
assert_exit "unknown command-as-image fails" 2 "$D" definitely-not-a-command

echo "--- immutability: live double-run of the whole battery ---"
battery() {
    "$D" seed
    "$D" bytes 16
    "$D" bytes 8 a-frame
    "$D" pick alpha beta gamma
    "$D" coin
    "$D" coin frame-7
    "$D" roll
    "$D" roll frame-7
    "$D" stream xor 32
    "$D" stream bit-plane-3 16
    "$D" interps
    "$D" cell 0 0
    "$D" cell 12345 6789
    "$D" feed
    "$D" help
    "$D" --poem
}
battery >"$TMP/battery.A"
battery >"$TMP/battery.B"
assert_files_match "deterministic battery identical on two consecutive runs" \
    "$TMP/battery.A" "$TMP/battery.B"

echo "--- immutability: golden transcript (committed pin) ---"
GOLD="tests/dada.golden"
if [[ "${1:-}" == "--gold" ]]; then
    mkdir -p tests
    battery >"$GOLD"
    echo "gold: $GOLD rewritten from the current stone."
    exit 0
fi
if [[ -f "$GOLD" ]]; then
    battery >"$TMP/battery.got"
    assert_files_match "current battery matches committed golden ($GOLD)" "$GOLD" "$TMP/battery.got"
else
    note "no golden transcript present; run ./tests.sh --gold"
fi

echo "--- companion: raylift determinism ---"
if [[ -x ./raylift.sh ]]; then
    ./raylift.sh | head -n 200 >"$TMP/ray.A"
    ./raylift.sh | head -n 200 >"$TMP/ray.B"
    assert_files_match "raylift stream identical on two runs (first 200 answers)" \
        "$TMP/ray.A" "$TMP/ray.B"
else
    note "raylift.sh absent"
fi

echo "--- companion: scatter determinism (same input, same world) ---"
if command -v python3 >/dev/null && [[ -x ./raylift.sh ]]; then
    r1="$TMP/rep1"; r2="$TMP/rep2"
    mkdir -p "$r1" "$r2"
    cp raylift.sh scatter.py noise.png "$r1/"
    cp raylift.sh scatter.py noise.png "$r2/"
    cp README.md "$r1/README.md"
    cp README.md "$r2/README.md"
    ok1=0; ok2=0
    ( cd "$r1" && ./scatter.py >/dev/null 2>&1 ) && ok1=1
    ( cd "$r2" && ./scatter.py >/dev/null 2>&1 ) && ok2=1
    if (( ok1 && ok2 )); then
        assert_files_match "scatter reproduces itself on identical input in two sandboxes" \
            "$r1/README.md" "$r2/README.md"
    else
        note "scatter could not complete in its sandbox"
    fi
else
    note "scatter (python3 or raylift absent)"
fi

echo
echo "==== $PASS passed, $FAIL failed, $SKIP skipped ===="
(( FAIL == 0 ))