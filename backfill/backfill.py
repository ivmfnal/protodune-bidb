import time, sys, getopt
from datetime import datetime, timedelta, date, time as dtime

t0 = datetime(2018,8,29)
t1 = datetime(2018,12,20)

devices = ["XBH4.XTDC.022.713","XBH4.XTDC.022.716"]
fields = ["COARSE","FRAC","SECONDS","TIMESTAMP_COUNT"]

Usage = """
python backfill.py [options]

Options:
  -t [yyyy/mm/dd]:[yyyy/mm/dd]      - date range
  -d device[,device,...]
  -f field[,field,...]

Defaults:
  date range: {}/{}/{}:{}/{}/{}
  devices: {}
  fields: {}
""".format(t0.year, t0.month, t0.day, t1.year, t1.month, t1.day,
	",".join(devices), ",".join(fields))

# Full run: 2018/8/29-2018/12/20
"""
2018-08-29 00:00:00 - 2018-09-05 00:00:00 17238
2018-09-05 00:00:00 - 2018-09-12 00:00:00 24231
2018-09-12 00:00:00 - 2018-09-19 00:00:00 22547
2018-09-19 00:00:00 - 2018-09-26 00:00:00 28392
2018-09-26 00:00:00 - 2018-10-03 00:00:00 23431
2018-10-03 00:00:00 - 2018-10-10 00:00:00 33876
2018-10-10 00:00:00 - 2018-10-17 00:00:00 30145
2018-10-17 00:00:00 - 2018-10-24 00:00:00 22900
2018-10-24 00:00:00 - 2018-10-31 00:00:00 27111
2018-10-31 00:00:00 - 2018-11-07 00:00:00 16085
2018-11-07 00:00:00 - 2018-11-14 00:00:00 19935
2018-11-14 00:00:00 - 2018-11-21 00:00:00 11494
2018-11-21 00:00:00 - 2018-11-28 00:00:00 14424
2018-11-28 00:00:00 - 2018-12-05 00:00:00 11966
2018-12-05 00:00:00 - 2018-12-12 00:00:00 21398
2018-12-12 00:00:00 - 2018-12-19 00:00:00 40016
2018-12-19 00:00:00 - 2018-12-26 00:00:00 7965
"""


opts, args = getopt.getopt(sys.argv[1:], "t:f:d:?h")
opts = dict(opts)
if "-?" in opts or "-h" in opts:
	print Usage
	sys.exit(0)

import pytimber, numpy as np

if "-t" in opts:
	date_range = opts["-t"].split(":",1)
	if date_range[0]:
		t0 = datetime(*map(int, date_range[0].split("/")))
	if date_range[1:]:
		# round the end time up to the midnight to cover whole 24 hours
		t1 = date(*map(int, date_range[1].split("/"))) + \
			timedelta(hours=24)
		t1 = datetime.combine(t1, dtime(0,0,0))
	
		
if "-d" in opts:
	devices = opts["-d"].split(",")
if "-f" in opts:
	fields = opts["-f"].split(",")

print "Downloading data for devices:", ",".join(devices)
print "  fields:", ",".join(fields)
print "  for timeinterval from", t0, "to", t1

ldb = pytimber.LoggingDB()

vars = ["%s:%s" % (d, f) for f in fields for d in devices]

rename_map = {}
for v in vars:
	rename_map[v] = "timber/"+v.replace('.','/')


delta = timedelta(1)

t = t0
while t < t1:
	data = ldb.get(vars, t, t+delta)
	time_dict = {}
	for name, (times, values) in data.items():
		translated = rename_map[name]
		if len(times):
			times = np.asarray(times*1000, np.int64)
			for it, value in zip(times, values):
				dct = time_dict.get(it)
				if not dct:
					dct = {}
					time_dict[it] = dct
				if len(value.shape) == 0:
					dct[translated] = int(value)
				else:
					dct[translated] = np.asarray(value, dtype=np.uint32)
					
	times = sorted(time_dict.keys())
	print t, len(times)
	if times:
		fname = "out/%04d-%02d-%02d.data" % (t.year, t.month, t.day)
		ofile = open(fname, "w")
		for it in times:
			data = time_dict[it]
			ofile.write("*z,pdune %d 0\n" % (it,))
			for translated, values in sorted(data.items()):
				if isinstance(values, int):
					ofile.write("%s\t%d\tnull\t%d\tnull\n" % (translated, it, values))
				else:
					ofile.write("%s\t%d\tnull\tnull\t{%s}\n" % (translated, it, 
						','.join(["%d" % x for x in values])))
		ofile.close()
	t = t + delta
