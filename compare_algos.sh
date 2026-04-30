#!/usr/bin/env bash
set -euo pipefail

DATASET="${1:-/home/imorozko/maga/fdcd/data/adult_super_trimmed.csv}"
FDCD_DIR="/home/imorozko/maga/fdcd"
CPP_BINDINGS="/home/imorozko/maga/Desbordante/build/src/python_bindings"
OUT="/tmp/dc_compare"

mkdir -p "$OUT"
DATASET="$(realpath "$DATASET")"

echo "Dataset: $DATASET"
echo "Artifacts in: $OUT"
echo ""

# ── C++ HybridDC ──────────────────────────────────────────────────────────────
echo "=== Running HybridDC (C++) ==="
EVIDENCE_DUMP="$OUT/cpp_evidences.txt" \
PREDICATE_DUMP="$OUT/cpp_predicates.txt" \
python3 - <<PYEOF 2>"$OUT/cpp_stderr.txt"
import sys
sys.path.insert(0, "$CPP_BINDINGS")
import desbordante

t = desbordante.dc.algorithms.HybridDC()
t.load_data(table=("$DATASET", ",", True))
t.execute(threads=1, shard_length=0)
dcs = t.get_dcs()
with open("$OUT/cpp_dcs.txt", "w") as f:
    for dc in dcs:
        f.write(str(dc) + "\n")
print(f"C++ DCs: {len(dcs)}", flush=True)
PYEOF
cat "$OUT/cpp_stderr.txt"

# ── Java fdcd ─────────────────────────────────────────────────────────────────
echo ""
echo "=== Running Java fdcd ==="
(
  cd "$FDCD_DIR"
  PREDICATE_DUMP="$OUT/java_predicates.txt" \
  EVIDENCE_DUMP="$OUT/java_evidences.txt" \
  java -jar target/discoverDCs.jar "$DATASET" -e HEI -o "$OUT/java_dcs.txt" \
    >"$OUT/java_stderr.txt" 2>&1
)
echo "Java DCs: $(wc -l < "$OUT/java_dcs.txt" | tr -d ' ')"
grep -E "\[fdcd\].*(time|Time)" "$OUT/java_stderr.txt" || true

# ── Compare & plot ────────────────────────────────────────────────────────────
echo ""
echo "=== Comparing artifacts ==="
python3 - <<'PYEOF'
import re, sys

OUT = "/tmp/dc_compare"

# ── Timing ────────────────────────────────────────────────────────────────────
def parse_ms(text, pattern):
    m = re.search(pattern, text)
    return int(m.group(1)) if m else 0

cpp_log  = open(f"{OUT}/cpp_stderr.txt").read()
java_log = open(f"{OUT}/java_stderr.txt").read()

cpp_evi  = parse_ms(cpp_log,  r"\[HybridDC\] Evidence time: (\d+)ms")
cpp_inv  = parse_ms(cpp_log,  r"\[HybridDC\] Inversion time: (\d+)ms")
java_evi = parse_ms(java_log, r"\[fdcd\] Evidence time: (\d+)ms")
java_inv = parse_ms(java_log, r"\[fdcd\] Enumeration time: (\d+)ms")

print("Timing (ms):")
print(f"  C++  evidence={cpp_evi}  inversion={cpp_inv}  total={cpp_evi+cpp_inv}")
print(f"  Java evidence={java_evi}  inversion={java_inv}  total={java_evi+java_inv}")

# ── Predicate normalization ───────────────────────────────────────────────────
# C++ format:  "t.age int == s.fnlwgt int"
# Java format: "(0)t1.AGE==t2.FNLWGT"
def norm_cpp_pred(s):
    s = re.sub(r'\b[ts]\s*\.\s*', '', s)        # strip t. / s.
    s = re.sub(r'\s+(int|str|float)\b', '', s)  # strip type suffix
    s = re.sub(r'\s*([=!<>]+)\s*', r'\1', s)    # remove spaces around operator
    return s.upper().strip()

def norm_java_pred(s):
    s = re.sub(r'^\(\d+\)', '', s)              # strip leading (N)
    s = re.sub(r'\bt[12]\s*\.\s*', '', s)       # strip t1. / t2.
    s = s.replace('<>', '!=')
    s = re.sub(r'\s*([=!<>]+)\s*', r'\1', s)
    return s.upper().strip()

def load_pred_map(path, norm_fn):
    result = {}
    with open(path) as f:
        for line in f:
            parts = line.strip().split('\t', 1)
            if len(parts) == 2:
                result[int(parts[0])] = norm_fn(parts[1])
    return result

