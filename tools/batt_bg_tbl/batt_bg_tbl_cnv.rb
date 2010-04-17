#! ruby -Ks

############################################################################################################
#
#   �퓬�w�i�Ή��\�R���o�[�^
#
############################################################################################################

  FILE_IMD = 0
  FILE_ICA = 1
  FILE_ITA = 2
  FILE_IMA = 3

  FILE_SPRING = 0
  FILE_SUMMER = 1
  FILE_AUTUMN = 2
  FILE_WINTER = 3
  DATA_EDGE_COLOR  = 4

#	�]�[���ʎw��N���X
class	ZoneSpec
	def initialize
		@zone_spec = []
	end

  #
  # �]�[���ʎw��ǉ�
  #
  # @param str      �]�[���ʎw��̃��x����
  # @param attr_num �A�g���r���[�g�̐�
  #
	def	add_zone_spec( str, attr_num )
		@zone_spec << ZoneSpecParam.new( str, attr_num )
	end

  def get_zone_spec( str )
    for i in 0..(@zone_spec.size - 1)
      if @zone_spec[ i ].get_zone_spec_name == str
        break
      end
    end
    if i == @zone_spec.size
      print "�]�[���ʎw�胉�x�����������܂���ł���\n"
    end
    @zone_spec[ i ]
  end

  def get_zone_spec_index( index )
    @zone_spec[ index ]
  end

  def get_zone_spec_size
    @zone_spec.size
  end
end

#	�]�[���p�����[�^
class	ZoneSpecParam
	def initialize( str, attr_num )
    @zone_spec_name = str
    @time_zone
    @season
    @attr_bg = []
    @attr_stage = []
    @attr_max = attr_num
  end

  def set_zone_spec_name( str )
    @zone_spec_name = str
  end

  def set_time_zone( str )
    @time_zone = str
  end

  def set_season( str )
    @season = str
  end

  def set_attr_bg( str )
    @attr_bg << str
    if @attr_bg.size > @attr_max
      #p @zone_spec_name
      #printf("attr_bg:%s %d\n",str,@attr_bg.size)
      #p @attr_max
      print "bg:�A�g���r���[�g�̒ǉ����ő�l�𒴂��܂���\n"
      exit( 1 )
    end
  end

  def set_attr_stage( str )
    @attr_stage << str
    if @attr_stage.size > @attr_max
      #p @zone_spec_name
      #printf("attr_stage:%s %d\n",str,@attr_stage.size)
      #p @attr_max
      print "stage:�A�g���r���[�g�̒ǉ����ő�l�𒴂��܂���\n"
      exit( 1 )
    end
  end

  def get_zone_spec_name
    @zone_spec_name
  end

  def get_time_zone
    @time_zone
  end

  def get_season
    @season
  end

  def get_attr_bg
    @attr_bg
  end

  def get_attr_stage
    @attr_stage
  end

  def get_attr_bg( num )
    if num > @attr_max
      print "�A�g���r���[�g�̍ő�l�𒴂��ăA�N�Z�X���悤�Ƃ��Ă��܂�\n"
      exit( 1 )
    end
    @attr_bg[ num ]
  end

  def get_attr_stage( num )
    if num > @attr_max
      print "�A�g���r���[�g�̍ő�l�𒴂��ăA�N�Z�X���悤�Ƃ��Ă��܂�\n"
      exit( 1 )
    end
    @attr_stage[ num ]
  end
end

#�O���t�B�b�N�t�@�C���e�[�u���N���X
class GraFile
  def initialize( str )
    @table_name = str
    @imd = []
    @ica = []
    @ita = []
    @ima = []
  end

  def set_file_name( str, kind, season )
    case kind
    when FILE_IMD 
      @imd[ season ] = str
    when FILE_ICA 
      @ica[ season ] = str
    when FILE_ITA 
      @ita[ season ] = str
    when FILE_IMA 
      @ima[ season ] = str
    end
  end

  def get_table_name
    @table_name
  end

  def get_file_name( kind, season )
    case kind
    when FILE_IMD 
      @imd[ season ]
    when FILE_ICA 
      @ica[ season ]
    when FILE_ITA 
      @ita[ season ]
    when FILE_IMA 
      @ima[ season ]
    end
  end
end

