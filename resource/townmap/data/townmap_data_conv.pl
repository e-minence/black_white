#[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
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
@TOWNMAP_XLS_HEADER	  = ();		#�^�E���}�b�v�G�N�Z���f�[�^�w�b�_
@TOWNMAP_XLS_DATA		  = ();		#�^�E���}�b�v�G�N�Z���f�[�^�̃f�[�^�{��
@TOWNMAP_XLS_REPLACE  = ();		#�^�E���}�b�v�G�N�Z���f�[�^�̒u�������f�[�^�{��
@TOWNMAP_XLS_NGLIST  = ();		#�^�E���}�b�v�G�N�Z���f�[�^��NGLIST�f�[�^�{��

@ZONETABLE_XLS_DATA	= ();		#�]�[���e�[�u���̃f�[�^

#��������f�[�^��
$OUTPUTNAME_DATA		= "townmap_data";
$OUTPUTNAME_PREFIX  = ".dat";
$OUTPUTNAME_HEADER	= "townmap_data.h";
$OUTPUTNAME_REPLACE	= "townmap_replace.dat";
$OUTPUTNAME_REPLACE_CDAT	= "townmap_replace.cdat";

$OUTPUTNAME_ERROR 	= "townmap_error.txt";

#��`���擾�p�t�@�C���l�[��
$ZONEID_FILENAME		=	"../../fldmapdata/zonetable/zone_id.h";
$GMM_DIR						= "../../message/dst/";	#�R���o�[�g���msg_xxx.h��define���Q�Ƃ��邽��
$GUIDGMM_FILENAME		=	"";
$PLACEGMM_FILENAME	= "";
$SYSFLAG_FILENAME		= "../../fldmapdata/flagwork/flag_define.h";
$ZONE_TABLE_FILENAME= "zonetable.xls";
$ZONE_TABLE_FILEPATH= "../../fldmapdata/zonetable/";

$ZONE_ID_PREFIX     = "ZONE_ID_";

#��`���擾�̂��߂̃o�b�t�@
@ZONEID_BUFF				= ();		#�]�[��ID�p�o�b�t�@
@GUIDGMM_BUFF				= ();		#�K�C�h����ID
@PLACEGMM_BUFF			=	();		#��������ID
@TYPE_BUFF					= ();		#�^�C�v��	���ꂾ��header������o��
@ZONE_SEARCH				= ();		#�����̏ꏊ�𒲂ׂ���@
@SYSFLAG_BUFF				= ();		#�V�X�e���t���O
@USERFLAG_BUFF			= ();	  #header�Ŏw�肵�����[�U�[�w��t���O
%ZONEGROUP_HASH			= ();	  #�]�[���O���[�v�p�n�b�V��
@VERSION_TYPE       = ();   #�o�[�W�����^�C�v

#�擾�����f�[�^
@DATA_ZONEID				=	();		#�]�[��ID
@DATA_ZONESEARCH		=	();		#�T�����@
@DATA_POS_X					=	();		#���WX
@DATA_POS_Y					=	();		#���WY
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
@DATA_ARRIVE_FLAG		= ();		#�����t���O
@DATA_HIDE_FLAG		  = ();		#�B���t���O
@DATA_GUIDE					= ();		#�K�C�hID
@DATA_PLACE1				= ();		#�����PID
@DATA_PLACE2				= ();		#�����QID
@DATA_PLACE3				= ();		#�����RID
@DATA_PLACE4				= ();		#�����SID
@DATA_PLACE5				= ();		#�����TID
@DATA_PLACE6				= ();		#�����UID
@DATA_ZKN_ANM				=	();		#�}�ӕ��z�A�j���ԍ�
@DATA_ZKN_POS_X			=	();		#�}�ӕ��z�z�u���WX
@DATA_ZKN_POS_Y			=	();		#�}�ӕ��z�z�u���WY
@DATA_VERSION 			=	();		#�o�[�W�����ʏ��

#�擾�����u�������f�[�^
$DATA_REPLACE_LENGTH  = 0;  #����
@DATA_REPLACE_ZONEID  = (); #�u��������]�[���O���[�v
@DATA_REPLACE_SRCID00  = (); #�u���������]�[���O���[�v00

#NG���X�g
$DATA_NGLIST_LENGTH  = 0;  #����
@DATA_NGLIST_ID  = (); #�^�E���}�b�v�֓o�^���Ȃ��Ă��悢ID

