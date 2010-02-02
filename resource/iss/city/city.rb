#---------------------------------------------------------------------------------- 
# @brief �]�[��������, �]�[��ID���擾����
# @param str_zone_id �]�[��ID��\��������
# @return �w�肵���]�[��ID�̒l
#---------------------------------------------------------------------------------- 
def GetZoneID( str_zone_id )

	# �t�@�C�����J��
	file_def = File.open( "../../../prog/arc/fieldmap/zone_id.h" )

	# 1�s�����Ă���
	file_def.each do |line|
		if line.index(str_zone_id)!=nil then
			i0 = line.index("(") + 1
			i1 = line.index(")") - 1
			def_val = line[i0..i1].to_i
			return def_val
		end
	end
	
	# �t�@�C�������
	file_def.close

  puts( "===============================================================" );
  puts( "error: �w�肳�ꂽ�]�[��ID[#{str_zone_id}]�͒�`����Ă��܂���B" );
  puts( "===============================================================" );
end


#==================================================================================
# ���XISS���j�b�g�f�[�^
#==================================================================================
class UnitData
  def initialize()
    @zoneID_lavel = nil        # �]�[��ID ( ���x�� )
    @zoneID       = 0          # �]�[��ID
    @gridX        = 0          # x���W[grid]
    @gridY        = 0          # y���W[grid]
    @gridZ        = 0          # z���W[grid]
    @volume       = Array.new  # �{�����[��
    @distanceX    = Array.new  # ����x
    @distanceY    = Array.new  # ����y
    @distanceZ    = Array.new  # ����z
  end

  attr_accessor :zoneID_lavel, :zoneID, :gridX, :gridY, :gridZ, 
                :volume, :distanceX, :distanceY, :distanceZ

  #--------------------------------------
  # @brief �o�C�i���f�[�^���o�͂���
  # @param pass �o�͐�̃p�X
  def OutBinaryData( pass ) 
    # �o�̓f�[�^�쐬
    outData = Array.new
    outData << @zoneID
    outData << 0  # padding
    outData << @gridX
    outData << @gridY
    outData << @gridZ
    @volume.each    do |vol|  outData << vol  end
    @distanceX.each do |dist| outData << dist end
    @distanceY.each do |dist| outData << dist end
    @distanceZ.each do |dist| outData << dist end
    # �o��
    filename = "#{pass}/iss_city_unit_#{@zoneID_lavel.downcase}.bin"
    file = File.open( filename, "wb" )
    file.write( outData.pack( "SSiiiCCCCCCCCCCCCCCCCCCCCCCCC" ) )
    file.close
  end
end


#----------------------------------------------------------------------------------
# @brief main
# @param ARGV[0] �R���o�[�g�Ώۃt�@�C����(�G�N�Z���̃^�u��؂�f�[�^)
# @param ARGV[1] �o�C�i���f�[�^�̏o�͐�f�B���N�g��
#----------------------------------------------------------------------------------

# �s�C���f�b�N�X
ROW_ZONE_ID    = 0  # �]�[��ID
ROW_POS_X      = 1  # x���W
ROW_POS_Y      = 2  # y���W
ROW_POS_Z      = 3  # z���W
ROW_VOLUME     = 4  # �{�����[��
ROW_DISTANCE_X = 5  # x����
ROW_DISTANCE_Y = 6  # y����
ROW_DISTANCE_Z = 7  # z����

# ��C���f�b�N�X
COLUMN_ZONE_ID        = 0  # �]�[��ID
COLUMN_POS_X          = 1  # x���W
COLUMN_POS_Y          = 1  # y���W
COLUMN_POS_Z          = 1  # z���W
COLUMN_VOLUME_SPACE_0 = 6  # ���ʋ�ԃf�[�^0
COLUMN_VOLUME_SPACE_1 = 5  # ���ʋ�ԃf�[�^1
COLUMN_VOLUME_SPACE_2 = 4  # ���ʋ�ԃf�[�^2
COLUMN_VOLUME_SPACE_3 = 3  # ���ʋ�ԃf�[�^3
COLUMN_VOLUME_SPACE_4 = 2  # ���ʋ�ԃf�[�^4
COLUMN_VOLUME_SPACE_5 = 1  # ���ʋ�ԃf�[�^5


# ���j�b�g�z��
units = Array.new


#-------------------------
# �t�@�C���f�[�^�̓ǂݍ���
#-------------------------
file = File.open( ARGV[0], "r" )
fileLines = file.readlines
file.close


