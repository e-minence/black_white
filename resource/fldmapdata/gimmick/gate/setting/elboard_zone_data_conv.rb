######################################################################################
#
# @brief 電光掲示板データコンバータ
# @file elboard_data_conv.rb
# @author obata
# @date   2009.10.27
#
######################################################################################
require "conv_tool"

#-------------------------------------------------------------------------------------
# @brief 指定した文字列に対応するモニターのアニメ番号を取得する
# @param no_str アニメ番号を文字列で指定
# @return 指定した文字列に対応するモニターアニメ番号
#-------------------------------------------------------------------------------------
def GetMonitorAnimeNo( no_str )

  # ハッシュテーブル作成
  hash_table = Hash::new
  hash_table["C01"]    = 0
  hash_table["C02"]    = 1
  hash_table["C03"]    = 2
  hash_table["C04"]    = 3
  hash_table["C05"]    = 4
  hash_table["C08_W"]  = 5
  hash_table["C08_B"]  = 6
  hash_table["TG"]     = 7
  hash_table["ST"]     = 8
  hash_table["WF"]     = 9
  hash_table["BC"]     = 10
  hash_table["D03"]    = 11 
  hash_table["BROKEN"] = 12
  hash_table["C07"]    = 0 #dummy
  hash_table["D08"]    = 0 #dummy
  hash_table["D10"]    = 0 #dummy
  hash_table["D12"]    = 0 #dummy
  hash_table["D13"]    = 0 #dummy
  hash_table["T02"]    = 0 #dummy

  # 存在しないキーが指定されたらコンバートを止める
  if hash_table.has_key?(no_str) != true then
    abort("モニターのアニメ番号[#{no_str}]は定義されていません")
  end

  # 指定されたキーに対応する値を返す
  return hash_table[no_str]
end

#-------------------------------------------------------------------------------------
# @brief ニュース表示有効フラグの値を取得する
# @param str 有効かどうかを表す『掲示板ニュース表示』列の文字列
# @return 指定した文字列に対応する値
#-------------------------------------------------------------------------------------
def GetNewsDispValidFlag( str )
  hash = Hash.new
  hash[ "×" ] = 0
  hash[ "○" ] = 1

  # 存在しないキーが指定された
  if hash.has_key?( str ) == false then
    abort( "掲示板ニュース表示：#{str} は対応していません" )
  end

  return hash[ str ]
end

#-------------------------------------------------------------------------------------
# @brief main 指定したファイルをコンバートする
# @param ARGV[0] コンバート対象ファイル名(エクセルのタブ区切りデータ)
# @param ARGV[1] バイナリデータの出力先ディレクトリ
#-------------------------------------------------------------------------------------
# .gmmファイル名
GMM_FILENAME = "gate.gmm"

# データインデックス
ROW_ZONE_ID            = 0   # ゾーンID
ROW_VERSION            = 1   # バージョン ( WB or W or B )
ROW_X                  = 2   # x座標
ROW_Y                  = 3   # y座標
ROW_Z                  = 4   # z座標
ROW_DIR                = 5   # 向き
ROW_MSG_ID_DATE        = 6   # 日付
ROW_MSG_ID_WEATHER     = 7   # 天気
ROW_MSG_ID_PROPAGATION = 8   # 天気
ROW_MSG_ID_INFO_A      = 9   # 情報A
ROW_MSG_ID_INFO_B      = 10  # 情報B
ROW_MSG_ID_INFO_C      = 11  # 情報C
ROW_MSG_ID_INFO_D      = 12  # 情報D
ROW_MSG_ID_INFO_E      = 13  # 情報E
ROW_MSG_ID_INFO_F      = 14  # 情報F
ROW_MSG_ID_INFO_G      = 15  # 情報G
ROW_MSG_ID_INFO_H      = 16  # 情報H
ROW_MSG_ID_INFO_I      = 17  # 情報I
ROW_MSG_ID_CM_MON      = 18  # CM月
ROW_MSG_ID_CM_TUE      = 19  # CM火
ROW_MSG_ID_CM_WED      = 20  # CM水
ROW_MSG_ID_CM_THU      = 21  # CM木
ROW_MSG_ID_CM_FRI      = 22  # CM金
ROW_MSG_ID_CM_SAT      = 23  # CM土
ROW_MSG_ID_CM_SUN      = 24  # CM日
ROW_WEATHER_ZONE_1     = 25  # 天気を表示する場所1
ROW_WEATHER_ZONE_2     = 26  # 天気を表示する場所2
ROW_WEATHER_ZONE_3     = 27  # 天気を表示する場所3
ROW_WEATHER_ZONE_4     = 28  # 天気を表示する場所4
ROW_MONITOR_ANIME_NO   = 29  # モニター・アニメーション番号
ROW_NEWS_VALID_FLAG    = 30  # 掲示板ニュース表示
                     
# 出力ファイル名のリスト
bin_file_list = Array.new

# ファイルデータを読み込み
file = File.open( ARGV[0], "r" )
file_lines = file.readlines
file.close

# 2行目以降の全ラインをコンバート
1.upto( file_lines.size - 1 ) do |i|
  # 出力データを作成
  line = file_lines[i]
  in_data = line.split(/\s/)
  out_data = Array.new
  out_data << GetZoneID("ZONE_ID_"+in_data[ROW_ZONE_ID])
  out_data << GetVersion( in_data[ROW_VERSION] )
  out_data << in_data[ROW_X].to_i
  out_data << in_data[ROW_Y].to_i
  out_data << in_data[ROW_Z].to_i
  out_data << GetDir(in_data[ROW_DIR])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_DATE])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_WEATHER])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_PROPAGATION])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_INFO_A])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_INFO_B])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_INFO_C])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_INFO_D])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_INFO_E])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_INFO_F])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_INFO_G])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_INFO_H])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_INFO_I])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_CM_MON])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_CM_TUE])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_CM_WED])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_CM_THU])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_CM_FRI])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_CM_SAT])
  out_data << GetMsgID(GMM_FILENAME, in_data[ROW_MSG_ID_CM_SUN])
  out_data << GetZoneID("ZONE_ID_"+in_data[ROW_WEATHER_ZONE_1])
  out_data << GetZoneID("ZONE_ID_"+in_data[ROW_WEATHER_ZONE_2])
  out_data << GetZoneID("ZONE_ID_"+in_data[ROW_WEATHER_ZONE_3])
  out_data << GetZoneID("ZONE_ID_"+in_data[ROW_WEATHER_ZONE_4])
  out_data << GetMonitorAnimeNo(in_data[ROW_MONITOR_ANIME_NO])
  out_data << GetNewsDispValidFlag(in_data[ROW_NEWS_VALID_FLAG])
  # バイナリデータを出力
  filename = ARGV[1] + "/elboard_zone_data_" + 
             in_data[ROW_ZONE_ID].downcase + "_" + 
             in_data[ROW_VERSION].downcase + ".bin"
  file = File.open( filename, "wb" )
  file.write( out_data.pack("I*") )
  file.close
  # 出力ファイル名を記憶
  bin_file_list << filename
end


# 出力したバイナリファイル名を表示
bin_file_list.each do |filename|
  puts "-output: #{filename}"
end 
