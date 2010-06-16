#! ruby -Ks

  require File.dirname(__FILE__) + '/../gmm_make/gmm_make'
  require File.dirname(__FILE__) + '/../constant'
  require 'csv'

class PARA
  enum_const_set %w[
    TR_ID
    MSG_KIND
    MSG_JPN
    MSG_KANJI
  ]
end

  #���b�Z�[�W�^�C�v�e�[�u��
  msg_kind = {
    "�t�B�[���h�퓬�J�n�O"                            =>"TRMSG_FIGHT_START",
    "�t�B�[���h�퓬�O"                                =>"TRMSG_FIGHT_START",
    "�t�B�[���h�퓬����"                              =>"TRMSG_FIGHT_LOSE",
    "�t�B�[���h�퓬��"                                =>"TRMSG_FIGHT_AFTER",
    "�t�B�[���h�퓬�J�n�O�F�E"                        =>"TRMSG_FIGHT_START_1",
    "�t�B�[���h�퓬�����F�E"                          =>"TRMSG_FIGHT_LOSE_1",
    "�t�B�[���h�퓬��F�E"                            =>"TRMSG_FIGHT_AFTER_1",
    "�|�P�����P�C�F�E"                                =>"TRMSG_POKE_ONE_1",
    "�t�B�[���h�퓬�J�n�O�F��"                        =>"TRMSG_FIGHT_START_2",
    "�t�B�[���h�퓬�����F��"                          =>"TRMSG_FIGHT_LOSE_2",
    "�t�B�[���h�퓬��F��"                            =>"TRMSG_FIGHT_AFTER_2",
    "�|�P�����P�C�F��"                                =>"TRMSG_POKE_ONE_2",
    "����A�퓬�Ȃ���"                                =>"TRMSG_FIGHT_NONE_DN",
    "�t�B�[���h�퓬�J�n�O�F��"                        =>"TRMSG_FIGHT_START_1",
    "�t�B�[���h�퓬�����F��"                          =>"TRMSG_FIGHT_LOSE_1",
    "�t�B�[���h�퓬��F��"                            =>"TRMSG_FIGHT_AFTER_1",
    "�|�P�����P�C�F��"                                =>"TRMSG_POKE_ONE_1",
    "�t�B�[���h�퓬�J�n�O�F��"                        =>"TRMSG_FIGHT_START_2",
    "�t�B�[���h�퓬�����F��"                          =>"TRMSG_FIGHT_LOSE_2",
    "�t�B�[���h�퓬��F��"                            =>"TRMSG_FIGHT_AFTER_2",
    "�|�P�����P�C�F��"                                =>"TRMSG_POKE_ONE_2",
    "���A�퓬�Ȃ���"                                  =>"TRMSG_FIGHT_NONE_D",
    "�t�B�[���h�퓬��F�񕜑O"                        =>"TRMSG_FIGHT_AFTER_RECOVER_BEFORE",
    "�t�B�[���h�퓬��F�񕜌�"                        =>"TRMSG_FIGHT_AFTER_RECOVER_AFTER",
    "�t�B�[���h�퓬��F�A�C�e���������O"            =>"TRMSG_FIGHT_AFTER_GIFTITEM_BEFORE",
    "�t�B�[���h�퓬��F�A�C�e�������ꂽ��"            =>"TRMSG_FIGHT_AFTER_GIFTITEM_AFTER",
    "�퓬���F����|�P�����ɍŏ��̃_���[�W��^�����Ƃ�"=>"TRMSG_FIGHT_FIRST_DAMAGE",
    "�퓬���F����|�P�����̂g�o�����ȉ�"              =>"TRMSG_FIGHT_POKE_HP_HALF",
    "�퓬���F����|�P�������c��P�C"                  =>"TRMSG_FIGHT_POKE_LAST",
    "�퓬���F����|�P�������c��P�C�łg�o�����ȉ�"    =>"TRMSG_FIGHT_POKE_LAST_HP_HALF",
    "�|�P�����Q�C�ȉ��i�퓬���Ȃ��j"                  =>"TRMSG_POKE_UNDER_TWO",
  }

	if ARGV.size < 2
		print "error: ruby trmsgconv.rb read_file gmm_file\n"
		print "read_file:�ǂݍ��ރt�@�C��\n"
		print "gmm_file:gmm�t�@�C���������o�����߂̌��ɂȂ�t�@�C��\n"
		exit( 1 )
	end

  ARGV_READ_FILE = 0
  ARGV_READ_GMM_FILE = 1

  read_data = []
  cnt = 0

  CSV.open( ARGV[ ARGV_READ_FILE ], 'r' ) {|row|
    next if row[ 0 ] == nil && row[ 1 ] == nil
    if row[ 0 ] != nil
      next if row[ 0 ].index("#") == 0
      next if row[ 0 ].index("��") == 0
    end
    read_data[ cnt ] = row
    cnt += 1
  }

  gmm = GMM::new
  gmm.open_gmm( ARGV[ ARGV_READ_GMM_FILE ] , "temp.gmm" )

  fp_tbl = open( "trmsgtbl.s", "w" )
  fp_tbl.print( "\t.text\n\n" )
  fp_tbl.print( "\t.include trmsg.h\n\n" )

  tr_id = ""

  read_data.each {|row|
    if row[ PARA::TR_ID ] != nil
      tr_id = row[ PARA::TR_ID ]
    end
    if msg_kind[ row[ PARA::MSG_KIND ] ] == nil
      p "��`����Ă��Ȃ����b�Z�[�W�^�C�v�ł�"
      p row[ PARA::TR_ID ]
      p row[ PARA::MSG_KIND ]
      exit( 1 )
    end
    label = "TRID_" + tr_id.upcase + "_" + msg_kind[ row[ PARA::MSG_KIND ] ]
    if row[ PARA::MSG_JPN ] == nil && row[ PARA::MSG_KANJI ] == nil
      gmm.make_row_kanji( label, "����́@���b�Z�[�W", "���́@���b�Z�[�W" )
    elsif row[ PARA::MSG_JPN ] == nil
      gmm.make_row_kanji( label, "����́@���b�Z�[�W", row[ PARA::MSG_KANJI ] )
    elsif row[ PARA::MSG_KANJI ] == nil
      gmm.make_row_kanji( label, row[ PARA::MSG_JPN ], "���́@���b�Z�[�W" )
    else
      gmm.make_row_kanji( label, row[ PARA::MSG_JPN ], row[ PARA::MSG_KANJI ] )
    end
    fp_tbl.printf( "\t.short\tTRID_%s,\t%s\n", tr_id.upcase, msg_kind[ row[ PARA::MSG_KIND ] ] )
  }

  fp_tbl.close
  gmm.close_gmm

  fp_w = open( "trmsg.gmm", "w" )

  #���̕������L���ɕϊ�����
  open( "temp.gmm" ) {|fp_r|
    while str = fp_r.gets
      if str.index("Pok?mon") != nil
        buf = "Pok" + "%c" % 0xc3 + "%c" % 0xa9 + "mon"
        str.sub!( "Pok?mon", buf )
      end
      if str.index("~") != nil
        buf = "%c" % 0xc3 + "%c" % 0xa9
        str.sub!( "~", buf )
      end
      if str.index("^") != nil
        buf = "%c" % 0xc3 + "%c" % 0xa0
        str.sub!( "^", buf )
      end
      fp_w.printf( "%s", str )
    end
  }

  fp_w.close

  #�^�C���X�^���v��r�p�̃_�~�[�t�@�C���𐶐�
  fp_w = open( "out_end", "w" )
  fp_w.close

