#[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
#
#
#	@file		namein_change_conv.pl
#	@brief	���O���͉�ʂŕ����ϊ��G�N�Z���̃R���o�[�^
#	@author	Toru=Nagihashi
#	@data		2009.10.8
#]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#=============================================================================
#
#					grobal
#
#=============================================================================
#�G�N�Z���R���o�[�^
$EXCEL_CONV_EXE			=	$ENV{"PROJECT_ROOT"}."/tools/exceltool/ExcelSeetConv.exe";

#�G�N�Z���f�[�^
@NAMEIN_XLS_DATA		= ();		#�G�N�Z���f�[�^�̃f�[�^�{��

#��������f�[�^��
$OUTPUTNAME_DATA		= "";		#�t�@�C�����{�g���q.dat
$TEMPORARY_FILENAME	= "temp.dat"; #�G���R�[�h�p�e���|����
$TEMP_BODY_FILENAME	= "body.dat"; #�����p�p�e���|����
$TEMP_HEADER_FILENAME	= "header.dat"; #�����p�e���|����

#�擾�����f�[�^
$DATA_WIDTH					=	0;		#�f�[�^��
$DATA_HEIGHT				=	0;		#�f�[�^����

@DATA_INPUT					= ();		#�ϊ����镶����
@DATA_CHANGE				= ();		#�ϊ�����������
@DATA_SHIFT					= ();		#SHIFT���ϊ�����������
@DATA_REST					= ();		#�ϊ���Ɏc�镶����
$DATA_LENGTH				= 0;		#�f�[�^��

#�g��Ȃ��������G���[�l�Ƃ��Ĉ���
$DATA_ERROR_VALUE		= "��";

#��������
$WORD_LENGTH			= 3;
$DATA_END_VALUE		= "�@";

#=============================================================================
#
#					main
#
#=============================================================================
#-------------------------------------
#	�G���[
#=====================================
if( @ARGV < 1 )
{
	print( "USAGE\n" );
	print( "perl namein_change_conv.pl change_***.xls\n" );
	exit(1);
}
#-------------------------------------
#	�G�N�Z���f�[�^�R���o�[�g
#=====================================
&EXCEL_GetData( $ARGV[0], "data", \@NAMEIN_XLS_DATA );

#-------------------------------------
#	�f�[�^���擾
#=====================================
$is_row_start	= 0;
foreach $line ( @NAMEIN_XLS_DATA )
{
	$line	=~ s/\r\n//g;
	@word	= split( /,/, $line );

	#�f�[�^�͈�
	if( $word[0] eq "#start" )
	{
		@TAG_WORD	= @word;
		$is_row_start	= 1;

		next;
	}
	elsif( $word[0] eq "#end_row" )
	{
		$is_row_start	= 0;
	}


	#�f�[�^�擾
	if( $is_row_start == 1 )
	{
		for( my $i = 0; $i < @TAG_WORD; $i++ ) 
		{
			#�^�O�ƃf�[�^���擾
			my $tag	= $TAG_WORD[$i];
			my $w		= $word[$i];

			#�ϊ����镶����
			if( $tag eq "#input" )
			{
				&UndefAssert( $w );
				push( @DATA_INPUT, $w );
			}
			#�ϊ�����镶����
			if( $tag eq "#change" )
			{
				&UndefAssert( $w );
				push( @DATA_CHANGE, $w );
			}
			#SHIFT���������Ƃ��ɕϊ�����镶����
			if( $tag eq "#shift" )
			{
				push( @DATA_SHIFT, $w );
			}
			#�ϊ���Ɏc�镶����
			if( $tag eq "#rest" )
			{
				push( @DATA_REST, $w );
			}
			#���
			if( $tag eq "#end_col" )
			{
				$DATA_LENGTH++;
			}
		}
	}
}
#-------------------------------------
#	�������������I��$WORD_LENGTH�ɂ���B
#=====================================
{
	use Encode;

	my $len	= 0;
	for( my $i = 0; $i < $DATA_LENGTH; $i++ )
	{
		$len	= length(decode('shiftjis', $DATA_INPUT[$i])); 
		while( $len < 3 )
		{
			$DATA_INPUT[$i]	= join '', $DATA_INPUT[$i], $DATA_END_VALUE;
			$len++;
		}
		$len	= length(decode('shiftjis', $DATA_CHANGE[$i])); 
		while( $len < 3 )
		{
			$DATA_CHANGE[$i]	= join '', $DATA_CHANGE[$i], $DATA_END_VALUE;
			$len++;
		}
		$len	= length(decode('shiftjis', $DATA_SHIFT[$i])); 
		while( $len < 3 )
		{
			$DATA_SHIFT[$i]	= join '', $DATA_SHIFT[$i], $DATA_END_VALUE;
			$len++;
		}
		$len	= length(decode('shiftjis', $DATA_REST[$i])); 
		while( $len < 3 )
		{
			$DATA_REST[$i]	= join '', $DATA_REST[$i],$DATA_END_VALUE;
			$len++;
		}
	}
}

