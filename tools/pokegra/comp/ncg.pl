
#====================================================================================
#
#	12�~12�ŕ`���ꂽNCG�L������1D�t�H�[�}�b�g�ɃR���o�[�g���ēf���o��
#
#====================================================================================

#====================================================================================
#
#	�����J�n
#
#====================================================================================
use File::Basename;
use Fcntl qw(:seek); # seek �̈ʒu���w�肷��I�v�V�������g�p���邽�߂Ɏg�p


use constant NCG_HEAD_SIZE			=>	(4 + 2 + 2 + 4 + 2 + 2 + 4 + 4 + 4 + 4 + 4);
use constant NCG_LINE_SIZE4			=>	0x80;
use constant NCG_LINE_SIZE8			=>	0x100;
use constant NCG_LINE_ALL_SIZE	=>	0x400;
use constant NCG_LINE_SIZE8_AMARI		=>	(NCG_LINE_ALL_SIZE - NCG_LINE_SIZE8);
use constant NCG_LINE_SIZE12_AMARI	=>	(NCG_LINE_ALL_SIZE - (NCG_LINE_SIZE4 + NCG_LINE_SIZE8));
use constant NCG_X	=>	12;
use constant NCG_Y	=>	12;
use constant SIZE	=>	NCG_X * NCG_Y * 0x20;

	@NCGRHead = ( 0x52,0x47,0x43,0x4e,0xff,0xfe,0x01,0x01,0x30,0x20,0x00,0x00,0x10,0x00,0x01,0x00,
				  0x52,0x41,0x48,0x43,0x20,0x20,0x00,0x00,NCG_X,0x00,NCG_Y,0x00,0x03,0x00,0x00,0x00,
				  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 );
  @NCGRSize = ( SIZE, 0x00000018 );

	$argc = @ARGV;

	if( $argc < 2 ){
		print "error:perl perl_file read_file write_dir\n";
		die;
	}

	$file_name = @ARGV[0];

	$size = -s $file_name;

	if( $size ){
		&NCGRMake;
		$file_name = basename( @ARGV[0], 'ncg' );
		if( $file_name =~ m/_m\./ ){
			$file_name =~ s/_m\./_f\./g;
			$file_name = @ARGV[1] . $file_name . "NCGR";
			$size = -s $file_name;
      print $file_name . "\n";
			if( $size eq "" ){
				open( DUMMY, "> " . $file_name );
				close( DUMMY );

			}
		}
	}
	else{
#		$file_name = basename( @ARGV[0], '.ncg' );
#		$file_name = @ARGV[1] . $file_name . ".NCGR";
#		open( DUMMY, "> " . $file_name );
#		close( DUMMY );
		print "error:FileOpenError\n";
		die;
	}

#====================================================================================
#
#	NCGRMake
#
#====================================================================================
sub NCGRMake
{
	$file_name = basename( @ARGV[0], '.ncg' );
	$file_name = @ARGV[1] . $file_name . ".NCGRs";
	open( NCG, @ARGV[0] );
	open( NCGR, "> " . $file_name );
	binmode( NCG );
	binmode( NCGR );

	for( $i = 0 ; $i < @NCGRHead ; $i++ ){
		$write = pack "C", @NCGRHead[ $i ];
		print NCGR $write;
	}
	for( $i = 0 ; $i < @NCGRSize ; $i++ ){
		$write = pack "L", @NCGRSize[ $i ];
		print NCGR $write;
	}

	#�w�b�_�[�f�[�^��ǂݍ���
	read NCG, $header, NCG_HEAD_SIZE;

	#12x12��OAM�͖����̂ŁA8x8,4*8,8*4,4*4��4��OAM�ɂ��킹��
	#8x8
	seek( NCG, NCG_HEAD_SIZE, SEEK_SET);
	for( $i = 0 ; $i < 8; $i++ ){
		read NCG, $data, NCG_LINE_SIZE8;
		print NCGR $data;
		read NCG, $data, NCG_LINE_SIZE8_AMARI;
	}
	#4x8
	seek( NCG, NCG_HEAD_SIZE, SEEK_SET);
	for( $i = 0 ; $i < 8; $i++ ){
		read NCG, $data, NCG_LINE_SIZE8;
		read NCG, $data, NCG_LINE_SIZE4;
		print NCGR $data;
		read NCG, $data, NCG_LINE_SIZE12_AMARI;
	}
	#8x4
	seek( NCG, (NCG_HEAD_SIZE+(NCG_LINE_ALL_SIZE*8)), SEEK_SET);
	for( $i = 0 ; $i < 4; $i++ ){
		read NCG, $data, NCG_LINE_SIZE8;
		print NCGR $data;
		read NCG, $data, NCG_LINE_SIZE8_AMARI;
	}
	#4x4
	seek( NCG, (NCG_HEAD_SIZE+(NCG_LINE_ALL_SIZE*8)), SEEK_SET);
	for( $i = 0 ; $i < 4; $i++ ){
		read NCG, $data, NCG_LINE_SIZE8;
		read NCG, $data, NCG_LINE_SIZE4;
		print NCGR $data;
		read NCG, $data, NCG_LINE_SIZE12_AMARI;
	}

	close( NCG );
	close( NCGR );

    &NCGR_Comp( $file_name );
}


#���k
sub NCGR_Comp
{
	$file_name = basename( $_[0], '.NCGRs' );
	$r1 = @ARGV[1] . $file_name . ".NCGR";
	$r2 = @ARGV[1] . $file_name . ".NCGRs";
    $cmd = "ntrcomp -lex -o " . $r1 . " " . $r2;
    system $cmd;
    $cmd = "rm " . $r2;
    system $cmd;
}