#�f�[�^����
$DATA_LENGTH				  = 0;  #����
@DATA_VERSION_LENGTH  = (); #�o�[�W�������Ƃ̑���

#�G���[�f�[�^
$DATA_ERROR_VALUE		= 0xFFFF;

#���[�U�[�w��t���O�̊J�n���l
$USER_DEFINED_FLAG_START_NUM  = 0xF000;

#�o�[�W������`�Ȃ�
$DATA_VERSION_NONE  = 0xFFFF;

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
&EXCEL_GetData( $ARGV[0], "replace", \@TOWNMAP_XLS_REPLACE );
&EXCEL_GetData( $ARGV[0], "ng_list", \@TOWNMAP_XLS_NGLIST);

#-------------------------------------
#	�w�b�_�����擾
#=====================================
&TownmapData_ConvertHeader;

#-------------------------------------
#	��`���̂��߂̃f�[�^�ǂݍ���
#=====================================
&FILE_GetData( $ZONEID_FILENAME, \@ZONEID_BUFF );
&FILE_GetData( $GUIDGMM_FILENAME, \@GUIDGMM_BUFF );
&FILE_GetData( $PLACEGMM_FILENAME, \@PLACEGMM_BUFF );
&FILE_GetData( $SYSFLAG_FILENAME, \@SYSFLAG_BUFF );

#-------------------------------------
#	�]�[���O���[�v���~�����̂�
#	zonetable.xls���R���o�[�g
#=====================================
&TownmapData_ConvertZoneTable;

#-------------------------------------
#	�f�[�^���擾
#=====================================
&TownmapData_ConvertData;
&TownmapData_ConvertReplace;
&TownmapData_ConvertNgList;

#-------------------------------------
#	�����蔻����ɏꏊ�̈ʒu�𑫂�
#=====================================
for( my $i = 0; $i < $DATA_LENGTH; $i++ )
{
  $DATA_HIT_S_X[$i]	+= $DATA_POS_X[$i];
  $DATA_HIT_S_Y[$i]	+= $DATA_POS_Y[$i];
  $DATA_HIT_E_X[$i]	+= $DATA_POS_X[$i];
  $DATA_HIT_E_Y[$i]	+= $DATA_POS_Y[$i];
}

#-------------------------------------
#	�f�o�b�O�v�����g
#=====================================
#&TownmapData_Print;

#-------------------------------------
#	�w�b�_�쐬
#=====================================
&TownmapData_CreateHeader;

#-------------------------------------
#	�o�[�W�����ʃA�[�J�C�u����邽�߂�
#	  ������o�C�i�������
#=====================================
foreach $ver ( @VERSION_TYPE )
{
  #	�f�[�^��
  &TownmapData_CreateBinaryData( $ver );
}
#&TownmapData_CreateBinaryReplace;
&TownmapData_CreateReplaceCdat;

#-------------------------------------
#	�^�E���}�b�v�ɓo�^����Ă��Ȃ��A
#	  �]�[���e�[�u���̃O���[�v������ꍇ�̃`�F�b�N
#	  ->�o�^����ĂȂ��ƁA�A�v�����ŃA�T�[�g���N����܂�
#=====================================
$is_error = 0;
open( FILEOUT, ">$OUTPUTNAME_ERROR" );
foreach my $key ( keys %ZONEGROUP_HASH )
{ 
  $is_exist_main = 0;
  foreach my $id (@DATA_ZONEID)
  {
    if( $ZONEGROUP_HASH{$key} == $id )
    {
      $is_exist_main = 1;
      last;
    }
  }

  $is_exist_sub = 0;
  foreach my $rep (@DATA_REPLACE_SRCID00)
  {
    if( $ZONEGROUP_HASH{$key} == $rep )
    {
      $is_exist_sub = 1;
      last;
    }
  }

  $is_exist_ng  = 0;
  foreach my $ng (@DATA_NGLIST_ID)
  {
    if( $ZONEGROUP_HASH{$key} == $ng )
    {
      $is_exist_ng = 1;
      last;
    }
  }

  if( $is_exist_main == 0 && $is_exist_sub == 0 && $is_exist_ng == 0 )
  {
    $is_error = 1;
    print( FILEOUT "$key = $ZONEGROUP_HASH{$key}\n" );
  }
}
close( FILEOUT );

