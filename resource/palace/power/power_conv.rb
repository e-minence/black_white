=begin
���s�菇
ruby power_conv.rb [CSV�t�@�C���ւ̃p�X]

�o�̓t�@�C��
power_data.cdat
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
class POWER_DATA
  def initialize()
    @no = "0"                  #�p���[�ԍ�
    @level_w = "0"             #�K�vWhite���x��
    @level_b = "0"             #�K�vBlack���x��
    @point = "0"               #�g�p�|�C���g
    @time = "0"                #�p������(��)
    @msg_id_title = "0"        #�p���[����msg_id
    @msg_id_explain = "0"      #�p���[����msg_id
    @gpower_id = "0"           #G�p���[ID
    @comment = "0"             #���ʐ���
    @type = "0"                #���ʌn��
    @data = "0"                #���ʃf�[�^
    @enable_f = "0"            #�L�������t���O
  end

  attr_accessor :no, :level_w, :level_b, :point, :time;
  attr_accessor :msg_id_title, :msg_id_explain, :type, :data, :gpower_id, :comment, :enable_f;
end


PowerData = [];

POWER_TYPE_ARRAY_MAX = 100;
PowerType = [];
$DummyStartNo = 0;
$DummyEndNo = -1;
$PalaceStartNo = 0;
$PalaceEndNo = -1;
$DistributionStartNo = 0;
$DistributionEndNo = -1;
$DistributionStartNoSw = 0;
$DistributionEndNoSw = -1;

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
    if ((/^no/ =~ line[0]) != nil)
      title += 1;
      next;
    end
    if(title == 0)
      next;
    end

    cell = 0;
    PowerData[s] = POWER_DATA.new();

    PowerData[s].no = line[cell];
    cell += 1;
    PowerData[s].level_w = line[cell];
    cell += 1;
    PowerData[s].level_b = line[cell];
    cell += 1;
    PowerData[s].point = line[cell];
    cell += 1;
    PowerData[s].msg_id_title = line[cell];
    cell += 1;
    PowerData[s].msg_id_explain = line[cell];
    cell += 1;
    PowerData[s].type = line[cell];
    cell += 1;
    PowerData[s].data = line[cell];
    cell += 2;
    PowerData[s].gpower_id = line[cell];
    cell += 1;
    PowerData[s].comment = line[cell];
    cell += 1;
    PowerData[s].time = line[cell];
    cell += 1;
    PowerData[s].enable_f = line[cell];
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
  
  distribution_start = 0;
  distribution_start_sw = 0;
  palace_start = 0;
  dummy_start = 0;
  
  for i in 0..PowerData.size-1
    if(PowerData[i].type == "ENCOUNT_UP" || PowerData[i].type == "ENCOUNT_DOWN" \
        || PowerData[i].type == "HATCH_UP" || PowerData[i].type == "EXP_UP" \
        || PowerData[i].type == "EXP_DOWN" || PowerData[i].type == "MONEY_UP" \
        || PowerData[i].type == "SALE" || PowerData[i].type == "CAPTURE_UP")
      num = PowerData[i].data.to_f;
      num *= 100; #�����𐮐��ɕϊ�����(100�{�̌Œ菬��)
      #PowerData[i].data = (0x100 * num / 100).to_i;  #����8bit�Œ菬���ɂ���ɕϊ�
      PowerData[i].data = num.to_i;  #����8bit�Œ菬���ɂ���ɕϊ�
    end

    if(PowerData[i].enable_f == "�~")
      if($DummyEndNo != -1)
        print "�_�~�[���A�ԂɂȂ��Ă��Ȃ�"
        print i;
        print "\n"
        exit(200);
      end
      if(dummy_start == 0)
        $DummyStartNo = i
        dummy_start = 1
      end
    elsif(dummy_start == 1 && $DummyEndNo == -1)
      $DummyEndNo = i - 1;
    end

    if(PowerData[i].level_w != "�z�z" && PowerData[i].level_b != "�z�z" \
        && PowerData[i].level_w != "�p���X" && PowerData[i].level_b != "�p���X")
=begin
      if(palace_start != 0)
        print "�p���X�̌�Ƀm�[�}����G�p���[���ݒ肳��Ă���"
        print i;
        print "\n"
        exit(200);
      end
=end
      if(distribution_start_sw != 0)
        print "�z�z�̌�Ƀm�[�}����G�p���[���ݒ肳��Ă���"
        print i;
        print "\n"
        exit(200);
      end
    end
    
    if(PowerData[i].level_w == "�z�z" || PowerData[i].level_b == "�z�z")
=begin
      if(palace_start != 0 )
        print "�z�z������ or �r���� �p���X���ݒ肳��Ă���\n"
        exit(200);
      end
