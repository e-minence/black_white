#! ruby -Ks

  require File.dirname(__FILE__) + '/../constant'
  require File.dirname(__FILE__) + '/../gmm_make/gmm_make'
  require File.dirname(__FILE__) + '/../hash/monsno_hash.rb'
  require File.dirname(__FILE__) + '/../hash/wazano_hash.rb'
  require File.dirname(__FILE__) + '/../hash/item_hash.rb'
  require File.dirname(__FILE__) + '/../hash/trtype_hash.rb'

  require 'bts_tr_arr.rb'

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
		print "error: ruby btl_subway_conv.rb poke_data_list trainer_data template_gmm\n"
		print "poke_data_list:ポケモンデータファイルリスト\n"
		print "trainer_data:トレーナーデータファイル\n"
		print "template_gmm:GMMファイルを生成する元になるファイル\n"
		exit( 1 )
	end

  ARGV_PD_FILE = 0
  ARGV_TD_FILE = 1
  ARGV_GMM_FILE = 2

  pd_list = []
  pd_data = []
  td_data = []
  gmm = GMM::new
  gmm.open_gmm( ARGV[ ARGV_GMM_FILE ] , "bst_name.gmm" )
  gmm.make_row_index_hyphen( "BST_NAME_", 0, 5, 0 )

  cnt = 0
  open( ARGV[ ARGV_PD_FILE ] ) {|fp_r|
    while str = fp_r.gets
      pd_list[ cnt ] = str
      cnt += 1
    end
  }

  pd_list.size.times {|i|
    data = Array.new
    open( pd_list[ i ].tr( "\r\n", "" ) ) {|fp_r|
      while str = fp_r.gets
        str = str.tr( "\"\r\n", "" )
        split_data = str.split(/\t/)
        data << str
      end
    }
    pd_data << data
  }

  cnt = 0
  open( ARGV[ ARGV_TD_FILE ] ) {|fp_r|
    while str = fp_r.gets
      str = str.tr( "\"\r\n", "" )
      split_data = str.split(/\t/)
      td_data[ cnt ] = str
      cnt += 1
    end
  }

  #バトルサブウェイポケモン＆トレーナーデータ生成
  print "バトルサブウェイポケモン＆トレーナーデータ　生成中\n"

  print "ポケモンデータ生成中\n"
  #bspd_???.bin生成
  print "bspd_000.bin 生成中\n"
  fp_bspd = open( "bspd_000.bin", "wb" )

  data = [ 0,0,0,0,0,0,0,0,0 ].pack( "S5 C2 S2" )

	data.size.times{ |c|
		fp_bspd.printf("%c",data[ c ])
	}

  fp_bspd.close
  print "bspd_000.bin 生成終了\n"

  cnt = 1
  for i in 0..999
    find = 0
    find_j = 0
    pd_list.size.times {|j|
      split_data = pd_data[ j ][ i ].split(/\t/)
      if i == 0
        if split_data[ PD::GRA_NO ] != "BTDMPD"
          print "バトルサブウェイ用のポケモンデータではありません\n"
          exit( 1 )
        end
      else
        if split_data[ PD::GRA_NO ] != "999"
          if find == 0
            find = 1
            find_j = j
          else
            print "データ競合が起きています\n"
            printf( "No:%d file1:%s file2:%s\n", i, pd_list[ find_j ].tr( "\r\n", "" ), pd_list[ j ].tr( "\r\n", "" ) )
            exit( 1 )
          end
        end
      end
    }

    next if i == 0

    split_data = pd_data[ find_j ][ i ].split(/\t/)
    if split_data[ PD::GRA_NO ] != "999"
      monsno = $gra2zukan_hash[ split_data[ PD::GRA_NO ] ]
    else
      monsno = 0
    end

    waza1   = $wazano_hash[ split_data[ PD::WAZA1 ] ]
    waza2   = $wazano_hash[ split_data[ PD::WAZA2 ] ]
    waza3   = $wazano_hash[ split_data[ PD::WAZA3 ] ]
    waza4   = $wazano_hash[ split_data[ PD::WAZA4 ] ]

    exp = 0
    for j in 0..5
      exp |= split_data[ PD::HP_EXP + j ].to_i << j
    end

    seikaku = split_data[ PD::SEIKAKU ].to_i

    item    = $item_hash[ split_data[ PD::ITEM ] ]

    formno  = split_data[ PD::FORM_NO ].to_i

    next if monsno == nil

    if waza1 == nil
      print "waza1:不明な技が設定されています\n"
      printf( "%s\n", split_data[ PD::WAZA1 ] )
      exit( 1 )
    end

    if waza2 == nil
      print "waza2:不明な技が設定されています\n"
      printf( "%s\n", split_data[ PD::WAZA2 ] )
      exit( 1 )
    end

    if waza3 == nil
      print "waza3:不明な技が設定されています\n"
      printf( "%s\n", split_data[ PD::WAZA3 ] )
      exit( 1 )
    end

    if waza4 == nil
      print "waza4:不明な技が設定されています\n"
      printf( "%s\n", split_data[ PD::WAZA4 ] )
      exit( 1 )
    end

    if item == nil
      print "不明なアイテムが設定されています\n"
      printf( "%s\n", split_data[ PD::ITEM ] )
      exit( 1 )
    end

    printf( "bspd_%03d.bin 生成中\n", cnt )
    str = sprintf( "bspd_%03d.bin", cnt )
    fp_bspd = open( str, "wb" )

    data = [ monsno, waza1, waza2, waza3, waza4, exp, seikaku, item, formno ].pack( "S5 C2 S2" )

	  data.size.times{ |c|
		  fp_bspd.printf("%c",data[ c ])
	  }

    fp_bspd.close
    printf( "bspd_%03d.bin 生成終了\n", cnt )
    cnt += 1
  end

  print "トレーナーデータ生成中\n"
  #bstd_???.bin生成
  print "bstd_000.bin 生成中\n"
  fp_bstd = open( "bstd_000.bin", "wb" )

  data = [ 0,1,0 ].pack( "S3" )

	data.size.times{ |c|
		fp_bstd.printf("%c",data[ c ])
	}

  fp_bstd.close
  print "bstd_000.bin 生成終了\n"

  cnt = 1
  td_data.size.times {|i|
    split_data = td_data[ i ].split(/\t/)

    next if split_data[ TD::NAME ] == "#"

    #tr_type = $trtype_hash[ split_data[ TD::TYPE ] ]
    tr_type_index = $bts_trainername_array[ split_data[ TD::TYPE ].to_i ]
    tr_type = $trtype_hash[ tr_type_index ]
    poke_cnt = split_data[ TD::POKE_CNT ].to_i

    if tr_type == nil
      print "不明なトレーナータイプ(#{split_data[TD::TYPE]})が設定されています\n"
      exit( 1 )
    end

    printf( "bstd_%03d.bin 生成中\n", cnt )
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
    printf( "bstd_%03d.bin 生成終了\n", cnt )
    cnt += 1
  }

  #タイムスタンプ比較用のダミーファイルを生成
  fp_w = open( "out_end", "w" )
  fp_w.close


