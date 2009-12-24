###########################################################################
#
# @brief  �d���f���� �Վ��j���[�X �f�[�^ �R���o�[�^
# @file   elboard_spnews_data.rb
# @author obata
# @date   2009.12.14
#
###########################################################################
require "conv_tool" 

#==========================================================================
# ���萔
#==========================================================================
# �s�C���f�b�N�X
ROW_DATAHEAD = 2  # ���f�[�^�J�n�s

# ��C���f�b�N�X
COLUMN_FLAG         = 0  # �t���O 
COLUMN_FLAG_CONTROL = 1  # �t���O����
COLUMN_MSG_ID       = 2  # �\������j���[�X�̃��b�Z�[�WID
COLUMN_NEWS_TYPE    = 3  # �^�C�v
COLUMN_ZONE_ID_1    = 4  # �\������]�[��
COLUMN_ZONE_ID_2    = 5  # �\������]�[��
COLUMN_ZONE_ID_3    = 6  # �\������]�[��
COLUMN_ZONE_ID_4    = 7  # �\������]�[��


#==========================================================================
# ���o�̓f�[�^
#==========================================================================
class OutputData 
  #-----------------
  # �R���X�g���N�^
  #-----------------
  def initialize 
    # ���x��
    @lavel_flag
    @lavel_flagControl
    @lavel_msgID
    @lavel_newsType
    @lavel_zoneID_1
    @lavel_zoneID_2
    @lavel_zoneID_3
    @lavel_zoneID_4
    # �f�[�^
    @priority    = 0  # �v���C�I���e�B
    @flag        = 0  # �t���O 
    @flagControl = 0  # �t���O����
    @msgID       = 0  # �\������j���[�X�̃��b�Z�[�WID
    @newsType    = 0  # �^�C�v
    @zoneID_1    = 0  # �\������]�[��
    @zoneID_2    = 0  # �\������]�[��
    @zoneID_3    = 0  # �\������]�[��
    @zoneID_4    = 0  # �\������]�[��
  end 
  #------------
  # �A�N�Z�b�T
  #------------ 
  attr_accessor :lavel_flag, :lavel_flagControl, :lavel_msgID, :lavel_newsType,
                :lavel_zoneID_1, :lavel_zoneID_2, :lavel_zoneID_3, :lavel_zoneID_4,
                :priority, :flag, :flagControl, :msgID, :newsType,
                :zoneID_1, :zoneID_2, :zoneID_3, :zoneID_4
  #--------------------
  # �o�̓t�@�C�����擾
  #--------------------
  def GetFilename()
    filename = "elboard_spnews_data_#{@priority}"
    return filename
  end
  #--------------
  # �o�C�i���o��
  #--------------
  def BinaryOut(path)
    filename = path + "/" + self.GetFilename() + ".bin"
    file = File::open(filename, "wb")
    data = Array.new
    data << @flag << @flagControl << @msgID << @newsType <<
            @zoneID_1 << @zoneID_2 << @zoneID_3 << @zoneID_4
    file.write(data.pack("I*"))
    file.close
  end
  #--------------
  # �f�o�b�O�o��
  #--------------
  def DebugOut(path)
    filename = path + "/" + self.GetFilename() + ".txt"
    file = File::open(filename, "w")
    file.puts("FLAG         = #{@lavel_flag}(#{@flag})")
    file.puts("FLAG_CONTROL = #{@lavel_flagControl}(#{@flagControl})")
    file.puts("MSG_ID       = #{@lavel_msgID}(#{@msgID})")
    file.puts("NEWS_TYPE    = #{@lavel_newsType}(#{@newsType})")
    file.puts("ZONE_ID_1    = #{@lavel_zoneID_1}(#{@zoneID_1})")
    file.puts("ZONE_ID_2    = #{@lavel_zoneID_2}(#{@zoneID_2})")
    file.puts("ZONE_ID_3    = #{@lavel_zoneID_3}(#{@zoneID_3})")
    file.puts("ZONE_ID_4    = #{@lavel_zoneID_4}(#{@zoneID_4})")
    file.close
  end
end

#==========================================================================
# ���T�u���[�`��
#==========================================================================

