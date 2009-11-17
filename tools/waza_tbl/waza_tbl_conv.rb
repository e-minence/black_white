#! ruby -Ks

  require File.dirname(__FILE__) + '/../label_make/label_make'
  require File.dirname(__FILE__) + '/../gmm_make/gmm_make'
  require File.dirname(__FILE__) + '/../constant'
  require 'date'

class PARA
  enum_const_set %w[
    WAZANAME
    ATKMSG
    TYPE
    KIND
    POW
    HIT
    PP
    MAX
  ]
end

	if ARGV.size < 2
		print "error: ruby waza_tbl_conv.rb read_file gmm_file\n"
		print "read_file:�ǂݍ��ރt�@�C��\n"
		print "gmm_file:�|�P��������gmm�t�@�C���������o�����߂̌��ɂȂ�t�@�C��\n"
		exit( 1 )
	end

  ARGV_READ_FILE = 0
  ARGV_READ_GMM_FILE = 1

  label = LabelMake.new
  read_data = []
  cnt = 0
  open( ARGV[ ARGV_READ_FILE ] ) {|fp_r|
    while str = fp_r.gets
      str = str.tosjis
      str = str.tr( "\"\r\n", "" )
      split_data = str.split(/\t/)
      next if split_data.size <= 1       #�T�[�o����̃G�N�X�|�[�g�ŃS�~���R�[�h�����邱�Ƃ�����̂ŁA�r������
      read_data[ cnt ] = str
      cnt += 1
    end
  }

  wazano = Hash::new
  wazaname = []
  
  #�Z���x����gmm�t�@�C������
  print "�Z���x����gmm�t�@�C���@������\n"
  fp_wazano = open( "wazano_def.h", "w" )

  fp_wazano.print( "//============================================================================================\n" )
  fp_wazano.print( "/**\n" )
  fp_wazano.print( " * @file	wazano_def.h\n" )
  fp_wazano.print( " * @bfief	�ZNo��Define��`�t�@�C��\n" )
  fp_wazano.print( " * @author	WazaTableConverter\n" )
  fp_wazano.printf( " * @date\t%s\n", Date::today.to_s )
  fp_wazano.print( " * �Z�e�[�u���R���o�[�^���琶������܂���\n" )
  fp_wazano.print( "*/\n")
  fp_wazano.print( "//============================================================================================\n" )
  fp_wazano.print( "\n#pragma once\n\n" )
  fp_wazano.printf( "#define\t\tWAZANO_NULL\t\t\t\t\t\t( 0 )\n" )

  #�n�b�V��
  fp_hash = open( "wazano_hash.rb", "w" )
  fp_hash.printf("#! ruby -Ks\n\n" )
  fp_hash.printf("\t$wazano_hash = {\n" )

  #GMM
  gmm = GMM::new
  gmm.open_gmm( ARGV[ ARGV_READ_GMM_FILE ] , "wazaname.gmm" )
  gmm.make_row_index( "WAZANO_", 0, "�[�[�[�[�[" )

  cnt = 1

  read_data.size.times {|i|
    split_data = read_data[ i ].split(/\t/)
    fp_wazano.print( "#define\t\t" )
    label_str = label.make_label( "WAZANO_", split_data[ PARA::WAZANAME ] )
    fp_wazano.print( label_str )
    tab_cnt = ( 19 - label_str.length ) / 2 + 2
    for j in 1..tab_cnt
      fp_wazano.print( "\t" )
    end
    fp_wazano.printf( "( %d )\t\t//%s\n", cnt, split_data[ PARA::WAZANAME ] )
    fp_hash.printf("\t\t\"%s\"=>%d,\n", split_data[ PARA::WAZANAME ], cnt )
    gmm.make_row_index( "WAZANAME_", cnt, split_data[ PARA::WAZANAME ] )
    cnt += 1
  }

  fp_wazano.printf( "#define\t\tWAZANO_MAX\t\t\t\t\t\t( %d )\n", cnt )
  fp_wazano.close

  fp_hash.printf("\t}\n" )
  fp_hash.close

  gmm.close_gmm

  print "�Z���x����gmm�t�@�C���@�����I��\n"

