import os

HEADER_TYPES = (".h", ".hpp", ".hxx")
SOURCE_TYPES = (".c", ".cpp", ".cxx")
ALL_TYPES = HEADER_TYPES + SOURCE_TYPES

def make_cmake():
	tmp = []
	for directory in { "include", "src" }:
		for dirpath, dirnames, filenames in os.walk(directory):
			for filename in filenames:
				if filename.endswith(ALL_TYPES):
					path = os.path.join(dirpath, filename)
					tmp.append(os.path.normpath(path))

	sources = []
	for file in tmp:
		name = file.replace("\\", "/")
		sources.append(name)
	sources.sort()

	def do_make(a_filename, a_varname, a_files):
		with open("cmake/{}.cmake".format(a_filename), "w", encoding="utf-8") as out:
			out.write("set({}\n".format(a_varname))
			for file in a_files:
				out.write("\t{}\n".format(file))
			out.write(")\n")

	do_make("sourcelist", "SOURCES", sources)

def main():
	make_cmake()

if __name__ == "__main__":
	main()
