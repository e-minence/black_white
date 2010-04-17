#! ruby -Ks

  require File.dirname(__FILE__) + '/../label_make/label_make'
  require File.dirname(__FILE__) + '/../gmm_make/gmm_make'
  require File.dirname(__FILE__) + '/../constant'

class PARA
  enum_const_set %w[
    NO
    NAME
    INFO1
    INFO2
  ]
end

	if ARGV.size < 2
		print "error: ruby tokusei_conv.rb read_file gmm_file\n"
		print "read_file:�ǂݍ��ރt�@�C��\n"
		print "gmm_file:��������gmm�t�@�C���������o�����߂̌��ɂȂ�t�@�C��\n"
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
      split_data = str.split(/,/)
      next if split_data.size <= 1       #�T�[�o����̃G�N�X�|�[�g�ŃS�~���R�[�h�����邱�Ƃ�����̂ŁA�r������
      read_data[ cnt ] = str
      cnt += 1
    end
  }

  tokuno = Hash::new
  tokuname = []
  
  #�������x����gmm�t�@�C������
  print "�������x����gmm�t�@�C���@������\n"
  fp_tokuno = open( "tokusyu_def.h", "w" )

  fp_tokuno.print( "//============================================================================================\n" )
  fp_tokuno.print( "/**\n" )
  fp_tokuno.print( " * @file	tokusyu_def.h\n" )
  fp_tokuno.print( " * @bfief	����No��Define��`�t�@�C��\n" )
  fp_tokuno.print( " * @author	TokuseiConverter\n" )
  fp_tokuno.print( " * �����R���o�[�^���琶������܂���\n" )
  fp_tokuno.print( "*/\n")
  fp_tokuno.print( "//============================================================================================\n" )
  fp_tokuno.print( "\n#pragma once\n\n" )
  fp_tokuno.printf( "#define\t\tTOKUSYU_NULL\t\t\t\t\t\t( 0 )\n" )

  #�n�b�V��
  fp_hash = open( "tokusei_hash.rb", "w" )
  fp_hash.printf("#! ruby -Ks\n\n" )
  fp_hash.printf("\t$tokusei_hash = {\n" )

  #GMM
  tokuname_gmm = GMM::new
  tokuname_gmm.open_gmm( ARGV[ ARGV_READ_GMM_FILE ] , "tokusei.gmm" )
  tokuname_gmm.make_row_index_kanji_hyphen( "TOKUSEI_", 0, 1, 0 )
  tokuinfo_gmm = GMM::new
  tokuinfo_gmm.open_gmm( ARGV[ ARGV_READ_GMM_FILE ] , "tokuseiinfo.gmm" )
  tokuinfo_gmm.make_row_index_kanji_hyphen( "TOKUSEIINFO_", 0, 2, 1 )

  cnt = 1

  read_data.size.times {|i|
    split_data = read_data[ i ].split(/,/)
    fp_tokuno.print( "#define\t\t" )
    tokuname << "%s\n" % [ split_data[ PARA::NAME ] ]
    label_str = label.make_label( "TOKUSYU_", split_data[ PARA::NAME ] )
    fp_tokuno.print( label_str )
    tab_cnt = ( 25 - label_str.length ) / 2 + 2
    for j in 1..tab_cnt
      fp_tokuno.print( "\t" )
    end
    fp_tokuno.printf( "( %d )\t\t//%s\n", cnt, split_data[ PARA::NAME ] )
    fp_hash.printf("\t\t\"%s\"=>%d,\n", split_data[ PARA::NAME ], cnt )
    tokuname_gmm.make_row_index_kanji( "TOKUSEI_", cnt, split_data[ PARA::NAME ], split_data[ PARA::NAME ] )
    if split_data[ PARA::INFO1 ] == nil
      info1 = ""
    else
      info1 = split_data[ PARA::INFO1 ]
    end
    if split_data[ PARA::INFO2 ] == nil
      info2 = ""
    else
      info2 = split_data[ PARA::INFO2 ]
    end
    info = info1 + "\r\n" + info2
    #@todo ��������������Ȃ�ł����A����f�[�^���Ȃ��̂ŁA����������Ő���
    tokuinfo_gmm.make_row_index_kanji( "TOKUSEIINFO_", cnt, info, info )
    cnt += 1
  }

  fp_tokuno.printf( "#define\t\tTOKUSYU_MAX\t\t\t\t\t\t( %d )\n", cnt - 1 )
  fp_tokuno.close

  fp_hash.printf("\t}\n" )

  fp_hash.close

  tokuname_gmm.close_gmm
  tokuinfo_gmm.close_gmm

  fp_speabi = open( "speabi.txt", "w" )
  tokuname.sort!
  tokuname.size.times {|i|
    fp_speabi.printf( "%s", tokuname[ i ] )
  }
	fp_speabi.close

  print "�������x����gmm�t�@�C���@�����I��\n"

