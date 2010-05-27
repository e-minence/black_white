# @brief  パーソナルデータから, 各ポケモンが習得可能なマシン技のハッシュテーブルを作成する
# @file   machinewaza_hash_conv.rb
# @author obata
# @date   2010.05.26 

require "personal_index.rb"
require "machine_wazaname_hash.rb"


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
