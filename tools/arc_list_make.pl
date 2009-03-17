##################################################
#
#	lsで生成したリストの先頭と後ろに"を追加する
#
##################################################

if( @ARGV < 1 ){
	print "arc_list_make.pl param err\n";
	print "param1	input_file\n";
	exit(1);
}

open( FILEIN, $ARGV[0] );
@INPUT_FILE	= <FILEIN>;
close( FILEIN );

if( @INPUT_FILE == 0 ){
	print "arc_list_make.pl	file open err\n";
	print "input_file".$ARGV[0]."\n";
	exit(1);
}

open( FILEOUT, ">".$ARGV[0] );

#"を追加
foreach $one ( @INPUT_FILE ){
	$one =~ s/\r\n/\n/;	
	$one =~ s/^/"/;	
	$one =~ s/$/"/;	

	print( FILEOUT $one );
}

close( FILEOUT );