=end
      if( $DistributionEndNo != -1 )
        print "BW�z�z����ɔz�z���ݒ肳��Ă���\n"
        exit(200);
      end

      if(distribution_start == 0)
        $DistributionStartNo = i;
        distribution_start = 1;
      elsif(distribution_start_sw == 0 && $DistributionEndNo != -1)
        $DistributionStartNoSw = i
        distribution_start_sw = 1
      end
      PowerData[i].level_w = "POWER_LEVEL_DISTRIBUTION";
      PowerData[i].level_b = "POWER_LEVEL_DISTRIBUTION";
    else
      if(distribution_start == 1 && $DistributionEndNo == -1)
        $DistributionEndNo = i - 1;
      end
      if(distribution_start_sw == 1 && $DistributionEndNoSw == -1 )
        $DistributionEndNoSw = i - 1;
      end
    end
    
    if(PowerData[i].level_w == "�p���X" || PowerData[i].level_b == "�p���X")
      if(palace_start == 0)
        $PalaceStartNo = i;
        palace_start = 1;
      end
      PowerData[i].level_w = "POWER_LEVEL_PALACE";
      PowerData[i].level_b = "POWER_LEVEL_PALACE";
    else
      if(palace_start == 1 && $PalaceEndNo == -1)
        $PalaceEndNo = i - 1;
      end
    end
  end
  if($PalaceEndNo == -1)
    $PalaceEndNo = PowerData.size - 1;
  end
  
  #���ʌn���𒊏o
  power_type_count = 0;
  for i in 0..POWER_TYPE_ARRAY_MAX 
    PowerType[i] = "NULL";
  end
  for i in 0..PowerData.size-1
    type = PowerData[i].type;
    for s in 0..POWER_TYPE_ARRAY_MAX 
      if(type == PowerType[s])
        break;
      end
      if(PowerType[s] == "NULL")
        PowerType[s] = type;
        power_type_count += 1;
        break;
      end
    end
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
  
  File.open("power_data.cdat", "w"){|file|
    file.printf("//============================================================\n");
    file.printf("//       power_conv.rb �ŏo�͂��ꂽ�t�@�C���ł�\n");
    file.printf("//============================================================\n\n");
    file.printf("const POWER_CONV_DATA PowerData[] = {\n");

    for i in 0..PowerData.size-1
      file.printf("\t{\t//%d\n", i);
      file.printf("\t\t%s,\t\t//level_w\n", PowerData[i].level_w);
      file.printf("\t\t%s,\t\t//level_b\n", PowerData[i].level_b);
      file.printf("\t\t%s,\t\t//point\n", PowerData[i].point);
      file.printf("\t\t%s,\t\t//msg_id_title\n", PowerData[i].msg_id_title);
      file.printf("\t\t%s,\t\t//msg_id_explain\n", PowerData[i].msg_id_explain);
      file.printf("\t\t%s,\t\t//data\n", PowerData[i].data);
      file.printf("\t\tGPOWER_TYPE_%s,\t\t//type\n", PowerData[i].type);
      file.printf("\t\t%s,\t\t//time\n", PowerData[i].time);
      file.printf("\t},\n");
    end
    
    file.printf("};\n");
  }

  File.open("gpower_id.h", "w"){|file|
    file.printf("//============================================================\n");
    file.printf("//       power_conv.rb �ŏo�͂��ꂽ�t�@�C���ł�\n");
    file.printf("//============================================================\n\n");
    file.printf("#pragma once\n\n");
    
    file.printf("typedef enum{\n");
    
    for i in 0..PowerData.size-1
      file.printf("\t%s,\t//%d : %s\n", PowerData[i].gpower_id, i, PowerData[i].comment);
    end
   
    if $DistributionEndNoSw == -1 then
      $DistributionEndNoSw = PowerData.size-1
    end
    file.printf("\n");
    file.printf("\tGPOWER_ID_DISTRIBUTION_START = %d,\t\t//�z�zG�p���[�J�nID(����ID���܂�)\n", $DistributionStartNo);
    file.printf("\tGPOWER_ID_DISTRIBUTION_END = %d,\t\t//�z�zG�p���[�I��ID(����ID���܂�)\n\n", $DistributionEndNo);
    if $DistributionStgartNoSw == 0 then
      file.printf("\tGPOWER_ID_DISTRIBUTION_START_SW = %d,\t\t//SWAN�z�zG�p���[�J�nID(����ID���܂�)\n", $DistributionStartNoSw);
      file.printf("\tGPOWER_ID_DISTRIBUTION_END_SW = %d,\t\t//SWAN�z�zG�p���[�I��ID(����ID���܂�)\n\n", $DistributionEndNoSw);
    end
#   file.printf("\tGPOWER_ID_PALACE_START = %d,\t\t//�p���XG�p���[�J�nID(����ID���܂�)\n", $PalaceStartNo); #���̒�`�͖��g�p�Ȃ̂ō폜111128
#   file.printf("\tGPOWER_ID_PALACE_END = %d,\t\t//�p���XG�p���[�I��ID(����ID���܂�)\n\n", $PalaceEndNo); #���̒�`�͖��g�p�Ȃ̂ō폜11128
    file.printf("\tGPOWER_ID_DUMMY_START = %d,\t\t//BW�̖������p���[�J�nID(����ID���܂�)\n", $DummyStartNo);
    file.printf("\tGPOWER_ID_DUMMY_END = %d,\t\t//BW�̖������p���[�I��ID(����ID���܂�)\n\n", $DummyEndNo);
    file.printf("\tGPOWER_ID_MAX = %d,\t\t//G�p���[�ő吔\n", PowerData.size);
    #file.printf("\tGPOWER_ID_NULL = GPOWER_ID_MAX,\t\t//G�p���[���������Ă��Ȃ�\n");
    file.printf("}GPOWER_ID;\n\n");
    
    file.printf("\n\n//���ʌn���@�@!!�o�͌��f�[�^�ɂ����enum�̕��т��ς��ꍇ������܂�!!\n");
    file.printf("typedef enum{\n");
    for i in 0..POWER_TYPE_ARRAY_MAX
      if(PowerType[i] == "NULL")
        break;
      end
      file.printf("\tGPOWER_TYPE_%s,\t\t//%d\n", PowerType[i], i);
    end
    file.printf("\n\tGPOWER_TYPE_MAX,\n");
    file.printf("\tGPOWER_TYPE_NULL = GPOWER_TYPE_MAX,\n");
    file.printf("}GPOWER_TYPE;\n\n");
  }
  
end


#==============================================================================
#  ���C��
#==============================================================================
CsvConvFileCheck();
DataConv();
DataFileOutput();
