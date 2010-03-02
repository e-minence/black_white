#! ruby -Ks

  require File.dirname(__FILE__) + '/../gmm_make/gmm_make'

#�G�t�F�N�g�r���[���[�Ŏg�p����BGM���X�g�𐶐�
	if ARGV.size < 2
		print "error: ruby effectviewer.rb read_file gmm_file\n"
		print "read_file:�ǂݍ��ރt�@�C��\n"
		print "gmm_file:BGM���X�g��gmm�t�@�C���������o�����߂̌��ɂȂ�t�@�C��\n"
		exit( 1 )
	end

  ARGV_READ_FILE = 0
  ARGV_READ_GMM_FILE = 1

  read_data = []
  cnt = 0
  open( ARGV[ ARGV_READ_FILE ] ) {|fp_r|
    while str = fp_r.gets
      read_data[ cnt ] = str.sub(/\r\n/,"")
      cnt += 1
    end
  }

  fp_w = open( "ev_bgm_list.cdat", "w" )
  gmm = GMM::new
  gmm.open_gmm( ARGV[ ARGV_READ_GMM_FILE ] , "ev_bgm_list.gmm" )

  fp_w.print( "static  const int ev_bgm_table[]={\n" )
  read_data.size.times {|i|
    fp_w.printf( "\t%s,\n", read_data[ i ] )
    gmm.make_row_index( "EV_BGM_", i, read_data[ i ] )
  }
  fp_w.print( "};\n" )

  fp_w.close
  gmm.close_gmm

  #�^�C���X�^���v��r�p�̃_�~�[�t�@�C���𐶐�
  fp_w = open( "out_end", "w" )
  fp_w.close
