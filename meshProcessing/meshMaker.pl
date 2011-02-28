#!/usr/bin/perl



if($#ARGV+1 != 3) {
	print "Usage: meskMaker [Directory with 3D data] [Directory with image data] [Directory to place final models]\n";
	exit;
}



$path3D = $ARGV[0];
$pathImg = $ARGV[1];
$pathFinal = $ARGV[2];
#Add a '/' to the end of the path if it doesnt exist
if($path3D !~ /.+\/$/) {
	$path3D = $path3D . "/";
}

if($pathImg !~ /.+\/$/) {
	$pathImg = $pathImg . "/";
}

if(-d "calib")
{}
else {
	system("mkdir calib");
	system("mkdir calib/proj");
}
if(-d "tmp")
{}
else{
	system("mkdir tmp");
}

#Get the Calibration files
$calibPath = $path3D . "calib/proj";
system("cp $calibPath/* calib/proj");

system("ls -tr $pathImg  > filelist.txt");

open(FILELIST, "<filelist.txt");
@files = <FILELIST>;
$lastFile = $#files;

$numRunning = 0;
$currFile = 0;
foreach $file (@files) {
	chomp($file);
	if(-d $pathImg.$file) {
		#If file is flatfield, change current flatfield to it
		if($file =~ /flat|Flat/) {
			print("Changing Flat Field\n");
			system("scripts/composeShell $file $pathImg");
			$pngName = $file . ".png";
			system("mv $pngName flatfield.png");
		}
		#Find 3D directories and save the name minus (3D) for later use
		if($file =~ /(\w+-*\d\d\d\w*)-*3D$/) {
			$shortName = $1;
			if($currFile == $lastFile) {
				system("./generateMesh $shortName $path3D $pathImg tmp");
				$numRunning = 0;
			}
			elsif($files[$currFile+1] =~ /flat|Flat/) {
				system("./generateMesh $shortName $path3D $pathImg tmp");
				$numRunning = 0;
			}
			elsif($numRunning == 7) {
				system("./generateMesh $shortName $path3D $pathImg tmp");
				$numRunning = 0;
			}
			else {
				system("./generateMesh $shortName $path3D $pathImg tmp&");
				$numRunning++;
			}
				

		}
	}
	$currFile++;
}

print("Moving files to $finalPath\n");
@finalFiles = <tmp/*>;
foreach $file (@finalFiles) {
	print("Moving $file\n");
	system("mv $file $pathFinal");
}

system("rmdir tmp");
