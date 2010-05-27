# @brief  �p�[�\�i���f�[�^����, �����Z�̃n�b�V���e�[�u���𐶐�����
# @file   oshiewaza_hash_conv.rb
# @author obata
# @date   2010.05.27

require "personal_index.rb"
require "monsno_hash.rb"


all_oshiewaza_list = [
  "�����̂�����",
  "�ق̂��̂�����",
  "�݂��̂�����",
  "�n�[�h�v�����g",
  "�u���X�g�o�[��",
  "�n�C�h���J�m��",
  "��イ��������",
] 


# main #######################################################

# �p�[�\�i���f�[�^�擾
personal_file = File::open( "personal_wb.csv", "r" )
personal_lines = personal_file.readlines
personal_file.close

output_file = File::open( "oshiewaza_hash.rb", "w" )
output_file.puts( "$oshiewaza_hash = {" )

personal_lines.each do |line|
  line.gsub!( "\"", "" )
  line.gsub!( /\s/, "" )
  items = line.split( "," )
  mons_name = items[ PARA::POKENAME ]
  oshie_data = items[ PARA::WAZA_OSHIE ]
  oshie_data_split = oshie_data.split( // )
  output_file.puts( "\t\"#{mons_name}\"=>[" )
  0.upto( oshie_data_split.size - 1 ) do |idx|
    if oshie_data_split[ idx ] == "��" then
      waza_name = all_oshiewaza_list[ idx ]
      output_file.puts( "\t\t\"#{waza_name}\"," )
    end
  end
  output_file.puts( "\t]," )
end

output_file.puts( "}" )
