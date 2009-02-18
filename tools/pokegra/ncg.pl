
#====================================================================================
#
#	�����J�n
#
#====================================================================================
use File::Basename;

use constant NCG_HEAD_SIZE	=>	4 + 2 + 2 + 4 + 2 + 2 + 4 + 4 + 4 + 4 + 4;
use constant NCG_LINE_SIZE	=>	0x200;

	@NCGRHead = ( 0x52,0x47,0x43,0x4e,0xff,0xfe,0x01,0x01,0x30,0x20,0x00,0x00,0x10,0x00,0x01,0x00,
				  0x52,0x41,0x48,0x43,0x20,0x20,0x00,0x00,0x10,0x00,0x10,0x00,0x03,0x00,0x00,0x00,
				  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x18,0x00,0x00,0x00 );

	$argc = @ARGV;

	if( $argc < 2 ){
		print "error:perl perl_file read_file write_dir\n";
		die;
	}

	$file_name = @ARGV[0];

	$size = -s $file_name;

	if( $size ){
		&NCGRMake;
	}
	else{
		$file_name = basename( @ARGV[0], '.ncg' );
		$file_name = @ARGV[1] . $file_name . ".NCGR";
		open( DUMMY, "> " . $file_name );
		close( DUMMY );
	}

#====================================================================================
#
#	NCGRMake
#
#====================================================================================
sub NCGRMake
{
	$file_name = basename( @ARGV[0], '.ncg' );
	$file_name = @ARGV[1] . $file_name . ".NCGR";
	open( NCG, @ARGV[0] );
	open( NCGR, "> " . $file_name );
	binmode( NCG );
	binmode( NCGR );

	for( $i = 0 ; $i < @NCGRHead ; $i++ ){
		$write = pack "C", @NCGRHead[ $i ];
		print NCGR $write;
	}

	#�w�b�_�[�f�[�^��ǂݍ���
	read NCG, $header, NCG_HEAD_SIZE; 

	for( $i = 0 ; $i < 16 ; $i++ ){
		read NCG, $data, NCG_LINE_SIZE; 
		print NCGR $data;
		read NCG, $data, NCG_LINE_SIZE; 
	}

	close( NCG );
	close( NCGR );
}
