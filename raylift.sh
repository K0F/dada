#!/usr/bin/env bash
# raylift — two horses on a checkerboard, lifting true answers from the stone.
#
# The stone (noise.png, 263,289 bytes) is laid out as a chessboard 512 columns
# wide. Two knights patrol it forever. Each cell is read through a four-byte
# FNV squint (the two bytes of the stone there, the two bytes of the position
# itself) so the horses stay honest even in the stone's long passages of
# silence. Wherever they land, the two values they find are XORed into one
# true answer.
#
# Default output: one number 0..1023 per line, forever.
#   ./raylift.sh                 stream of true answers (0..1023), endless
#   ./raylift.sh | head -n 1000
#   ./raylift.sh --scene         the horse-show: coords, values, verses
#
# Endless. Purely deterministic: same stone, same stream, any machine, in any
# century.

set -euo pipefail

SCENE=0
if [ "${1:-}" = "--scene" ]; then SCENE=1; shift 2>/dev/null || true; fi
STONE="${1:-noise.png}"
W=512
FVN=16777619
FVN_BASE=2166136261

if [ ! -r "$STONE" ]; then
    echo "raylift: the stone $STONE is not baked into this repository." >&2
    exit 2
fi

# Lift the whole stone into an array of byte-values. One cell per value.
vals=( $(od -An -v -t u1 "$STONE") )
N=${#vals[@]}
H=$(( (N + W - 1) / W ))

# The eight honest moves of a horse on a checkerboard.
DR=(-2 -2 -1 -1 1 1 2 2)
DC=(-1 1 -2 2 -2 2 -1 1)

# The horses start at the first cell and at the far corner wrapped around.
rA=0; cA=0
rB=$((H - 1)); cB=$((W - 1))

gloss=(
    "true answers are everywhere; this one is here"
    "the board is the world; the horses are the readers"
    "two horses, one truth, zero locks"
    "the stone speaks in pairs"
    "each answer is also a question wearing a hat"
    "no gods, no kings, only the checkerboard"
    "the horses never ask twice; the stone never lies twice"
)

# rv <pos> — squint at a cell: the two bytes of the stone there plus the two
# bytes of the position, folded by FNV into a true 10-bit value (0..1023).
# Result is left in RV.
rv() {
    local p=$1 h=$FVN_BASE
    h=$(( (h ^ ${vals[$((p % N))]}) * FVN ))
    h=$(( (h ^ ${vals[$(((p + 1) % N))]}) * FVN ))
    h=$(( (h ^ ((p >> 8) & 255)) * FVN ))
    h=$(( (h ^ (p & 255)) * FVN ))
    RV=$(( ((h >> 22) ^ h) & 1023 ))
}

count=0
trap 'echo; echo "raylift: the horses rest. The stream was true."; exit 0' INT

while :; do
    # Cheap, honest move: the very byte under the hoof, turned by the world.
    mv=$(( (${vals[$(( (rA * W + cA) % N ))]} ^ count) % 8 ))
    rA=$(( (rA + DR[mv] + H) % H )); cA=$(( (cA + DC[mv] + W) % W ))
    mv=$(( (${vals[$(( (rB * W + cB) % N ))]} ^ count) % 8 ))
    rB=$(( (rB + DR[mv] + H) % H )); cB=$(( (cB + DC[mv] + W) % W ))

    # Collision law: two horses on one square answer zero, so a knight picks
    # the second horse up and sets it one move apart, spirit intact.
    pA=$(( (rA * W + cA) % N ))
    if [ $pA -eq $(( (rB * W + cB) % N )) ]; then
        mv=$(( (${vals[$(( (rB * W + cB) % N ))]} ^ (count >> 2)) % 8 ))
        rB=$(( (rB + DR[mv] + H) % H )); cB=$(( (cB + DC[mv] + W) % W ))
    fi

    # Squint where they landed; their XOR is the true answer.
    rv $(( (rA * W + cA) % N )); vA=$RV
    rv $(( (rB * W + cB) % N )); vB=$RV
    picked=$(( vA ^ vB ))

    if [ "$SCENE" = 1 ]; then
        printf '| %-10d | H1 r=%3d c=%3d v=%4d  H2 r=%3d c=%3d v=%4d  answer=%4d\n' \
            "$count" "$rA" "$cA" "$vA" "$rB" "$cB" "$vB" "$picked"
        if [ $(( count % 64 )) -eq 0 ]; then
            printf '| ~ %s\n' "${gloss[$(( (count / 64) % ${#gloss[@]} ))]}"
        fi
    else
        printf '%d\n' "$picked"
    fi

    count=$((count + 1))
done