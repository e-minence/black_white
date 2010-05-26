# @brief  パーソナルデータから, 各ポケモンが習得可能なマシン技のハッシュテーブルを作成する
# @file   machinewaza_hash_conv.rb
# @author obata
# @date   2010.05.26 

require "constant.rb" # for 列挙子の定義
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

# 全角数字への変換
def GetZenkakuNumber( num )
  num_hash = {
    0 => "０",
    1 => "１",
    2 => "２",
    3 => "３",
    4 => "４",
    5 => "５",
    6 => "６",
    7 => "７",
    8 => "８",
    9 => "９",
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

# 技マシン名の取得
def GetWazaMachineName( machine_no ) 
  str_number = GetZenkakuNumber( machine_no ) 
  machine_name = nil
  if machine_no < 10 then
    machine_name = "わざマシン０" + str_number
  else
    machine_name = "わざマシン" + str_number
  end 
  return machine_name
end

# 秘伝マシン復活にともなう変換
def CorrectWazaMachineName_byHidenMachineRecovery( machine_name )
  correct_hash = {
    "わざマシン９４"   => "ひでんマシン０１", # いあいぎり
    "わざマシン９５"   => "ひでんマシン０４", # かいりき
    "わざマシン９６"   => "ひでんマシン０３", # なみのり
    "わざマシン９７"   => "ひでんマシン０２", # そらをとぶ
    "わざマシン９８"   => "ひでんマシン０６", # ダイビング
    "わざマシン９９"   => "ひでんマシン０５", # たきのぼり
    "わざマシン１００" => "わざマシン９４",   # いわくだき
    "わざマシン１０１" => "わざマシン９５",   # バークアウト
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
    if machine_data_split[ machine_idx ] == "●" then 
      machine_name = GetWazaMachineName( machine_idx + 1 )
      machine_name = CorrectWazaMachineName_byHidenMachineRecovery( machine_name )
      waza_name = $machine_wazaname_hash[ machine_name ]
      output_file.puts( "\t\t\"#{waza_name}\"," )
    end
  end
  output_file.puts( "\t]," )
end

output_file.puts( "}" )
