import sys, glob, os
import psycopg2, traceback
from cStringIO import StringIO


infiles = sys.argv[2:]
connect = sys.argv[1]

db_connection = psycopg2.connect(connect)

def parse_line(line):
	words = line.split("\t")
	line_type = words[0]
	if line_type == 't':
		return 't', {'t':float(words[1]), 'var':words[2]}
	elif line_type == 'v':
		field = words[1]
		vtype = words[2]
		values = words[4:]
		if vtype == 'i':
			values = [int(x) for x in values]
		elif vtype == 'f':
			values = [float(x) for x in values]
		return 'v', dict(field=field, vtype=vtype, values=values)

def store_data(t, var, data):
	data_dict = { x["field"]:(x["vtype"], x["values"]) for x in data }
	c = db_connection.cursor()
	c.execute("select field from measurements where t=%s and variable=%s", (t,var))
	for (f,) in c.fetchall():
		if f in data_dict:
			del data_dict[f]
	data = []
	lines = []
	for f, (vtype, values) in data_dict.items():
		if vtype == 'i':
			data.append((t, var, f, values, None, None))
			lines.append('%.3f\t%s\t%s\t{%s}\tnull\tnull' % (
				t, var, f, ','.join(["%d" % (x,) for x in values])))
		elif vtype == 'f':
			data.append((t, var, f, None, values, None))
			lines.append('%.3f\t%s\t%s\tnull\t{%s}\tnull' % (
				t, var, f, ','.join(["%f" % (x,) for x in values])))
		elif vtype == 't':
			data.append((t, var, f, None, None, values))
			lines.append('%.3f\t%s\t%s\tnull\tnull\t{%s}' % (
				t, var, f, ','.join(["'%s'" % (x,) for x in values])))
	text = StringIO("\n".join(lines))
	c.copy_from(text, "measurements" , null='null',
                                columns=['t', 'variable','field','ival','fval','sval'])


	


	#c.executemany("""
	#	insert into measurements(t, variable, field, ival, fval, sval)
	#		values(%s, %s, %s, %s, %s, %s)
	#""", data)
	c.execute("commit")
	
			

	
	
	

def parse_file(fn):
	f = open(fn, "r")
	t = None
	var = None
	accumulated_data = []
	for l in f.readlines():
		l = l.strip()
		ltype, data = parse_line(l)
		if ltype == 't':
			store_data(t, var, accumulated_data)
			accumulated_data = []
			t = data["t"]
			var = data["var"]
		elif ltype == 'v':
			accumulated_data.append(data)
	store_data(t, var, accumulated_data)

for p in infiles:
	for fn in glob.glob(p):
		try:
			parse_file(fn)
			os.unlink(fn)
		except:
			print "Error processing file:", fn
			traceback.print_exc()
			os.rename(fn, fn+".error")


