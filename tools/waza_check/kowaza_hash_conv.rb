# @brief  �q�Z�n�b�V���e�[�u�����쐬����
# @file   kowaza_hash_conv.rb
# @author obata
# @date   2010.05.27 

# ��C���f�b�N�X
COLUMN_MONS_NAME    = 2
COLUMN_KOWAZA_FIRST = 7
COLUMN_KOWAZA_LAST  = 22

# �q�Z�f�[�^�̓ǂݍ���
kowaza_file = File::open( "kowaza_table.txt", "r" )
kowaza_lines = kowaza_file.readlines
kowaza_lines.slice!(0) # �擪�s�͏��O
kowaza_file.close

output_file = File::open( "kowaza_hash.rb", "w" )
output_file.puts( "$kowaza_hash = {" )

kowaza_lines.each do |line|
  items = line.split( /\s/ )
  mons_name = items[ COLUMN_MONS_NAME ]
  kowaza_list = Array.new
  COLUMN_KOWAZA_FIRST.upto( COLUMN_KOWAZA_LAST ) do |column_idx|
    waza_name = items[ column_idx ]
    if waza_name != nil then
      kowaza_list << waza_name
    end
  end
  output_file.puts( "\t\"#{mons_name}\"=>[" )
  kowaza_list.each do |waza_name|
    output_file.puts( "\t\t\"#{waza_name}\"," )
  end
  output_file.puts( "\t]," )
end

output_file.puts( "}" )
