#!ruby -Ks
#!/usr/bin/ruby

#===================================================================
#
# @brief  �f���Đ��A�v���p�f�[�^����
#
# @data   10.03.08 
# @author iwasawa
#
#===================================================================

#====================================================
# �ݒ�
#====================================================
# �����R�[�h�w��
$KCODE = 'SJIS'
# String �N���X�� tr ���\�b�h�œ��{����g�p�\�ɂ���B
require "jcode"

arg = $*
$input_path = arg[0]
$output_path = arg[1]
$f_name = arg[2];

#====================================================
# �萔�錾
#====================================================
#�w�b�_�t�@�C���o��
DST_FILENAME = "demo3d_resdata.cdat"  #�o�̓t�@�C����
DST_BRIEF = "�R���o�[�g���ɂ���f�[�^����f���Đ��Ɏg���e�[�u���𐶐����܂��B"
DST_AUTOR = "miyuki iwasawa"
DST_NOTE = "���̃t�@�C����demo3d/make_cdat.rb�ɂ���Ď����������ꂽ���̂ł��B"
ARC_ID = "ARCID_DEMO3D_GRA"
STR_NARC_PRE = "NARC_data_demo3d_"
STR_COMMENT_LINE = "//=========================================================================\n"

#�f�[�^�e�[�u��
$onoff_list = ["OFF","ON"]
$frame_rate_list = ["60fps","30fps"]
$timezone_list = ["TIMEZONE_NONE","TIMEZONE_MORNING","TIMEZONE_NOON","TIMEZONE_EVENING","TIMEZONE_NIGHT","TIMEZONE_MIDNIGHT"]
$timezone_suffix = ["","_morning","_noon","_evening","_night","_midnight"]

$nsx_list = ["nsbtp","nsbta","nsbca","nsbma","nsbva"]

#�\���̒�`
$_scene = Struct.new("SceneParam",\
	:frame_rate, :camera_name, :zone_id, :bgm_no, :fovy_sin, :fovy_cos, :near, :far, :alpha_sort_mode, :buffer_mode, \
	:double_view_f, :fog_f, :fog_rgba, :fog_shift, :fog_offset, :fog_mode, :fog_tbl, \
  :edge_f, :edge_rgb_tbl, :clear_rgba, :fadein_col, :fadein_wait, :fadeout_col, :fadeout_wait )

$_unit_prm = Struct.new("UnitParam",\
  :time_zone, :res_tbl )

$_unit = Struct.new("Unit", :time_zone_f, :unit_name, :unit_num, :unit_tbl )

$_scene_unit = Struct.new("SceneUnit", :main_name, :chg_type, :chg_tbl )

$_command = Struct.new("Command", :command, :frame, :param )

