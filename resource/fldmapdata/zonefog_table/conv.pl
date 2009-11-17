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
	my( $i, $onedata, $tmp );
	
	for( $i=0; $i<@$data; $i++ )
	{
		$onedata = $$data[$i];
		$onedata =~ s/\s//g;
		$onedata =~ s/\t//g;

		if( $onedata =~ /$key\:\:(.*)/ )
		{
			$tmp = $1;
			if( $tmp =~ /0x/ )	#16�i���`�F�b�N
			{
				$tmp = oct($tmp);
			}
			return $tmp;
		}
	}

	print( "$key�@���@����܂���B" );
	exit(1);
}
