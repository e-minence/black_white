#===============================================================
# WB�G���J�E���g�f�[�^�R���o�[�^
# 09.09.27 iwasawa
#===============================================================

load "encount_common.def"
#require 'jstring'
$KCODE = "SJIS" #�����R�[�h��SJIS�ɐݒ�Bmonsno_hash.rb�̓ǂݍ��݂ɕK�v
require "../../tools/hash/monsno_hash.rb"

arg = $*

##############################################
#�\���̒�`
$_enc_poke = Struct.new("EncPoke", :monsno, :form, :lv_min, :lv_max)
#�G���J�E�g�����X�^�[�f�[�^�\���̒�
ENC_MONS_DATA_SIZ = 4

SEASON_NONE = 0
SEASON_SPRING = 1
SEASON_SUMMER = 2
SEASON_AUTUMN = 3
SEASON_WINTER = 4

FORM_RND_CODE_STR = "-"
FORM_RND_CODE = 31

##############################################
#�O���[�o���ϐ���`

#�o�̓p�X
$output_path = [WFBIN_DIR,BFBIN_DIR]
#�o�̓T�t�B�b�N�X������
$output_suffix = [".bin","_sp.dat","_su.dat","_au.dat","_wi.dat"]

#�e��f�[�^�e�[�u����
$table_num = [GROUND_MONS_NUM,GROUND_MONS_NUM,GROUND_MONS_NUM,WATER_MONS_NUM,WATER_MONS_NUM,WATER_MONS_NUM,WATER_MONS_NUM]

#�G�ߖ����X�g
$season_list = ["�t","��","�H","�~"]
#�A�C�e���e�[�u�������X�g
$item_tbl_list = ["�Ȃ�","���A","��"]

#�G���J�E���g�e�[�u��
class CEncTable
  MONSNO_BIT_NUM = 11
  FORM_BIT_NUM = 5

  @zone_name

  @prob
  @list_num
  @mons_list

  #������
  def initialize zone_name
    @zone_name  = zone_name

    @prob = 0
    @list_num = 0
    @mons_list = Array.new
  end

  #�f�[�^�G���[�`�F�b�N
  def check_disable column,enc_prob,dat_len,null_check_len,dat_name
    #�f�[�^�̗L���`�F�b�N
    if enc_prob == 0 then
      #�ǂݔ�΂��ďI��
      column.slice!(0..(dat_len-1))
      return true
    end
    #�G���[�`�F�b�N
    null_idx = column.index("")
    if null_idx != nil && null_idx < null_check_len then
      printf("%s �f�[�^�G���[ %d��ڂɕs���ȋ󗓂�����܂� <= %s �m��=%d\n",@zone_name,null_idx,dat_name,enc_prob)
      exit 1 
    end
    return false
  end

  #�t�H�����擾
  def form_get str_form
    if str_form == FORM_RND_CODE_STR then
      form = FORM_RND_CODE
    else
      form = str_form == "" ? 0 : str_form.to_i
    end
    return form
  end

  #�O�����h�f�[�^���l�p�[�X(�j��I���\�b�h�Ȃ̂Œ���)
  def perse_ground_val column,enc_prob,dat_name
    if check_disable(column,enc_prob,GROUND_MONS_DATA_LEN,GROUND_MONS_NUM,dat_name) == true then
      return 
    end

    #lv��form�̎擾
    GROUND_MONS_NUM.times{ |i|
      @mons_list << $_enc_poke.new 
      cp = @mons_list.last()
      cp.lv_min = cp.lv_max = column[i].to_i 
      cp.form = form_get(column[i+GROUND_MONS_NUM])
    }
    column.slice!(0..(GROUND_MONS_DATA_LEN-1))
  end

  #����f�[�^���l�p�[�X
  def perse_water_val column,enc_prob,dat_name
    if check_disable(column,enc_prob,WATER_MONS_DATA_LEN,WATER_MONS_NUM*2,dat_name) == true then
      return 
    end

    #lv��form�̎擾
    WATER_MONS_NUM.times{ |i|
      @mons_list << $_enc_poke.new 
      cp = @mons_list.last()
      cp.lv_min = column[i].to_i 
      cp.lv_max = column[i+1].to_i 
      cp.form = form_get(column[i+(WATER_MONS_NUM*2)])
    }
    column.slice!(0..(WATER_MONS_DATA_LEN-1))
  end

  #�����X�^�[���p�[�X
  def perse_monsno column,data_num
    if @mons_list.size == 0 then
      column.slice!(0..(data_num-1))
      return
    end

    data_num.times{ |i|
      pl = @mons_list[i]
      pl.monsno = $monsno_hash.fetch(column[i],nil)
      print(column[i]+",")
      if pl.monsno == nil then
        printf("\nERROR! %s �����X�^�[���G���[ %s\n",@zone_name,column[i])
        exit 1
      end
    }
    column.slice!(0..(data_num-1))
    print "\n"
  end

  #�G���J�E���g�f�[�^�o�� 4byte
  def output_bin fp
    buf = Array.new
    buf.fill(0,0,4)
    for pl in @mons_list do
      buf[0] = ( pl.monsno | (pl.form << MONSNO_BIT_NUM)) 
      buf[1] = pl.lv_min
      buf[2] = pl.lv_max
      fp.write(buf.pack("S1C2"))
    end
  end

