import pytimber, time, numpy as np
from datetime import datetime, timedelta

devices = ["XBH4.XTDC.022.713","XBH4.XTDC.022.716"]
fields = [" COARSE","FRAC","SECONDS","TIMESTAMP_COUNT"]

ldb = pytimber.LoggingDB()

#t0 = datetime(2018, 8, 1)
t0 = datetime(2018, 9, 19)
delta = timedelta(7)
tmax = datetime(2019, 1, 1)
t = t0

while t < tmax:
	t1 = t + delta
	for d in devices:
		vars = [d+':'+f for f in fields]
		data = ldb.get(vars, t, t1)
		times_0 = []
		for var, (times, values) in data.items():
			if len(times):
				print np.asarray(times*1000, np.int64)
				times = np.asarray(times, np.int64)
			if len(times_0) == 0:	times_0 = times
			if np.any(times_0 != times):
				print "Inconsistent times arrays: \n%d %s\n%d %s" % (len(times_0), times_0, len(times), times)
		if len(times_0):	print d,t,"-",t1, len(times_0)
	t = t1


