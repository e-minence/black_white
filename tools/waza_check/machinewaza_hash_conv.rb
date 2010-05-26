# @brief  �p�[�\�i���f�[�^����, �e�|�P�������K���\�ȃ}�V���Z�̃n�b�V���e�[�u�����쐬����
# @file   machinewaza_hash_conv.rb
# @author obata
# @date   2010.05.26 

require "constant.rb" # for �񋓎q�̒�`
require "machine_wazaname_hash.rb"

class PARA
  enum_const_set %w[
    POKENAME
    SHINKA_POKE1
    SHINKA_POKE2
    SHINKA_POKE3
    SHINKA_POKE4
    SHINKA_POKE5
    SHINKA_POKE6
    SHINKA_POKE7
    WAZA1
    WAZA2
    WAZA3
    WAZA4
    WAZA5
    WAZA6
    WAZA7
    WAZA8
    WAZA9
    WAZA10
    WAZA11
    WAZA12
    WAZA13
    WAZA14
    WAZA15
    WAZA16
    WAZA17
    WAZA18
    WAZA19
    WAZA20
    WAZA21
    WAZA22
    WAZA23
    WAZA24
    WAZA25
    ITEM1
    ITEM2
    ITEM3
    SPEABI1
    SPEABI2
    SPEABI3
    ZENKOKU_NO
    CHIHOU_NO
    SHINKA_LEVEL
    TYPE1
    TYPE2
    GROW
    EXP
    GET
    GENDER
    EGG_GROUP1
    EGG_GROUP2
    BIRTH_STEP
    FRIEND
    ESCAPE
    HP
    POW
    DEF
    AGI
    SPEPOW
    SPEDEF
    HP_EXP
    POW_EXP
    DEF_EXP
    AGI_EXP
    SPEPOW_EXP
    SPEDEF_EXP
    WAZA_LV1
    WAZA_LV2
    WAZA_LV3
    WAZA_LV4
    WAZA_LV5
    WAZA_LV6
    WAZA_LV7
    WAZA_LV8
    WAZA_LV9
    WAZA_LV10
    WAZA_LV11
    WAZA_LV12
    WAZA_LV13
    WAZA_LV14
    WAZA_LV15
    WAZA_LV16
    WAZA_LV17
    WAZA_LV18
    WAZA_LV19
    WAZA_LV20
    WAZA_LV21
    WAZA_LV22
    WAZA_LV23
    WAZA_LV24
    WAZA_LV25
    SHINKA_COND1
    SHINKA_COND2
    SHINKA_COND3
    SHINKA_COND4
    SHINKA_COND5
    SHINKA_COND6
    SHINKA_COND7
    WAZA_MACHINE
    GRA_NO
    FORM_NAME
    COLOR
    REVERSE
    OTHERFORM
    RANK
    PLTT_ONLY
    HEIGHT
    WEIGHT
    WAZA_OSHIE
    NO_JUMP
    MAX
  ]
end


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
