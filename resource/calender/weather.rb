###################################################################
#
# @brief  �V�C�J�����_�[�̃R���o�[�g
# @author obata
# @date   2009.08.04
#
# === �o�C�i���E�f�[�^�E�t�H�[�}�b�g ===
# 
# [0] zone_num (�o�^�]�[����:u16)
# [1]
# [2] zone_id (�]�[��ID:u16)								|
# [3]																				|--- x zone_num
# [4] offset (�f�[�^�ւ̃I�t�Z�b�g:u16)			|
# [5]																				|
# [x+0] date_num (�o�^���t��:u16)											|
# [x+1]																								|
# [y+0] month   (��:u8)							|									|--- x zone_num
# [y+1] date    (��:u8)							|--- x date_num		|
# [y+2] weather (�V�C:u8)						|									|
# 
###################################################################


#==================================================================
#
# @brief 1�΂̌��E���E�V�C��ێ�����N���X
#
#==================================================================
class WeatherInfo

  # �R���X�g���N�^
  def initialize( month, day, weather )
    @month   = month    # ��
    @day     = day      # ��
    @weather = weather  # �V�C
  end

  # �A�N�Z�b�T�E���\�b�h
  def weather
    @weather
  end

  def month
    @month
  end

  def day
    @day
  end

  # ������擾���\�b�h
  def to_s
    "#@month/#@day = #@weather"
  end 

end


#==================================================================
#
# @brief 1�̃]�[���̃f�[�^��ێ�����N���X
#
#==================================================================
class ZoneWeatherInfo

  # �R���X�g���N�^
  def initialize( zone )
    @zone_id = zone        # �]�[��ID
    @data    = Array.new   # �V�C���z��
  end

  # �f�[�^���擾
  def GetDataNum
    @data.length
  end

  # �f�[�^�擾���\�b�h
  def GetZoneID
    @zone_id
  end

  def GetWeatherNo( i )
    @data[i].weather
  end

  def GetMonth( i )
    @data[i].month
  end

  def GetDay( i )
    @data[i].day
  end

  # �f�[�^�ǉ����\�b�h
  def AddData( data )
    @data << data
  end

  # ���k���\�b�h
  def Compress
    prev = -1
    del  = false
    @data.reject! do |d| 
      # �O���Ɠ����V�C�Ȃ�, �폜����
      if d.weather == prev then 
        del = true  
      else
        del = false
      end   
      prev = d.weather
      del
    end
  end

  # ������擾���\�b�h
  def to_s
    str = "zone id = #@zone_id\n"
    str += "data num = "
    str += (@data.length).to_s
    str += "\n"
    @data.each do |d|
      str += d.to_s
      str += "\n"
    end
    str
  end 

end

  
#==================================================================
#
# @brief ���t�����񂩂�, ���̐��l�����o��
#
# @param str_date ���t��\��������(*��*��)
#
# @return ���̒l
#
#==================================================================
def GetMonthValue( str_date )

  # "��"�̈ʒu�����߂�
  month_index = str_date.index( "��" )

  # ���E���̒l�𐔒l�ɕϊ�
  val = str_date[0..month_index-1].to_i

  # ���̒l��Ԃ�
  return val

end
  

#==================================================================
#
# @brief ���t�����񂩂�, ���̐��l�����o��
#
# @param str_date ���t��\��������(*��*��)
#
# @return ���̒l
#
#==================================================================
def GetDayValue( str_date )

  # ���E���̈ʒu�����߂�
  month_index = str_date.index( "��" )
  date_index  = str_date.index( "��" )

  # ���̒l�𐔒l�ɕϊ�
  val = str_date[month_index+2..date_index-1].to_i

  # ���̒l��Ԃ�
  return val

end


#==================================================================
#
# @brief �]�[��������, �]�[��ID���擾����
#
# @param name �]�[����(��FT01)
#
# @return �w��]�[����ID
#==================================================================
def GetZoneIDValue( name )

	zone_id = 0;

	# ��������萔�����쐬
	def_name = "ZONE_ID_" + name

	# �]�[��ID��`�t�@�C�����J��
	file_def = File.open( "../../prog/arc/fieldmap/zone_id.h" )

	# 1�s�����Ă���
	file_def.each do |line|

		# �����萔�������������ꍇ
		if( line.index( def_name ) != nil ) then

			# ()���̕�����𐔒l�ɕϊ�����
			i0 = line.index( "(" ) + 1
			i1 = line.index( ")" ) - 1
			str_num = line[ i0..i1 ]
			zone_id = str_num.to_i 
			break
			
		end
		
	end
	
	# �t�@�C�������
	file_def.close

	return zone_id
end


#==================================================================
#
# @brief �V�C���𐔒l�ɕϊ�����
#
# @param name �V�C��( ��: SUNNY )
#
# @return �w�肳�ꂽ�V�C�̔ԍ�
#
#==================================================================
def GetWeatherNo( name )

  weather_no = 0

  # �萔�����쐬
  def_name = "WEATHER_NO_" + name

  # �V�C�ԍ�����`����Ă���t�@�C�����J��
  file = File.open( "../../prog/include/field/weather_no.h" )

  # �萔��������
  file.each do |line|
    # ����
    if line.index( def_name ) != nil then 
      # ( )���̒l���擾
      i0         = line.index( "(" ) + 1
      i1         = line.index( ")" ) - 1
      str        = line[i0..i1];
      weather_no = str.to_i
      break
    end
  end

  # �t�@�C�������
  file.close

  return weather_no
