=begin
���s�菇
ruby cross_comm_conv.rb [CSV�t�@�C���ւ̃p�X]

�o�̓t�@�C��
lv_tbl.bin
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
class SYMBOL_DATA
  def initialize()
    @no = "0"             #No
    @mons_name = "0"      #�|�P������
    @level = "0"          #���x��
  end

  attr_accessor :no, :mons_name, :level;
end


SymbolData = [];

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
    SymbolData[s] = SYMBOL_DATA.new();

    SymbolData[s].no = line[cell];
    cell += 1;
    SymbolData[s].mons_name = line[cell];
    cell += 1;
    SymbolData[s].level = line[cell].sub(/\.0/, ""); #csv�̎��_�ŏ����_�����Ă���̂Ŏ�菜��
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
 * �ǂݍ��񂾃t�@�C�����o�C�i���Ƃ��ďo��
 */
//==================================================================
=end
def DataFileOutput()
  
  File.open("lv_tbl.bin", "wb"){|file|
    first_data = 0  #1origin�p�̃_�~�[�f�[�^
    file.write([first_data].pack("C"))
    for i in 0..SymbolData.size-1
      file.write([SymbolData[i].level.to_i].pack("C"))
    end
  }
  
end


#==============================================================================
#  ���C��
#==============================================================================
CsvConvFileCheck();
DataConv();
DataFileOutput();
