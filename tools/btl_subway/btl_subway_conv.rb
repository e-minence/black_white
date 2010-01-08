#! ruby -Ks

  require File.dirname(__FILE__) + '/../gmm_make/gmm_make'
  require File.dirname(__FILE__) + '/../hash/monsno_hash.rb'
  require File.dirname(__FILE__) + '/../hash/wazano_hash.rb'
  require File.dirname(__FILE__) + '/../hash/item_hash.rb'
  require File.dirname(__FILE__) + '/../hash/trtype_hash.rb'

class PD
  enum_const_set %w[
    GRA_NO
    ITEM
    WAZA1
    WAZA2
    WAZA3
    WAZA4
    SEIKAKU
    HP_EXP
    POW_EXP
    DEF_EXP
    AGI_EXP
    SPEPOW_EXP
    SPEDEF_EXP
    FORM_NO
    MAX
  ]
end

class TD
  enum_const_set %w[
    NAME
    TYPE
    KAIWA0
    KAIWA1
    KAIWA2
    KAIWA3
    KAIWA4
    KAIWA5
    KAIWA6
    KAIWA7
    KAIWA8
    KAIWA9
    KAIWA10
    KAIWA11
    KAIWA12
    KAIWA13
    KAIWA14
    KAIWA15
    KAIWA16
    KAIWA17
    POKE_CNT
    POKE_NO
    MAX
  ]