#-------------------------------------
#	�f�o�b�O�v�����g
#=====================================
if(1)
{
	print( "DEBUG_PRINT_START\n" );
	print( "�f�[�^��".$DATA_LENGTH	."\n" );
	for( my $i = 0; $i < $DATA_LENGTH; $i++ )
	{
		print( "����=".$DATA_INPUT[$i]."\n" );
		print( "�ϊ�=".$DATA_CHANGE[$i]."\n" );
		print( "SHIFT".$DATA_SHIFT[$i]."\n" );
		print( "�c��=".$DATA_REST[$i]."\n" );
	}
}

#-------------------------------------
#	�A�E�g�v�b�g��
#=====================================
$OUTPUTNAME_DATA	= $ARGV[0];
$OUTPUTNAME_DATA	=~ s/\.xls/\.dat/g;

#-------------------------------------
#	�f�[�^��
#=====================================
#-------------------------------------
#	�w�b�_��������
#=====================================
open( FILEOUT, ">$TEMP_HEADER_FILENAME" );
binmode( FILEOUT );
print( FILEOUT pack( "S", $DATA_LENGTH ) );
close( FILEOUT ); 
#-------------------------------------
#	�{�f�B��������
#=====================================
open( FILEOUT, ">$TEMP_BODY_FILENAME" );
for( my $i = 0; $i < $DATA_LENGTH; $i++ )
{
	print( FILEOUT $DATA_INPUT[$i] );
	print( FILEOUT $DATA_CHANGE[$i] );
	print( FILEOUT $DATA_SHIFT[$i] );
	print( FILEOUT $DATA_REST[$i] );
}
close( FILEOUT ); 
#-------------------------------------
#	UTF16LE�R���o�[�g
#=====================================
&EnocdeUnicode( $TEMP_BODY_FILENAME, $TEMPORARY_FILENAME );
unlink $TEMP_BODY_FILENAME;
rename $TEMPORARY_FILENAME, $TEMP_BODY_FILENAME;

#-------------------------------------
#	�f�[�^�̃w�b�_�A�{�f�B����
#=====================================
open( HEADER, "<$TEMP_HEADER_FILENAME");
open( BODY, "<$TEMP_BODY_FILENAME");
open( MAIN, ">$OUTPUTNAME_DATA");
#�w�b�_��������
while( my $data = <HEADER> ) 
{
	print MAIN $data;
}
#�{�f�B��������
while( my $data = <BODY> ) 
{
	print MAIN $data;
}
close( MAIN ); 
close( BODY );
close( HEADER );

unlink $TEMP_HEADER_FILENAME;
unlink $TEMP_BODY_FILENAME;


#-------------------------------------
#	����I��
#=====================================
exit(0);


