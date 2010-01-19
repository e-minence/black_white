#[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
#
#
#	@file		wifi_unionobj_xls_conv.pl
#	@brief	���j�I��OBJ�̃G�N�Z���f�[�^�R���o�[�^
#	@author	Toru=Nagihashi
#	@data		2010.01.19
#]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#=============================================================================
#
#					grobal
#
#=============================================================================
#�G�N�Z���R���o�[�^
$EXCEL_CONV_EXE			=	$ENV{"PROJECT_ROOT"}."/tools/exceltool/ExcelSeetConv.exe";

#�G�N�Z���f�[�^
@XLS_DATA		= ();		#�G�N�Z���f�[�^�̃f�[�^�{��

#��������f�[�^��
$OUTPUTNAME_DATA		= "wifi_unionobj_plt.cdat";

#�擾�����f�[�^
@DATA_PLT   				=	();		#�p���b�gID

#�f�[�^����
$DATA_LENGTH				= 0;

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
	print( "perl wifi_unionobj_xls_conv.pl wifi_unionobj_plt.xls\n" );
	exit(1);
}
#-------------------------------------
#	�G�N�Z���f�[�^�R���o�[�g
#=====================================
&EXCEL_GetData( $ARGV[0], "data", \@XLS_DATA );

#-------------------------------------
#	�f�[�^���擾
#=====================================
$is_start	= 0;
foreach $line ( @XLS_DATA )
{
	$line	=~ s/\r\n//g;
	@word	= split( /,/, $line );

	#�f�[�^�͈�
	if( $word[0] eq "#file_start" )
	{
		@TAG_WORD	= @word;
		$is_start	= 1;
		next;
	}
	elsif( $word[0] eq "#file_end" )
	{
		$is_start	= 0;
	}

	#�f�[�^�擾
	if( $is_start == 1 )
	{
		for( my $i = 0; $i < @TAG_WORD; $i++ ) 
		{
			#�^�O�ƃf�[�^���擾
			my $tag	= $TAG_WORD[$i];
			my $w		= $word[$i];

			#�p���b�g�ԍ�
			if( $tag eq "#plt_num" )
			{
				&UndefAssert( $w );
        #&NumberAssert( $w ); 
				push( @DATA_PLT, $w );
			}
			#�I��
			elsif( $tag eq "#end" )
			{
				$DATA_LENGTH++;
			}
		}
	}
}
#-------------------------------------
#	�f�o�b�O�v�����g
#=====================================
if(0)
{
	print( "DEBUG_PRINT_START\n" );
	for( my $i = 0; $i < $DATA_LENGTH; $i++ )
	{
		print( "DATA=".$i."\n" );
		print( "ZONE=".$DATA_PLT[$i]."\n" );
		print( "\n" );
	}
}

#-------------------------------------
#	�w�b�_�쐬
#=====================================
open( FILEOUT, ">$OUTPUTNAME_DATA" );
print( FILEOUT "#pragma once\n" );
print( FILEOUT "//���̃t�@�C����wifi_unionobj_xls_conv.pl�ɂ���Ď�����������Ă��܂�\n" );
print( FILEOUT "//UNIONOBJ�͂P�U�L�����N�^�[�����W�p���b�g�ł܂��Ȃ��Ă���̂�\n" );
print( FILEOUT "//�ǂ̃L�����N�^�[���ǂ̃p���b�g���Q�Ƃ��Ă��邩�𒲂ׂ邽�߂̃e�[�u���ł�\n" );
print( FILEOUT "//�e�[�u���͌����ڔԍ����ɕ���ł��܂�(my_status��trainer_view)\n" );
$cnt	= 0;
print( FILEOUT "static const u8 sc_wifi_unionobj_plt[] =\n" );
print( FILEOUT "{\n" );

foreach $plt ( @DATA_PLT )
{
	print( FILEOUT "\t$plt,\n" );
	$cnt++;
}
print( FILEOUT "};\n" );

close( FILEOUT ); 

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
#	@brief	�t���O�̒�`�̐��l���擾
#	@param	��`���X�J���[
#	@retval	���l
#=====================================
sub GetFlagNumber
{
	my( $name ) = @_;

	foreach $data ( @SYSFLAG_BUFF )
	{
		#print( " $name == $data \n" );
		if( $data =~ /#define $name\s*([0-9]*)/ )
		{
			print( " $name == $data \n" );
			print "ok $1 \n";
			return $1;
		}
	}

	print( "$name not find\n" );
	exit(1);

	return $DATA_ERROR_VALUE;
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
#	@brief	���l�`�F�b�N
#	@param	�l
#=====================================
sub NumberAssert
{
	my( $val )	= @_;
	if ($val =~ /[0-9]+/)
	{
		print ( "���l�����Ă�������\n" );
		exit(1);
	}
}
