
#====================================================================================
#
#	�����J�n
#
#====================================================================================
use File::Basename;

	$argc = @ARGV;

	if( $argc < 2 ){
		print "error:perl perl_file read_file write_dir\n";
		die;
	}

	$cmd = "g2dcvtr @ARGV[0] -bg -cr/0_0_32_16 -o/@ARGV[1]";
	system $cmd;
	if( $? >> 8 != 0 ){
		exit(1);
	}

	$filename = basename( @ARGV[0] );
	$filename = basename( $filename, '.nmc' );
	$filename =~ s/pfwb/pmwb/g;
	$cmd = "rm " . $filename . "_n.NCLR";
	system $cmd;
	$cmd = "rm " . $filename . "_r.NCLR";
	system $cmd;

