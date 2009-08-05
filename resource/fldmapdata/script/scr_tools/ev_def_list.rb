#=======================================================================
# ev_def_list.rb
# �t�@�C���ꗗ��ruby���s�`���ŏo��
#=======================================================================
$KCODE = "SJIS"

#script_offset_id.h �p�X
EVDEF_FPATH_OFFS_ID = "../../../prog/src/field/script_offset_id.h"

fname_list = ARGV[0]

if( fname_list == nil )
  printf( "ERROR ev_def_list file none\n" )
  exit 1
end

file_list = File.open( fname_list, "r" )

while line = file_list.gets
  line = line.strip
  str = line.split()
  
  if( str[0] =~ /\Asp_/ ) #����X�N���v�g�͑Ώۂ���O��
    next
  end
  
  printf( "ruby ./scr_tools/ev_def.rb %s %s\n", str[0], EVDEF_FPATH_OFFS_ID )
end

file_list.close
