=begin
���s�菇
ruby cross_comm_conv.rb [CSV�t�@�C���ւ̃p�X]

�o�̓t�@�C��
cross_comm.cdat
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
    @No = "0"             #No
    @ID = "0"             #ID
    @comment = "0"        #�ڍ�
    @pri = "0"            #�v���C�I���e�B
    @life = "0"           #���M����
  end

  attr_accessor :No, :ID, :comment, :pri, :life;
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

    ZoneData[s].No = line[cell];
    cell += 1;
    ZoneData[s].ID = line[cell];
    cell += 1;
    ZoneData[s].comment = line[cell].sub(/\.0/, ""); #csv�̎��_�ŏ����_�����Ă���̂Ŏ�菜��
    cell += 1;
    ZoneData[s].pri = line[cell].sub(/\.0/, "");
    cell += 1;
    ZoneData[s].life = line[cell].sub(/\.0/, "");
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
end


=begin
//==================================================================
/**
 * �ǂݍ��񂾃t�@�C����C�f�[�^�Ƃ��ďo��
 */
//==================================================================
=end
def DataFileOutput()
  
  File.open("cross_comm.cdat", "w"){|file|
    file.printf("//============================================================\n");
    file.printf("//       cross_comm_conv.rb �ŏo�͂��ꂽ�t�@�C���ł�\n");
    file.printf("//============================================================\n\n");
    file.printf("static const struct{\n\tu32 priority:8;\n\tu32 life:24;\n}CrossCommData[] = {\n");

    for i in 0..ZoneData.size-1
      file.printf("\t{\t//%d  %s(%s)\n", i, ZoneData[i].comment, ZoneData[i].ID);
      file.printf("\t\t%s,\t\t//priority\n", ZoneData[i].pri);
      file.printf("\t\t%s * 60,\t\t//life\n", ZoneData[i].life);
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
