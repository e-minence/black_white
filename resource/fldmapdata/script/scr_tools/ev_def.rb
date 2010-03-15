#=======================================================================
# ev_def.rb
# �X�N���v�g�t�@�C��*.ev����X�N���v�gID�𐶐����A*_def.h�ɏ�������
# ev_def.rb ev_file offs_id_file
# ev_file �ϊ�����.ev�t�@�C���p�X
# offs_id_file �X�N���v�gID�J�n�I�t�Z�b�g����`���ꂽ�t�@�C���p�X
#=======================================================================
$KCODE = "SJIS"

#=======================================================================
# �萔
#=======================================================================
RET_ERROR = (0xffffffff)

#����offs_id_file�ŎQ�Ƃ���J�n�X�N���v�gID�V���{��
SCROFFS_START = "ID_START_SCR_OFFSET"

#����offs_id_file�ŎQ�Ƃ���J�n�g���[�i�[2v2�X�N���v�gID�V���{��
SCROFFS_START_TRAINER_2VS2 = "ID_TRAINER_2VS2_OFFSET"

#�g���[�i�[�X�N���v�g�t�@�C����
FNAME_TRAINER = "trainer.ev"

#=======================================================================
# �֐�
#=======================================================================
#-----------------------------------------------------------------------
# �ُ�I��
#-----------------------------------------------------------------------
def error_end( file0, file1, file2, filepath_del )
  file0.close
  file1.close
  file2.close
  File.delete( filepath_del )
  printf( "ERROR ev_def create\n" )
end

#-----------------------------------------------------------------------
# �w�b�_�[�t�@�C�� �w��̕����񂪊܂܂ꂽdefine��`�̐��l����
# hfile ��������w�b�_�[�t�@�C��
# search �������镶����
# none_str ��������O�������� nil=����
# error_ret search�������ꍇ�ɕԂ��l
#-----------------------------------------------------------------------
def hfile_search( hfile, search, none_str, error_ret )
  pos = hfile.pos
  hfile.pos = 0
  
  num = error_ret
  search = search.strip
  
  while line = hfile.gets
    line = line.strip
    
    if( line =~ /\A#define/ )
      len = line.length
      str = line.split()
      def_start = str[1].sub(/^ID_/,"").sub(/_OFFSET$/,"")
      
      if( none_str != nil && str[1].include?(none_str) )
        next #�����ƂȂ镶�����܂�ł���
      end
      
      if ( str[1] == search )
      #if( str[1].include?(search) ) #�V���{�� search�܂�
        str_num = str[2] #�V���{�����l
        
        if( str_num =~ /\A0x/ ) #16�i
          /([\A0x0-9a-fA-F]+)/ =~ str_num
          str_num = $1
          num = str_num.hex
        else					#10�i
          /([\A0-9]+)/ =~ str_num
          str_num = $1
          num = str_num.to_i
        end
        
        break
      end
    end
  end
  
  hfile.pos = pos
  return num
end

#=======================================================================
# *_def.h����
#=======================================================================
fname_ev = ARGV[0]

if( fname_ev == nil )
  printf( "ERROR SCRIPT ev_def file name none\n" )
  exit 1
end

if( fname_ev =~ /^sp_.*.ev/ )
  #sp_�`.ev�͓���X�N���v�g�Ȃ̂�SCRID�𐶐�����K�v���Ȃ�
  exit 0
end

fname_id = ARGV[1]

if( fname_id == nil )
  printf( "ERROR SCRIPT ev_def id file name none\n" )
  exit 1
end

file_ev = File.open( fname_ev, "r" )
file_id = File.open( fname_id, "r" )

fname_def_h = fname_ev.gsub( "\.ev", "_def\.h" )
file_def_h = File.open( fname_def_h, "w" )

fname_ev_big = File::basename( fname_ev )
fname_ev_big = fname_ev_big.gsub( "\.ev", "" )
fname_ev_big = fname_ev_big.upcase

search_id_offset = "ID_" + fname_ev_big + "_OFFSET"
#�J�nID����
start_id = hfile_search( file_id, search_id_offset, "OFFSET_END", RET_ERROR )

if( start_id == RET_ERROR )
  start_id = hfile_search( file_id, SCROFFS_START, nil, RET_ERROR )
  
  if( start_id == RET_ERROR )
    printf( "can't find start_id %s \n", fname_ev_big )
    error_end( file_ev, file_id, file_def_h, fname_def_h )
    exit 1
  end
end

#�t�@�C�����o����������
file_def_h.printf( "\/\/%s\n", fname_def_h )
file_def_h.printf( "\/\/���̃t�@�C���̓R���o�[�^�ɂ���č쐬����܂���\n\n" )
file_def_h.printf( "#ifndef _%s_DEF_H_\n", fname_ev_big )
file_def_h.printf( "#define _%s_DEF_H_\n\n", fname_ev_big )
file_def_h.printf( "\/\/�X�N���v�g�f�[�^ID��`\n" )

#ID��`
count = 0
start_id_trainer = RET_ERROR

if( fname_ev.include?(FNAME_TRAINER) )
  start_id_trainer = hfile_search(
    file_id, SCROFFS_START_TRAINER_2VS2, nil, RET_ERROR )
  
  if( start_id_trainer == RET_ERROR )
    printf( "can't find start_id %s \n", SCROFFS_START_TRAINER_2VS2 )
    error_end( file_ev, file_id, file_def_h, fname_def_h )
    exit 1
  end
end

while line = file_ev.gets
  line = line.strip
  
  if( line =~ /\A_EVENT_DATA_END/ )
    break
  end
  
  if( line =~ /\A_EVENT_DATA/ )
    str = line.split()
    id_name = str[1]
    
    if( id_name == nil )
      printf( "ERROR ev_def EVENT_DATA label name none\n" )
      error_end( file_ev, file_id, file_def_h, fname_def_h )
      exit 1
    end
    
    id_name = id_name.gsub( /\Aev_/, "" ) #�擪��ev_���폜
    id_name = id_name.upcase
    
    file_def_h.printf( "#define SCRID_%s (%d)\n", id_name, start_id+count )
    
    if( start_id_trainer != RET_ERROR )
      file_def_h.printf( "#define SCRID_%s_2 (%d)\n",
            id_name, start_id_trainer+count )
    end
    
    count = count + 1
  end
end

#�t�@�C���I�[��������
file_def_h.printf( "\n#endif \/\/_%s_DEF_H_", fname_ev_big )

#�I��
file_def_h.close
file_id.close
file_ev.close

#�Ȃ���unix�s���R�[�h�iLF)�ɂȂ��Ă���������邽�߁A�����ϊ�
system("unix2dos -D " + fname_def_h )