#=============================================================================
#
#					sub routine
#
#============================================================================
#-------------------------------------
#	@brief	�G�N�Z���f�[�^���J���}��؂�œǂݍ���
#	@param	�t�@�C�����X�J���[
#	@param	�V�[�g���X�J���[
#	@param	�i�[����̃o�b�t�@�i�z��̃��t�@�����X\@buff�j
#=====================================
sub EXCEL_GetData
{
	my( $filename, $sheetname, $buff ) = @_;
	my( $EXCEL_DEFDATA_FILE_NAME );

	#��{���V�[�g���R���o�[�g
	$EXCEL_DEFDATA_FILE_NAME = "def_data.txt";
	system( $EXCEL_CONV_EXE.' -o '.$EXCEL_DEFDATA_FILE_NAME.' -n '. $sheetname.' -s csv '.$filename );

	open( EXCEL_DEF_FILEIN, $EXCEL_DEFDATA_FILE_NAME );
	@$buff = <EXCEL_DEF_FILEIN>;
	close( EXCEL_DEF_FILEIN );

	system( 'rm '.$EXCEL_DEFDATA_FILE_NAME );
}
#-------------------------------------
#	@brief	�t�@�C���ǂݍ���
#	@param	�t�@�C�����X�J���[
#	@param	�i�[����̃o�b�t�@�i�z��̃��t�@�����X\@buff�j
#=====================================
sub FILE_GetData
{
	my( $filename, $buff ) = @_;

	#��{���V�[�g���R���o�[�g
	open( FILEIN, $filename );
	@$buff = <FILEIN>;
	close( FILEIN );
}

#-------------------------------------
#	@brief	��`�̐��l���擾
#	@param	��`���X�J���[
#	@param	��`�����������t�@�C���̃o�b�t�@�i���t�@�����X\@buff�j
#	@retval	���l
#=====================================
sub GetTypeNumber
{
	my( $name, $buff ) = @_;

	foreach $data ( @$buff )
	{
		#print( " $name == $data \n" );
		if( $data =~ /$name\s*\(\s*([0-9]*)\)/ )
		{
			#print "ok\n";
			return $1;
		}
	}

	print( "$name not find\n" );
	exit(1);
}
#-------------------------------------
#	@brief	�z��̖��O�ƈ�v�����C���f�b�N�X��Ԃ�
#	@param	���O
#	@param	�z��i���t�@�����X\@buff�j
#	@retval	���l
#=====================================
sub GetArrayNumber
{
	my( $name, $buff ) = @_;
	my( $cnt );

	$cnt	= 0;
	foreach $data ( @$buff )
	{
		if( $data =~ /$name/ )
		{
			return $cnt;
		}
		$cnt++;
	}

	print( "$name not find\n" );
	exit(1);
}

#-------------------------------------
#	@brief	���ݒ�`�F�b�N
#	@param	�l
#=====================================
sub UndefAssert
{
	my( $val )	= @_;
	if( $val eq "" )
	{
		print ( "���ݒ�ł�\n" );
		exit(1);
	}
}
#-------------------------------------
#	@brif �t�@�C�����������ʂ�UTF-16�ɃG���R�[�h
#	@param	���̃t�@�C����
#	@param	�G���R�[�h��̃t�@�C����
#=====================================
sub EnocdeUnicode 
{
	my( $src, $dst ) = @_;
	my( $data );
	use Encode;
if( 0 )
{
	use Encode qw(from_to);
	use Encode::Guess qw/euc-jp shift-jis/;	#UTF8�͎�������

	open IN, "<$src";
	open OUT, ">$dst";
	my $content = join '', <IN>;

	my $guess = Encode::Guess::guess_encoding($content);
	unless(ref $guess){
		# ���ʎ��s
		die('Encode�Ɏ��s���܂����B���ʂł��܂��� guess:' . $guess);
	}else{
		# ���ʐ���
		$contents = $guess->decode($content);
		$contents = Encode::encode("UTF-16LE", $content);

		print OUT $content;
	}

	close IN;
	close OUT;
}
else
{
	open READ, '<:encoding(shiftjis)', $src;
	open WRITE, '>:encoding(UTF-16LE)', $dst;
	while(my $data = <READ>){
		print WRITE $data;
	}
	close WRITE;
	close READ;
}
}
