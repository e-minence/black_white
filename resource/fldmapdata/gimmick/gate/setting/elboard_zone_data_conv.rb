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
  hash_table["C01"]   = 0
  hash_table["C02"]   = 1
  hash_table["C03"]   = 2
  hash_table["C04"]   = 3
  hash_table["C05"]   = 4
  hash_table["C08_W"] = 5
  hash_table["C08_B"] = 6
  hash_table["TG"]    = 7
  hash_table["ST"]    = 8
  hash_table["WF"]    = 9
  hash_table["BC"]    = 10
  hash_table["D03"]   = 11 
  hash_table["C06"]   = 0 #dummy
  hash_table["C07"]   = 0 #dummy
  hash_table["D08"]   = 0 #dummy
  hash_table["D10"]   = 0 #dummy
  hash_table["D12"]   = 0 #dummy
  hash_table["D13"]   = 0 #dummy
  hash_table["T02"]   = 0 #dummy

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
ROW_VERSION            = 1   # �o�[�W���� ( WB or W or B )
ROW_X                  = 2   # x���W
ROW_Y                  = 3   # y���W
ROW_Z                  = 4   # z���W
ROW_DIR                = 5   # ����
ROW_MSG_ID_DATE        = 6   # ���t
ROW_MSG_ID_WEATHER     = 7   # �V�C
ROW_MSG_ID_PROPAGATION = 8   # �V�C
ROW_MSG_ID_INFO_A      = 9   # ���A
ROW_MSG_ID_INFO_B      = 10  # ���B
ROW_MSG_ID_INFO_C      = 11  # ���C
ROW_MSG_ID_INFO_D      = 12  # ���D
ROW_MSG_ID_INFO_E      = 13  # ���E
ROW_MSG_ID_INFO_F      = 14  # ���F
ROW_MSG_ID_INFO_G      = 15  # ���G
ROW_MSG_ID_INFO_H      = 16  # ���H
ROW_MSG_ID_INFO_I      = 17  # ���I
ROW_MSG_ID_CM_MON      = 18  # CM��
ROW_MSG_ID_CM_TUE      = 19  # CM��
ROW_MSG_ID_CM_WED      = 20  # CM��
ROW_MSG_ID_CM_THU      = 21  # CM��
ROW_MSG_ID_CM_FRI      = 22  # CM��
ROW_MSG_ID_CM_SAT      = 23  # CM�y
ROW_MSG_ID_CM_SUN      = 24  # CM��
ROW_WEATHER_ZONE_1     = 25  # �V�C��\������ꏊ1
ROW_WEATHER_ZONE_2     = 26  # �V�C��\������ꏊ2
ROW_WEATHER_ZONE_3     = 27  # �V�C��\������ꏊ3
ROW_WEATHER_ZONE_4     = 28  # �V�C��\������ꏊ4
ROW_MONITOR_ANIME_NO   = 29  # ���j�^�[�E�A�j���[�V�����ԍ�
                     
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
  # �o�C�i���f�[�^���o��
  filename = ARGV[1] + "/elboard_zone_data_" + 
             in_data[ROW_ZONE_ID].downcase + "_" + 
             in_data[ROW_VERSION].downcase + ".bin"
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
