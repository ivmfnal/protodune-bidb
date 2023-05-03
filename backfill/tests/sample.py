import pytimber

log = pytimber.LoggingDB()
lst =  log.search("XBH4.XTDC.022.71%")

for var in sorted(lst):
	print var
