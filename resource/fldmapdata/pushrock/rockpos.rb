#=======================================================================
# rockpos.rb
# �����肫�ŉ����킪�Q�ƁA�ۑ�������W�f�[�^�ʒu���쐬
#
# �����@rockpos.rb fpath_csv fpath_cdat
# fpath_csv �Ǘ��\csv�t�@�C���p�X
# fpath_cdat �쐬����cdat�t�@�C���p�X
#=======================================================================
$KCODE = "SJIS"

#=======================================================================
# �萔
#=======================================================================
STR_PUSHROCK_DATA_END = "ROCKPOS_DATA_END"

#=======================================================================
# �ϊ�
#=======================================================================
fpath_csv = ARGV[0]
fpath_cdat = ARGV[1]
file_csv = File.open( fpath_csv, "r" )
file_cdat = File.open( fpath_cdat, "w" )

file_cdat.printf( "//%s\n", fpath_cdat )
file_cdat.printf( "//���̃t�@�C���̓R���o�[�^����o�͂���܂���\n\n" )
file_cdat.printf( "#define %s (0xffff)\n\n", STR_PUSHROCK_DATA_END )
file_cdat.printf( "//[Zone ID, OBJ ID, Use Number, Padding 4byte]\n" )
file_cdat.printf( "unsigned short DATA_MMDL_PushRockPosNum[][4] = {\n" )

line = file_csv.gets #�f�[�^�J�n�ʒu�܂Ői�߂�
line = file_csv.gets
line = file_csv.gets

i = 0

while line = file_csv.gets
  line = line.strip
  str = line.split( "," )
  
  if( str[0] != nil && str[1] != nil && str[2] != nil )
    file_cdat.printf( "  {%s,%s,%d,0},\n", str[1], str[2], i )
  end
  
  i = i + 1
end

file_cdat.printf( "  {%s,%s,%d,0},\n", STR_PUSHROCK_DATA_END, STR_PUSHROCK_DATA_END, i )
file_cdat.printf( "};" )
file_cdat.close
file_csv.close