#���C�����[�`��
	if ARGV.size < 2
		print "error: ruby batt_bg_tbl_cnv.rb read_file output_dir\n"
		print "read_file:�ǂݍ��ރt�@�C��\n"
		print "output_dir:�R���o�[�g�����t�@�C�����i�[����f�B���N�g��\n"
		exit( 1 )
	end

	ARGV_READ_FILE = 0
	ARGV_OUT_DIR = 1

  ZONE_SPEC = 0
  TIME_ZONE = 1
  SEASON    = 2
  DATA_KIND = 3
  ATTR_1    = 4

  zone_spec = ZoneSpec.new
  attribute = []
  stage = []
  bg = []

	begin
		fp_r = open( ARGV[ ARGV_READ_FILE ] )
	rescue
		print "FileOpenError:" + ARGV[ ARGV_READ_FILE ]
		exit( 1 )
	end

	fp_spec   = open( ARGV[ ARGV_OUT_DIR ] +	"zone_spec_table.bin", "wb" )
	fp_hash   = open( ARGV[ ARGV_OUT_DIR ] +	"zone_spec.rb", "w" )
	fp_header = open( ARGV[ ARGV_OUT_DIR ] +	"batt_bg_tbl.h", "w" )
	fp_stage  = open( ARGV[ ARGV_OUT_DIR ] +	"batt_stage.s", "w" )
	fp_bg     = open( ARGV[ ARGV_OUT_DIR ] +	"batt_bg.s", "w" )
  fp_scr    = open( ARGV[ ARGV_OUT_DIR ] +	"battgra_3d_wb.scr", "w" )

  #�w�b�_�����ǂݍ���
	str = fp_r.gets
  str = str.tr( "\"\r\n", "" )
  split_data = str.split(/,/)

  if split_data[ ZONE_SPEC ] != "�]�[���ʎw��"
    print "�퓬�w�i�Ή��\�ł͂���܂���\n"
    exit( 1 )
  end
  
  #�A�g���r���[�g�ǂݍ���
	str = fp_r.gets
  str = str.tr( "\"\r\n", "" )
  split_data = str.split(/,/)

  split_data.size.times{|i|
    next if i < ATTR_1
    attribute << split_data[ i ]
  }

  #�]�[���ʎw��f�[�^�ǂݍ���
  while str = fp_r.gets
    str = str.tr( "\"\r\n", "" )
    split_data = str.split(/,/)
    if split_data[ ZONE_SPEC ] == "#"
      break
    end
    zone_spec.add_zone_spec( split_data[ ZONE_SPEC ], attribute.size )
    zone_spec.get_zone_spec( split_data[ ZONE_SPEC ] ).set_time_zone( split_data[ TIME_ZONE ] )
    zone_spec.get_zone_spec( split_data[ ZONE_SPEC ] ).set_season( split_data[ SEASON ] )
    for i in 0..(attribute.size - 1)
      zone_spec.get_zone_spec( split_data[ ZONE_SPEC ] ).set_attr_bg( split_data[ ATTR_1 + i ] )
    end
    str = fp_r.gets
    str = str.tr( "\"\r\n", "" )
    split_data2 = str.split(/,/)
    for i in 0..(attribute.size - 1)
      zone_spec.get_zone_spec( split_data[ ZONE_SPEC ] ).set_attr_stage( split_data2[ ATTR_1 + i ] )
    end
  end

  ATTR_NAME = 0
  FILE_KIND = 1
  SPRING_FILE = 2
  SUMMER_FILE = 3
  AUTUMN_FILE = 4
  WINTER_FILE = 5
  EDGE_COLOR  = 6

  file_kind = {
    "imd"=>FILE_IMD,
    "ica"=>FILE_ICA,
    "ita"=>FILE_ITA,
    "ima"=>FILE_IMA,
  }

  #���~�e�[�u���ǂݍ���
  while str = fp_r.gets
    str = str.tr( "\"\r\n", "" )
    split_data = str.split(/,/)
    if split_data[ ATTR_NAME ] == "���~�e�[�u��"
      break
    end
  end

  while str = fp_r.gets
    str = str.tr( "\"\r\n", "" )
    split_data = str.split(/,/)
    next if split_data[ ATTR_NAME ] == "" && split_data[ FILE_KIND ] == ""
    if split_data[ ATTR_NAME ] == nil
      break
    end
    if split_data[ ATTR_NAME ] != ""
      stage << GraFile.new( split_data[ ATTR_NAME ] )
    end
    cnt = stage.size - 1
    for i in FILE_SPRING..DATA_EDGE_COLOR
      stage[ cnt ].set_file_name( split_data[ SPRING_FILE + i ], file_kind[ split_data[ FILE_KIND ] ], i )
    end
  end

  #�w�i�a�f�e�[�u���ǂݍ���
  while str = fp_r.gets
    str = str.tr( "\"\r\n", "" )
    split_data = str.split(/,/)
    if split_data[ ATTR_NAME ] == "�w�i�a�f"
      break
    end
  end

  while str = fp_r.gets
    str = str.tr( "\"\r\n", "" )
    split_data = str.split(/,/)
    next if split_data[ ATTR_NAME ] == "" && split_data[ FILE_KIND ] == ""
    if split_data[ ATTR_NAME ] == "#"
      break
    end
    if split_data[ ATTR_NAME ] != ""
      bg << GraFile.new( split_data[ ATTR_NAME ] )
    end
    cnt = bg.size - 1
    for i in 0..3
      bg[ cnt ].set_file_name( split_data[ SPRING_FILE + i ], file_kind[ split_data[ FILE_KIND ] ], i )
    end
  end

  #�]�[���ʎw��A�z�z�񐶐�
  fp_hash.printf( "#! ruby -Ks\n\n" )
  fp_hash.printf( "\t$zone_spec = {\n" )
  zone_spec.get_zone_spec_size.times{|i|
    fp_hash.printf( "\t\t\"%s\"=>%d,\n", zone_spec.get_zone_spec_index( i ).get_zone_spec_name, i )
  }
  fp_hash.printf( "\t}\n" )

  ext_table = [ "NSBMD", "NSBCA", "NSBTA", "NSBMA" ]
  stage_hash = { "�~"=>0xff }
  bg_hash = { "�~"=>0xff }

  #���~�t�@�C���e�[�u������
  fp_stage.printf( "\t.text\n\n" )
  fp_stage.print( "\t.include ../../resource/battle/battgra_wb_def.h\n\n" )
  fp_stage.printf( "#define BATT_BG_TBL_NO_FILE ( 0xffffffff )\n\n" )
  stage.size.times{|i|
    fp_stage.printf( "//%s\n", stage[ i ].get_table_name )
    stage_hash[ stage[ i ].get_table_name ] = i
    for kind in FILE_IMD..FILE_IMA
      if kind == FILE_IMD
        edge_color = stage[ i ].get_file_name( kind, DATA_EDGE_COLOR )
        if edge_color == nil || edge_color == ""
          p "�G�b�W�J���[���ݒ肳��Ă��܂���"
          exit( 1 )
        end
        split_ec = edge_color.split(/:/)
        fp_stage.printf( "\t.long\t(%s<<10)|(%s<<5)|(%s)\n", split_ec[ 0 ], split_ec[ 1 ], split_ec[ 2 ] )
      end
      for season in FILE_SPRING..FILE_WINTER
        if stage[ i ].get_file_name( kind, season ) == "�~"
          fp_stage.printf( "\t.long\tBATT_BG_TBL_NO_FILE\n" )
        else
          fp_stage.printf( "\t.long\t%s_%s\n", stage[ i ].get_file_name( FILE_IMD, season ).upcase, ext_table[ kind ] )
          fp_scr.printf( "\"%s.%s\"\n", stage[ i ].get_file_name( FILE_IMD, season ), ext_table[ kind ] )
        end
      end
    end
  }
  
  #�w�i�a�f�t�@�C���e�[�u������
  fp_bg.printf( "\t.text\n\n" )
  fp_bg.print( "\t.include ../../resource/battle/battgra_wb_def.h\n\n" )
  fp_bg.printf( "#define BATT_BG_TBL_NO_FILE ( 0xffffffff )\n\n" )
  bg.size.times{|i|
    fp_bg.printf( "//%s\n", bg[ i ].get_table_name )
    bg_hash[ bg[ i ].get_table_name ] = i
    for kind in FILE_IMD..FILE_IMA
      for season in FILE_SPRING..FILE_WINTER
        if bg[ i ].get_file_name( kind, season ) == "�~"
          fp_bg.printf( "\t.long\tBATT_BG_TBL_NO_FILE\n" )
        else
          fp_bg.printf( "\t.long\t%s_%s\n", bg[ i ].get_file_name( FILE_IMD, season ).upcase, ext_table[ kind ] )
          fp_scr.printf( "\"%s.%s\"\n", bg[ i ].get_file_name( FILE_IMD, season ), ext_table[ kind ] )
        end
      end
    end
  }

  padding = 4 - ( attribute.size * 2 + 2 ) % 4

  if padding == 4
    padding = 0
  end

  for i in 0..(zone_spec.get_zone_spec_size-1)
    if zone_spec.get_zone_spec_index( i ).get_time_zone == "�~"
      time_zone = 0
    else
      time_zone = 1
    end
    if zone_spec.get_zone_spec_index( i ).get_season == "�~"
      season = 0
    else
      season = 1
    end
    write_data = [ time_zone, season ].pack("CC")
    fp_spec.write( write_data )
    for j in 0..(attribute.size-1)
      attr_bg = zone_spec.get_zone_spec_index( i ).get_attr_bg( j )
      if bg_hash[ attr_bg ] == nil
        printf( "�]�[���ʎw��[%s]\n�A�g���r���[�g[%s]\n�w�i�f�[�^�ɕs���Ȏw��[%s]������܂�\n", zone_spec.get_zone_spec_index( i ).get_zone_spec_name, attribute[ j ], attr_bg )
        exit( -1 )
        #write_data = [ 0 ].pack("C")
      else
        write_data = [ bg_hash[ attr_bg ] ].pack("C")
      end
      fp_spec.write( write_data )
    end
    for j in 0..(attribute.size-1)
      attr_stage = zone_spec.get_zone_spec_index( i ).get_attr_stage( j )
      if stage_hash[ attr_stage ] == nil
        printf( "�]�[���ʎw��[%s]�A�g���r���[�g[%s]�̂��~�ɕs���Ȏw��[%s]������܂�\n", zone_spec.get_zone_spec_index( i ).get_zone_spec_name, attribute[ j ], attr_stage )
        exit( -1 )
        #write_data = [ 0 ].pack("C")
      else
        write_data = [ stage_hash[ attr_stage ] ].pack("C")
      end
      fp_spec.write( write_data )
    end
	  padding.times{
		  fp_spec.print("0")
	  }
  end

  #�w�b�_�t�@�C������
  fp_header.print( "\n//============================================================================================\n" )
  fp_header.print( "/**\n" )
  fp_header.print( " * @file	batt_bg_tbl.h\n" )
  fp_header.print( " * @bfief	�퓬�w�i�e�[�u��\n" )
  fp_header.print( " * @author	BattBGConverter\n" )
  fp_header.print( " * �퓬�w�i�e�[�u���R���o�[�^���琶������܂���\n" )
  fp_header.print( "*/\n")
  fp_header.print( "//============================================================================================\n\n" )
  fp_header.printf( "#pragma once\n\n" )
  fp_header.printf( "#define  ZONE_SPEC_ATTR_MAX  ( %d )\n", attribute.size )
  fp_header.printf( "#define  BATT_BG_TBL_NO_FILE     ( 0xffffffff )\n" )
  fp_header.printf( "#define  BATT_BG_TBL_FILE_MAX  ( 4 )\n\n" )
  fp_header.printf( "#define  BATT_BG_TBL_SEASON_MAX  ( 4 )\n\n" )
  fp_header.printf( "typedef enum\n" )
  fp_header.printf( "{\n" )
  fp_header.printf( "\tBATT_BG_TBL_FILE_NSBMD = 0,\n" )
  fp_header.printf( "\tBATT_BG_TBL_FILE_NSBCA,\n" )
  fp_header.printf( "\tBATT_BG_TBL_FILE_NSBTA,\n" )
  fp_header.printf( "\tBATT_BG_TBL_FILE_NSBMA,\n" )
  fp_header.printf( "}BATT_BG_TBL_FILE;\n\n" )
  fp_header.printf( "typedef struct\n{\n" )
  fp_header.printf( "\tu8 time_zone;\n" )
  fp_header.printf( "\tu8 season;\n" )
  fp_header.printf( "\tu8 bg_file[ ZONE_SPEC_ATTR_MAX ];\n" )
  fp_header.printf( "\tu8 stage_file[ ZONE_SPEC_ATTR_MAX ];\n" )
  if padding != 0
    fp_header.printf( "\tu8 padding[ %d ];\n", padding )
  end
  fp_header.printf( "}BATT_BG_TBL_ZONE_SPEC_TABLE;\n\n" )
  fp_header.printf( "typedef struct\n{\n" )
  fp_header.printf( "\tu16  edge_color;\n" )
  fp_header.printf( "\tu16  padding;\n" )
  fp_header.printf( "\tARCDATID file[BATT_BG_TBL_FILE_MAX ][ BATT_BG_TBL_SEASON_MAX ];\n" )
  fp_header.printf( "}BATT_BG_TBL_STAGE_TABLE;\n\n" )
  fp_header.printf( "typedef struct\n{\n" )
  fp_header.printf( "\tARCDATID file[BATT_BG_TBL_FILE_MAX ][ BATT_BG_TBL_SEASON_MAX ];\n" )
  fp_header.printf( "}BATT_BG_TBL_BG_TABLE;\n" )

  fp_r.close
  fp_spec.close
  fp_hash.close
  fp_header.close
  fp_stage.close
  fp_bg.close
  fp_scr.close

  #�^�C���X�^���v��r�p�̃_�~�[�t�@�C���𐶐�
  fp_w = open( "out_end", "w" )
  fp_w.close

