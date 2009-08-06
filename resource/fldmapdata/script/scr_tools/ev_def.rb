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
      
      if( none_str != nil && str[1].include?(none_str) )
        next #�����ƂȂ镶�����܂�ł���
      end
      
      if( str[1].include?(search) ) #�V���{�� search�܂�
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

#�J�nID����
start_id = hfile_search( file_id, fname_ev_big, "_END", RET_ERROR )

if( start_id == RET_ERROR )
  start_id = hfile_search( file_id, SCROFFS_START, nil, RET_ERROR )
  
  if( start_id == RET_ERROR )
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
    
    file_def_h.printf( "#define SCRID_%s (%d)\n", id_name, start_id )
    start_id = start_id + 1
  end
end

#�t�@�C���I�[��������
file_def_h.printf( "\n#endif \/\/_%s_DEF_H_", fname_ev_big )

#�I��
file_def_h.close
file_id.close
file_ev.close
