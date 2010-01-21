###################################################################################
# @brief  ����BGM�f�[�^�R���o�[�^
# @file   special_bgm.rb
# @author obata
# @date   2010.01.20
###################################################################################

#==================================================================================
# �萔
#==================================================================================
# ��C���f�b�N�X
ROW_FLAG_NO   = 0  # �t���ONo.
ROW_ZONE_ID   = 1  # �]�[��ID
ROW_SOUND_IDX = 2  # BGM No.


#==================================================================================
# ��^��
#==================================================================================
SUMMARY = "// resource/fldmapdata/special_bgm/special_bgm.rb �ɂ�莩������" 
INCLUDE = 
"
#include \"sound/wb_sound_data.sadl\"
#include \"../../../resource/fldmapdata/flagwork/flag_define.h\"
#include \"../../../resource/fldmapdata/zonetable/zone_id.h\"
" 
STRUCT = 
"
typedef struct
{
  u16 flagNo;
  u16 zoneID;
  u32 soundIdx;

} SPECIAL_BGM;
" 

#==================================================================================
# �s�f�[�^
#==================================================================================
class Row
  def initialize
    @flagNo_lavel   = 0  # �t���ONo.
    @zoneID_lavel   = 0  # �]�[��ID
    @soundIdx_lavel = 0  # BGM No.
  end
  attr_accessor :flagNo_lavel, :zoneID_lavel, :soundIdx_lavel
end

#----------------------------------------------------------------------------------
# @brief ����BGM�e�[�u�����擾����
# @param �S�Ă̍s�f�[�^�����z��
# @return ����BGM�e�[�u�����`���镶����
#----------------------------------------------------------------------------------
def GetSpecialBGMTable( rows )
  string = "static const SPECIAL_BGM specialBGMTable[] = \n"
  string += "{\n"

  rows.each do |row|
    string += "  {#{row.flagNo_lavel}, #{row.zoneID_lavel}, #{row.soundIdx_lavel}},\n"
  end

  string += "};"

  return string
end


#----------------------------------------------------------------------------------
# @brief �t���O�̎��l���擾����
# @param flagName �t���O��
# @return �w�肵���t���O�̒l
#----------------------------------------------------------------------------------
def GetFlagNo( flagName ) 
  # ��`�t�@�C�����J��
  filename = ENV["PROJECT_RSCDIR"] + "fldmapdata/flagwork/flag_define.h"
  file = File.open( filename, "r" )

  # �w�肳�ꂽ�t���OID������
  file.each do |line|
		if line.index(/#define.*#{flagName}\s*(\d*)\s*/) != nil then 
			return $1.to_i
		end
  end
  file.close

  # �w�肳�ꂽ�t���OID����`����Ă��Ȃ��ꍇ
  abort("�t���OID:#{flagName}�͒�`����Ă��܂���") 
end

#----------------------------------------------------------------------------------
# @brief �]�[��ID�̎��l���擾����
# @param zoneName �]�[��ID���x����
# @return �w�肵���]�[��ID�̒l
#----------------------------------------------------------------------------------
def GetZoneID( zoneName ) 
  # ��`�t�@�C�����J��
  filename = ENV["PROJECT_RSCDIR"] + "fldmapdata/zonetable/zone_id.h"
  file = File.open( filename, "r" )

  # �w�肳�ꂽ�]�[��ID������
  file.each do |line|
		if line.index(/#define.*#{zoneName}\s*\((\d*)\)\s*/) != nil then 
			return $1.to_i
		end
  end
  file.close

  # �w�肳�ꂽ�]�[��ID����`����Ă��Ȃ��ꍇ
  abort("�]�[��ID:#{zoneName}�͒�`����Ă��܂���") 
end

#----------------------------------------------------------------------------------
# @brief BGM No.�̎��l���擾����
# @param bgmLavel BGM���x����
# @return �w�肵��BGM�̒l
#----------------------------------------------------------------------------------
def GetSoundIdx( bgmLavel ) 
  # ��`�t�@�C�����J��
  filename = ENV["PROJECT_RSCDIR"] + "sound/wb_sound_data.sadl"
  file = File.open( filename, "r" )

  # �w�肳�ꂽ�]�[��ID������
  file.each do |line|
		if line.index(/#define.*#{bgmLavel}\s*(\d*)\s*/) != nil then 
			return $1.to_i
		end
  end
  file.close

  # �w�肳�ꂽBGM����`����Ă��Ȃ��ꍇ
  abort("BGM:#{bgmLavel}�͒�`����Ă��܂���") 
end


#==================================================================================
# @brief ���C��
# @param ARGV[0] �R���o�[�g�Ώۂ̃^�u��؂�f�[�^�t�@�C���̃p�X
#==================================================================================
# �t�@�C���f�[�^�擾
file = File.open( ARGV[0], "r" )
fileData = file.readlines
file.close

# �s�f�[�^�z��
rows = Array.new

# �f�[�^���o
1.upto( fileData.size - 1 ) do |rowIndex|
  rowData = fileData[rowIndex]
  rowItem = rowData.split(/\s/)
  row = Row.new
  row.flagNo_lavel   = rowItem[ROW_FLAG_NO]
  row.zoneID_lavel   = rowItem[ROW_ZONE_ID]
  row.soundIdx_lavel = rowItem[ROW_SOUND_IDX]
  rows << row
end

# ���x���`�F�b�N
rows.each do |row|
  GetFlagNo( row.flagNo_lavel )
  GetZoneID( row.zoneID_lavel )
  GetSoundIdx( row.soundIdx_lavel )
end

# �o��
file = File.open( "special_bgm.cdat", "w" )
file.write( SUMMARY )
file.write( INCLUDE )
file.write( STRUCT )
file.write( GetSpecialBGMTable(rows) )
file.close 
