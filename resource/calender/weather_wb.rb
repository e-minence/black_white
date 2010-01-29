###############################################################################
#
# @brief WB�J�����_�[�R���o�[�^
# @file  weather_wb.rb
# @author obata
# @date   2010.01.28
#
###############################################################################


#==============================================================================
# ���萔
#==============================================================================
# ��C���f�b�N�X
COLUMN_DATE       = 3  # ���ɂ�
COLUMN_FIRST_ZONE = 4  # �ŏ��̃]�[��

# �s�C���f�b�N�X
ROW_ZONE_ID    = 0  # �]�[��ID
ROW_FIRST_DATE = 1  # 1��1��


#==============================================================================
# ���V�C�f�[�^
#
# ��1���̓V�C��ێ�����B
#==============================================================================
class WeatherData

  def initialize
    @date_lavel    = nil   # ���ɂ� ( ���x�� )
    @weather_lavel = nil   # �V�C   ( ���x�� )
    @month         = 0     # ��
    @day           = 0     # ��
    @weather       = 0     # �V�C
  end

  attr_reader :date_lavel, :weather_lavel, :month, :day, :weather

  #----------------------------------------
  # @brief ���t��ݒ肷��
  # @param date_lavel ���ɂ� ( ���x�� )
  #----------------------------------------
  def SetDate( date_lavel )
    # ���x�����`
    date_lavel.strip!
    date_lavel =~ /\s*(\d+)��\s*(\d+)��\s*/  # �p�^�[���}�b�`
    # �}�b�`����
    if $1 == nil || $2 == nil then abort( "date lavel error. (#{date_lavel})" ) end
    # �}�b�`�������t���L��
    @date_lavel = date_lavel
    @month      = $1.to_i
    @day        = $2.to_i
  end

  #----------------------------------------
  # @brief �V�C��ݒ肷��
  # @param weather_lavel �V�C ( ���x�� )
  #----------------------------------------
  def SetWeather( weather_lavel )
    # ���x�����`
    weather_lavel.strip!
    # �Q�ƃt�@�C���f�[�^�擾
    file = File.open( "#{ENV["PROJECT_PROGDIR"]}include/field/weather_no.h", "r" )
    lines = file.readlines
    file.close
    # ����
    lines.each do |line|
      line =~ /#define\s*WEATHER_NO_#{weather_lavel}\s*\((\d+)\)/  # �p�^�[���}�b�`
      # ����
      if $1 != nil then
        @weather_lavel = weather_lavel
        @weather       = $1.to_i
        return
      end
    end
    # ���x���������炸
    abort( "weather lavel error. (#{weather_lavel})" )
  end

end

#==============================================================================
# ���J�����_�[
#
# ��1�N�̓V�C��ێ�����B
#==============================================================================
class Calender

  def initialize
    @zoneID_lavel = nil        # �]�[��ID (���x��)
    @zoneID       = 0          # �]�[��ID
    @weatherData  = Array.new  # �V�C�z��
  end

  attr_reader :zoneID_lavel, :zoneID, :weatherData

  #----------------------------------------
  # @brief �]�[��ID��ݒ肷��
  # @param zoneID_lavel �]�[��ID�̃��x��
  #----------------------------------------
  def SetZoneID( zoneID_lavel )
    # ���x�����`
    zoneID_lavel.strip!
    # �Q�ƃt�@�C���f�[�^�擾
    file = File.open( "#{ENV["PROJECT_RSCDIR"]}fldmapdata/zonetable/zone_id.h", "r" )
    lines = file.readlines
    file.close
    # ����
    lines.each do |line|
      line  =~ /#define\s*ZONE_ID_#{zoneID_lavel}\s*\((\s*\d+\s*)\)/  # �p�^�[���}�b�`
      # ����
      if $1 != nil then
        @zoneID_lavel = zoneID_lavel
        @zoneID       = $1.to_i
        return
      end
    end
    # ���x���������炸
    abort( "zoneID lavel error. (#{zoneID_lavel})" )
  end

  #----------------------------------------
  # @brief �V�C�f�[�^��o�^����
  # @param data �o�^����V�C�f�[�^
  #----------------------------------------
  def AddWeatherData( data )
    lastData = @weatherData.last
    # ���t�̏����ɂ����o�^�ł��Ȃ�
    if lastData != nil then
      if data.month < lastData.month then abort( "weather data month order error\n" )
      elsif data.month == lastData.month && data.day <= lastData.day then abort( "weather data day order error\n" ) end
    end
    # �ǉ�
    @weatherData << data
  end

  #----------------------------------------
  # @brief �w�肵�����t�̓V�C���擾����
  # @param month ��
  # @param day   ��
  # @return �w�肵�����t�̓V�C�f�[�^
  #----------------------------------------
  def GetWeatherData( month, day )
    # ����
    @weatherData.each do |data|
      # ����
      if data.month == month && data.day == day then return data end
    end
    # �o�^����Ă��Ȃ�
    abort( "weather data not found #{month}/#{day}" )
  end

  #----------------------------------------
  # @brief �N�Ԃ̓V�C�f�[�^���擾����
  # @return �S�Ă̓��t�̓V�C�z��
  #----------------------------------------
  def GetAnnualWeather()
    annualData = Array.new
    @weatherData.each do |data|
      annualData << data.weather
    end
    return annualData
  end

  #----------------------------------------
  # @brief �f�o�b�O�f�[�^���o�͂���
  # @param destPass �o�͐�̃p�X
  #----------------------------------------
  def DebugOut( destPass )
    # �t�@�C�����J��
    filename = "#{destPass}/debug_#{@zoneID_lavel.downcase}.txt"
    file = File.open( filename, "w" )
    # �o�� ( �]�[��ID )
    file.puts( "#{@zoneID_lavel}(#{@zoneID})" )
    # �o�� ( �V�C )
    @weatherData.each do |data|
      file.puts( "#{"%2s" % data.month}/#{"%2s" % data.day} = #{data.weather_lavel}(#{data.weather})" )
    end
    # �t�@�C�������
    file.close
  end