#-----------------------------------------------
# demo3d ���\�[�X�f�[�^csv
#-----------------------------------------------
class CDemo3DRes 
  
  #�f�[�^�o��
  def output fpath
	  fp = File.open(fpath,"w")
    
    fp.puts("//=========================================================================");
    fp.puts("/**");
    fp.puts(" * @fp\t#{@base_name}.cdat");
    fp.puts(" * @brief\t" + DST_BRIEF);
    fp.puts(" * @autor\t" + DST_AUTOR);
    fp.puts(" * @note\t" + DST_NOTE);
    fp.puts(" */");
    fp.puts("//=========================================================================\n\n");

    #�V�[���p�����[�^�o��
    output_scene(fp,@scene)

    #�R�}���h�f�[�^
    output_command(fp, @com_tbl, "")
    output_command(fp, @end_com_tbl, "end_")

    #���j�b�g�p�����[�^�o��
    output_unit(fp)

	  fp.close()
  end
 
  #�V�[���p�����[�^�o��
  def output_scene fp, scene
    fp.puts("//============================================\n");
    fp.puts("// �V�[���p�����[�^")
    fp.puts("//============================================\n\n");

    fp.puts("static const DEMO3D_SCENE_DATA data_#{@base_name}_scene = {")

    fp.puts("\t#{STR_NARC_PRE}#{scene.camera_name}_bin,\t///<�J����ID")
    if scene.bgm_no == "BGM_NONE" then
      fp.puts("\tDEMO3D_BGM_NONE,\t///<BGM No")
    else
      fp.puts("\t#{scene.bgm_no},\t///<BGM No")
    end 
    fp.puts("\t#{scene.zone_id},\t///<�]�[��ID")
    fp.puts("\tDEMO3D_FRAME_RATE_#{scene.frame_rate.upcase},\t///<�t���[�����[�g��`")
    fp.puts("\t#{scene.double_view_f},#{scene.fog_f},#{scene.edge_f},\t///<2��ʘA��,�t�H�OOn/Off,�G�b�W�}�[�L���OOn/Off")
    
    fp.puts("\tFX32_CONST(#{scene.fovy_sin}),FX32_CONST(#{scene.fovy_cos}),\t///<fovy_sin,fovy_cos")
    fp.puts("\tFX32_CONST(#{scene.near}),FX32_CONST(#{scene.far}),\t///<near,far")
   
    buf = make_rgb_str(scene.clear_rgba)
    fp.puts("\t#{buf},#{scene.clear_rgba[3]},\t///<�N���A�J���[RGB,A")
    
    buf = make_rgb_str(scene.fog_rgba)
    fp.puts("\t#{buf},#{scene.fog_rgba[3]},\t///<�t�H�ORGB,A")
    fp.puts("\t#{scene.fog_shift},#{scene.fog_offset},\t///<�t�H�Oshift,�t�H�Ooffset")
    
    buf = "{ "
    for i in 0..7 do
      buf += "#{scene.fog_tbl[i]},"
    end
    fp.puts("\t#{buf} }, \t///<�t�H�O�Z�x�e�[�u��")
   
    #�G�b�W�}�[�L���O�J���[
    buf = "{ "
    for i in 0..3 do
      buf += (make_rgb_str(scene.edge_rgb_tbl[i])+",")
    end
    fp.puts("\t#{buf}\t///<�G�b�W�}�[�L���O�J���[�e�[�u��")
    buf = "  "
    for i in 4..7 do
      buf += (make_rgb_str(scene.edge_rgb_tbl[i])+",")
    end
    fp.puts("\t#{buf} },")

    #�t�F�[�h�C��
    fp.puts("\tGFL_FADE_MASTER_BRIGHT_#{scene.fadein_col}OUT, #{scene.fadein_wait},\t///<�t�F�[�h�C��")
    fp.puts("\tGFL_FADE_MASTER_BRIGHT_#{scene.fadeout_col}OUT, #{scene.fadeout_wait},\t///<�t�F�[�h�A�E�g")
    #�t�F�[�h�A�E�g

    fp.puts("};\n\n")
  end
  
  #�V�[���R�}���h�p�����[�^�o��
  def output_command fp, tbl, str
    fp.puts("//============================================\n");
    fp.puts("// �V�[���R�}���h�p�����[�^")
    fp.puts("//============================================\n\n");

    fp.puts("static const DEMO3D_CMD_DATA demo3d_cmd_#{@base_name}_#{str}data[] = {")
    com_end = ""
    for n in tbl do
      buf = " { DEMO3D_CMD_TYPE_#{n.command}, #{n.frame}, { "
      for i in 0..3 do
        if n.param[i] == nil || n.param[i] == "" then
          buf += "DEMO3D_CMD_PARAM_NULL, "
        else
          buf += "#{n.param[i] }, "
        end
      end
      buf += "} },"
      fp.puts(buf)

      com_end = n.command
    end
    fp.puts("};\n\n")

    if com_end != "END" then
      print("Error! �R�}���h��END�ŏI�����Ă��܂���\n�Ō�̃R�}���h�͕K��END�ɂ��Ă�������\n")
      exit 1
    end
  end

  #���j�b�g�e�[�u���o��
  def output_unit fp
    fp.puts("\n//============================================\n");
    fp.puts("// Unit�p�����[�^")
    fp.puts("//============================================\n\n");
   
    for unit in @unit do
      output_unit_res(fp,unit,@base_name)
    end
   
    output_scene_unit(fp,@scene_unit)
  end

  #���j�b�g�t�@�C�����\�[�X�p�����[�^�o��
  def output_unit_res fp, unit, base_name
    u_name = unit.unit_name
    b_name = "res_#{base_name}_file_"

    fp.puts("///==================================\n");
    fp.puts("/// #{u_name} Unit File Resource\n\n")

    ct = 0
    for prm in unit.unit_tbl do
      suffix = $timezone_suffix[ct+unit.time_zone_f]
      fp.puts("static const u16 #{b_name}#{u_name}#{suffix}[] = {")

      for n in prm.res_tbl do
        fp.puts(" #{STR_NARC_PRE}#{n},")
      end

      fp.puts("};\n")

      ct += 1
    end
    
    ct = 0
    fp.puts("\nstatic const DEMO3D_FILE_UNIT #{b_name}unit_#{u_name}[] = {")
    for prm in unit.unit_tbl do
      suffix = $timezone_suffix[ct+unit.time_zone_f]
      buf = "#{b_name}#{u_name}#{suffix}"
      fp.puts(" { #{buf}, #{prm.res_tbl.size} },")
      ct += 1
    end
    fp.puts("};\n\n")
  end

  #�V�[�����j�b�g�e�[�u���o��
  def output_scene_unit fp, scene_unit
    b_name = "res_#{@base_name}_file_unit"
    ct = 0
    
    fp.puts("///========================================================\n");
    fp.puts("/// �V�[�����j�b�g�e�[�u��\n")
    fp.puts("///========================================================\n");

    print("Scene unit num = #{scene_unit.size}\n")
    for set in scene_unit do
      name = set.main_name
      fp.puts("\nstatic const DEMO3D_FILE_UNIT_TBL #{b_name}_#{name}_tbl[] = {")
    
      for list in set.chg_tbl do
        unit  = get_unit_from_name( list )
        fp.puts(" { #{b_name}_#{unit.unit_name}, #{unit.unit_tbl.size} },")
      end

      fp.puts("};\n")
    end
    
    fp.puts("\nstatic const DEMO3D_SCENE_UNIT res_#{@base_name}_scene_unit[] = {")

    for set in scene_unit do
      name = set.main_name
      fp.puts(" { #{b_name}_#{name}_tbl, #{set.chg_tbl.size}, DEMO3D_UNITCHG_#{set.chg_type.upcase} },")
    end
    fp.puts("};\n\n")
  end

  #�V�[�����j�b�g����
  def get_unit_from_name name
    for unit in @unit do
      if unit.unit_name == name then
        return unit
      end
    end
    print("Error! �w�肵�����j�b�g��->#{name}��������܂���\n")
    exit 1

    return nil
  end

  #�J���[�����񐶐�
  def make_rgb_str rgb
    return "GX_RGB(#{rgb[0]},#{rgb[1]},#{rgb[2]})"
  end

  #�d�b�ԍ�ID��`�w�b�_�o��
  def output_head fpath
	fp = File.open(fpath,"wb")

	#�w�b�_�o��
	fp.puts $buf_head01
	
	ct = 0
	#���C���o��
	for m in @data do
	  fp.puts("#define PMTEL_NUMBER_" + m.number + "\t(" + ct.to_s + ")\n")
	  ct += 1
	end
	fp.puts("#define PMTEL_NUMBER_MAX\t(" + ct.to_s + ")\n")
	
	#�t�b�^�[�o��
	fp.puts $buf_foot01

	fp.close()
  end
  
  #defineID�T�[�`
  def search_def obj,tag,d_tag,d_no,msg
	if d_tag != nil then
	  if tag == d_tag then
		return d_no
	  end
	end

	tmpI = obj.searchID(tag)
	if tmpI != nil
	  return tmpI
	else
	  print msg + " " + tag + "\n"
	  exit 1
	end
  end
 
  #�V�[���p�����[�^�擾
  def get_scene_param fp
    @scene = $_scene.new
    
    #��s�ǂݔ�΂�
    get_line(fp,0)

    #�t���[�����[�g
    work = get_line(fp,2)
    @scene.frame_rate = work[0]

    #�J����
    work = get_line(fp,2)
    @scene.camera_name = work[0]
   
    #�]�[��
    work = get_line(fp,2)
    @scene.zone_id = work[0]

    #BGM
    work = get_line(fp,2)
    @scene.bgm_no = work[0]

    #�Ίp
    work = get_line(fp,2)
    @scene.fovy_sin = work[0].to_f
    @scene.fovy_cos = work[1].to_f

    #�Ίp
    work = get_line(fp,2)
    @scene.near = work[0].to_f
    @scene.far = work[1].to_f
    
    #SwapBuffers
    work = get_line(fp,2)
    @scene.alpha_sort_mode = ("GX_" + work[0])
    @scene.buffer_mode = ("GX_" + work[1])

    #2��ʘA��
    work = get_line(fp,2)
    @scene.double_view_f = work[0]
    
    #���u�����f�B���O
    work = get_line(fp,2)
    
    #���e�X�g
    work = get_line(fp,2)
    
    #�A���`�G�C���A�X
    work = get_line(fp,2)
    
    #1dot�|���S���\��
    work = get_line(fp,2)

    #�t�H�O
    work = get_line(fp,2)
    @scene.fog_f = work[0]
    @scene.fog_rgba = get_rgba( work[1] )
    @scene.fog_shift = work[2]
    @scene.fog_offset = work[3]
    @scene.fog_mode = ("GX_"+work[4])

    #�t�H�O�Z�x�e�[�u��
    work = get_line(fp,2)
    @scene.fog_tbl = Array.new
    for i in 0..7 do
      @scene.fog_tbl << work[i]
    end

    #�G�b�W�}�[�L���O
    work = get_line(fp,2)
    @scene.edge_f = work[0]
    @scene.edge_rgb_tbl = Array.new
    for i in 0..7 do
      @scene.edge_rgb_tbl << get_rgb( work[1+i] )
    end
    
    #�N���A�J���[
    work = get_line(fp,2)
    @scene.clear_rgba = get_rgba( work[0] );
    
    #�t�F�[�h�C��
    work = get_line(fp,2)
    @scene.fadein_col = work[0];
    @scene.fadein_wait = work[1];
   
    #�t�F�[�h�A�E�g
    work = get_line(fp,2)
    @scene.fadeout_col = work[0];
    @scene.fadeout_wait = work[1];
  end

  #Unit�p�����[�^�擾
  def get_unit_param unit, work
    idx = $timezone_list.index(work[2])
    if idx == nil then
      print("Error! �^�C���]�[���w�肪�Ԉ���Ă��܂� -> #{work[2]}\n")
      exit 1
    end

    if work[3] == "" then
      print("Error! imd���͕K���w�肵�Ă�������\n")
      exit 1
    end
    unit.unit_tbl << $_unit_prm.new
    cp = unit.unit_tbl.last()

    cp.time_zone = idx
    cp.res_tbl = Array.new
    
    cp.res_tbl << (work[3]+"_nsbmd")

    for i in 0..4 do
      if work[4+i] == nil || work[4+i] == "" then next end
      cp.res_tbl << (work[4+i]+"_"+$nsx_list[i]) 
    end

    return idx
  end

  #Unit���X�g�擾
  def get_unit fp
    @unit = Array.new
    @unit_num = 0
    @unit_main_num = 0

    #��s�ǂݔ�΂�
    get_line(fp,0)
    
    while 1
      work = get_line(fp,0)
      if check_tag_block_end(work[0],"#UNIT_RESOURCE") == 1 then
        if @unit.size == 0 then
          print("Error! Unit��`������܂��� -> #{work[0]}\n�Œ�1��unit���`���Ă�������\n")
          exit 1
        end
        break
      end
      
      @unit << $_unit.new
      cp = @unit.last()
      cp.unit_name = work[1]

      cp.unit_tbl = Array.new
      cp.time_zone_f = work[2] == "TIMEZONE_NONE" ? 0 : 1
      if cp.time_zone_f == 0 then
        get_unit_param( cp, work )
      else
        for i in 0..4 do
          idx = get_unit_param( cp, work )
          if idx != i+1 then
            print("Error! idx = #{idx}, check = #{i+1} TIME_ZONE��MORNING�`MIDNIGHT�܂ł�5�S�Ă�������ƒ�`���Ă�������\n")
            exit 1
          end
          if i < 4 then
            work = get_line(fp,0)
          end
        end
      end
      @unit_num += 1
    end
  end

  #�V�[�����j�b�g��`�擾
  def get_scene_unit fp
    @scene_unit = Array.new
    @scene_unit_num = 0

    #��s�ǂݔ�΂�
    get_line(fp,0)
   
    print("\n�V�[�����j�b�g�����J�n\n")
    while 1
      work = get_line(fp,0)
      if check_tag_block_end(work[0],"#SCENE_UNIT") == 1 then
        break
      end
      
      @scene_unit << $_scene_unit.new
      cp = @scene_unit.last()
      cp.chg_type = work[1]
      cp.main_name = work[2]
      print(" Get scene unit -> #{work[2]}\n")
      cp.chg_tbl = Array.new
      for i in 0..9 do
        if work[2+i] == nil || work[2+i] == "" then break end
        cp.chg_tbl << work[2+i]
      end
      if work[2+10] != nil && work[2+10] != "" then
        print("Error! �w��ł���p�^�[����10�ȉ��ł�\n10�ȏ�̃p�^�[�����w�肵���ꍇ�A�v���O���}�ɂ����k��������\n")
        exit 1
      end

      @scene_unit_num += 1
    end
  end

  #�R�}���h��擾
  def get_command_list fp, com, tag
    #��s�ǂݔ�΂�
    get_line(fp,0)
    
    while 1
      work = get_line(fp,0)
      if check_tag_block_end(work[0], tag ) == 1 then
        break
      end
      com << $_command.new
      cp = com.last()

      cp.command = work[1]
      cp.frame = work[2]

      if cp.frame == nil then
        if cp.command == "END" then
          cp.frame = "0"
        else
          print("Error! �R�}���h�̃t���[�����w�肵�Ă�������->#{work[1]}\n")
          exit 1
        end
      elsif cp.frame == "INIT" then
        cp.frame = "DEMO3D_CMD_SYNC_INIT"
      end
      cp.param = Array.new
      for i in 0..3 do
        if work[3+i] == "" then break end
        cp.param << work[3+i]
      end
    end 
  end

  #RGB�擾
  def get_rgb buf
    buf.gsub!("\"","") 
    rgb  = buf.split(",")

    if rgb.size != 3 then
      print("Error! RGB�l�̎w�肪�s�� �t�H�[�}�b�g R,G,B -> #{buf}\n")
      exit 1
    end
    for val in rgb do
      n = val.to_i
      if n < 0 || n > 31 then
        print("Error! RGB�l�̎w�肪�s�� �l��0�`31�ł� -> #{buf}\n")
        exit 1
      end
    end
    return rgb;
  end
  
  #
  def get_rgba buf
    buf.gsub!("\"","") 
    rgba  = buf.split(",")
    if rgba.size != 4 then
      print("Error! RGBA�l�̎w�肪�s�� �t�H�[�}�b�g R,G,B,A -> #{buf}\n")
      exit 1
    end
    for val in rgba do
      n = val.to_i
      if n < 0 || n > 31 then
        print("Error! RGBA�l�̎w�肪�s�� �l��0�`31�ł� -> #{buf}\n")
        exit 1
      end
    end
    return rgba;
  end
 
  #���C���擾
  def get_line fp, idx
    line = fp.gets
    line.chomp!
    work = line.split("\t")
    size = work.size()
    if idx > size then
      idx = 0
    end
    return work.slice(idx..size)
  end

  #�^�O�u���b�N�`�F�b�N
  def check_tag_block_end key, s_tag
    e_tag = s_tag+"_END"
    if key == e_tag then
      print("EndTagGet #{e_tag} -> key = #{key}\n")
      return 1
    end
    if /^#.*/ =~ key then
      print("Error! �^�O " + s_tag + "�� " + e_tag + "�ŕ��Ă��܂���\n")
      exit 1
    end
    return 0
  end

  #�^�O�p�[�X
  def parse_tag fp, key
    ct = 0
    print("search tag = "+key+"\n")
    while line = fp.gets
      work = line.split("\t")
      if key == work[0] then
        print("search tag get_key = "+key+"\n")
        return
      end
      if (work[0] == "#END_SCENE") || (ct > 200) then
        break 
      end
      ct += 1
    end
    print("Error! �^�O " + key + " ��������܂���\n")
    exit 1
  end

  #�������A�I�u�W�F�N�g�쐬
  def initialize fpath
	  @num = 0
    @base_name = ""
    @scene_name = ""

    line_ct = 0

    @base_name = File::basename(fpath,'.txt')

    print("\nConvertStart " + fpath + "\n")
    fp = File.open(fpath,"r")
    if fp == nil then
      exit 1
    end
    #�V�[�����擾
    work = get_line(fp,1)
    @scene_name = work[1]
    print("�V�[�� " + @base_name + " ->" +@scene_name  + "�R���o�[�g�J�n\n")

    #�V�[���p�����[�^�擾�J�n
    parse_tag( fp, "#SCENE_PARAM")
    get_scene_param( fp )

    #���j�b�g���\�[�X�p�����[�^�擾
    parse_tag( fp, "#UNIT_RESOURCE")
    get_unit(fp)

    #�V�[�����j�b�g��`�擾
    parse_tag( fp, "#SCENE_UNIT")
    get_scene_unit(fp)

    #�R�}���h�p�����[�^�擾
    parse_tag( fp, "#COMMAND_PARAM")
    @com_tbl = Array.new
    get_command_list(fp, @com_tbl, "#COMMAND_PARAM")

    #�I���R�}���h�p�����[�^�擾
    parse_tag( fp, "#END_COMMAND_PARAM")
    @end_com_tbl = Array.new
    get_command_list(fp, @end_com_tbl, "#END_COMMAND_PARAM")
    
    #���X�g�I�[�擾
    parse_tag( fp, "#END_SCENE")
  end
end
#end class CDemo3DRes 

def convert
  c_demo3d = CDemo3DRes.new($input_path)
  c_demo3d.output($output_path)
end

convert