end

#1�}�b�v�̃G���J�E���g�f�[�^
class CMapData
  @zone_name  #�]�[����
  @item_idx   #�A�C�e���e�[�u���w��
  @enc_prob #�G���J�E���g���e�[�u��
  @table  #�f�[�^�e�[�u��

  #������
  def initialize zone_name
    @zone_name = zone_name
    @item_idx = 0
    @enc_prob = Array.new
    @enc_prob.fill(0,0,IDX_MAX+1)

    @table = Array.new
    IDX_MAX.times{ |i|
      @table[i] = CEncTable.new(zone_name)
    }
  end

  #1�}�b�v�̃f�[�^�p�[�X
  def perse column
    print(column.join(",")) 
    #�A�C�e���e�[�u��index�擾
    @item_idx = $item_tbl_list.index(column[COL_ZONE_OFS_ITEM_TABLE])
    if @item_idx == nil then
      @item_idx = 0
    end
    #�G���J�E���g���擾
    @enc_prob[IDX_GROUND_L] = column[COL_ZONE_OFS_PROB_GROUND_L] == "" ? 0 : column[COL_ZONE_OFS_PROB_GROUND_L].to_i
    @enc_prob[IDX_GROUND_H] = column[COL_ZONE_OFS_PROB_GROUND_H] == "" ? 0 : column[COL_ZONE_OFS_PROB_GROUND_H].to_i
    @enc_prob[IDX_WATER] = column[COL_ZONE_OFS_PROB_WATER] == "" ? 0 : column[COL_ZONE_OFS_PROB_WATER].to_i
    @enc_prob[IDX_FISHING] = column[COL_ZONE_OFS_PROB_FISHING] == "" ? 0 : column[COL_ZONE_OFS_PROB_FISHING].to_i
    
    #�z��̗v�f�폜
    column.slice!(0..(COL_ZONE_HEADER_NUM-1))
=begin
    column.delete_at(COL_ZONE_OFS_ITEM_TABLE)
    column.delete_at(COL_ZONE_OFS_PROB_GROUND_L)
    column.delete_at(COL_ZONE_OFS_PROB_GROUND_H)
    column.delete_at(COL_ZONE_OFS_PROB_WATER)
    column.delete_at(COL_ZONE_OFS_PROB_FISHING)
