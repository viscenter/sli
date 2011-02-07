Image Processing:

Binaries:
	findPageSize <Page Image>
		-Prints size of rectangle bounding the page to stdout
	cropToPage <Page Image> <New Image> [<New Width> <New Height>]
		-Finds the page within <Page Image>, either crops to the page or to the specified size, and saves out to <New Image>

Dependecncies (Dependent upon the following packages):
	libcv4
	libcv4-dev
	libcvaux4
	libcvaux-dev
