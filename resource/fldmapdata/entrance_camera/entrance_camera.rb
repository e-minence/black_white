#########################################################################################
# @brief  �o������i�����̃J�������o�f�[�^ �R���o�[�^
# @file   entrance_camera.rb
# @author obata
# @date   2010.01.19
#########################################################################################

# �Q�ƃt�@�C����
EXIT_TYPE_DEF_FILE = "./eventdata_type.h"

# ��C���f�b�N�X
COLUMN_EXIT_TYPE      =  0  # �o������^�C�v
COLUMN_PITCH          =  1  # �s�b�`
COLUMN_YAW            =  2  # ���[
COLUMN_LENGTH         =  3  # ����
COLUMN_TARGET_MODE    =  4  # �^�[�Q�b�g���W�w�胂�[�h
COLUMN_TARGET_X       =  5  # �^�[�Q�b�g���W x
COLUMN_TARGET_Y       =  6  # �^�[�Q�b�g���W x
COLUMN_TARGET_Z       =  7  # �^�[�Q�b�g���W x
COLUMN_OFFSET_X       =  8  # �^�[�Q�b�g�I�t�Z�b�g x
COLUMN_OFFSET_Y       =  9  # �^�[�Q�b�g�I�t�Z�b�g y
COLUMN_OFFSET_Z       = 10  # �^�[�Q�b�g�I�t�Z�b�g z
COLUMN_FRAME          = 11  # �t���[����
COLUMN_VALID_FLAG_IN  = 12  # �������( IN )
COLUMN_VALID_FLAG_OUT = 13  # �������( OUT )
COLUMN_TARGET_BIND    = 14  # ����ݒ�( TargetBind )
COLUMN_CAMERA_AREA    = 15  # ����ݒ�( CameraArea )

# �s�C���f�b�N�X
ROW_HEAD_DATA = 2  # �擪�f�[�^


