#=======================================================================
# rockpos.rb
# �����肫�ŉ����킪�Q�ƁA�ۑ�������W�f�[�^�ʒu���쐬
# land_list.rb land.csv..
# land.csv �e�L�X�g�ϊ��ς݂̒n�`�f�[�^�Ǘ��\ �����w��\
#=======================================================================
$KCODE = "SJIS"

#=======================================================================
# �萔
#=======================================================================

#=======================================================================
# �ϊ�
#=======================================================================
fname_csv = ARGV[0]
fname_cdat = ARGV[1]
file_csv = File.open( fname_csv, "r" )
file_cdat = File.open( fname_cdat, "w" )

file_cdat.printf( "//%s\n", fname_cdat )
file_cdat.printf( "//���̃t�@�C���̓R���o�[�^����o�͂���܂���\n\n" )
file_cdat.printf( "unsigned short DATA_MMDL_PushRockPosNum[][2] = {\n" )

line = file_csv.gets #�f�[�^�J�n�ʒu�܂Ői�߂�
line = file_csv.gets
line = file_csv.gets

while line = file_csv.gets
  line = line.strip
  str = line.split( "," )
  
  if( str[0] != nil && str[1] != nil && str[2] != nil )
    file_cdat.printf( "  \{%s,%s\},\n", str[1], str[2] )
  end
end

file_cdat.printf( "  {0xffff,0xffff},\n" )
file_cdat.printf( "};" )
file_cdat.close
file_csv.close
