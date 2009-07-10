#################################################################
#
#	�t�H�O�e�[�u���쐬
#		conv.pl .fog
#
#
#################################################################


#�����`�F�b�N
if( @ARGV != 1 )
{
	print( "conv.pl .fog\n" );
	exit(1);
}


#�ǂݍ���
open( FILEIN, $ARGV[0] );
@filedata = <FILEIN>;
close( FILEIN );


#�����o��
$outfile = $ARGV[0];
$outfile =~ s/\.fog//;
open( FILEOUT, ">".$outfile.".dat" );
binmode( FILEOUT );
print( FILEOUT pack("i", &getParam(\@filedata, "OFFSET") ) );
print( FILEOUT pack("i", &getParam(\@filedata, "SLOPE") ) );

close( FILEOUT );

exit(0);


sub getParam
{
	my( $data, $key ) = @_;
	my( $i, $onedata );
	
	for( $i=0; $i<@$data; $i++ )
	{
		$onedata = $$data[$i];
		$onedata =~ s/\s//g;
		$onedata =~ s/\t//g;

		if( $onedata =~ /$key\:\:(.*)/ )
		{
			return oct($1);
		}
	}

	print( "$key�@���@����܂���B" );
	exit(1);
}
