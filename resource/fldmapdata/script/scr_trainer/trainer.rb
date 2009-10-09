#=======================================================================
# trainer.rb
# _EVENT_DATA�L�q��trainer.ev�ɏo�͂���
#
# ���� trainer.rb path_trid path_sc 
# path_trid �Q�Ƃ���g���[�i�[ID����`���ꂽ�t�@�C���p�X
# path_sc �f�[�^�ɋL�q����X�N���v�g�t�@�C���p�X
#=======================================================================
$KCODE = "SJIS"

#=======================================================================
# �萔
#=======================================================================
FNAME_TARGET = "trainer.ev"

#=======================================================================
# trainer.ev����
#=======================================================================
fname_id = ARGV[0]
fname_sc = ARGV[1]
file_id = File.open( fname_id, "r" )
file_sc = File.open( fname_sc, "r" )
file_ev = File.open( FNAME_TARGET, "w" )

file_ev.printf( "//======================================================================\n" )
file_ev.printf( "// trainer.ev\n" )
file_ev.printf( "// %s�����ɃR���o�[�g���Ă��܂�\n", fname_id )
file_ev.printf( "//  ���R���o�[�g���s\n" )
file_ev.printf( "//    trainer(.bat)\n" )
file_ev.printf( "//  ���X�V�����t�@�C��\n" )
file_ev.printf( "//    trainer.ev\n" )
file_ev.printf( "//    trainer_def.h\n" )
file_ev.printf( "//======================================================================\n" )
file_ev.printf( "\n" )
file_ev.printf( "\t.text\n" )
file_ev.printf( "\t.include \"scr_seq_def.h\"\n " )
file_ev.printf( "\t.include \"../../../prog/include/tr_tool/tr_tool.h\"\n" )
file_ev.printf( "\n" )
file_ev.printf( "//----------------------------------------------------------------------\n" )
file_ev.printf( "// �X�N���v�g�e�[�u��\n" )
file_ev.printf( "//----------------------------------------------------------------------\n" )

count = 0
id_tbl = []

while line = file_id.gets
  if( line =~ /\A#define/ && !(line.include?("TRNO_DEF_H")) )
    str = line.split()
    id_tbl[count] = str[1]
    file_ev.printf( "_EVENT_DATA ev_%s\n", id_tbl[count] )
    count = count + 1
  end
end

file_ev.printf( "_EVENT_DATA ev_trainer_talk_battle\n" )
file_ev.printf( "_EVENT_DATA ev_trainer_move_battle\n" )
file_ev.printf( "_EVENT_DATA_END\n\n" )

no = 0
while no < count
  file_ev.printf( "ev_%s:\n", id_tbl[no] )
  no = no + 1
end

file_ev.printf( "\n" )
file_ev.printf( "//----------------------------------------------------------------------\n" )
file_ev.printf( "// %s\n", fname_sc )
file_ev.printf( "//----------------------------------------------------------------------\n" )
file_ev.write( file_sc.read )
file_ev.close
file_sc.close
file_id.close
