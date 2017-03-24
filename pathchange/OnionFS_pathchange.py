import sys;

patchaddr = 15664;

print("");
try:
    sys.argv[1];
except IndexError:
	print("Invalid syntax: OnionFS_pathchange.py <plg file path> <new path>");
	sys.exit(-1);
try:
    sys.argv[2];
except IndexError:
	print("Invalid syntax: OnionFS_pathchange.py <plg file path> <new path>");
	sys.exit(-1);

path = sys.argv[2];
if path[0] == "/":
	path = path[1:];
if path[len(path) - 1] == "/":
	path = path[:len(path) - 1]
newpath = "ram:/" + path + "/";
displaypath = "SD:/" + path + "/";
if (len(newpath) <= 100):
	print("Setting new path to: %s" % displaypath)
	with open(sys.argv[1], 'r+b') as f:
		code = f.read();
		f.seek(patchaddr)
		for i in range(101):
			f.write("\x00");
			f.seek(patchaddr + i);
		f.seek(patchaddr)
		f.write(newpath);
		f.close();
		print("Done!");
else:
	print("Your path exceeds 99 characters! (%s characters)" % len(newpath));