if( $is_error == 1 )
{
  print "�]�[���e�[�u���ɂ����āA�^�E���}�b�v�֐ݒ肵�Ă��Ȃ��]�[���O���[�v������܂����B\n";
  print "Townmap_error.txt���Q�l��replace���֒ǉ������Ă��������B\n";
  exit(1);
}

#-------------------------------------
#	����I��
#=====================================
exit(0);

#=============================================================================
#
#					�v���C�x�[�g
#
#============================================================================
#-------------------------------------
#	@brief	�w�b�_���R���o�[�g
#=====================================
sub TownmapData_ConvertHeader
{
  my $line_cnt	= 0;
  my $is_file_name_start	= 0;
  my $is_type_start	= 0;
  my $zone_search_start	= 0;
  my $user_defined_flag_start	= 0;
  my $version_type  = 0;
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
    elsif( $word[0] eq "#zone_search_start" )
    {
      $line_cnt	= 0;
      $zone_search_start	= 1;
    }
    elsif( $word[0] eq "#zone_search_end" )
    {
      $zone_search_start	= 0;
    }
    elsif( $word[0] eq "#user_defined_flag_start" )
    {
      $line_cnt	= 0;
      $user_defined_flag_start	= 1;
    }
    elsif( $word[0] eq "#user_defined_flag_end" )
    {
      $user_defined_flag_start	= 0;
    }
    elsif( $word[0] eq "#version_type_start" ) 
    {
      $line_cnt	= 0;
      $version_type = 1;
    }
    elsif( $word[0] eq "#version_type_end" )
    {
      $version_type = 0;
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
    #��ރf�[�^�擾
    elsif( $zone_search_start == 1 )
    {
      if( $line_cnt >= 1 ) 
      {
        #print "$word[1]"."\n";
        push( @ZONE_SEARCH, $word[1] );
      }
    }
    #���[�U�[�w��t���O�擾
    elsif( $user_defined_flag_start == 1 )
    {
      if( $line_cnt >= 1 ) 
      {
        push( @USERFLAG_BUFF, $word[1] );
      }
    }
    #�o�[�W�����ʒ�`
    elsif( $version_type )
    {
      if( $line_cnt >= 1 ) 
      {
        #print "$word[1]"."\n";
        push( @VERSION_TYPE, $word[1] );
      }
    }

    #�s���J�E���g
    $line_cnt++;
  }
}
#-------------------------------------
#	@brief	�]�[���O���[�v�̂��߂Ƀ]�[���e�[�u�����R���o�[�g
#=====================================
sub TownmapData_ConvertZoneTable
{
  system( 'cp '.$ZONE_TABLE_FILEPATH.$ZONE_TABLE_FILENAME.' .' );
  &EXCEL_GetData( $ZONE_TABLE_FILENAME, "Sheet1", \@ZONETABLE_XLS_DATA );

  my $zonetable_start  = 0;
  foreach $line ( @ZONETABLE_XLS_DATA )
  {
    $line	=~ s/\r\n//g;
    @word	= split( /,/, $line );

    if( $word[0] eq "ID" )
    {
      @TAG_WORD = @word;
      $zonetable_start  = 1;
      next;
    }
    elsif( $word[0] eq "END" )
    {
      $zonetable_start  = 0;
    }

    if( $zonetable_start == 1 )
    {
      for( my $i = 0; $i < @TAG_WORD; $i++ )
      {
        my $tag = $TAG_WORD[$i];
        my $w   = $word[$i];


        if( $tag eq "ZONE_GROUP" )
        {
          $w    = $ZONE_ID_PREFIX . $w;
          $num  = &GetTypeNumber( $w, \@ZONEID_BUFF );

          $ZONEGROUP_HASH{ $w }  = $num;
          #print "$w, $num\n";
        }
      }
    }
  }
  system( 'rm '.$ZONE_TABLE_FILENAME );
}