end


#==================================================================
#
# @brief ���C��
#
#==================================================================

LINE_INDEX_ZONE_NAME  = 1 # �o�^�]�[�������w�肳��Ă���s�ԍ�
LINE_INDEX_START      = 2 # ���f�[�^�̐擪�s�ԍ�
ROW_INDEX_DATE        = 2 # ���t�f�[�^�̗�ԍ�
ROW_INDEX_FIRST_ZONE  = 3 # �ŏ��̃]�[���w��̗�ԍ�

SRC_FILENAME = "weather.txt"  # ���̓t�@�C����
DST_FILENAME = "weather.bin"  # �o�̓t�@�C����


zone_id    = Array.new # �]�[��ID�z��
month      = Array.new # ���z��(366����)
date       = Array.new # ���ɂ��z��(366����)
weather    = Array.new # �V�C�z��(�e�]�[��366����)

info_array = Array.new

#-----------------------------------
# �S�f�[�^�̓ǂݍ���
#-----------------------------------
# �t�@�C���ǂݍ���
file  = File.open( SRC_FILENAME )
lines = file.readlines
file.close

# �o�^����Ă���S�]�[��ID���擾
item  = lines[ LINE_INDEX_ZONE_NAME ].split( /\t/ )
ROW_INDEX_FIRST_ZONE.upto( item.length - 1 ) do |i|
  zone_name  = item[i].strip
  zone_id   << GetZoneIDValue( zone_name )
end

# ���ׂĂ̓��ɂ����擾
LINE_INDEX_START.upto( lines.length - 1 ) do |i|
  item   = lines[i].split( /\t/ )
  month << GetMonthValue( item[ ROW_INDEX_DATE ] )
  date  << GetDayValue( item[ ROW_INDEX_DATE ] )
end

# �o�^����Ă���S�]�[������������
0.upto( zone_id.length - 1 ) do |i|
  array = Array.new
  # 366�����ׂĂ̓V�C���擾
  LINE_INDEX_START.upto( lines.length - 1 ) do |j|
    item         = lines[j].split( /\t/ )
    str_weather  = item[ ROW_INDEX_FIRST_ZONE + i ].strip
    array       << GetWeatherNo( str_weather )
  end
  weather << array
end


# �܂Ƃ߂�
0.upto( zone_id.length - 1 ) do |i| 
  data = ZoneWeatherInfo.new( zone_id[i] )
  0.upto( month.length - 1 ) do |j|
    data.AddData( WeatherInfo.new( month[j], date[j], weather[i][j] ) )
  end
  info_array << data
end


# DEBUG:
=begin
0.upto( info_array.length - 1 ) do |i|
  puts info_array[i].to_s
end
=end


#-----------------------------------
# ���k
#----------------------------------- 
# ���k
info_array.each do |info|
  info.Compress
end

# DEBUG:
=begin
info_array.each do |info|
  puts info.to_s
end
=end

#-----------------------------------
# �I�t�Z�b�g�v�Z
#----------------------------------- 
offset = Array.new

pos = 2 + 4 * info_array.length
0.upto( info_array.length - 1 ) do |i|
  offset << pos
  pos += 2
  pos += info_array[i].GetDataNum * 3
end


#-----------------------------------
# ��������
#-----------------------------------

# �t�@�C�����J��
file = File.open( DST_FILENAME, "wb" )

# OUT: �o�^�]�[����(u16)
file.write( [ info_array.length ].pack( "S" ) )

# ���ׂẴ]�[��ID�ƃI�t�Z�b�g�̑g���o�͂���
0.upto( info_array.length - 1 ) do |i|
  # OUT: �]�[��ID(u16), �I�t�Z�b�g(u16)
  file.write( [ info_array[i].GetZoneID, offset[i] ].pack( "SS" ) )
end

# �S�]�[���̃f�[�^���o��
info_array.each do |info|
  # OUT: �f�[�^��(u16)
  file.write( [ info.GetDataNum ].pack( "S" ) )
  # OUT: ��(u8)�E��(u8)�E�V�C(u8)
  0.upto( info.GetDataNum - 1 ) do |i|
    file.write( [ info.GetMonth(i), info.GetDay(i), info.GetWeatherNo(i) ].pack( "CCC" ) )
  end
end

# �t�@�C�������
file.close


#-----------------------------------
# DEBUG:
#-----------------------------------
=begin
0.upto( zone_id.length - 1 ) do |i|
  puts zone_id[i]
end
=end

=begin
0.upto( date.length - 1 ) do |i|
  str = month[i].to_s + "/" + date[i].to_s
  0.upto( zone_id.length - 1 ) do |j|
    str += " "
    str += weather[j][i].to_s
  end
  puts str
end
=end
