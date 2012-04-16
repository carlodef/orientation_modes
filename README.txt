A contrario detection of modes in orientation histogram
C++ implementation for IPOL

This software is written by
    Carlo De Franchis <carlo.de-franchis@polytechnique.org>
It is distributed under the terms of the BSD license. This is the
version 1.O, released 16/04/2012. Future versions and updates
will be distributed via a git repository, available online at
    http://dev.ipol.im/git/?p=carlo/modes.git;a=snapshot;h=refs/heads/master;sf=tgz


# OVERVIEW

This software performs the estimation of the dominant orientation(s) of a local
region in an image, with two different methods: the a contrario detection of modes,
and the local maxima detection. This is the implementation of the algorithm detailed
in the IPOL article "A contrario detection of modes in orientation histogram",
available at
    https://edit.ipol.im/edit/algo/d_a_contrario_orientation_modes/


# COMPILATION

This software requires an ANSI C++ compiler and the libpng library [1].
Pre-compiled versions of the libpng library are available for Linux 
(libpng-dev in most distributions), Mac OS X (in fink [2]) and 
Windows (provided by GnuWin32 [3]).

To compile modes_detection, use the makefile with simply `make`. 
Alternatively, change directory to the src/ folder, then just call
your C++ compiler with
    cxx main.cpp Histo.cpp modes_detection.cpp libpng_io.cpp -lpng -o modes_detection

[1] http://www.libpng.org/pub/png/libpng.html
[2] http://pdb.finkproject.org/pdb/browse.php?summary=libpng
[3] http://gnuwin32.sourceforge.net/packages/libpng.htm


# USAGE

This program reads a PNG file, and write several text files. The call syntax is
    modes_detection image.png x y r n_bins flag_norm
where
	image.png    is the input image
	x, y         are the coordinates of the keypoint (int)
	r			 is the scale of the keypoint (int)
	n_bins		 is the number of bins used to build the orientations histogram (int)
	flag_norm 	 is a flag to decide if the histogram is weighted by the norm
				 of the gradient (flag=1) or not (flag=0)
	

The input image is read into a 32bit float array, converted to gray. The output
files are
	histo_ac.txt		int array of size n_bins, containing values of the histogram
						of orientations used for the a contrario detection of modes.
	histo_lowe.txt		float: array of size n_bins, containing values of the histogram
						of orientations used for the peaks detection. This histogram
						is built with flag_norm=1
	modes_ac.txt		list of modes detected with the a contrario algorithm. Each line
						gives the bounds of the mode, its orientation, and its meaningfullness.
						Example of line:
						[15,2] ; 1.50014 ; 1.34529
						Here [15,2] is the mode, 1.50014 is the associated orientation (in radians),
						and 1.34529 is its meaningfullness (-log(NFA)).
	modes_lowe.txt		list of local maxima detected with the lowe's approach. Each line
						gives the local maxima, its orientation, and the ratio between this maxima
						and the global maximum of the histogram.
						Example of line:
						[18,18] ; -0.0824485 ; 1
						Here 18 is the unique local maximum, -0.0824485 is the associated
						orientation (in radians), and 1 is the ratio between the local
						maximum value and the global maximum value.
	nb_pixels_ac.txt	Number of pixels in the disc of radius r centered in (x,y) that
						passed the threshold on the gradient norm. The pixels whose gradient
						norm is less than 3*sqrt(2) are not counted in the histogram used for
						the a contrario detection of modes. But they are counted in the
						histogram used for the sift-like orientation assignment.

# EXAMPLE

An example input image is provided in the example folder, with a script "test.sh" that you
can run by typing
	./test.sh
It runs the following line
	../modes_detection lena.png 102 147 15 36 0