#========================================================================================
# �J�������o�f�[�^
#========================================================================================
class CameraData 
  def initialize
    @exitType_label   = nil # �o������^�C�v(���x��)
    @exitType         = 0   # �o������^�C�v(�l)
    @pitch            = 0   # �s�b�`
    @yaw              = 0   # ���[
    @length           = 0   # ����
    @targetMode_label = nil # �^�[�Q�b�g���W�w�胂�[�h ( ���x�� )
    @targetMode       = 0   # �^�[�Q�b�g���W�w�胂�[�h ( �l )
    @targetX          = 0   # �^�[�Q�b�g���W x
    @targetY          = 0   # �^�[�Q�b�g���W y
    @targetZ          = 0   # �^�[�Q�b�g���W z
    @offsetX          = 0   # �^�[�Q�b�g�I�t�Z�b�gx
    @offsetY          = 0   # �^�[�Q�b�g�I�t�Z�b�gy
    @offsetZ          = 0   # �^�[�Q�b�g�I�t�Z�b�gz
    @frame            = 0   # �t���[����
    @validFlag_IN     = 0   # �i�����ɗL���ȃf�[�^���ǂ���
    @validFlag_OUT    = 0   # �ޏo���ɗL���ȃf�[�^���ǂ���
    @targetBind_label = nil # ����ݒ� ( TargetBind�E���x�� )
    @targetBind       = 0   # ����ݒ� ( TargetBind�E�l )
    @cameraArea_label = nil # ����ݒ� ( CameraArea�E���x�� )
    @cameraArea       = 0   # ����ݒ� ( CameraArea�E�l )
  end

  # �A�N�Z�b�T
  attr_accessor :exitType_label, :exitType, 
                :pitch, :yaw, :length, 
                :targetMode_label, :targetMode, :targetX, :targetY, :targetZ,
                :offsetX, :offsetY, :offsetZ, :frame,
                :validFlag_IN, :validFlag_OUT,
                :targetBind_label, :targetBind, :cameraArea_label, :cameraArea

  #-----------------------------------------
  # @brief �o�C�i���f�[�^���o�͂���
  # @param pass �o�͐�f�B���N�g���ւ̃p�X
  def OutputBinaryData( pass )
    # �o��
    filename = "#{pass}/#{GetFileName()}.bin"
    file = File.open( filename, "wb" )
    file.write( [@exitType].pack("I") )
    file.write( [@pitch].pack("I") )
    file.write( [@yaw].pack("I") )
    file.write( [@length].pack("I") )
    file.write( [@targetMode].pack("I") )
    file.write( [@targetX].pack("I") )
    file.write( [@targetY].pack("I") )
    file.write( [@targetZ].pack("I") )
    file.write( [@offsetX].pack("I") )
    file.write( [@offsetY].pack("I") )
    file.write( [@offsetZ].pack("I") )
    file.write( [@frame].pack("I") )
    file.write( [@validFlag_IN].pack("I") )
    file.write( [@validFlag_OUT].pack("I") )
    file.write( [@targetBind].pack("I") )
    file.write( [@cameraArea].pack("I") )
    file.close()
  end 
  #-----------------------------------------
  # @brief �f�o�b�O�f�[�^���o�͂���
  # @param pass �o�͐�f�B���N�g���ւ̃p�X
  def OutputDebugData( pass )
    # �o��
    filename = "#{pass}/#{GetFileName()}.txt"
    file = File.open( filename, "wb" )
    file.puts( "exitType      = #{@exitType}(#{@exitType_label})" )
    file.puts( "pitch         = 0x#{@pitch.to_s(16)}" )
    file.puts( "yaw           = 0x#{@yaw.to_s(16)}" )
    file.puts( "length        = 0x#{@length.to_s(16)}" )
    file.puts( "targetMode    = #{@targetMode}(#{@targetMode_label})" )
    file.puts( "targetX       = 0x#{@targetX.to_s(16)}" )
    file.puts( "targetY       = 0x#{@targetY.to_s(16)}" )
    file.puts( "targetZ       = 0x#{@targetZ.to_s(16)}" )
    file.puts( "offsetX       = 0x#{@offsetX.to_s(16)}" )
    file.puts( "offsetY       = 0x#{@offsetY.to_s(16)}" )
    file.puts( "offsetZ       = 0x#{@offsetZ.to_s(16)}" )
    file.puts( "frame         = #{@frame}" )
    file.puts( "validFlag_IN  = #{@validFlag_IN}" )
    file.puts( "validflag_OUT = #{@validFlag_OUT}" )
    file.puts( "targetBind    = #{@targetBind}(#{@targetBind_label})" )
    file.puts( "cameraArea    = #{@cameraArea}(#{@cameraArea_label})" )
    file.close()
  end
  #-----------------------------------------
  # @brief �t�@�C�������擾����
  # @return �t�@�C����
  def GetFileName() 
    type = @exitType_label
    # ���x�����ϊ��n�b�V�����쐬
    exitTypeHash = Hash.new
    exitTypeHash[ "EXIT_TYPE_SP1" ] = "EXIT_TYPE_SP01"
    exitTypeHash[ "EXIT_TYPE_SP2" ] = "EXIT_TYPE_SP02"
    exitTypeHash[ "EXIT_TYPE_SP3" ] = "EXIT_TYPE_SP03"
    exitTypeHash[ "EXIT_TYPE_SP4" ] = "EXIT_TYPE_SP04"
    exitTypeHash[ "EXIT_TYPE_SP5" ] = "EXIT_TYPE_SP05"
    exitTypeHash[ "EXIT_TYPE_SP6" ] = "EXIT_TYPE_SP06"
    exitTypeHash[ "EXIT_TYPE_SP7" ] = "EXIT_TYPE_SP07"
    exitTypeHash[ "EXIT_TYPE_SP8" ] = "EXIT_TYPE_SP08"
    exitTypeHash[ "EXIT_TYPE_SP9" ] = "EXIT_TYPE_SP09"
    # ���x������ϊ�
    if exitTypeHash.has_key?( @exitType_label ) == true then
      type = exitTypeHash[ type ]
    end
    # �t�@�C�������쐬
    filename = "#{type.downcase}"
    return filename
  end
end


#========================================================================================
# @brief �w�肵�����x�������݂���s�����擾����
# @param filename �����Ώۂ̃t�@�C����
# @param label    �������郉�x����
# @return �w�肵�����x�����ŏ��ɏo������s��
#         ���݂��Ȃ��ꍇ nil
#========================================================================================
def GetFirstLineIndex( filename, label )
  # �f�[�^�擾
  file = File.open( filename, "r" )
  fileData = file.readlines
  file.close

  # ����
  lineIndex = 0
  fileData.each do |lineData|
    if lineData.index(label)!=nil then return lineIndex end
    lineIndex = lineIndex + 1
  end

  # �����ł���
  return nil
end

