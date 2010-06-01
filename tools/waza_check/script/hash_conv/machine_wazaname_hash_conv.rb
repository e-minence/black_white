# @brief  �A�C�e���f�[�^����, �킴�}�V���� ==> �Z�� �̕ϊ��n�b�V���e�[�u�����쐬����
# @file   machine_wazaname_hash_conv.rb
# @author obata
# @date   2010.05.26 

# ��ԍ�
COLUMN_ITEM_NAME = 2
COLUMN_WAZA_NAME = 21


def CreateMachineWazaNameHash( item_filename, output_path )

  file = File::open( item_filename, "r" )
  file_lines = file.readlines
  file_lines.slice( 0 ) # �擪�s���폜
  file.close 

  # ���X�g�쐬
  machine_name_list = Array.new
  waza_name_list = Array.new

  file_lines.each do |line|
    items = line.split( /\s/ )
    item_name = items[ COLUMN_ITEM_NAME ]
    waza_name = items[ COLUMN_WAZA_NAME ]
    if item_name.include?( "�킴�}�V��" ) || item_name.include?( "�Ђł�}�V��" ) then
      machine_name_list << item_name
      waza_name_list << waza_name
    end
  end

  # �o��
  output_filename = output_path + "machine_wazaname_hash.rb"
  output_file = File::open( output_filename, "w" )
  output_file.puts( "$machine_wazaname_hash = {" ) 
  0.upto( machine_name_list.size - 1 ) do |idx|
    machine_name = machine_name_list[ idx ]
    waza_name = waza_name_list[ idx ]
    output_file.puts( "  \"#{machine_name}\" => \"#{waza_name}\"," )
  end
  output_file.puts( "}" ) 

end