#-------------------------------------
#	@brief	�G�N�Z���f�[�^�R���o�[�g
#=====================================
sub TownmapData_ConvertData
{

  my $is_start	= 0;
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

          unless( exists( $ZONEGROUP_HASH{ $w } ) )
          {
            print ( "�]�[���O���[�v�ł͂Ȃ��l���ݒ肳��Ă��܂� $w\n" );
            exit(1);
          } 

          push( @DATA_ZONEID, $ZONEGROUP_HASH{ $w } );
        }
        #�o�[�W������`
        elsif( $tag eq "#version" )
        {
          my $val;
          if( $w eq "" )
          {
            $val	= $DATA_VERSION_NONE;
          }
          else
          {
            $val	= &GetArrayNumber( $w, \@VERSION_TYPE );
            $DATA_VERSION_LENGTH[ $val ]++;
          }
          push( @DATA_VERSION, $val );
        }
        #�����̈ʒu�T��
        elsif( $tag eq "#zone_search" )
        {
          &UndefAssert( $w );
          $val	= &GetArrayNumber( $w, \@ZONE_SEARCH );
          push( @DATA_ZONESEARCH, $val );
        }
        #���WX
        elsif( $tag eq "#pos_x" )
        {
          &UndefAssert( $w );
          push( @DATA_POS_X, $w );
        }
        #���WY
        elsif( $tag eq "#pos_y" )
        {
          &UndefAssert( $w );
          push( @DATA_POS_Y, $w );
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
        #�����t���O
        elsif( $tag eq "#arrive_flag" )
        {
          my $val;
          if( $w eq "" )
          {
            $val	= $DATA_ERROR_VALUE;
          }
          else
          {
            $val	= &GetFlagNumber( $w );
          }
          push( @DATA_ARRIVE_FLAG, $val );
        }
        #�B���}�b�v�t���O
        elsif( $tag eq "#hide_flag" )
        {
          my $val;
          if( $w eq "" )
          {
            $val	= $DATA_ERROR_VALUE;
          }
          else
          {
            $val	= &GetFlagNumber( $w );
          }
          push( @DATA_HIDE_FLAG, $val );
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
				#�}�ӕ��z�A�j���ԍ�
				elsif( $tag eq "#zkn_anm" )
				{
					&UndefAssert( $w );
					push( @DATA_ZKN_ANM, $w );
				}
				#�}�ӕ��z�z�u���WX
				elsif( $tag eq "#zkn_pos_x" )
				{
					&UndefAssert( $w );
					push( @DATA_ZKN_POS_X, $w );
				}
				#�}�ӕ��z�z�u���WY
				elsif( $tag eq "#zkn_pos_y" )
				{
					&UndefAssert( $w );
					push( @DATA_ZKN_POS_Y, $w );
				}
				#�I��
				elsif( $tag eq "#end" )
				{
					$DATA_LENGTH++;
				}
			}
		}
  }
}

#-------------------------------------
#	@brief	�G�N�Z���u�������f�[�^�R���o�[�g
#=====================================
sub TownmapData_ConvertReplace
{

  my $is_start	= 0;
  foreach $line ( @TOWNMAP_XLS_REPLACE )
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

        #�u��������ID
        if( $tag eq "#replace_id" )
        {
          &UndefAssert( $w );

          unless( exists( $ZONEGROUP_HASH{ $w } ) )
          {
            print ( "�]�[���O���[�v�ł͂Ȃ��l���ݒ肳��Ă��܂� $w\n" );
            exit(1);
          } 

          push( @DATA_REPLACE_ZONEID, $ZONEGROUP_HASH{ $w } );
        }
        #�u��������ID00
        elsif( $tag eq "#src_id_00" )
        {
          &UndefAssert( $w );

          unless( exists( $ZONEGROUP_HASH{ $w } ) )
          {
            print ( "�]�[���O���[�v�ł͂Ȃ��l���ݒ肳��Ă��܂� $w\n" );
            exit(1);
          } 

          push( @DATA_REPLACE_SRCID00, $ZONEGROUP_HASH{ $w } );
        }
        #�I��
				elsif( $tag eq "#end" )
				{
					$DATA_REPLACE_LENGTH++;
				}
			}
		}
  }
}

#-------------------------------------
#	@brief	�G�N�Z��NGLIST�f�[�^�R���o�[�g
#=====================================
sub TownmapData_ConvertNgList
{
  my $is_start	= 0;
  foreach $line ( @TOWNMAP_XLS_NGLIST )
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

        #NG_ID
        if( $tag eq "#ng_id" )
        {
          &UndefAssert( $w );

          unless( exists( $ZONEGROUP_HASH{ $w } ) )
          {
            print ( "�]�[���O���[�v�ł͂Ȃ��l���ݒ肳��Ă��܂� $w\n" );
            exit(1);
          } 

          push( @DATA_NGLIST_ID, $ZONEGROUP_HASH{ $w } );
        }
        #�I��
				elsif( $tag eq "#end" )
				{
					$DATA_NGLIST_LENGTH++;
				}
			}
		}
  }
}

