import pytimber, time
from datetime import datetime

vars = "XBH4.XTDC.022.71%"

ldb = pytimber.LoggingDB()
vars = sorted(ldb.search(vars))

t0 = datetime(2018, 9, 2, 23, 59, 0)


for v in vars:
	t, data = ldb.get(v, t0)[v]
	print v, time.ctime(t), data.dtype, data.shape
	sample = data if len(data.shape) == 1 else data[0]
	if len(sample):
		x = sample[0]
		if int(x) == x:	
			print v, "appears to be integer"
			xi = int(x)
			if xi >> 32:
				print v, "appears to be 64 bit integer"



