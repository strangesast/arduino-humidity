import json
from datetime import datetime
from collections import defaultdict
from pprint import pprint

lines = []

with open('log.txt', 'r') as f:
    for line in f:
        try:
            lines.append(json.loads(line))
        except:
            pass

d = defaultdict(int)
for line in lines:
    d[line['value']] += 1

pprint(d)
print(datetime.fromtimestamp(min(a['ts'] for a in lines) / 1000))
print(datetime.fromtimestamp(max(a['ts'] for a in lines) / 1000))
print(len(lines))
