#! ruby -Ks

  require File.dirname(__FILE__) + '/../label_make/label_make'
  require File.dirname(__FILE__) + '/../gmm_make/gmm_make'
  require File.dirname(__FILE__) + '/../constant'

class PARA
  enum_const_set %w[
    WAZANAME
    ATKMSG
    INFO1
    INFO2
    INFO3
    INFO4
    INFO5
    Type
    Category
    DamageType
    Power
    HitPer
    BasePP
    Priority
    HitCntMax
    HitCntMin
    SickID
    SickPer
    SickCont
    SickTurnMin
    SickTurnMax
    CriticalRank
    ShrinkPer
    RankEffType1
    RankEffType2
    RankEffType3
    RankEffValue1
    RankEffValue2
    RankEffValue3
    RankEffPer1
    RankEffPer2
    RankEffPer3
    DmgRecoverRatio
    HPRecoverRatio
    Target
    FLG_Touch
    FLG_Tame
    FLG_Tire
    FLG_Mamoru
    FLG_MagicCoat
    FLG_Yokodori
    FLG_Oumu
    FLG_TetsuNoKobusi
    FLG_Bouon
    FLG_Juryoku
    FLG_KoriMelt
    FLG_TripleFar
  ]
end

  atkmsg_table = {
    "���������I"=>"���@�������I",
    "�����I"=>"�����I",
    "�������I"=>"���@�����I",
    "���������I"=>"�@���������I",
    "�I"=>"�I",
  }

  minemsg_table = {
    "���������I"  =>"[1:01:�j�b�N�l�[��:0]��\r\n",
    "�����I"        =>"[1:01:�j�b�N�l�[��:0]��\r\n",
    "�������I"      =>"[1:01:�j�b�N�l�[��:0]��\r\n",
    "���������I"    =>"[1:01:�j�b�N�l�[��:0]��\r\n",
    "�I"            =>"[1:01:�j�b�N�l�[��:0]��\r\n",
  }


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
  wazaname_gmm = GMM::new
  wazaname_gmm.open_gmm( ARGV[ ARGV_READ_GMM_FILE ] , "wazaname.gmm" )
  wazaname_gmm.make_row_index( "WAZANAME_", 0, "�[�[�[�[�[" )
  wazainfo_gmm = GMM::new
  wazainfo_gmm.open_gmm( ARGV[ ARGV_READ_GMM_FILE ] , "wazainfo.gmm" )
  wazainfo_gmm.make_row_index_kanji( "WAZAINFO_", 0, "�[\r�[\r�[\r�[\r�[", "�[\r�[\r�[\r�[\r�[" )
  atkmsg_gmm = GMM::new
  atkmsg_gmm.open_gmm( ARGV[ ARGV_READ_GMM_FILE ] , "btl_attack.gmm" )
  atkmsg_gmm.make_row_index( "ATKMSG_M_", 0, "�I" )
  atkmsg_gmm.make_row_index( "ATKMSG_Y_", 0, "�I" )
  atkmsg_gmm.make_row_index( "ATKMSG_E_", 0, "�I" )

  #�^���Z���X�g
  tame_waza = []

  cnt = 1

  read_data.size.times {|i|
    split_data = read_data[ i ].split(/\t/)
    if split_data[ PARA::FLG_Tame ] == "1"
      tame_waza << cnt
    end
    fp_wazano.print( "#define\t\t" )
    label_str = label.make_label( "WAZANO_", split_data[ PARA::WAZANAME ] )
    fp_wazano.print( label_str )
    tab_cnt = ( 19 - label_str.length ) / 2 + 2
    for j in 1..tab_cnt
      fp_wazano.print( "\t" )
    end
    fp_wazano.printf( "( %d )\t\t//%s\n", cnt, split_data[ PARA::WAZANAME ] )
    fp_hash.printf("\t\t\"%s\"=>%d,\n", split_data[ PARA::WAZANAME ], cnt )
    wazaname_gmm.make_row_index( "WAZANAME_", cnt, split_data[ PARA::WAZANAME ] )
    info = split_data[ PARA::INFO1 ] + "\r\n" + split_data[ PARA::INFO2 ] + "\r\n" + split_data[ PARA::INFO3 ] + "\r\n" + split_data[ PARA::INFO4 ] + "\r\n" + split_data[ PARA::INFO5 ]
    #@todo ��������������Ȃ�ł����A����f�[�^���Ȃ��̂ŁA����������Ő���
    wazainfo_gmm.make_row_index_kanji( "WAZAINFO_", cnt, info, info )
    if atkmsg_table[ split_data[ PARA::ATKMSG ] ] == nil
      p split_data[ PARA::ATKMSG ]
    end
    atkmsg = minemsg_table[ split_data[ PARA::ATKMSG ] ] + split_data[ PARA::WAZANAME ] + atkmsg_table[ split_data[ PARA::ATKMSG ] ]
    atkmsg_gmm.make_row_index( "ATKMSG_M_", cnt, atkmsg )
    atkmsg = "�₹���́@" + minemsg_table[ split_data[ PARA::ATKMSG ] ] + split_data[ PARA::WAZANAME ] + atkmsg_table[ split_data[ PARA::ATKMSG ] ]
    atkmsg_gmm.make_row_index( "ATKMSG_Y_", cnt, atkmsg )
    atkmsg = "�����Ắ@" + minemsg_table[ split_data[ PARA::ATKMSG ] ] + split_data[ PARA::WAZANAME ] + atkmsg_table[ split_data[ PARA::ATKMSG ] ]
    atkmsg_gmm.make_row_index( "ATKMSG_E_", cnt, atkmsg )
    cnt += 1
  }

  fp_wazano.printf( "#define\t\tWAZANO_MAX\t\t\t\t\t\t( %d )\n", cnt )
  fp_wazano.close

  fp_hash.printf("\t}\n" )

  fp_hash.print("\n\t$tame_waza = [\n")

  tame_waza.size.times{ |cnt|
    fp_hash.printf("\t\t%d,\n",tame_waza[ cnt ])
  }
  fp_hash.printf("\t]\n" )

  fp_hash.close

  wazaname_gmm.close_gmm
  wazainfo_gmm.close_gmm
  atkmsg_gmm.close_gmm

  print "�Z���x����gmm�t�@�C���@�����I��\n"

