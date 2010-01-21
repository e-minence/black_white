###################################################################################
# @brief  ����BGM�f�[�^�R���o�[�^
# @file   special_bgm.rb
# @author obata
# @date   2010.01.20
###################################################################################

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
  u32 flagNo;
  u32 zoneID;
  u32 soundIdx;

} SPECIAL_BGM;
" 

#==================================================================================
# �s�f�[�^
#==================================================================================
class Row
  def initialize
    @flagNo   = 0  # �t���ONo.
    @zoneID   = 0  # �]�[��ID
    @soundIdx = 0  # BGM No.
  end
  attr_accessor :flagNo, :zoneID, :soundIdx
end

#----------------------------------------------------------------------------------
# @brief ����BGM�e�[�u�����擾����
# @param �SRow�f�[�^�̔z��
# @return ����BGM�e�[�u�����`���镶����
#----------------------------------------------------------------------------------
def GetSpecialBGMTable( rows )
  string = "static const SPECIAL_BGM specialBGMTable[] = \n"
  string += "{\n"

  rows.each do |row|
    string += "  {#{row.flagNo}, #{row.zoneID}, #{row.soundIdx}},\n"
  end

  string += "};"

  return string
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
  row.flagNo   = rowItem[0]
  row.zoneID   = rowItem[1]
  row.soundIdx = rowItem[2]
  rows << row
end

# �o��
file = File.open( "special_bgm.cdat", "w" )
file.write( SUMMARY )
file.write( INCLUDE )
file.write( STRUCT )
file.write( GetSpecialBGMTable(rows) )
file.close 
