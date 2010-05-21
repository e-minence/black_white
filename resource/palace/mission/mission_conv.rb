=begin
���s�菇
ruby mission_conv.rb [CSV�t�@�C���ւ̃p�X]

�o�̓t�@�C��
mission_data.cdat
=end


#--------------------------------------------------------------
#  �萔��`
#--------------------------------------------------------------
OBJID_MAX = 3;
DATA_MAX = 2;

ITEM_LIST_ARRAY_MAX = 999;
OBJID_LIST_ARRAY_MAX = 999;

#--------------------------------------------------------------
#  �O���[�o���ϐ�
#--------------------------------------------------------------
InputExcelFile = ARGV[0];

$RewardMax = 0;   #��V�ő�l
$TimeMax = 0;     #���ԍő�l
ItemList = [];
ObjIDList = [];



#--------------------------------------------------------------
#  �~�b�V�����f�[�^
#--------------------------------------------------------------
class MISSION_DATA
  def initialize()
    @mission_no = "0"                  #�~�b�V�����ԍ�
    @type = "0"                        #�~�b�V�����n��
    @level = "0"                       #�������x��
    @odds = "0"                        #������
    @version = "0"                     #�I�o�o�[�W����
    @msg_id_contents = "0"            #�~�b�V�������egmm��msg_id
    @msg_id_contents_monolith = "0"   #�~�b�V�������e���m���Xgmm��msg_id
    
    @obj_id = [];
    for i in 0..OBJID_MAX
      @obj_id[i] = "BOY1";      #�ω�OBJ ID
    end

    @obj_sex = [];
    for i in 0..OBJID_MAX
      @obj_sex[i] = "�j";       #�ω�OBJ�̐���
    end
    @obj_sex_bit = 0;

    @talk_type = [];
    for i in 0..OBJID_MAX
      @talk_type[i] = "����"  #�ω�OBJ�̉�b�^�C�v
    end
    
    @data = [];
    for i in 0..DATA_MAX
      @data[i] = "0";        #�f�[�^(�~�b�V�����n�����Ɉ������ω�)
    end
    
    @time = "0"           #����(�b)
    
    @reward = "0";      #��V
  end

  attr_accessor :mission_no, :type, :level, :odds, :version, :msg_id_contents, :msg_id_contents_monolith;
  attr_accessor :obj_id, :obj_sex, :obj_sex_bit, :talk_type, :data, :time, :reward;
end


