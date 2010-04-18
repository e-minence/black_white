=begin
���s�菇
ruby secret_item_conv.rb [CSV�t�@�C���ւ̃p�X]

�o�̓t�@�C��
intrude_secret_item.cdat
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
class SECRET_DATA
  def initialize()
    @no = "0"                  #�z�u�ꏊID
    @grid_x = "0"              #�O���b�hX
    @grid_y = "0"              #�O���b�hY
    @grid_z = "0"              #�O���b�hZ
    @zone_id = "0"             #�]�[��ID
    @reverse_zone_id = "0"     #���]�[��ID
  end

  attr_accessor :no, :grid_x, :grid_y, :grid_z, :zone_id, :reverse_zone_id;
end


SecretData = [];
$secret_data_max = 0;


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
    if ((/^�z�u�ꏊID/ =~ line[0]) != nil)
      title += 1;
      next;
    end
    if(title == 0)
      next;
    end

    cell = 0;
    SecretData[s] = SECRET_DATA.new();

    SecretData[s].no = line[cell];
    cell += 1;
    SecretData[s].grid_x = line[cell];
    cell += 1;
    SecretData[s].grid_y = line[cell];
    cell += 1;
    SecretData[s].grid_z = line[cell];
    cell += 1;
    SecretData[s].zone_id = line[cell];
    cell += 1;
    SecretData[s].reverse_zone_id = line[cell];
    cell += 1;

    s += 1;
    $secret_data_max = s;
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
  
  File.open("intrude_secret_item.cdat", "w"){|file|
    file.printf("//============================================================\n");
    file.printf("//       secret_item_conv.rb �ŏo�͂��ꂽ�t�@�C���ł�\n");
    file.printf("//============================================================\n\n");
    file.printf("///�N���B���A�C�e���z�u�f�[�^\n");
    file.printf("const INTRUDE_SECRET_ITEM_POSDATA IntrudeSecretItemPosDataTbl[] = {\n");

    for i in 0..SecretData.size-1
      file.printf("\t{%s, %s, %s, %s},\t\t//%d\n", SecretData[i].grid_x, SecretData[i].grid_y, SecretData[i].grid_z, SecretData[i].zone_id, i);
    end
    
    file.printf("};\n");


    file.printf("\n\n///�N���B���A�C�e���z�u�f�[�^�̗��t�B�[���h�]�[��ID�e�[�u��\n");
    file.printf("//  ��IntrudeSecretItemPosDataTbl[3].zone_id�̗��t�B�[���h�̃]�[��ID��IntrudeSecretItemReverseZoneTbl[3]�A�ƂȂ�\n");
    file.printf("const u16 IntrudeSecretItemReverseZoneTbl[] = {\n");

    for i in 0..SecretData.size-1
      file.printf("\t%s,\t\t//%d\n", SecretData[i].reverse_zone_id, i);
    end

    file.printf("};\n");
  }

  File.open("intrude_secret_item_def.h", "w"){|file|
    file.printf("//============================================================\n");
    file.printf("//       secret_item_conv.rb �ŏo�͂��ꂽ�t�@�C���ł�\n");
    file.printf("//============================================================\n\n");
    file.printf("#pragma once\n\n");
    
    file.printf("///IntrudeSecretItemPosDataTbl�̃e�[�u����\n");
    file.printf("#define SECRET_ITEM_DATA_TBL_MAX\t\t\t%d\n", $secret_data_max);
  }
  
end


#==============================================================================
#  ���C��
#==============================================================================
CsvConvFileCheck();
DataConv();
DataFileOutput();
