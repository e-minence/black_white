# @brief  �p�[�\�i���f�[�^����, �e�|�P�������K���\�ȃ}�V���Z�̃n�b�V���e�[�u�����쐬����
# @file   machinewaza_hash_conv.rb
# @author obata
# @date   2010.05.26 

require "personal_parser.rb"
require "machine_wazaname_hash.rb"


# modules ################################################### 

# �S�p�����ւ̕ϊ�
def GetZenkakuNumber( num )
  num_hash = {
    0 => "�O",
    1 => "�P",
    2 => "�Q",
    3 => "�R",
    4 => "�S",
    5 => "�T",
    6 => "�U",
    7 => "�V",
    8 => "�W",
    9 => "�X",
  }

  string = "" 
  while true
    key = num % 10
    num = num / 10
    string = num_hash[ key ] + string
    if num == 0 then break end
  end
  return string
end

# �Z�}�V�����̎擾
def GetWazaMachineName( machine_no ) 
  str_number = GetZenkakuNumber( machine_no ) 
  machine_name = nil
  if machine_no < 10 then
    machine_name = "�킴�}�V���O" + str_number
  else
    machine_name = "�킴�}�V��" + str_number
  end 
  return machine_name
end

# ��`�}�V�������ɂƂ��Ȃ��ϊ�
def CorrectWazaMachineName_byHidenMachineRecovery( machine_name )
  correct_hash = {
    "�킴�}�V���X�S"   => "�Ђł�}�V���O�P", # ����������
    "�킴�}�V���X�T"   => "�Ђł�}�V���O�S", # �����肫
    "�킴�}�V���X�U"   => "�Ђł�}�V���O�R", # �Ȃ݂̂�
    "�킴�}�V���X�V"   => "�Ђł�}�V���O�Q", # ������Ƃ�
    "�킴�}�V���X�W"   => "�Ђł�}�V���O�U", # �_�C�r���O
    "�킴�}�V���X�X"   => "�Ђł�}�V���O�T", # �����̂ڂ�
    "�킴�}�V���P�O�O" => "�킴�}�V���X�S",   # ���킭����
    "�킴�}�V���P�O�P" => "�킴�}�V���X�T",   # �o�[�N�A�E�g
  } 
  
  if correct_hash.has_key?( machine_name ) then
    machine_name = correct_hash[ machine_name ]
  end
  return machine_name
end


# main ######################################################

def CreateMachineWazaHash( personal_filename, output_path )

  personal_parser = PersonalDataParser.new
  personal_parser.load_personal_file( personal_filename )

  out_data = Array.new
  out_data << "$machinewaza_hash = {"

  $monsname.each do |mons_name| 
    if mons_name == "�|�|�|�|�|" then
      next
    end 

    usable_waza_machine_list = Array.new
    machine_list = personal_parser.get_usable_waza_machine_list( mons_name )

    # ��`�}�V�������ɂƂ��Ȃ��ϊ�
    machine_list.each do |machine_name|
      usable_waza_machine_list << CorrectWazaMachineName_byHidenMachineRecovery( machine_name )
    end

    machine_waza_list = Array.new
    usable_waza_machine_list.each do |machine_name|
      machine_waza_list << $machine_wazaname_hash[ machine_name ]
    end

    # �o�̓f�[�^�쐬
    out_data << "\t\"#{mons_name}\"=>["
    machine_waza_list.each do |waza_name|
      out_data << "\t\t\"#{waza_name}\","
    end
    out_data << "\t],"
  end

  out_data << "}"

  # �o��
  output_filename = output_path + "machinewaza_hash.rb"
  output_file = File::open( output_filename, "w" )
  out_data.each do |data|
    output_file.puts( data )
  end
  output_file.close

end