end


#==============================================================================
# �����C��
#==============================================================================

#------------------------------------------------------------------------------
# �S�J�����_�[���
#------------------------------------------------------------------------------
allCalenders = Array.new

# �t�@�C���S�f�[�^�擾
file = File.open( "weather_wb.txt", "r" )
lines = file.readlines
file.close

# �o�^�]�[����
zoneNum = lines[ ROW_ZONE_ID ].split(/\t/).size - COLUMN_FIRST_ZONE

# ���ׂẴ]�[���̃J�����_�[�쐬
COLUMN_FIRST_ZONE.upto( COLUMN_FIRST_ZONE + zoneNum - 1 ) do |columnIdx|

  calender = Calender.new

  # �]�[��ID�擾
  zoneID_lavel = lines[ ROW_ZONE_ID ].split(/\t/)[ columnIdx ]
  calender.SetZoneID( zoneID_lavel )

  # DEBUG:
  print "#{"%-8s" % zoneID_lavel} start --> "

  # ���ׂĂ̓��t�f�[�^�擾
  ROW_FIRST_DATE.upto( lines.size - 1 ) do |rowIdx|
    items = lines[ rowIdx ].split(/\t/)
    weatherData = WeatherData.new
    weatherData.SetDate( items[ COLUMN_DATE ] )
    weatherData.SetWeather( items[ columnIdx ] )
    calender.AddWeatherData( weatherData )
  end

  # DEBUG:
  puts "finish!"

  # �z��ɒǉ�
  allCalenders << calender

end

#------------------------------------------------------------------------------
# �f�o�b�O�f�[�^�o��
#------------------------------------------------------------------------------
allCalenders.each do |calender|
  # DEBUG:
  puts "output debug data: #{calender.zoneID_lavel}"
  # �o��
  calender.DebugOut("./debug")
end

#------------------------------------------------------------------------------
# �C���f�b�N�X�f�[�^�o��
#------------------------------------------------------------------------------
# �t�@�C�����J��
file = File.open( "./bin/weather_index.bin", "w" )

# �o�� ( �o�^�]�[���� )
file.write( [allCalenders.size].pack("S") )

0.upto( allCalenders.size - 1 ) do |calenderIdx| 
  calender = allCalenders[ calenderIdx ]
  # �]�[�� / �擪�f�[�^�̃C���f�b�N�X
  file.write( [calender.zoneID, calenderIdx * 366].pack("SS") )
end 

# �t�@�C�������
file.close

#------------------------------------------------------------------------------
# �V�C�f�[�^�o��
#------------------------------------------------------------------------------
# �t�@�C�����J��
file = File.open( "./bin/weather_data.bin", "w" )

outData = Array.new
allCalenders.each do |calender|
  annualData = calender.GetAnnualWeather()
  outData.concat( annualData )
end 
file.write( outData.pack("C*") )

# �t�@�C�������
file.close
