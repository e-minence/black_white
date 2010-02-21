=begin
���s�菇
ruby mission_conv.rb [CSV�t�@�C���ւ̃p�X]

�o�̓t�@�C��
mission_data.cdat
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
class MISSION_DATA
  def initialize()
    @mission_no = "0"                  #�~�b�V�����ԍ�
    @type = "0"                        #�~�b�V�����n��
    @level = "0"                       #�������x��
    @odds = "0"                        #������
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

    @talk_type = [];
    for i in 0..OBJID_MAX
      @talk_type[i] = "����"  #�ω�OBJ�̉�b�^�C�v
    end
    
    @data = [];
    for i in 0..DATA_MAX
      @data[i] = "0";        #�f�[�^(�~�b�V�����n�����Ɉ������ω�)
    end
    
    @time = "0"           #����(�b)
    
    @reward = [];
    for i in 0..REWARD_MAX
      @reward[i] = "0";      #��V 0(1��) �` 3(4��)
    end
    
    @confirm_type = "0"   #�~�b�V�����m�F�^�C�v
    @limit_run = "0"       #��������@TRUE:����֎~
    @limit_talk = "0"      #�����b�@TRUE:�b�֎~
  end

  attr_accessor :mission_no, :type, :level, :odds, :msg_id_contents, :msg_id_contents_monolith;
  attr_accessor :obj_id, :obj_sex, :talk_type, :data, :time, :reward, :confirm_type;
  attr_accessor :limit_run, :limit_talk;
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
    MissionData[s].msg_id_contents = line[cell];
    cell += 1;
    MissionData[s].confirm_type = line[cell];
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
      MissionData[s].data[g] = line[cell];
      cell += 1;
    end
    
    MissionData[s].time = line[cell];
    cell += 1;

    for g in 0..REWARD_MAX-1
      MissionData[s].reward[g] = line[cell];
      cell += 1;
    end
    
    MissionData[s].limit_run = line[cell];
    cell += 1;
    MissionData[s].limit_talk = line[cell];
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
    MissionData[i].confirm_type = "CONFIRM_" + MissionData[i].confirm_type;
    
    for s in 0..OBJID_MAX-1
      if(MissionData[i].obj_id[s] =~ /.*-.*/)
        MissionData[i].obj_id[s] = "0";
      end
    end

    for s in 0..OBJID_MAX-1
      if(MissionData[i].obj_sex[s] =~ /�j/)
        MissionData[i].obj_sex[s] = "0";
      elsif(MissionData[i].obj_sex[s] =~ /��/)
        MissionData[i].obj_sex[s] = "1";
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
    
    for s in 0..REWARD_MAX-1
      if(MissionData[i].reward[s] =~ /.*-.*/)
        MissionData[i].reward[s] = "0";
      end
    end
    
    if(MissionData[i].limit_run =~ /.*-.*/)
      MissionData[i].limit_run = "FALSE";
    elsif(MissionData[i].limit_run =~ /.*��.*/)
      MissionData[i].limit_run = "TRUE";
    end

    if(MissionData[i].limit_talk =~ /.*-.*/)
      MissionData[i].limit_talk = "FALSE";
    elsif(MissionData[i].limit_talk =~ /.*��.*/)
      MissionData[i].limit_talk = "TRUE";
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
      file.printf("\t\t%s,\t\t//mission_no\n", MissionData[i].mission_no);
      file.printf("\t\t%s,\t\t//type\n", MissionData[i].type);
      file.printf("\t\t%s,\t\t//level\n", MissionData[i].level);
      file.printf("\t\t%s,\t\t//odds\n", MissionData[i].odds);
      file.printf("\t\t%s,\t\t//msg_id_contents\n", MissionData[i].msg_id_contents);
      file.printf("\t\t%s,\t\t//msg_id_contents_monolith\n", MissionData[i].msg_id_contents_monolith);

      file.printf("\t\t{");
      for s in 0..OBJID_MAX-1
        file.printf("%s,", MissionData[i].obj_id[s]);
      end
      file.printf("},\t\t//obj_id\n");

      file.printf("\t\t{");
      for s in 0..OBJID_MAX-1
        file.printf("%s,", MissionData[i].obj_sex[s]);
      end
      file.printf("},\t\t//obj_sex\n");

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
      
      file.printf("\t\t%s,\t\t//time\n", MissionData[i].time);

      file.printf("\t\t{");
      for s in 0..REWARD_MAX-1
        file.printf("%s,", MissionData[i].reward[s]);
      end
      file.printf("},\t\t//reward\n");

      file.printf("\t\t%s,\t\t//confirm_type\n", MissionData[i].confirm_type);
      file.printf("\t\t%s,\t\t//limit_run\n", MissionData[i].limit_run);
      file.printf("\t\t%s,\t\t//limit_talk\n", MissionData[i].limit_talk);
      file.printf("\t\t0,\t\t//padding\n");

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
