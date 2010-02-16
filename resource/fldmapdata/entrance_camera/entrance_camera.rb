#########################################################################################
# @brief  �o������i�����̃J�������o�f�[�^ �R���o�[�^
# @file   entrance_camera.rb
# @author obata
# @date   2010.01.19
#########################################################################################

# �Q�ƃt�@�C����
EXIT_TYPE_DEF_FILE = "./eventdata_type.h"

# ��C���f�b�N�X
COLUMN_EXIT_TYPE      = 0  # �o������^�C�v
COLUMN_PITCH          = 1  # �s�b�`
COLUMN_YAW            = 2  # ���[
COLUMN_LENGTH         = 3  # ����
COLUMN_OFFSET_X       = 4  # �^�[�Q�b�g�I�t�Z�b�gx
COLUMN_OFFSET_Y       = 5  # �^�[�Q�b�g�I�t�Z�b�gy
COLUMN_OFFSET_Z       = 6  # �^�[�Q�b�g�I�t�Z�b�gz
COLUMN_FRAME          = 7  # �t���[����
COLUMN_VALID_FLAG_IN  = 8  # �������( IN )
COLUMN_VALID_FLAG_OUT = 9  # �������( OUT )

# �s�C���f�b�N�X
ROW_HEAD_DATA = 2  # �擪�f�[�^


#========================================================================================
# �J�������o�f�[�^
#========================================================================================
class CameraData 
  def initialize
    @exitType_lavel = nil # �o������^�C�v(���x��)
    @exitType       = 0   # �o������^�C�v(�l)
    @pitch          = 0   # �s�b�`
    @yaw            = 0   # ���[
    @length         = 0   # ����
    @offsetX        = 0   # �^�[�Q�b�g�I�t�Z�b�gx
    @offsetY        = 0   # �^�[�Q�b�g�I�t�Z�b�gy
    @offsetZ        = 0   # �^�[�Q�b�g�I�t�Z�b�gz
    @frame          = 0   # �t���[����
    @validFlag_IN   = 0   # �i�����ɗL���ȃf�[�^���ǂ���
    @validFlag_OUT  = 0   # �ޏo���ɗL���ȃf�[�^���ǂ���
  end

  # �A�N�Z�b�T
  attr_accessor :exitType_lavel, :exitType, 
                :pitch, :yaw, :length, :offsetX, :offsetY, :offsetZ, :frame,
                :validFlag_IN, :validFlag_OUT

  #-----------------------------------------
  # @brief �o�C�i���f�[�^���o�͂���
  # @param pass �o�͐�f�B���N�g���ւ̃p�X
  def OutputBinaryData( pass )
    # �o��
    filename = "#{pass}/#{@exitType_lavel.downcase}.bin"
    file = File.open( filename, "wb" )
    file.write( [@exitType].pack("I") )
    file.write( [@pitch].pack("I") )
    file.write( [@yaw].pack("I") )
    file.write( [@length].pack("I") )
    file.write( [@offsetX].pack("I") )
    file.write( [@offsetY].pack("I") )
    file.write( [@offsetZ].pack("I") )
    file.write( [@frame].pack("I") )
    file.write( [@validFlag_IN].pack("C") )
    file.write( [@validFlag_OUT].pack("C") )
    file.close()
  end 
  #-----------------------------------------
  # @brief �f�o�b�O�f�[�^���o�͂���
  # @param pass �o�͐�f�B���N�g���ւ̃p�X
  def OutputDebugData( pass )
    # �o��
    filename = "#{pass}/#{@exitType_lavel.downcase}.txt"
    file = File.open( filename, "wb" )
    file.puts( "exitType      = #{@exitType}(#{@exitType_lavel})" )
    file.puts( "pitch         = #{@pitch}" )
    file.puts( "yaw           = #{@yaw}" )
    file.puts( "length        = #{@length}" )
    file.puts( "offsetX       = #{@offsetX}" )
    file.puts( "offsetY       = #{@offsetY}" )
    file.puts( "offsetZ       = #{@offsetZ}" )
    file.puts( "frame         = #{@frame}" )
    file.puts( "validFlag_IN  = #{@validFlag_IN}" )
    file.puts( "validflag_OUT = #{@validFlag_OUT}" )
    file.close()
  end
end


