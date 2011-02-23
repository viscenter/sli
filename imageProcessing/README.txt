Image Processing:

Binaries:

findPageSize <Page Image>
	-Prints size of rectangle bounding the page with x and y offsets to stdout

cropToPage <Page Image> [Options]
	Without options will crop image down to page
	Default output file will be <Input File>_crop.png
	-Options:
		-o <file> -- supply name for output file
		-h <height> -- suppy a specific height to crop image to
		-w <width> -- supply a specific width to crop image to
		-x <xOffset>-- Supply x offset for page bound
        -y <yOffset>-- Supply y offest for page bound
		-r -- Overwrite input image file with new cropped image
				->Will be overridden by -o if supplied

reTexture <obj file> <original width> <original height> <new width> <new height><x offset> <y offset>  >  <new obj>
	Will reapply texture with a cropped image to the given obj file.
	Printed to stdout so redirect to a new obj

Wrap Scripts:

cropToLargest <Directory>
	Will take supplied directory containing page images, find the largest
	page size, and crop all images to the same (largest) size.

	Adds a set number of pixel border to the returned largest size. To change
	the size of the border, change the BORDER variable at the beginning of the 
	script

	Files will be saved with the default names from cropToPage


Dependecncies (Dependent upon the following packages):
	libcv4
	libcv4-dev
	libcvaux4
	libcvaux-dev
