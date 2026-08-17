
import re, sys
from collections import defaultdict

def parse(path):
    cur_label, cur_fn, total = None, None, 0
    sizes = defaultdict(int)
    for line in open(path):
        line = line.rstrip('\n')
        m = re.match(r'^[0-9a-f]{7,8} <(.+)>:$', line)          # top-level symbol
        if m:
            cur_fn = cur_label = m.group(1); continue
        if (line and not line[0].isspace() and not line.startswith(('/', '#'))
                and not re.match(r'^[0-9a-f]{7,8}[ :]', line) and line.endswith(':')
                and 'Disassembly of section' not in line and 'file format' not in line):
            cur_label = line[:-1]; continue                      # inline-frame label
        m = re.match(r'^\s*([0-9a-f]+):\t([0-9a-f ]+)\t', line)  # instruction/data
        if m:
            nbytes = len(m.group(2).replace(' ', '')) // 2
            sizes[cur_label or cur_fn] += nbytes
            total += nbytes
    return sizes, total

sizes, total = parse(sys.argv[1])
cat = defaultdict(int)
for label, sz in sizes.items():
    cat['bitfilled' if 'bitfilled' in label else 'other'] += sz
for c, sz in sorted(cat.items(), key=lambda x: -x[1]):
    print(f"{c:10s} {sz:5d}  ({100*sz/total:.1f}%)")