MissionData = [];


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
    MissionData[s] = MISSION_DATA.new();

    MissionData[s].mission_no = line[cell];
    cell += 1;
    MissionData[s].type = line[cell];
    cell += 1;
    MissionData[s].level = line[cell];
    cell += 1;
    MissionData[s].odds = line[cell];
    cell += 1;
    MissionData[s].version = line[cell];
    cell += 1;
    MissionData[s].msg_id_contents = line[cell];
    cell += 1;
    MissionData[s].msg_id_contents_monolith = line[cell];
    cell += 1;

    for g in 0..OBJID_MAX-1
      MissionData[s].obj_id[g] = line[cell];
      cell += 1;
      MissionData[s].obj_sex[g] = line[cell];
      cell += 1;
      MissionData[s].talk_type[g] = line[cell];
      cell += 1;
    end
    
    for g in 0..DATA_MAX-1
      MissionData[s].data[g] = line[cell].sub(/\.0/, "");#csv�̎��_�ŏ����_�����Ă���̂Ŏ�菜��
      cell += 1;
    end
    
    MissionData[s].time = line[cell];
    cell += 1;

    MissionData[s].reward = line[cell];
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
  for i in 0..MissionData.size-1
    
    MissionData[i].type = "MISSION_TYPE_" + MissionData[i].type;
    
    version_bit = 0;
    if(MissionData[i].version =~ /.*W.*/)
      version_bit |= 1 << 0;
    end
    if(MissionData[i].version =~ /.*B.*/)
      version_bit |= 1 << 1;
    end
    if(MissionData[i].version =~ /.*N.*/)
      version_bit |= 1 << 2;
    end
    MissionData[i].version = version_bit;
    
    for s in 0..OBJID_MAX-1
      if(MissionData[i].obj_id[s] =~ /.*-.*/)
        MissionData[i].obj_id[s] = "0";
      end
    end

    MissionData[i].obj_sex_bit = 0;
    for s in 0..OBJID_MAX-1
      if(MissionData[i].obj_sex[s] =~ /�j/)
        MissionData[i].obj_sex[s] = "0";
      elsif(MissionData[i].obj_sex[s] =~ /��/)
        MissionData[i].obj_sex[s] = "1";
        MissionData[i].obj_sex_bit |= 1 << s;
      else
        MissionData[i].obj_sex[s] = "0";
      end
    end

    for s in 0..OBJID_MAX-1
      if(MissionData[i].talk_type[s] =~ /����/)
        MissionData[i].talk_type[s] = "TALK_TYPE_CUTE";
      elsif(MissionData[i].talk_type[s] =~ /�|��/)
        MissionData[i].talk_type[s] = "TALK_TYPE_FRIGHTENING";
      elsif(MissionData[i].talk_type[s] =~ /����/)
        MissionData[i].talk_type[s] = "TALK_TYPE_RETICENCE";
      elsif(MissionData[i].talk_type[s] =~ /�l�Ԓj/)
        MissionData[i].talk_type[s] = "TALK_TYPE_MAN";
      elsif(MissionData[i].talk_type[s] =~ /�l�ԏ�/)
        MissionData[i].talk_type[s] = "TALK_TYPE_WOMAN";
      end
    end
    
    for s in 0..DATA_MAX-1
      if(MissionData[i].data[s] =~ /.*-.*/)
        MissionData[i].data[s] = "0";
      end
    end
    
    if(MissionData[i].reward =~ /.*-.*/)
      MissionData[i].reward = "0";
    end
    if(MissionData[i].reward.to_i > $RewardMax)
      $RewardMax = MissionData[i].reward.to_i;
    end
    
    if(MissionData[i].time.to_i > $TimeMax)
      $TimeMax = MissionData[i].time.to_i;
    end

  end

  #�A�C�e�����X�g�𒊏o
  item_list_count = 0;
  for i in 0..ITEM_LIST_ARRAY_MAX 
    ItemList[i] = "NULL";
  end
  for i in 0..MissionData.size-1
    for no in 0..DATA_MAX
      type = MissionData[i].data[no];
      for s in 0..ITEM_LIST_ARRAY_MAX 
        if(type =~ /ITEM_.*/)
          if(type == ItemList[s])
            break;
          end
          if(ItemList[s] == "NULL")
            ItemList[s] = type;
            item_list_count += 1;
            break;
          end
        end
      end
    end
  end

  #OBJID���X�g�𒊏o
  objid_list_count = 0;
  for i in 0..ITEM_LIST_ARRAY_MAX 
    ObjIDList[i] = "NULL";
  end
  for i in 0..MissionData.size-1
    for no in 0..OBJID_MAX
      type = MissionData[i].obj_id[no];
      for s in 0..OBJID_LIST_ARRAY_MAX 
        if(type == ObjIDList[s])
          break;
        end
        if(ObjIDList[s] == "NULL")
          ObjIDList[s] = type;
          objid_list_count += 1;
          break;
        end
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
  
  File.open("mission_data.cdat", "w"){|file|
    file.printf("//============================================================\n");
    file.printf("//       mission_conv.rb �ŏo�͂��ꂽ�t�@�C���ł�\n");
    file.printf("//============================================================\n\n");
    file.printf("const MISSION_CONV_DATA MissionData[] = {\n");

    for i in 0..MissionData.size-1
      file.printf("\t{\t//%d\n", i);

      file.printf("\t\t%s,\t\t//level\n", MissionData[i].level);
      file.printf("\t\t%s,\t\t//odds\n", MissionData[i].odds);
      file.printf("\t\t%s,\t\t//reward\n", MissionData[i].reward);
      file.printf("\t\t%s,\t\t//time\n", MissionData[i].time);

      file.printf("\t\t%s,\t\t//type\n", MissionData[i].type);
      file.printf("\t\t%s,\t\t//version_bit\n", MissionData[i].version);
      file.printf("\t\t%s,\t\t//msg_id_contents\n", MissionData[i].msg_id_contents);
      file.printf("\t\t%s,\t\t//msg_id_contents_monolith\n", MissionData[i].msg_id_contents_monolith);
      file.printf("\t\t%d,\t\t//obj_sex_bit\n", MissionData[i].obj_sex_bit);

      file.printf("\t\t{");
      for s in 0..OBJID_MAX-1
        file.printf("%s,", MissionData[i].obj_id[s]);
      end
      file.printf("},\t\t//obj_id\n");

      file.printf("\t\t{");
      for s in 0..OBJID_MAX-1
        file.printf("%s,", MissionData[i].talk_type[s]);
      end
      file.printf("},\t\t//talk_type\n");

      file.printf("\t\t{");
      for s in 0..DATA_MAX-1
        file.printf("%s,", MissionData[i].data[s]);
      end
      file.printf("},\t\t//data\n");
      
      file.printf("\t},\n");
    end
    
    file.printf("};\n");
  }

  #�~�b�V�������X�g�쐬�p�̍Œ���̃f�[�^�̉�
  File.open("mission_list_param.cdat", "w"){|file|
    file.printf("//============================================================\n");
    file.printf("//       mission_conv.rb �ŏo�͂��ꂽ�t�@�C���ł�\n");
    file.printf("//============================================================\n\n");
    file.printf("//�~�b�V�������X�g�쐬�p�̍Œ���̃f�[�^�̉�\n");
    file.printf("static const MISSION_CONV_DATA_LISTPARAM MissionConvDataListParam[] = {\n");

    for i in 0..MissionData.size-1
      file.printf("\t{\t//%d\n", i);
      file.printf("\t\t%s,\t\t//type\n", MissionData[i].type);
      file.printf("\t\t%s,\t\t//level\n", MissionData[i].level);
      file.printf("\t\t%s,\t\t//odds\n", MissionData[i].odds);
      file.printf("\t\t%s,\t\t//version_bit\n", MissionData[i].version);
      file.printf("\t},\n");
    end
    
    file.printf("};\n");
  }
  
  #�s���f�[�^�`�F�b�N�p�t�@�C��
  File.open("mission_ng_check.cdat", "w"){|file|
    file.printf("//============================================================\n");
    file.printf("//       mission_conv.rb �ŏo�͂��ꂽ�t�@�C���ł�\n");
    file.printf("//============================================================\n\n");

    #OBJID���X�g
    file.printf("\n\n//�o��ł���OBJID�e�[�u��(�s���`�F�b�N�p)\n");
    file.printf("ALIGN4 static const u16 IntrudeOBJID_List[] = {\n");
    for i in 0..OBJID_LIST_ARRAY_MAX
      if(ObjIDList[i] == "NULL")
        break;
      end
      file.printf("\t%s,\t\t//%d\n", ObjIDList[i], i);
    end
    file.printf("};\n");

    #�A�C�e�����X�g
    file.printf("\n\n//�o��ł���A�C�e���e�[�u��(�s���`�F�b�N�p)\n");
    file.printf("ALIGN4 static const u16 IntrudeItem_List[] = {\n");
    for i in 0..ITEM_LIST_ARRAY_MAX
      if(ItemList[i] == "NULL")
        break;
      end
      file.printf("\t%s,\t\t//%d\n", ItemList[i], i);
    end
    file.printf("};\n");

    #�ő�l��`
    file.printf("\n\n//�ő�l��`(�s���`�F�b�N�p)\n");
    file.printf("#define MISSION_REWARD_MAX   %d\t\t//��V�ő�l(�s���`�F�b�N�p)\n", $RewardMax);
    file.printf("#define MISSION_TIME_MAX   %d\t\t//�������ԍő�l(�s���`�F�b�N�p)\n", $TimeMax);
  }

end


#==============================================================================
#  ���C��
#==============================================================================
CsvConvFileCheck();
DataConv();
DataFileOutput();