=end
    print("\n->"+@zone_name+" �f�[�^�T�[�`�J�n �m�� "+@enc_prob.join(",")+"\n")
    print(column.join(",")) 
    #�O�����h�f�[�^value�p�[�X
    @table[IDX_GROUND_L].perse_ground_val(column,@enc_prob[IDX_GROUND_L],"�ʏ�1")
    @table[IDX_GROUND_H].perse_ground_val(column,@enc_prob[IDX_GROUND_H],"�ʏ�2")
    @enc_prob[IDX_GROUND_SP] = column[0] == "" ? 0 : 1
    @table[IDX_GROUND_SP].perse_ground_val(column,@enc_prob[IDX_GROUND_SP],"����")

    #����E�ނ�f�[�^value�p�[�X
    @table[IDX_WATER].perse_water_val(column,@enc_prob[IDX_WATER],"����")
    @enc_prob[IDX_WATER_SP] = column[0] == "" ? 0 : 1
    @table[IDX_WATER_SP].perse_water_val(column,@enc_prob[IDX_WATER_SP],"���ꐅ��")

    @table[IDX_FISHING].perse_water_val(column,@enc_prob[IDX_FISHING],"�ނ�")
    @enc_prob[IDX_FISHING_SP] = column[0] == "" ? 0 : 1
    @table[IDX_FISHING_SP].perse_water_val(column,@enc_prob[IDX_FISHING_SP],"����ނ�")
    
    #�����X�^�[���p�[�X
    IDX_MAX.times{ |i|
      @table[i].perse_monsno(column,$table_num[i])
    }
  end

  #�G���J�E���g���o�� 8byte
  def output_enc_prob fp
    @enc_prob[IDX_MAX] = @item_idx
    fp.write(@enc_prob.pack("C8")) 
  end

  #�G���J�E���g�f�[�^�o��
  def output_enc_data fp
    dmy_buf = Array.new
    dmy_buf.fill(0,0,ENC_MONS_DATA_SIZ*GROUND_MONS_NUM)

    IDX_MAX.times{ |i|
      if @enc_prob[i] == 0 then
        tmp  = "C" + ($table_num[i]*4).to_s
        fp.write(dmy_buf.pack(tmp))
        next
      end
      @table[i].output_bin(fp)
    }
  end

  #1�}�b�v�f�[�^�o��
  def output version,season_code
    path = $output_path[version] + "/" + @zone_name + $output_suffix[season_code]
    fp = File.open(path,"wb")
    #�G���J�E���g���o��
    output_enc_prob(fp)
    #�G���J�E���g�f�[�^�o��
    output_enc_data(fp)
    fp.close()
  end
end

#1�]�[���̃f�[�^
class CZoneData
  @zone_name  #�]�[����
  @white      #�z���C�gVer�̃f�[�^
  @black      #�u���b�NVer�̃f�[�^
  @v2_enable  #�u���b�N�f�[�^���L�����ǂ���
  @season     #�G��ID
  
  #������
  def initialize
    @zone_name = ""
    @v2_enable = FALSE
    @season = nil
  end #eof initialize

  #�f�[�^�z��̑���
  def perse column,season_code,old_zone
    @zone_name = column[COL_ZONE_NAME].downcase
    printf("\n#%s �R���o�[�g�J�n\n",@zone_name)

    if /[^0-9A-Za-z]/ =~ @zone_name then
      printf("%s �G���[ �]�[�������S�p�ŋL�q����Ă��܂�-> %s\n���p�p�����ɒ����Ă�������\n",@zone_name,@zone_name.scan(/[^0-9A-Za-z]/).join(","))
      exit 1
    end
    
    season_name = column[COL_SEASON]
    @v2_enable = column[COL_V2_ENABLE]  == "��" ? true : false

    #�G�߃R�[�h�`�F�b�N
    if season_name == "" then
      @season = SEASON_NONE
      if season_code != 0 then
        printf("%s �G�ߎw�肪�s�� �t�ďH�~�������Ă��܂���\n",old_zone)
        exit 1
      end
    else
      @season = $season_list.index(season_name)
      if @season == nil then
        printf("%s �G�ߖ�������������܂��� %s\n",@zone_name,season_name)
        exit 1
      end
      @season += 1
    end
    if @season != season_code && @season != SEASON_SPRING then
      printf("%s �G�ߎw�肪�s�� �t�ďH�~�������Ă��܂���\n",@zone_name)
      exit 1
    end