end

	if ARGV.size != 3
		print "error: ruby btl_subway_conv.rb poke_data trainer_data template_gmm\n"
		print "poke_data:�|�P�����f�[�^�t�@�C��\n"
		print "trainer_data:�g���[�i�[�f�[�^�t�@�C��\n"
		print "template_gmm:GMM�t�@�C���𐶐����錳�ɂȂ�t�@�C��\n"
		exit( 1 )
	end

  ARGV_PD_FILE = 0
  ARGV_TD_FILE = 1
  ARGV_GMM_FILE = 2

  pd_data = []
  td_data = []
  gmm = GMM::new
  gmm.open_gmm( ARGV[ ARGV_GMM_FILE ] , "bst_name.gmm" )
  gmm.make_row_index( "BST_NAME_", 0, "�[�[�[�[�[" )

  cnt = 0
  open( ARGV[ ARGV_PD_FILE ] ) {|fp_r|
    while str = fp_r.gets
      str = str.tr( "\"\r\n", "" )
      split_data = str.split(/,/)
      next if split_data.size <= 1       #�T�[�o����̃G�N�X�|�[�g�ŃS�~���R�[�h�����邱�Ƃ�����̂ŁA�r������
      pd_data[ cnt ] = str
      cnt += 1
    end
  }

  cnt = 0
  open( ARGV[ ARGV_TD_FILE ] ) {|fp_r|
    while str = fp_r.gets
      str = str.tr( "\"\r\n", "" )
      split_data = str.split(/,/)
      next if split_data.size <= 1       #�T�[�o����̃G�N�X�|�[�g�ŃS�~���R�[�h�����邱�Ƃ�����̂ŁA�r������
      td_data[ cnt ] = str
      cnt += 1
    end
  }

  #�o�g���T�u�E�F�C�|�P�������g���[�i�[�f�[�^����
  print "�o�g���T�u�E�F�C�|�P�������g���[�i�[�f�[�^�@������\n"

  print "�|�P�����f�[�^������\n"
  #bspd_???.bin����
  print "bspd_000.bin ������\n"
  fp_bspd = open( "bspd_000.bin", "wb" )

  data = [ 0,0,0,0,0,0,0,0,0 ].pack( "S5 C2 S2" )

	data.size.times{ |c|
		fp_bspd.printf("%c",data[ c ])
	}

  fp_bspd.close
  print "bspd_000.bin �����I��\n"

  cnt = 1
  pd_data.size.times {|i|
    split_data = pd_data[ i ].split(/\t/)
    if i == 0
      next if split_data[ PD::GRA_NO ] == "BTDMPD"
      print "�o�g���T�u�E�F�C�p�̃|�P�����f�[�^�ł͂���܂���\n"
      exit( 1 )
    end

    monsno  = $gra2zukan_hash[ split_data[ PD::GRA_NO ] ]

    waza1   = $wazano_hash[ split_data[ PD::WAZA1 ] ]
    waza2   = $wazano_hash[ split_data[ PD::WAZA2 ] ]
    waza3   = $wazano_hash[ split_data[ PD::WAZA3 ] ]
    waza4   = $wazano_hash[ split_data[ PD::WAZA4 ] ]

    exp = 0
    for j in 0..5
      exp   |= split_data[ PD::HP_EXP + j ] << j
    end

    seikaku = split_data[ PD::SEIKAKU ].to_i

    item    = $item_hash[ split_data[ PD::ITEM ] ]

    formno  = split_data[ PD::FORM_NO ].to_i

    next if monsno == nil

    if waza1 == nil
      print "�s���ȋZ���ݒ肳��Ă��܂�\n"
      exit( 1 )
    end

    if waza2 == nil
      print "�s���ȋZ���ݒ肳��Ă��܂�\n"
      exit( 1 )
    end

    if waza3 == nil
      print "�s���ȋZ���ݒ肳��Ă��܂�\n"
      exit( 1 )
    end

    if waza4 == nil
      print "�s���ȋZ���ݒ肳��Ă��܂�\n"
      exit( 1 )
    end

    if item == nil
      print "�s���ȃA�C�e�����ݒ肳��Ă��܂�\n"
      exit( 1 )
    end

    printf( "bspd_%03d.bin ������\n", cnt )
    str = sprintf( "bspd_%03d.bin", cnt )
    fp_bspd = open( str, "wb" )

    data = [ monsno, waza1, waza2, waza3, waza4, exp, seikaku, item, formno ].pack( "S5 C2 S2" )

	  data.size.times{ |c|
		  fp_bspd.printf("%c",data[ c ])
	  }

    fp_bspd.close
    printf( "bspd_%03d.bin �����I��\n", cnt )
    cnt += 1
  }

  print "�g���[�i�[�f�[�^������\n"
  #bstd_???.bin����
  print "bstd_000.bin ������\n"
  fp_bstd = open( "bstd_000.bin", "wb" )

  data = [ 0,1,0 ].pack( "S3" )

	data.size.times{ |c|
		fp_bstd.printf("%c",data[ c ])
	}

  fp_bstd.close
  print "bstd_000.bin �����I��\n"

  cnt = 1
  td_data.size.times {|i|
    split_data = td_data[ i ].split(/\t/)

    next if split_data[ TD::NAME ] == "#"

    tr_type = $trtype_hash[ split_data[ TD::TYPE ] ]
    poke_cnt = split_data[ TD::POKE_CNT ].to_i

    if tr_type == nil
      print "�s���ȃg���[�i�[�^�C�v���ݒ肳��Ă��܂�\n"
      exit( 1 )
    end

    printf( "bstd_%03d.bin ������\n", cnt )
    str = sprintf( "bstd_%03d.bin", cnt )
    fp_bstd = open( str, "wb" )

    gmm.make_row_index( "BST_NAME_", cnt, split_data[ TD::NAME ] )

    data = [ tr_type, poke_cnt ].pack( "S2" )

	  data.size.times{ |c|
		  fp_bstd.printf("%c",data[ c ])
	  }

    poke_cnt.times {|no|
      poke_no = split_data[ TD::POKE_NO + no ].to_i
      data = [ poke_no ].pack( "S" )

	    data.size.times{ |c|
		    fp_bstd.printf("%c",data[ c ])
	    }
    }

    fp_bstd.close
    printf( "bstd_%03d.bin �����I��\n", cnt )
    cnt += 1
  }

  #�^�C���X�^���v��r�p�̃_�~�[�t�@�C���𐶐�
  fp_w = open( "out_end", "w" )
  fp_w.close


