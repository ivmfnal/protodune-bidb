import pytimber, time, numpy as np
from datetime import datetime, timedelta

devices = ["XBH4.XTDC.022.716"]
fields = ["TIMESTAMP_COUNT"]

ldb = pytimber.LoggingDB()

#t0 = datetime(2018, 8, 1)
t0 = datetime(2018, 11, 7)
delta = timedelta(1)
tmax = datetime(2018, 11, 9)
t = t0

while t < tmax:
	t1 = t + delta
	for d in devices:
		vars = [d+':'+f for f in fields]
		data = ldb.get(vars, t, t1)
		times_0 = []
		for var, (times, values) in data.items():
			if len(times):
				for t in times:
					print t, time.ctime(t)
	t = t1