#-------------------------
# �擪�s���X�g�̐􂢏o��
#-------------------------
validRowList = Array.new
0.upto( fileLines.size - 1 ) do |rowIdx|
  line = fileLines[ rowIdx ]
  if line.index( "ZONE_ID_" ) != nil then
    validRowList << rowIdx
  end
end


#-------------------------
# ���X�g�̑S�f�[�^���擾
#-------------------------
validRowList.each do |baseRowIdx|

  unit = UnitData.new

  # �]�[��ID
  line  = fileLines[ baseRowIdx + ROW_ZONE_ID ]
  items = line.split(/\t/)
  unit.zoneID_lavel = items[ COLUMN_ZONE_ID ]
  unit.zoneID       = GetZoneID( items[ COLUMN_ZONE_ID ] )

  # x���W
  line  = fileLines[ baseRowIdx + ROW_POS_X ]
  items = line.split(/\t/)
  unit.gridX = items[ COLUMN_POS_X ].to_i

  # y���W
  line  = fileLines[ baseRowIdx + ROW_POS_Y ]
  items = line.split(/\t/)
  unit.gridY = items[ COLUMN_POS_Y ].to_i

  # z���W
  line  = fileLines[ baseRowIdx + ROW_POS_Z ]
  items = line.split(/\t/)
  unit.gridZ = items[ COLUMN_POS_Z ].to_i

  # �{�����[��
  line  = fileLines[ baseRowIdx + ROW_VOLUME ]
  items = line.split(/\t/) 
  unit.volume << items[ COLUMN_VOLUME_SPACE_0 ].to_i
  unit.volume << items[ COLUMN_VOLUME_SPACE_1 ].to_i
  unit.volume << items[ COLUMN_VOLUME_SPACE_2 ].to_i
  unit.volume << items[ COLUMN_VOLUME_SPACE_3 ].to_i
  unit.volume << items[ COLUMN_VOLUME_SPACE_4 ].to_i
  unit.volume << items[ COLUMN_VOLUME_SPACE_5 ].to_i

  # x����
  line  = fileLines[ baseRowIdx + ROW_DISTANCE_X ]
  items = line.split(/\t/) 
  unit.distanceX << items[ COLUMN_VOLUME_SPACE_0 ].to_i
  unit.distanceX << items[ COLUMN_VOLUME_SPACE_1 ].to_i
  unit.distanceX << items[ COLUMN_VOLUME_SPACE_2 ].to_i
  unit.distanceX << items[ COLUMN_VOLUME_SPACE_3 ].to_i
  unit.distanceX << items[ COLUMN_VOLUME_SPACE_4 ].to_i
  unit.distanceX << items[ COLUMN_VOLUME_SPACE_5 ].to_i

  # y����
  line  = fileLines[ baseRowIdx + ROW_DISTANCE_Y ]
  items = line.split(/\t/) 
  unit.distanceY << items[ COLUMN_VOLUME_SPACE_0 ].to_i
  unit.distanceY << items[ COLUMN_VOLUME_SPACE_1 ].to_i
  unit.distanceY << items[ COLUMN_VOLUME_SPACE_2 ].to_i
  unit.distanceY << items[ COLUMN_VOLUME_SPACE_3 ].to_i
  unit.distanceY << items[ COLUMN_VOLUME_SPACE_4 ].to_i
  unit.distanceY << items[ COLUMN_VOLUME_SPACE_5 ].to_i

  # z����
  line  = fileLines[ baseRowIdx + ROW_DISTANCE_Z ]
  items = line.split(/\t/) 
  unit.distanceZ << items[ COLUMN_VOLUME_SPACE_0 ].to_i
  unit.distanceZ << items[ COLUMN_VOLUME_SPACE_1 ].to_i
  unit.distanceZ << items[ COLUMN_VOLUME_SPACE_2 ].to_i
  unit.distanceZ << items[ COLUMN_VOLUME_SPACE_3 ].to_i
  unit.distanceZ << items[ COLUMN_VOLUME_SPACE_4 ].to_i
  unit.distanceZ << items[ COLUMN_VOLUME_SPACE_5 ].to_i

  # �z��ɒǉ�
  units << unit
end


#-------------------------
# �o�C�i���f�[�^�o��
#-------------------------
units.each do |unitData|
  unitData.OutBinaryData( "./bin" )
end
