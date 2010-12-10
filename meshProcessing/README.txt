Mesh Generation README
-Matt Seabold

Make instructions:
	Run make
	if fails make sure the directory structure is created propery and all dependencies are installed


Top Level Scripts:
generateMesh:
	Usage: generateMesh [shortened name] [3D path] [Img path] [Storage path]
		short name: Compressed name of page for reference. For instance the directory Chad003-3D would shorten to Chad003. E3-103r-3D would shorten to E3-103r
		3D path: path to directory containg the generate 3D ply coordinates
		Img path: the directory containing the Mega directory which contains the raw dng images
		Storage path: location to place the model files upon completion

		CAUTION: This script assumes you have set up some things that meshMaker.pl usually would do. You must have a flatfield.png in the running directory and the calibration folder directly as a subdirectory.


meshMaker.pl
	Usage: meshMaker.pl [3D path] [Img path] [Final path]
		3D path: directory which contains all of the seperate 3D directories for a daily scan
		Img path: directory which contains all the rawdata directories
		Final path: where to place the models upon completion


meshMaker.pl vs. generateMesh

generateMesh is for the creation of a single mesh. Given specific directories for a given mesh, it will create and texture the mesh and place it in the directory supplied

meshMaker.pl calls generateMesh. It is a wrapper that will create all of the meshes within a specified directory. Generated meshes will be stored in a directory tmp until completion of all meshes. Then all models will be copied to the supplied final directory



generateMesh requires a few local and system dependencies.
local:
	the scripts and sources in the subdirectories must be compiled and remaining in their current location
system:
	generateMesh requires the following packages to be installed:
		ufraw-batch
		imagemagick
		libcv4 (opencv)
		libcv4-dev
		libcvaux4
		libcvaux-dev


meshlab:
	generateMesh requires that meshlab and more specifically meshlabsever be built. If it is not yet built, download the recent source from http://sourceforge.net/projects/meshlab/files/ and make the source

	Once built link the the meshlabserver binary to the bin folder


