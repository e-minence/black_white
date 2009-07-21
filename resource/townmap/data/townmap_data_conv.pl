#[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
#
#
#	@file		townmap_data_conv.pl
#	@brief	�^�E���}�b�v�̃G�N�Z���f�[�^�R���o�[�^
#	@author	Toru=Nagihashi
#	@data		2009.07.17
#]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#=============================================================================
#
#					grobal
#
#=============================================================================
#�G�N�Z���R���o�[�^
$EXCEL_CONV_EXE			=	$ENV{"PROJECT_ROOT"}."/tools/exceltool/ExcelSeetConv.exe";

#�G�N�Z���f�[�^
@TOWNMAP_XLS_HEADER	= ();		#�^�E���}�b�v�G�N�Z���f�[�^�w�b�_
@TOWNMAP_XLS_DATA		= ();		#�^�E���}�b�v�G�N�Z���f�[�^�̃f�[�^�{��

#��������f�[�^��
$OUTPUTNAME_DATA		= "townmap_data.dat";
$OUTPUTNAME_HEADER	= "townmap_data.h";

#��`���擾�p�t�@�C���l�[��
$ZONEID_FILENAME		=	"../../fldmapdata/zonetable/zone_id.h";
$GMM_DIR						= "../../message/dst/";	#�R���o�[�g���msg_xxx.h��define���Q�Ƃ��邽��
$GUIDGMM_FILENAME		=	"";
$PLACEGMM_FILENAME	= "";

#��`���擾�̂��߂̃o�b�t�@
@ZONEID_BUFF				= ();		#�]�[��ID�p�o�b�t�@
@GUIDGMM_BUFF				= ();		#�K�C�h����ID
@PLACEGMM_BUFF			=	();		#��������ID
@TYPE_BUFF					= ();		#�^�C�v��	���ꂾ��header������o��

#�擾�����f�[�^
@DATA_ZONEID				=	();		#�]�[��ID
@DATA_CURSOR_X			=	();		#�J�[�\��X
@DATA_CURSOR_Y			=	();		#�J�[�\��Y
@DATA_HIT_S_X				=	();		#������J�nX
@DATA_HIT_S_Y				=	();		#������J�nY
@DATA_HIT_E_X				=	();		#������I��X
@DATA_HIT_E_Y				=	();		#������I��Y
@DATA_HIT_W					=	();		#�����蕝
@DATA_TYPE					=	();		#���
@DATA_SKY_FLAG			= ();		#�����
@DATA_WARP_X				= ();		#����X
@DATA_WARP_Y				= ();		#����Y
@DATA_GUIDE					= ();		#�K�C�hID
@DATA_PLACE1				= ();		#�����PID
@DATA_PLACE2				= ();		#�����QID
@DATA_PLACE3				= ();		#�����RID
@DATA_PLACE4				= ();		#�����SID
@DATA_PLACE5				= ();		#�����TID
@DATA_PLACE6				= ();		#�����UID

#�f�[�^����
$DATA_LENGTH				= 0;

#�G���[�f�[�^
$DATA_ERROR_VALUE		= 0xFFFF;

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
	print( "perl townmap_data_conv.pl townmapdata.xls\n" );
	exit(1);
}
#-------------------------------------
#	�G�N�Z���f�[�^�R���o�[�g
#=====================================
&EXCEL_GetData( $ARGV[0], "header", \@TOWNMAP_XLS_HEADER );
&EXCEL_GetData( $ARGV[0], "data", \@TOWNMAP_XLS_DATA );

#-------------------------------------
#	�w�b�_�����擾
#=====================================
$line_cnt	= 0;
$is_file_name_start	= 0;
$is_type_start	= 0;
foreach $line ( @TOWNMAP_XLS_HEADER )
{

	$line	=~ s/\r\n//g;
	@word	= split( /,/, $line );

	#�f�[�^�͈�
	if( $word[0] eq "#filename_start" )
	{
		$line_cnt	= 0;
		$is_file_name_start	= 1;
	}
	elsif( $word[0] eq "#filename_end" )
	{
		$is_file_name_start	= 0;
	}
	elsif( $word[0] eq "#type_start" )
	{
		$line_cnt	= 0;
		$is_type_start	= 1;
	}
	elsif( $word[0] eq "#type_end" )
	{
		$is_type_start	= 0;
	}

	#�t�@�C�����f�[�^�擾
	if( $is_file_name_start == 1 )
	{
		if( $line_cnt == 1 )
		{
			&UndefAssert( $word[1] );

			#�K�C�hGMM�t�@�C��
			my $name	= "msg_" . $word[1];
			$name	=~ s/gmm/h/g;
			$GUIDGMM_FILENAME		= $GMM_DIR . $name;
			#print $GUIDGMM_FILENAME."\n";
		}
		elsif( $line_cnt == 2 )
		{
			&UndefAssert( $word[1] );

			#����GMM�t�@�C��
			my $name	= "msg_" . $word[1];
			$name	=~ s/gmm/h/g;
			$PLACEGMM_FILENAME	= $GMM_DIR . $name;
			#print $PLACEGMM_FILENAME."\n";
		}
	}
	#��ރf�[�^�擾
	elsif( $is_type_start == 1 )
	{
		if( $line_cnt >= 1 ) 
		{
			#print "$word[1]"."\n";
			push( @TYPE_BUFF, $word[1] );
		}
	}

	#�s���J�E���g
	$line_cnt++;
}

