# @brief  �q���O���[�v �� �|�P�������X�g �̃n�b�V���e�[�u���𐶐�����
# @file   egg_group_poke_list_hash_conv.rb
# @author obata
# @date   2010.06.01

require "personal_parser.rb"
require "monsno_hash.rb"


def CreateEggGroupPokeListHash( personal_filename, output_path )

  personal_parser = PersonalDataParser.new
  personal_parser.load_personal_file( personal_filename )

  # ��̃n�b�V�����쐬
  egg_group_poke_list_hash = Hash.new
  $egg_group_list.each do |egg_group_name|
    egg_group_poke_list_hash[ egg_group_name ] = Array.new
  end

  # �|�P���������n�b�V���Ɋ���U��
  $monsname.each do |mons_name|
    if mons_name == "�|�|�|�|�|" then
      next
    end

    egg_group1 = personal_parser.get_egg_group1( mons_name )
    egg_group2 = personal_parser.get_egg_group2( mons_name )

    egg_group_poke_list_hash[ egg_group1 ] << mons_name
    if egg_group1 != egg_group2 then
      egg_group_poke_list_hash[ egg_group2 ] << mons_name
    end
  end

  # �o�̓f�[�^���쐬
  out_data = Array.new
  out_data << "$egg_group_poke_list_hash = {"
  $egg_group_list.each do |egg_group_name|
    poke_list = egg_group_poke_list_hash[ egg_group_name ]
    out_data << "\t\"#{egg_group_name}\" => ["
    poke_list.each do |mons_name|
      out_data << "\t\t\"#{mons_name}\","
    end
    out_data << "\t],"
  end
  out_data << "}"
  
  # �o��
  output_filename = output_path + "egg_group_poke_list_hash.rb"
  output_file = File.open( output_filename, "w" )
  out_data.each do |data|
    output_file.puts( data )
  end
  output_file.close

end
