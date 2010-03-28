=begin
���s�菇
ruby palace_zone_conv.rb [CSV�t�@�C���ւ̃p�X]

�o�̓t�@�C��
palace_zone_setting.cdat
=end


#--------------------------------------------------------------
#  �萔��`
#--------------------------------------------------------------
OBJID_MAX = 4;
DATA_MAX = 3;
REWARD_MAX = 4;

#--------------------------------------------------------------
#  �O���[�o���ϐ�
#--------------------------------------------------------------
InputExcelFile = ARGV[0];


#--------------------------------------------------------------
#  �~�b�V�����f�[�^
#--------------------------------------------------------------
class ZONE_DATA
  def initialize()
    @zone_id = "0"             #ZONE_ID
    @warp_zone_id = "0"        #WARP_ZONE_ID
    @warp_grid_x = "0"         #WARP�O���b�hX
    @warp_grid_y = "0"         #WARP�O���b�hY
    @warp_grid_z = "0"         #WARP�O���b�hZ
    @sub_x = "0"               #�����X
    @sub_y = "0"               #�����Y
    @comment = "0"             #���l��
  end

  attr_accessor :zone_id, :warp_zone_id, :warp_grid_x, :warp_grid_y, :warp_grid_z;
  attr_accessor :sub_x, :sub_y, :comment;
end


ZoneData = [];

=begin
//==================================================================
/**
 * Excel(�J���}��؂�)�t�@�C�����J���s�P�ʂŃ`�F�b�N
 *
 * @param   sp_array		
 *
 * @retval  def		
 */
//==================================================================
=end
def CsvConvFileCheck()
  
  s = 0;
  title = 0;
  
  require 'csv'
  csv = CSV.open(InputExcelFile, 'r');
  csv.each_with_index{|line, i|
    if ((/^#END/ =~ line[0]) != nil)
      break;
    end
    if(title == 0)
      title += 1;
      next;
    end

    cell = 0;
    ZoneData[s] = ZONE_DATA.new();

    ZoneData[s].zone_id = line[cell];
    cell += 1;
    ZoneData[s].warp_zone_id = line[cell];
    cell += 1;
    ZoneData[s].warp_grid_x = line[cell].sub(/\.0/, ""); #csv�̎��_�ŏ����_�����Ă���̂Ŏ�菜��
    cell += 1;
    ZoneData[s].warp_grid_y = line[cell].sub(/\.0/, "");
    cell += 1;
    ZoneData[s].warp_grid_z = line[cell].sub(/\.0/, "");
    cell += 1;
    ZoneData[s].sub_x = line[cell].sub(/\.0/, "");
    cell += 1;
    ZoneData[s].sub_y = line[cell].sub(/\.0/, "");
    cell += 1;
    ZoneData[s].comment = line[cell];
    cell += 1;

    s += 1;
  }
  
end


=begin
//==================================================================
/**
 * �ǂݍ��񂾃t�@�C�����o�͌`���ɍ��킹�ĕϊ�
 */
//==================================================================
=end
def DataConv()
  for i in 0..ZoneData.size-1
    ZoneData[i].zone_id = "ZONE_ID_" + ZoneData[i].zone_id;
    ZoneData[i].warp_zone_id = "ZONE_ID_" + ZoneData[i].warp_zone_id;
  end
end


=begin
//==================================================================
/**
 * �ǂݍ��񂾃t�@�C����C�f�[�^�Ƃ��ďo��
 */
//==================================================================
=end
def DataFileOutput()
  
  File.open("palace_zone_setting.cdat", "w"){|file|
    file.printf("//============================================================\n");
    file.printf("//       palace_zone_conv.rb �ŏo�͂��ꂽ�t�@�C���ł�\n");
    file.printf("//============================================================\n\n");
    file.printf("const PALACE_ZONE_SETTING PalaceZoneSetting[] = {\n");

    for i in 0..ZoneData.size-1
      file.printf("\t{\t//%d  %s\n", i, ZoneData[i].comment);
      file.printf("\t\t%s,\t\t//zone_id\n", ZoneData[i].zone_id);
      file.printf("\t\t%s,\t\t//warp_zone_id\n", ZoneData[i].warp_zone_id);
      file.printf("\t\t%s,\t\t//warp_grid_x\n", ZoneData[i].warp_grid_x);
      file.printf("\t\t%s,\t\t//warp_grid_y\n", ZoneData[i].warp_grid_y);
      file.printf("\t\t%s,\t\t//warp_grid_z\n", ZoneData[i].warp_grid_z);
      file.printf("\t\t%s,\t\t//sub_x\n", ZoneData[i].sub_x);
      file.printf("\t\t%s,\t\t//sub_y\n", ZoneData[i].sub_y);
      file.printf("\t},\n");
    end
    
    file.printf("};\n");
  }
  
end


#==============================================================================
#  ���C��
#==============================================================================
CsvConvFileCheck();
DataConv();
DataFileOutput();