#-------------------------------------
#	��`���̂��߂̃f�[�^�ǂݍ���
#=====================================
&FILE_GetData( $ZONEID_FILENAME, \@ZONEID_BUFF );
&FILE_GetData( $GUIDGMM_FILENAME, \@GUIDGMM_BUFF );
&FILE_GetData( $PLACEGMM_FILENAME, \@PLACEGMM_BUFF );

#-------------------------------------
#	�f�[�^���擾
#=====================================
$is_start	= 0;
foreach $line ( @TOWNMAP_XLS_DATA )
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

			#�]�[��ID
			if( $tag eq "#zone" )
			{
				&UndefAssert( $w );
				push( @DATA_ZONEID, &GetTypeNumber( $w, \@ZONEID_BUFF ) );
			}
			#�J�[�\��X
			elsif( $tag eq "#cursor_x" )
			{
				&UndefAssert( $w );
				push( @DATA_CURSOR_X, $w );
			}
			#�J�[�\��Y
			elsif( $tag eq "#cursor_y" )
			{
				&UndefAssert( $w );
				push( @DATA_CURSOR_Y, $w );
			}
			#������J�nX
			elsif( $tag eq "#hit_s_x" )
			{
				&UndefAssert( $w );
				push( @DATA_HIT_S_X, $w );
			}
			#������J�nY
			elsif( $tag eq "#hit_s_y" )
			{
				&UndefAssert( $w );
				push( @DATA_HIT_S_Y, $w );
			}
			#������I��X
			elsif( $tag eq "#hit_e_x" )
			{
				&UndefAssert( $w );
				push( @DATA_HIT_E_X, $w );
			}
			#������I��Y
			elsif( $tag eq "#hit_e_y" )
			{
				&UndefAssert( $w );
				push( @DATA_HIT_E_Y, $w );
			}
			#�����蕝
			elsif( $tag eq "#hit_w" )
			{
				&UndefAssert( $w );
				push( @DATA_HIT_W, $w );
			}
			#���
			elsif( $tag eq "#type" )
			{
				my $val;
				if( $w eq "" )
				{
					$val	= $DATA_ERROR_VALUE;
				}
				else
				{
					$val	= &GetArrayNumber( $w, \@TYPE_BUFF );
				}
				push( @DATA_TYPE, $val );
			}
			#�����
			elsif( $tag eq "#sky_flag" )
			{
				&UndefAssert( $w );
				push( @DATA_SKY_FLAG, $w );
			}
			#���nX
			elsif( $tag eq "#warp_x" )
			{
				push( @DATA_WARP_X, $w );
			}
			#���nY
			elsif( $tag eq "#warp_y" )
			{
				push( @DATA_WARP_Y, $w );
			}
			#�K�C�h����
			elsif( $tag eq "#guide" )
			{
				my $val;
				if( $w eq "" )
				{
					$val	= $DATA_ERROR_VALUE;
				}
				else
				{
					$val	= &GetTypeNumber( $w, \@GUIDGMM_BUFF );
				}
				push( @DATA_GUIDE, $val );
			}
			#�����P
			elsif( $tag eq "#place1" )
			{
				my $val;
				if( $w eq "" )
				{
					$val	= $DATA_ERROR_VALUE;
				}
				else
				{
					$val	= &GetTypeNumber( $w, \@PLACEGMM_BUFF );
				}
				push( @DATA_PLACE1, $val );
			}
			#�����Q
			elsif( $tag eq "#place2" )
			{
				my $val;
				if( $w eq "" )
				{
					$val	= $DATA_ERROR_VALUE;
				}
				else
				{
					$val	= &GetTypeNumber( $w, \@PLACEGMM_BUFF );
				}
				push( @DATA_PLACE2, $val );
			}
			#�����R
			elsif( $tag eq "#place3" )
			{
				my $val;
				if( $w eq "" )
				{
					$val	= $DATA_ERROR_VALUE;
				}
				else
				{
					$val	= &GetTypeNumber( $w, \@PLACEGMM_BUFF );
				}
				push( @DATA_PLACE3, $val );
			}
			#�����S
			elsif( $tag eq "#place4" )
			{
				my $val;
				if( $w eq "" )
				{
					$val	= $DATA_ERROR_VALUE;
				}
				else
				{
					$val	= &GetTypeNumber( $w, \@PLACEGMM_BUFF );
				}
				push( @DATA_PLACE4, $val );
			}
			#�����T
			elsif( $tag eq "#place5" )
			{
				my $val;
				if( $w eq "" )
				{
					$val	= $DATA_ERROR_VALUE;
				}
				else
				{
					$val	= &GetTypeNumber( $w, \@PLACEGMM_BUFF );
				}
				push( @DATA_PLACE5, $val );
			}
			#�����U
			elsif( $tag eq "#place6" )
			{
				my $val;
				if( $w eq "" )
				{
					$val	= $DATA_ERROR_VALUE;
				}
				else
				{
					$val	= &GetTypeNumber( $w, \@PLACEGMM_BUFF );
				}
				push( @DATA_PLACE6, $val );
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
		print( "ZONE=".$DATA_ZONEID[$i]."\n" );
		print( "curX=".$DATA_CURSOR_X[$i]."\n" );
		print( "curY=".$DATA_CURSOR_Y[$i]."\n" );
		print( "hisX=".$DATA_HIT_S_X[$i]."\n" );
		print( "hisY=".$DATA_HIT_S_Y[$i]."\n" );
		print( "hieX=".$DATA_HIT_E_X[$i]."\n" );
		print( "hieY=".$DATA_HIT_E_Y[$i]."\n" );
		print( "hitW=".$DATA_HIT_W[$i]."\n" );
		print( "type=".$DATA_TYPE[$i]."\n" );
		print( "skyf=".$DATA_SKY_FLAG[$i]."\n" );
		print( "warX=".$DATA_WARP_X[$i]."\n" );
		print( "warY=".$DATA_WARP_Y[$i]."\n" );
		print( "guid=".$DATA_GUIDE[$i]."\n" );
		print( "pla1=".$DATA_PLACE1[$i]."\n" );
		print( "pla2=".$DATA_PLACE2[$i]."\n" );
		print( "pla3=".$DATA_PLACE3[$i]."\n" );
		print( "pla4=".$DATA_PLACE4[$i]."\n" );
		print( "pla5=".$DATA_PLACE5[$i]."\n" );
		print( "pla6=".$DATA_PLACE6[$i]."\n" );
		print( "\n" );
	}
}

