import json
from datetime import datetime, timedelta


data = []
with open('log.txt') as f:
    last = None
    for line in f:
        o = json.loads(line)
        data.append(o)


diffs = set()
dates = {}
for a, b in zip(data[:-1],data[1:]):
    d = (b['ts'] - a['ts']) / 1000
    if d > 65:
        diffs.add(d)
        dates[d] = b['ts']


for d in diffs:
    print('{:>16.3f} {}'.format(d, datetime.fromtimestamp(dates[d] / 1000).isoformat()).rjust(10), len(b['value']))
'''
        ts = o['ts'] / 1000.0
        value = o['value']
        b = datetime.fromtimestamp(ts)
        a = b - timedelta(seconds=len(value))

        if last is not None:
            d = (a - last).total_seconds()
            if abs(d) > 0.1:
                print('b', b, 'a', a, 'last', last, 'd', d)

        last = b
'''
