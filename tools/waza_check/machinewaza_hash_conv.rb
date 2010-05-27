# @brief  �p�[�\�i���f�[�^����, �e�|�P�������K���\�ȃ}�V���Z�̃n�b�V���e�[�u�����쐬����
# @file   machinewaza_hash_conv.rb
# @author obata
# @date   2010.05.26 

require "personal_index.rb"
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

usable_wazamachine_list = Array.new

personal_file = File::open( "personal_wb.csv", "r" )
personal_lines = personal_file.readlines
personal_file.close


output_file = File::open( "machinewaza_hash.rb", "w" )
output_file.puts( "$machinewaza_hash = {" )

personal_lines.each do |line|
  line.gsub!( "\"", "" )
  line.gsub!( /\s/, "" )
  items = line.split( "," )
  mons_name = items[ PARA::POKENAME ]
  output_file.puts( "\t\"#{mons_name}\"=>[" )
  machine_data = items[ PARA::WAZA_MACHINE ]
  machine_data_split = machine_data.split( // )
  0.upto( machine_data_split.size - 1 ) do |machine_idx|
    if machine_data_split[ machine_idx ] == "��" then 
      machine_name = GetWazaMachineName( machine_idx + 1 )
      machine_name = CorrectWazaMachineName_byHidenMachineRecovery( machine_name )
      waza_name = $machine_wazaname_hash[ machine_name ]
      output_file.puts( "\t\t\"#{waza_name}\"," )
    end
  end
  output_file.puts( "\t]," )
end

output_file.puts( "}" )