def load_evs(path, idx_map):
    result = set()
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            result.add(frozenset(idx_map[int(x)] for x in line.split()))
    return result

cpp_pred_map  = load_pred_map(f"{OUT}/cpp_predicates.txt", norm_cpp_pred)
java_pred_map = load_pred_map(f"{OUT}/java_predicates.txt", norm_java_pred)

cpp_pred_set  = set(cpp_pred_map.values())
java_pred_set = set(java_pred_map.values())

print(f"\nPredicates:  C++={len(cpp_pred_set)}  Java={len(java_pred_set)}", end="  ")
if cpp_pred_set == java_pred_set:
    print("✓ IDENTICAL")
else:
    only_cpp  = cpp_pred_set  - java_pred_set
    only_java = java_pred_set - cpp_pred_set
    print(f"✗ MISMATCH — only C++: {len(only_cpp)}, only Java: {len(only_java)}")
    for p in list(only_cpp)[:3]:  print(f"    C++ only: {p}")
    for p in list(only_java)[:3]: print(f"   Java only: {p}")

# ── Evidence comparison (via normalized predicate strings) ────────────────────
cpp_evs  = load_evs(f"{OUT}/cpp_evidences.txt",  cpp_pred_map)
java_evs = load_evs(f"{OUT}/java_evidences.txt", java_pred_map)

print(f"Evidences:   C++={len(cpp_evs)}  Java={len(java_evs)}", end="  ")
if cpp_evs == java_evs:
    print("✓ IDENTICAL")
else:
    only_cpp  = cpp_evs  - java_evs
    only_java = java_evs - cpp_evs
    print(f"✗ MISMATCH — only C++: {len(only_cpp)}, only Java: {len(only_java)}")
    for ev in list(only_cpp)[:2]:  print(f"    C++ only: {sorted(ev)}")
    for ev in list(only_java)[:2]: print(f"   Java only: {sorted(ev)}")

# ── DC comparison ─────────────────────────────────────────────────────────────
def norm_java_dc(s):
    s = s.strip()
    m = re.match(r'^not\((.+)\)$', s)
    if not m: return None
    return frozenset(
        norm_java_pred(re.sub(r'\bt[12]\.', '', p))
        for p in m.group(1).split(' and ')
    )

def norm_cpp_dc(s):
    s = s.strip()
    m = re.match(r'^¬\{\s*(.+?)\s*\}$', s)
    if not m: return None
    return frozenset(norm_cpp_pred(p) for p in m.group(1).split(' ∧ '))

java_dcs = set(filter(None, (norm_java_dc(l) for l in open(f"{OUT}/java_dcs.txt"))))
cpp_dcs  = set(filter(None, (norm_cpp_dc(l)  for l in open(f"{OUT}/cpp_dcs.txt"))))

print(f"DCs:         C++={len(cpp_dcs)}  Java={len(java_dcs)}", end="  ")
if java_dcs == cpp_dcs:
    print("✓ IDENTICAL")
else:
    only_java = java_dcs - cpp_dcs
    only_cpp  = cpp_dcs  - java_dcs
    print(f"✗ MISMATCH — only Java: {len(only_java)}, only C++: {len(only_cpp)}")
    for dc in list(only_java)[:2]: print(f"   Java only: {sorted(dc)}")
    for dc in list(only_cpp)[:2]:  print(f"    C++ only: {sorted(dc)}")

# ── Timing bar chart ──────────────────────────────────────────────────────────
import matplotlib.pyplot as plt
import numpy as np

labels    = ["Evidence", "Inversion"]
cpp_vals  = [cpp_evi,  cpp_inv]
java_vals = [java_evi, java_inv]

x     = np.arange(len(labels))
width = 0.35

fig, ax = plt.subplots(figsize=(7, 5))
bars_cpp  = ax.bar(x - width/2, cpp_vals,  width, label="C++ HybridDC", color="#4c72b0")
bars_java = ax.bar(x + width/2, java_vals, width, label="Java fdcd",    color="#dd8452")

ax.bar_label(bars_cpp,  fmt="%dms", padding=3)
ax.bar_label(bars_java, fmt="%dms", padding=3)

ax.set_ylabel("Time (ms)")
ax.set_title("HybridDC vs fdcd — timing breakdown")
ax.set_xticks(x)
ax.set_xticklabels(labels)
ax.legend()
ax.set_ylim(0, max(max(cpp_vals), max(java_vals)) * 1.25 + 10)
plt.tight_layout()
plt.show()
PYEOF