#-------------------------------------
#	�w�b�_�쐬
#=====================================
open( FILEOUT, ">$OUTPUTNAME_HEADER" );
print( FILEOUT "#pragma once\n" );
print( FILEOUT "//���̃t�@�C����townmap_data_conv�ɂ���Ď�����������Ă��܂�\n\n" );
print( FILEOUT "//�f�[�^����\n" );
print( FILEOUT "#define TOWNMAP_DATA_MAX\t($DATA_LENGTH)\n\n" );
print( FILEOUT "//�ꏊ�̃^�C�v\n" );
$cnt	= 0;
foreach $type ( @TYPE_BUFF )
{
	print( FILEOUT "#define TOWNMAP_PLACETYPE_$type\t($cnt)\n" );
	$cnt++;
}
print( FILEOUT "#define TOWNMAP_PLACETYPE_MAX\t($cnt)\n\n" );
close( FILEOUT ); 

#-------------------------------------
#	�f�[�^��
#=====================================
open( FILEOUT, ">$OUTPUTNAME_DATA" );
binmode( FILEOUT );
for( my $i = 0; $i < $DATA_LENGTH; $i++ )
{
	print( FILEOUT pack( "S", $DATA_ZONEID[$i] ) );
	print( FILEOUT pack( "S", $DATA_CURSOR_X[$i] ) );
	print( FILEOUT pack( "S", $DATA_CURSOR_Y[$i] ) );
	print( FILEOUT pack( "S", $DATA_HIT_S_X[$i] ) );
	print( FILEOUT pack( "S", $DATA_HIT_S_Y[$i] ) );
	print( FILEOUT pack( "S", $DATA_HIT_E_X[$i] ) );
	print( FILEOUT pack( "S", $DATA_HIT_E_Y[$i] ) );
	print( FILEOUT pack( "S", $DATA_HIT_W[$i] ) );
	print( FILEOUT pack( "S", $DATA_TYPE[$i] ) );
	print( FILEOUT pack( "S", $DATA_SKY_FLAG[$i] ) );
	print( FILEOUT pack( "S", $DATA_WARP_X[$i] ) );
	print( FILEOUT pack( "S", $DATA_WARP_Y[$i] ) );
	print( FILEOUT pack( "S", $DATA_GUIDE[$i] ) );
	print( FILEOUT pack( "S", $DATA_PLACE1[$i] ) );
	print( FILEOUT pack( "S", $DATA_PLACE2[$i] ) );
	print( FILEOUT pack( "S", $DATA_PLACE3[$i] ) );
	print( FILEOUT pack( "S", $DATA_PLACE4[$i] ) );
	print( FILEOUT pack( "S", $DATA_PLACE5[$i] ) );
	print( FILEOUT pack( "S", $DATA_PLACE6[$i] ) );
}
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