#    column.slice!(COL_V2_ENABLE)  #�t���O������菜��
    column.slice!(0..COL_OFS_HEADER)  #�w�b�_������菜��

    white = column.slice!(0..COL_LEN_ZONE)
    black = column.slice!(0..COL_LEN_ZONE)

    @white = CMapData.new(@zone_name)
    @black = CMapData.new(@zone_name)

    @white.perse(white)
    if @v2_enable == false then
      return
    end
#    print black 
    @black.perse(black)
  end #eof perse

  #�f�[�^��̏o��
  def output
    @white.output(VER_WHITE,@season)
    if @v2_enable == true then
      @black.output(VER_BLACK,@season)
    else
      @white.output(VER_BLACK,@season)  #�z���C�g�̃R�s�[���o��
    end
  end

  #�]�[�����擾
  def get_zone_name
    return @zone_name
  end
  #�G�߃R�[�h�擾
  def get_season_code
    return @season
  end
end #End of Class CMapData

#�|�P����
class CEncountData
  @zone_num
  @zone_list

  #������
  def initialize
    @zone_num = 0
    @zone_list = Array.new
  end

  #�l�G�t�@�C���̘A��
  def season_bin_link f_path,bin_path,zone_name
    printf("%s �l�G�f�[�^�A���J�n -> %s\n",zone_name,f_path)
    File.open(f_path,"wb"){ |file|
      for i in SEASON_SPRING..SEASON_WINTER do
        src = bin_path+"/"+zone_name+$output_suffix[i]
        printf(" %s,",src)
        if FileTest.exist?(src)
          baseHandle = File::open(src)
          file.write(baseHandle.read)
          baseHandle.close
        else
          printf("%s ���K�v�ł��B�]�[��%s�̃f�[�^���������Ă�������\n",src,zone_name)
          exit 1
        end
      end
      print("\n")
    }
  end

  #�A�[�J�C�u���X�g�o��
  def output_arclist f_path,bin_path
    File.open(f_path,"w"){ |file|
      for n in @zone_list do
        season = n.get_season_code()
        zone_name = n.get_zone_name()
        buf = "%s/%s.bin" % [bin_path,zone_name]
        if season > SEASON_SPRING then
          next
        elsif season == SEASON_SPRING then
          season_bin_link(buf,bin_path,zone_name)
        end
        file.print("\""+buf+"\"\n")
      end
    }
  end

  #�t�@�C�����[�h
  def src_load f_path
    season_code = 0
    zone_ary = Array.new
    old_zone = "zone"

    File.open(f_path){ |file|
     while line = file.gets
       line = line.chomp #�s�����s����菜��
       line = line.gsub("\"","")  #"����菜��
       work = line.split(",")

       @zone_list << CZoneData.new
       cp = @zone_list.last()
       #�}�b�v�f�[�^�̃p�[�X
       cp.perse(work,season_code,old_zone)
       #�G�߃R�[�h�`�F�b�N
       s_code = cp.get_season_code()
       if s_code == SEASON_NONE || (s_code+1)>SEASON_WINTER then
         season_code = SEASON_NONE  #�R�[�h0�N���A
       else
         season_code = s_code+1 #���̋G�߃R�[�h�����Ă���
       end
       old_zone = cp.get_zone_name()

       if zone_ary.index( old_zone ) != nil then
         printf("ERROR: �]�[�����R�[�h���d�����Ă��܂��I -> %s\n",old_zone)
         exit 1
       end
       #�����擾
       if s_code == SEASON_NONE || s_code == SEASON_WINTER then
         zone_ary << old_zone
       end

       #�}�b�v�f�[�^�̏o��
       cp.output()
     end

     if season_code != SEASON_NONE then
      printf("%s �G�ߎw�肪�s�� �t�ďH�~�������Ă��܂���\n",old_zone)
      exit 1
     end
    }
  end
end

#---------------------------------------------
# �R���o�[�g���C��
#---------------------------------------------
def convert
  #�R���o�[�g���C��
  c_enc = CEncountData.new()
  c_enc.src_load("encount_wb.csv")
  c_enc.output_arclist(ARCW_LIST_FILENAME,WFBIN_DIR)
  c_enc.output_arclist(ARCB_LIST_FILENAME,BFBIN_DIR)
end

#���s
convert

