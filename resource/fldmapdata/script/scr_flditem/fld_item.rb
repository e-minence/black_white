#=======================================================================
# fld_item.rb
# _EVENT_DATA�L�q��fld_item.ev�ɏo�͂���
#
# ���� fld_item.rb path_csv path_id path_sc
# path_csv �A�C�e���Ǘ��\csv�t�@�C���p�X
# path_sc �f�[�^�ɋL�q����X�N���v�g�t�@�C���p�X
# path_id �A�C�e��ID��`�t�@�C��
#
#=======================================================================
$KCODE = "SJIS"

#=======================================================================
# �萔
#=======================================================================
FNAME_TARGET = "fld_item.ev"

CSVNO_MAPNAME = 0
CSVNO_MAPLABEL = 1
CSVNO_X = 2
CSVNO_Y = 3
CSVNO_ITEMNAME = 4
CSVNO_NUM = 5
CSVNO_ITEMSYM = 6
CSVNO_MAX = 7

FLAGNO_MAX = 328

STR_COMMENT0 = "//======================================================================"
STR_COMMENT1 = "//--------------------------------------------------------------"

#=======================================================================
# �֐�
#=======================================================================
#------------------------------------------------------------
# �G���[�I��
#------------------------------------------------------------
def error_end( f0, f1, f2, f3, fpath )
  f0.close
  f1.close
  f2.close
  f3.close
  File.delete( fpath )
end