#-------------------------------------
#	@brief	�w�b�_�쐬
#=====================================
sub TownmapData_CreateHeader
{
  #�o�[�W�������Ƃ̃t�@�C�����͓����Ƃ��Ă���
  my $len = $DATA_LENGTH-$DATA_VERSION_LENGTH[0];

  print "$DATA_LENGTH, $DATA_VERSION_LENGTH[0]\n";

  open( FILEOUT, ">$OUTPUTNAME_HEADER" );
  print( FILEOUT "#pragma once\n" );
  print( FILEOUT "//���̃t�@�C����townmap_data_conv�ɂ���Ď�����������Ă��܂�\n\n" );
  print( FILEOUT "//�f�[�^����\n" );
  print( FILEOUT "#define TOWNMAP_DATA_MAX\t($len)\n\n" );
  print( FILEOUT "//�f�[�^�G���[�l\n" );
  print( FILEOUT "#define TOWNMAP_DATA_ERROR\t($DATA_ERROR_VALUE)\n\n" );
  print( FILEOUT "//�ꏊ�̃^�C�v\n" );
  $cnt	= 0;
  foreach $type ( @TYPE_BUFF )
  {
    print( FILEOUT "#define TOWNMAP_PLACETYPE_$type\t($cnt)\n" );
    $cnt++;
  }
  $cnt	= 0;
  foreach $search ( @ZONE_SEARCH )
  {
    print( FILEOUT "#define TOWNMAP_ZONESEARCH_$search\t($cnt)\n" );
    $cnt++;
  }
  print( FILEOUT "#define TOWNMAP_PLACETYPE_MAX\t($cnt)\n\n" );

  print( FILEOUT "//���[�U�[�w��t���O\n" );
  print( FILEOUT "//�����t���O��B���}�b�v�t���O���ȉ��̒l�Ȃ�΃v���O�����Ōʐ�����s��Ȃ���΂Ȃ�Ȃ�\n" );
  $cnt	= $USER_DEFINED_FLAG_START_NUM;
  foreach $search ( @USERFLAG_BUFF )
  {
    print( FILEOUT "#define TOWNMAP_$search\t($cnt)\n" );
    $cnt++;
  }

  close( FILEOUT ); 
}

#-------------------------------------
#	@brief	�o�C�i���f�[�^�쐬
#	@param version
#=====================================
sub TownmapData_CreateBinaryData
{
  my( $ver ) = @_;

  $outputname = $OUTPUTNAME_DATA ."_". lc($ver) . $OUTPUTNAME_PREFIX;

  open( FILEOUT, ">$outputname" );
  binmode( FILEOUT );
  for( my $i = 0; $i < $DATA_LENGTH; $i++ )
  {
    if( !(&GetArrayNumber($ver, \@VERSION_TYPE) eq $DATA_VERSION[$i] || $DATA_VERSION_NONE eq $DATA_VERSION[$i]) )
    {
      next;
    }

    print( FILEOUT pack( "S", $DATA_ZONEID[$i] ) );
    print( FILEOUT pack( "S", $DATA_ZONESEARCH[$i] ) );
    print( FILEOUT pack( "S", $DATA_POS_X[$i] ) );
    print( FILEOUT pack( "S", $DATA_POS_Y[$i] ) );
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
    print( FILEOUT pack( "S", $DATA_ARRIVE_FLAG[$i] ) );
    print( FILEOUT pack( "S", $DATA_HIDE_FLAG[$i] ) );
    print( FILEOUT pack( "S", $DATA_GUIDE[$i] ) );
    print( FILEOUT pack( "S", $DATA_PLACE1[$i] ) );
    print( FILEOUT pack( "S", $DATA_PLACE2[$i] ) );
    print( FILEOUT pack( "S", $DATA_PLACE3[$i] ) );
    print( FILEOUT pack( "S", $DATA_PLACE4[$i] ) );
    print( FILEOUT pack( "S", $DATA_PLACE5[$i] ) );
    print( FILEOUT pack( "S", $DATA_PLACE6[$i] ) );
    print( FILEOUT pack( "S", $DATA_ZKN_ANM[$i] ) );
    print( FILEOUT pack( "S", $DATA_ZKN_POS_X[$i] ) );
    print( FILEOUT pack( "S", $DATA_ZKN_POS_Y[$i] ) );
  }
  close( FILEOUT ); 
}

