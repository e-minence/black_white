#=======================================================================
# d06stone.rb
# �d�C���A�ɔz�u���镂�V�Έꗗcdat�t�@�C���쐬
#=======================================================================
$KCODE = "SJIS"

#=======================================================================
# �萔
#=======================================================================

#=======================================================================
# �G���[�I��
#=======================================================================
def error_end( f0, f1, f2, del )
  f0.close
  f1.close
  f2.close
  File.delete( del )
end

#=======================================================================
# �ϊ�
#=======================================================================
fpath_cdat = ARGV[0]
fpath_csv0 = ARGV[1]
fpath_csv1 = ARGV[2]
file_cdat = File.open( fpath_cdat, "w" )
file_csv0 = File.open( fpath_csv0, "r" )
file_csv1 = File.open( fpath_csv1, "r" )

file_cdat.printf( "//%s\n", fpath_cdat )
file_cdat.printf( "//D06���V�Δz�u\n" )
file_cdat.printf( "//���̃t�@�C���̓R���o�[�^����o�͂���܂���\n\n" )

#���V�Α�
buf = Array.new()
buf << "#define BIGSTONE_MAX (99)\n"
buf << "static const s16 data_d06BigStonePos[BIGSTONE_MAX][3] = {\n"

i = 0
line = file_csv0.gets #�f�[�^�J�n�ʒu�܂Ői�߂�

while line = file_csv0.gets
  line = line.strip
  str = line.split( "," )
  
  if( str[1] != nil && str[1] != "" &&
      str[2] != nil && str[2] != "" &&
      str[3] != nil && str[3] != "" )
      buf << sprintf( "\t{%s,%s,%s},\n", str[1], str[2], str[3] )
  else
    break
  end
  
  i = i + 1
end

buf <<= "};\n"
buf[0] = sprintf( "#define BIGSTONE_MAX (%d)\n", i )

if( i == 0 )
  printf( "ERROR:D06 BIG STONE NOTHING\n" )
  error_end( file_cdat, file_csv0, file_csv1, fpath_cdat )
  exit 1
end

file_cdat.printf( "%s", buf );

#���V�Ώ�
buf = Array.new()
buf << "#define MINISTONE_MAX (99)\n"
buf << "static const s16 data_d06MiniStonePos[MINISTONE_MAX][3] = {\n"

i = 0
line = file_csv1.gets #�f�[�^�J�n�ʒu�܂Ői�߂�

while line = file_csv1.gets
  line = line.strip
  str = line.split( "," )
  
  if( str[1] != nil && str[1] != "" &&
      str[2] != nil && str[2] != "" &&
      str[3] != nil && str[3] != "" )
      buf << sprintf( "\t{%s,%s,%s},\n", str[1], str[2], str[3] )
  else
    break
  end
  
  i = i + 1
end

buf <<= "};\n"
buf[0] = sprintf( "#define MINISTONE_MAX (%d)\n", i )

if( i == 0 )
  printf( "ERROR:D06 MINI STONE NOTHING\n" )
  error_end( file_cdat, file_csv0, file_csv1, fpath_cdat )
  exit 1
end

file_cdat.printf( "%s", buf );

file_cdat.close
file_csv0.close
file_csv1.close