#------------------------------------------------------------
# �A�C�e��������A�C�e��ID�V���{���擾
#------------------------------------------------------------
def get_itemsym( file, str_item )
	file.pos = 0
	
	while line = file.gets
		if( line =~ /\A#define/ && line.include?(str_item) )
			len = line.length
			str = line.split() #�󔒕����ȊO����
			return str[1] #1 �V���{����
    end
  end
	
	return nil
end

#------------------------------------------------------------
# csv�z��@�擾
#------------------------------------------------------------
def get_csvdata( data, pos, no )
  return data[ (pos*CSVNO_MAX)+no ]
end

#------------------------------------------------------------
# csv�z��@�Z�b�g
#------------------------------------------------------------
def set_csvdata( data, pos, no, set )
  data[ (pos*CSVNO_MAX)+no ] = set
end

#=======================================================================
# trainer.ev����
#=======================================================================
fpath_csv = ARGV[0]
fpath_id = ARGV[1]
fpath_sc = ARGV[2]
file_csv = File.open( fpath_csv, "r" )
file_sc = File.open( fpath_sc, "r" )
file_id = File.open( fpath_id, "r" )
file_ev = File.open( FNAME_TARGET, "w" )

file_ev.printf( "%s\n", STR_COMMENT0 )
file_ev.printf( "// %s\n", FNAME_TARGET )
file_ev.printf( "// %s,%s�����ɃR���o�[�g���Ă��܂�\n", fpath_csv, fpath_id )
file_ev.printf( "// ���R���o�[�g���s\n" )
file_ev.printf( "//   fld_item(.bat)\n" )
file_ev.printf( "// ���X�V�����t�@�C��\n" )
file_ev.printf( "//   fld_item.ev\n" )
file_ev.printf( "//   fld_item_def.h\n" )
file_ev.printf( "%s\n", STR_COMMENT0 )
file_ev.printf( "\n" )
file_ev.printf( "\t.text\n" )
file_ev.printf( "\t.include \"scr_seq_def.h\"\n " )
file_ev.printf( "\t.include \"../../message/dst/script/msg_fld_item.h\n" )
file_ev.printf( "\n" )
file_ev.printf( "%s\n", STR_COMMENT1 )
file_ev.printf( "// �X�N���v�g�e�[�u��\n" )
file_ev.printf( "%s\n", STR_COMMENT1 )

line = file_csv.gets #�f�[�^�s�܂Ŕ�΂�
line = file_csv.gets
line = file_csv.gets

#CSV���o
count = 0
csvdata = Array.new()

while line = file_csv.gets
  line = line.strip
  str = line.split( "," )
  
  if( str == nil )
    break
  end
  
  set_csvdata( csvdata, count, CSVNO_MAPNAME, str[CSVNO_MAPNAME] )
  set_csvdata( csvdata, count, CSVNO_MAPLABEL, str[CSVNO_MAPLABEL] )
  set_csvdata( csvdata, count, CSVNO_X, str[CSVNO_X] )
  set_csvdata( csvdata, count, CSVNO_Y, str[CSVNO_Y] )
  set_csvdata( csvdata, count, CSVNO_ITEMNAME, str[CSVNO_ITEMNAME] )
  set_csvdata( csvdata, count, CSVNO_NUM, str[CSVNO_NUM] )

=begin
  printf( "count = %d\n", count )
  p get_csvdata( csvdata, count, CSVNO_MAPNAME )
  p get_csvdata( csvdata, count, CSVNO_MAPLABEL )
  p get_csvdata( csvdata, count, CSVNO_X )
  p get_csvdata( csvdata, count, CSVNO_Y )
  p get_csvdata( csvdata, count, CSVNO_ITEMNAME )
  p get_csvdata( csvdata, count, CSVNO_NUM )
=end

  #�A�C�e���V���{���擾
  set_csvdata( csvdata, count, CSVNO_ITEMSYM,
    get_itemsym(file_id,get_csvdata(csvdata,count,CSVNO_ITEMNAME)) )
  
  if( get_csvdata(csvdata,count,CSVNO_ITEMSYM) == nil )
    printf( "ERROR �A�C�e���� %s �͂���܂���\n",
           get_csvdata(csvdata,count,CSVNO_ITEMNAME) )
    error_end( file_csv, file_sc, file_id, file_ev, fpath_ev )
    exit 1
  end

  #�t���O�}�b�N�X�𒴂��Ă�����G���[
  strnum = get_csvdata( csvdata, count, CSVNO_NUM )
  if( strnum.to_i >= FLAGNO_MAX )
    printf( "ERROR �t���O�̍ő吔�𒴂��Ă��܂� No=%s\n", strnum )
    error_end( file_csv, file_sc, file_id, file_ev, fpath_ev )
    exit 1
  end
  
  #�����ԍ�(�t���O�i���o�[�̃I�t�Z�b�g)���g�p���Ă��Ȃ����`�F�b�N
  no = 0
  while no < count
    if( get_csvdata(csvdata,no,CSVNO_NUM) ==
        get_csvdata(csvdata,count,CSVNO_NUM) )
      printf( "ERROR �����l�̒�`������܂� ���X�g�ʒu %d,%d\n", no, count )
      error_end( file_csv, file_sc, file_id, file_ev, fpath_ev )
      exit 1
    end
    no = no + 1
  end
  
  count = count + 1
end

#�C�x���g�f�[�^��`
no = 0

while no < count
  file_ev.printf( "_EVENT_DATA ev_fld_item_%s\n",
    get_csvdata(csvdata,no,CSVNO_NUM) )
  no = no + 1
end

file_ev.printf( "_EVENT_DATA ev_fld_item2\n" )
file_ev.printf( "_EVENT_DATA_END\n\n" )

#�C�x���g�f�[�^�@�X�N���v�g��`
no = 0
while no < count
  #���o��
  file_ev.printf( "%s\n", STR_COMMENT1 )
  file_ev.printf( "// MAP=%s X=%s Y=%s ITEM=%s FLAG=%s\n",
    get_csvdata(csvdata,no,CSVNO_MAPLABEL),
    get_csvdata(csvdata,no,CSVNO_X),
    get_csvdata(csvdata,no,CSVNO_Y),
    get_csvdata(csvdata,no,CSVNO_ITEMNAME),
    get_csvdata(csvdata,no,CSVNO_NUM) )
  file_ev.printf( "%s\n", STR_COMMENT1 )
  
  #�X�N���v�g��`
  file_ev.printf( "EVENT_START ev_fld_item_%s\n",
    get_csvdata(csvdata,no,CSVNO_NUM) )
  file_ev.printf( "  _FLD_ITEM_EVENT %s,1\n",
    get_csvdata(csvdata,no,CSVNO_ITEMSYM) )
  file_ev.printf( "EVENT_END\n\n" )

  no = no + 1
end

#�x�[�X�X�N���v�g�o��
file_ev.printf( "\n" )
file_ev.printf( "%s\n", STR_COMMENT0 )
file_ev.printf( "// %s\n", fpath_sc )
file_ev.printf( "%s\n", STR_COMMENT0 )
file_ev.write( file_sc.read )

file_ev.close
file_sc.close
file_id.close
file_csv.close