#-------------------------------------
#	@brief	�u�������o�C�i���f�[�^�쐬
#	@param version
#=====================================
sub TownmapData_CreateBinaryReplace
{
  open( FILEOUT, ">$OUTPUTNAME_REPLACE" );
  binmode( FILEOUT );
  for( my $i = 0; $i < $DATA_REPLACE_LENGTH; $i++ )
  {
    print( FILEOUT pack( "S", $DATA_REPLACE_ZONEID[$i] ) );
    print( FILEOUT pack( "S", $DATA_REPLACE_SRCID00[$i] ) );
  }
  close( FILEOUT ); 
}

#-------------------------------------
#	@brief	�u��������CDAT�쐬
#	@param version
#=====================================
sub TownmapData_CreateReplaceCdat
{
  open( FILEOUT, ">$OUTPUTNAME_REPLACE_CDAT" );

  print( FILEOUT "#pragma once\n" );
  print( FILEOUT "//���̃t�@�C����townmap_data_conv�ɂ���Ď�����������Ă��܂�\n\n" );

  print( FILEOUT "enum{\n" );
  print( FILEOUT "  TOWNMAP_REPLACE_DST,\n" );
  print( FILEOUT "  TOWNMAP_REPLACE_SRC00,\n" );
  print( FILEOUT "  TOWNMAP_REPLACE_MAX,\n" );
  print( FILEOUT "};\n" );

  print( FILEOUT "#define TOWNMAP_REPLACE_MAX  $DATA_REPLACE_LENGTH\n", );

  print( FILEOUT "static const int sc_townmap_replace_data[][TOWNMAP_REPLACE_MAX] =\n" );
  print( FILEOUT "{\n" );
  for( my $i = 0; $i < $DATA_REPLACE_LENGTH; $i++ )
  {
    print( FILEOUT "\t{ $DATA_REPLACE_ZONEID[$i], $DATA_REPLACE_SRCID00[$i] },\n" );
  }
  print( FILEOUT "};\n" );
  close( FILEOUT ); 
}

#-------------------------------------
#	@brief	�f�o�b�O�v�����g
#=====================================
sub TownmapData_Print
{
	print( "DEBUG_PRINT_START\n" );
	for( my $i = 0; $i < $DATA_LENGTH; $i++ )
	{
		print( "DATA=".$i."\n" );
		print( "ZONE=".$DATA_ZONEID[$i]."\n" );
		print( "SEAR=".$DATA_ZONESEARCH[$i]."\n" );
		print( "posX=".$DATA_POS_X[$i]."\n" );
		print( "posY=".$DATA_POS_Y[$i]."\n" );
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
		print( "arrf=".$DATA_ARRIVE_FLAG[$i]."\n" );
    print( "hidf=".$DATA_HIDE_FLAG[$i]."\n" );
		print( "guid=".$DATA_GUIDE[$i]."\n" );
		print( "pla1=".$DATA_PLACE1[$i]."\n" );
		print( "pla2=".$DATA_PLACE2[$i]."\n" );
		print( "pla3=".$DATA_PLACE3[$i]."\n" );
		print( "pla4=".$DATA_PLACE4[$i]."\n" );
		print( "pla5=".$DATA_PLACE5[$i]."\n" );
		print( "pla6=".$DATA_PLACE6[$i]."\n" );
   	print( "zknAnm=".$DATA_ZKN_ANM[$i]."\n" );
		print( "zknPosX=".$DATA_ZKN_POS_X[$i]."\n" );
		print( "zknPosY=".$DATA_ZKN_POS_Y[$i]."\n" );
		print( "version=".$DATA_VERSION[$i]."\n" );
    print( "\n" );
	}
}

#=============================================================================
#
#					���ʃ��W���[����
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
      #print( " $name == $data \n" );
      #print "ok $1 \n";
			return $1;
		}
	}

  #USER�t���O�̃T�[�`
  for( $i = 0; $i < @USERFLAG_BUFF; $i++ )
  {
		if( $USERFLAG_BUFF[ $i ] eq $name )
		{
      #print( " $name == $USERFLAG_BUFF[ $i ] \n" );
      #print "ok $USER_DEFINED_FLAG_START_NUM + $i \n";
			return $USER_DEFINED_FLAG_START_NUM + $i;
		}
    #print( "$USERFLAG_BUFF[ $i ] == $name\n" );
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