#========================================================================================
# @brief �o������^�C�v�̒l���擾����
# @param label �o������^�C�v�̃��x��( EXIT_TYPE_xxxx )
# @return �w�肵�����x���ɑΉ�����l
#========================================================================================
def GetExitTypeValue( label )
  baseLineIndex   = GetFirstLineIndex( EXIT_TYPE_DEF_FILE,  "EXIT_TYPE_NONE" )  # �擪�̒�`�ʒu
  targetLineIndex = GetFirstLineIndex( EXIT_TYPE_DEF_FILE,  label )             # �����Ώۃ��x���̒�`�ʒu

  # �G���[����
  if targetLineIndex == nil then 
    abort( "�o������^�C�v #{label} �͒�`����Ă��܂���B" )
  end

  # �擪����̃I�t�Z�b�g�Œl�𐄑�����
  return targetLineIndex - baseLineIndex
end

#========================================================================================
# @brief �^�[�Q�b�g�w�胂�[�h�̒l���擾����
# @param label �^�[�Q�b�g�w�胂�[�h�̃��x��
# @return �w�肵�����x���ɑΉ�����l
#========================================================================================
def GetTargetModeValue( label )
  modeHash = Hash.new
  modeHash[ "auto"   ] = 0
  modeHash[ "manual" ] = 1

  # ���x�����G���[
  if modeHash.has_key?( label ) == false then 
    abort( "�^�[�Q�b�g���WMode �Ɍ�肪����܂��B" ) 
  end

  # ���x���ɑΉ�����l��Ԃ�
  return modeHash[ label ]
end

#========================================================================================
# @brief on/off�̒l���擾����
# @param label ���x�� ( on or off )
# @return �w�肵�����x���ɑΉ�����l
#========================================================================================
def GetOnOffValue( label )
  hash = Hash.new
  hash[ "on"  ] = 0
  hash[ "off" ] = 1

  # ���x�����G���[
  if hash.has_key?( label ) == false then 
    abort( "on/off �w��Ɍ�肪����܂��B" ) 
  end

  # ���x���ɑΉ�����l��Ԃ�
  return hash[ label ]
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
  cameraData.exitType_label   = rowItem[ COLUMN_EXIT_TYPE ]
  cameraData.exitType         = GetExitTypeValue( rowItem[ COLUMN_EXIT_TYPE ] )
  cameraData.pitch            = ConvertToNumber( "0x" + rowItem[ COLUMN_PITCH ] )
  cameraData.yaw              = ConvertToNumber( "0x" + rowItem[ COLUMN_YAW ] )
  cameraData.length           = ConvertToNumber( "0x" + rowItem[ COLUMN_LENGTH ] )
  cameraData.targetMode_label = rowItem[ COLUMN_TARGET_MODE ]
  cameraData.targetMode       = GetTargetModeValue( rowItem[ COLUMN_TARGET_MODE ] )
  cameraData.targetX          = ConvertToNumber( "0x" + rowItem[ COLUMN_TARGET_X ] )
  cameraData.targetY          = ConvertToNumber( "0x" + rowItem[ COLUMN_TARGET_Y ] )
  cameraData.targetZ          = ConvertToNumber( "0x" + rowItem[ COLUMN_TARGET_Z ] )
  cameraData.offsetX          = ConvertToNumber( "0x" + rowItem[ COLUMN_OFFSET_X ] )
  cameraData.offsetY          = ConvertToNumber( "0x" + rowItem[ COLUMN_OFFSET_Y ] )
  cameraData.offsetZ          = ConvertToNumber( "0x" + rowItem[ COLUMN_OFFSET_Z ] )
  cameraData.frame            = ConvertToNumber( rowItem[ COLUMN_FRAME ] )
  if rowItem[ COLUMN_VALID_FLAG_IN ].include?( "��" ) then cameraData.validFlag_IN = 1 end
  if rowItem[ COLUMN_VALID_FLAG_OUT ].include?( "��" ) then cameraData.validFlag_OUT = 1 end
  cameraData.targetBind_label = rowItem[ COLUMN_TARGET_BIND ]
  cameraData.cameraArea_label = rowItem[ COLUMN_CAMERA_AREA ]
  cameraData.targetBind       = GetOnOffValue( rowItem[ COLUMN_TARGET_BIND ] )
  cameraData.cameraArea       = GetOnOffValue( rowItem[ COLUMN_CAMERA_AREA ] )
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
