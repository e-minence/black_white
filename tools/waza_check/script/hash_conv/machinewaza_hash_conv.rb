# @brief  パーソナルデータから, 各ポケモンが習得可能なマシン技のハッシュテーブルを作成する
# @file   machinewaza_hash_conv.rb
# @author obata
# @date   2010.05.26 

require "personal_parser.rb"
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

def OutputMachineWazaHash_from_PersonalDat( personal_filename, output_path )

  personal_parser = PersonalDataParser.new( personal_filename )
  mons_fullname_list = personal_parser.get_mons_fullname_list

  out_data = Array.new
  out_data << "$machinewaza_hash = {"

  mons_fullname_list.each do |mons_fullname|

    usable_waza_machine_list = Array.new
    machine_list = personal_parser.get_usable_waza_machine_list( mons_fullname )

    # 秘伝マシン復活にともなう変換
    machine_list.each do |machine_name|
      usable_waza_machine_list << CorrectWazaMachineName_byHidenMachineRecovery( machine_name )
    end

    machine_waza_list = Array.new
    usable_waza_machine_list.each do |machine_name|
      machine_waza_list << $machine_wazaname_hash[ machine_name ]
    end

    # 出力データ作成
    out_data << "\t\"#{mons_fullname}\"=>["
    machine_waza_list.each do |waza_name|
      out_data << "\t\t\"#{waza_name}\","
    end
    out_data << "\t],"

  end

  out_data << "}" 

  # 出力
  output_filename = output_path + "machinewaza_hash.rb"
  output_file = File::open( output_filename, "w" )
  out_data.each do |data|
    output_file.puts( data )
  end
  output_file.close

end