#========================================================================================
# @brief �w�肵�����x�������݂���s�����擾����
# @param filename �����Ώۂ̃t�@�C����
# @param lavel    �������郉�x����
# @return �w�肵�����x�����ŏ��ɏo������s��
#         ���݂��Ȃ��ꍇ nil
#========================================================================================
def GetFirstLineIndex( filename, lavel )
  # �f�[�^�擾
  file = File.open( filename, "r" )
  fileData = file.readlines
  file.close

  # ����
  lineIndex = 0
  fileData.each do |lineData|
    if lineData.index(lavel)!=nil then return lineIndex end
    lineIndex = lineIndex + 1
  end

  # �����ł���
  return nil
end

#========================================================================================
# @brief �o������^�C�v�̒l���擾����
# @param lavel �o������^�C�v�̃��x��( EXIT_TYPE_xxxx )
# @return �w�肵�����x���ɑΉ�����l
#========================================================================================
def GetExitTypeValue( lavel )
  baseLineIndex   = GetFirstLineIndex( EXIT_TYPE_DEF_FILE,  "EXIT_TYPE_NONE" )  # �擪�̒�`�ʒu
  targetLineIndex = GetFirstLineIndex( EXIT_TYPE_DEF_FILE,  lavel )             # �����Ώۃ��x���̒�`�ʒu

  # �G���[����
  if targetLineIndex == nil then 
    abort( "�o������^�C�v #{lavel} �͒�`����Ă��܂���B" )
  end

  # �擪����̃I�t�Z�b�g�Œl�𐄑�����
  return targetLineIndex - baseLineIndex
end

#========================================================================================
# @brief ������𐔒l�ɕϊ�����
# @param string �����l��\��������(10�i������or16�i������)
# @return �w�肵���������ϊ������l
#========================================================================================
def ConvertToNumber( string )
  if string=~/0x(?:\d|[a-f])+/ then return string.hex  # 16�i
  else return string.to_i end  # 10�i
end


#========================================================================================
# ���C��
#
# @param ARGV[0] �R���o�[�g�Ώۂ̃^�u��؂�e�L�X�g�f�[�^(�p�X�w��)
# @param ARGV[1] �R���o�[�g��̃o�C�i���f�[�^�̏o�͐�
#======================================================================================== 
# �J�������o�f�[�^�z��
cameraDataArray = Array.new

# �f�[�^�擾
file = File.open( ARGV[0], "r" )
fileData = file.readlines
file.close

# �l���擾
ROW_HEAD_DATA.upto( fileData.size - 1 ) do |rowIdx|
  rowData = fileData[ rowIdx ]
  rowItem = rowData.split(/\s/)
  # �擾
  cameraData = CameraData.new
  cameraData.exitType_lavel = rowItem[ COLUMN_EXIT_TYPE ]
  cameraData.exitType       = GetExitTypeValue( rowItem[ COLUMN_EXIT_TYPE ] )
  cameraData.pitch          = ConvertToNumber( "0x" + rowItem[ COLUMN_PITCH ] )
  cameraData.yaw            = ConvertToNumber( "0x" + rowItem[ COLUMN_YAW ] )
  puts cameraData.yaw
  cameraData.length         = ConvertToNumber( "0x" + rowItem[ COLUMN_LENGTH ] )
  cameraData.offsetX        = ConvertToNumber( "0x" + rowItem[ COLUMN_OFFSET_X ] )
  cameraData.offsetY        = ConvertToNumber( "0x" + rowItem[ COLUMN_OFFSET_Y ] )
  cameraData.offsetZ        = ConvertToNumber( "0x" + rowItem[ COLUMN_OFFSET_Z ] )
  cameraData.frame          = ConvertToNumber( rowItem[ COLUMN_FRAME ] )
  if rowItem[ COLUMN_VALID_FLAG_IN ].include?( "��" ) then cameraData.validFlag_IN = 1 end
  if rowItem[ COLUMN_VALID_FLAG_OUT ].include?( "��" ) then cameraData.validFlag_OUT = 1 end
  # �z��ɓo�^
  cameraDataArray << cameraData
end

# �f�o�b�O�o��
cameraDataArray.each do |cameraData|
  cameraData.OutputDebugData( ARGV[1] )
end

# �o�C�i���o��
cameraDataArray.each do |cameraData|
  cameraData.OutputBinaryData( ARGV[1] )
end
