######################################################################################
#
# @brief �d���f���f�[�^�R���o�[�^
# @file elboard_data_conv.rb
# @author obata
# @date   2009.10.27
#
######################################################################################
require "conv_tool"

#-------------------------------------------------------------------------------------
# @brief �w�肵��������ɑΉ����郂�j�^�[�̃A�j���ԍ����擾����
# @param no_str �A�j���ԍ��𕶎���Ŏw��
# @return �w�肵��������ɑΉ����郂�j�^�[�A�j���ԍ�
#-------------------------------------------------------------------------------------
def GetMonitorAnimeNo( no_str )

  # �n�b�V���e�[�u���쐬
  hash_table = Hash::new
  hash_table["T02"]   = 0
  hash_table["C01"]   = 1
  hash_table["C02"]   = 2
  hash_table["C03"]   = 3
  hash_table["C04"]   = 4
  hash_table["C05"]   = 5
  hash_table["C06"]   = 6
  hash_table["C07"]   = 7
  hash_table["C08_W"] = 8
  hash_table["C08_B"] = 9
  hash_table["TG"]    = 10
  hash_table["ST"]    = 11
  hash_table["WF"]    = 12
  hash_table["BC"]    = 13
  hash_table["D03"]   = 14
  hash_table["D08"]   = 15
  hash_table["D10"]   = 16
  hash_table["D12"]   = 17
  hash_table["D13"]   = 18

  # ���݂��Ȃ��L�[���w�肳�ꂽ��R���o�[�g���~�߂�
  if hash_table.has_key?(no_str) != true then
    abort("���j�^�[�̃A�j���ԍ�[#{no_str}]�͒�`����Ă��܂���")
  end

  # �w�肳�ꂽ�L�[�ɑΉ�����l��Ԃ�
  return hash_table[no_str]
end


#-------------------------------------------------------------------------------------
# @brief main �w�肵���t�@�C�����R���o�[�g����
# @param ARGV[0] �R���o�[�g�Ώۃt�@�C����(�G�N�Z���̃^�u��؂�f�[�^)
# @param ARGV[1] �o�C�i���f�[�^�̏o�͐�f�B���N�g��
#-------------------------------------------------------------------------------------
# .gmm�t�@�C����
GMM_FILENAME = "gate.gmm"

# �f�[�^�C���f�b�N�X
ROW_ZONE_ID            = 0   # �]�[��ID
ROW_X                  = 1   # x���W
ROW_Y                  = 2   # y���W
ROW_Z                  = 3   # z���W
ROW_DIR                = 4   # ����
ROW_MSG_ID_DATE        = 5   # ���t
ROW_MSG_ID_WEATHER     = 6   # �V�C
ROW_MSG_ID_PROPAGATION = 7   # �V�C
ROW_MSG_ID_INFO_A      = 8   # ���A
ROW_MSG_ID_INFO_B      = 9   # ���B
ROW_MSG_ID_INFO_C      = 10  # ���C
ROW_MSG_ID_INFO_D      = 11  # ���D
ROW_MSG_ID_INFO_E      = 12  # ���E
ROW_MSG_ID_INFO_F      = 13  # ���F
ROW_MSG_ID_INFO_G      = 14  # ���G
ROW_MSG_ID_INFO_H      = 15  # ���H
ROW_MSG_ID_INFO_I      = 16  # ���I
ROW_MSG_ID_CM_MON      = 17  # CM��
ROW_MSG_ID_CM_TUE      = 18  # CM��
ROW_MSG_ID_CM_WED      = 19  # CM��
ROW_MSG_ID_CM_THU      = 20  # CM��
ROW_MSG_ID_CM_FRI      = 21  # CM��
ROW_MSG_ID_CM_SAT      = 22  # CM�y
ROW_MSG_ID_CM_SUN      = 23  # CM��
ROW_WEATHER_ZONE_1     = 24  # �V�C��\������ꏊ1
ROW_WEATHER_ZONE_2     = 25  # �V�C��\������ꏊ2
ROW_WEATHER_ZONE_3     = 26  # �V�C��\������ꏊ3
ROW_WEATHER_ZONE_4     = 27  # �V�C��\������ꏊ4
ROW_MONITOR_ANIME_NO   = 28  # ���j�^�[�E�A�j���[�V�����ԍ�
                     
# �o�̓t�@�C�����̃��X�g
bin_file_list = Array.new

# �t�@�C���f�[�^��ǂݍ���
file = File.open( ARGV[0], "r" )
file_lines = file.readlines
file.close

# 2�s�ڈȍ~�̑S���C�����R���o�[�g
1.upto( file_lines.size - 1 ) do |i|
  # �o�̓f�[�^���쐬
  line = file_lines[i]
  in_data = line.split(/\s/)
  out_data = Array.new
  out_data << GetZoneID("ZONE_ID_"+in_data[ROW_ZONE_ID])
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
  # �o�C�i���f�[�^���o��
  filename = ARGV[1] + "/elboard_zone_data_" + in_data[ROW_ZONE_ID].downcase + ".bin"
  file = File.open( filename, "wb" )
  file.write( out_data.pack("I*") )
  file.close
  # �o�̓t�@�C�������L��
  bin_file_list << filename
end


# �o�͂����o�C�i���t�@�C������\��
bin_file_list.each do |filename|
  puts "-output: #{filename}"
end

# �o�͂����o�C�i���t�@�C�����X�g���쐬
str = "ELBOARD_ZONE_DATA = "
bin_file_list.each do |filename|
  str += "\\"
  str += "\n"
  str += "#{filename} "
end
file = File.open( ARGV[1] + "/elboard_zone_data.list", "w" )
file.write( str )
file.close