#--------------------------------------------------------------------------
# @breif �t���O����^�C�v�̒l���擾����
# @param lavel ���x����
# @return �w�胉�x���̒l
#--------------------------------------------------------------------------
def GetFlagControlVal(lavel)
	# �Q�ƃt�@�C���ǂݍ���
  def_filename = ENV["PROJECT_PROGDIR"] + "src/field/elboard_spnews_data.h"
	file = File::open(def_filename, "r");
	file_line = file.readlines
	file.close
	# ����
	file_line.each do |line|
		if line.index(/#define.*#{lavel}.*\(\s*(\d*)\s*\)/) != nil then 
			return $1.to_i
		end
	end
	# �����炸
	abort("error: �t���O����^�C�v(#{lavel})�͒�`����Ă��܂���B") 
end

#--------------------------------------------------------------------------
# @breif �j���[�X�^�C�v�̒l���擾����
# @param lavel ���x����
# @return �w�胉�x���̒l
#--------------------------------------------------------------------------
def GetNewsTypeVal(lavel)
	# �Q�ƃt�@�C���ǂݍ���
  def_filename = ENV["PROJECT_PROGDIR"] + "src/field/elboard_spnews_data.h"
	file = File::open(def_filename, "r");
	file_line = file.readlines
	file.close
	# ����
	file_line.each do |line|
		if line.index(/#define.*#{lavel}.*\(\s*(\d*)\s*\)/) != nil then 
			return $1.to_i
		end
	end
	# �����炸
	abort("error: �j���[�X�^�C�v(#{lavel})�͒�`����Ă��܂���B") 
end


#==========================================================================
# �����C��
#==========================================================================

#--------------------------------------------------------------------------
# @brief �Վ��j���[�X �f�[�^���R���o�[�g����
# @param ARGV[0] �R���o�[�g�Ώۃt�@�C��
# @param ARGV[1] �o�C�i���o�͐�
#--------------------------------------------------------------------------
news_array = Array::new
# �t�@�C���f�[�^�ǂݍ���
file = File::open(ARGV[0], "r")
file_data = file.readlines
file.close 
# �f�[�^�擾
ROW_DATAHEAD.upto(file_data.size - 1) do |row_idx|
  line_data = file_data[row_idx].split(/\s/)
  news = OutputData::new
  # ������Ƃ��Ď擾
  news.lavel_flag        = line_data[COLUMN_FLAG]
  news.lavel_flagControl = line_data[COLUMN_FLAG_CONTROL]
  news.lavel_msgID       = line_data[COLUMN_MSG_ID]
  news.lavel_newsType    = line_data[COLUMN_NEWS_TYPE]
  news.lavel_zoneID_1    = line_data[COLUMN_ZONE_ID_1]
  news.lavel_zoneID_2    = line_data[COLUMN_ZONE_ID_2]
  news.lavel_zoneID_3    = line_data[COLUMN_ZONE_ID_3]
  news.lavel_zoneID_4    = line_data[COLUMN_ZONE_ID_4]
  # �]�[��ID�ŗL�ϊ�
  if news.lavel_zoneID_1 == "-" then news.lavel_zoneID_1 = "ZONE_ID_MAX" end
  if news.lavel_zoneID_2 == "-" then news.lavel_zoneID_2 = "ZONE_ID_MAX" end
  if news.lavel_zoneID_3 == "-" then news.lavel_zoneID_3 = "ZONE_ID_MAX" end
  if news.lavel_zoneID_4 == "-" then news.lavel_zoneID_4 = "ZONE_ID_MAX" end
  # ���l�ɕϊ�
  news.priority    = row_idx - ROW_DATAHEAD
  news.flag        = GetFlagID(news.lavel_flag)
  news.flagControl = GetFlagControlVal(news.lavel_flagControl)
  news.msgID       = GetMsgID("gate", news.lavel_msgID)
  news.newsType    = GetNewsTypeVal(news.lavel_newsType)
  news.zoneID_1    = GetZoneID(news.lavel_zoneID_1)
  news.zoneID_2    = GetZoneID(news.lavel_zoneID_2)
  news.zoneID_3    = GetZoneID(news.lavel_zoneID_3)
  news.zoneID_4    = GetZoneID(news.lavel_zoneID_4)
  # �z��ɓo�^
  news_array << news
  # �f�o�b�O�o��
  news.DebugOut(ARGV[1])
  # �o�C�i���o��
  news.BinaryOut(ARGV[1])
end 
