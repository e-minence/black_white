#!/usr/bin/ruby
#===================================================================
#
# @brief  �f���Đ��A�v��
#
# @data   09.10.10
# @author genya_hosaka
#
#===================================================================

#====================================================
# �ݒ�
#====================================================
# �����R�[�h�w��
$KCODE = 'SJIS'
# String �N���X�� tr ���\�b�h�œ��{����g�p�\�ɂ���B
require "jcode"

#====================================================
# �萔�錾
#====================================================
#�w�b�_�t�@�C���o��
DST_FILENAME = "demo3d_resdata.cdat"  #�o�̓t�@�C����
DST_BRIEF = "�R���o�[�g���ɂ���f�[�^����f���Đ��Ɏg���e�[�u���𐶐����܂��B"
DST_AUTOR = "genya_hosaka"
DST_NOTE = "���̃t�@�C����demo3d/make_header.rb�ɂ���Ď����������ꂽ���̂ł��B"
ARC_ID = "ARCID_DEMO3D_GRA"

#�g���q�`�F�b�N
def check_ext(file)
  ext = file.slice(/...\z/m)
  
  return ( ext == "imd" || ext == "ica" || ext == "ita" ||
           ext == "ima" || ext == "itp" || ext == "iva" );
end

#setup�e�[�u�������o��
def print_setup(file,dir,cnt)
  file.puts "//setup"
  file.puts "static const GFL_G3D_UTIL_SETUP " + dir + "_setup[] = {"
  for i in 1..cnt-1
    str_unit = dir + "_unit" + sprintf("%02d",i);
    file.puts "\t{ res_" + str_unit + ", NELEMS(res_" + str_unit + "), obj_" + str_unit + ", NELEMS(obj_" + str_unit +  ") },"
  end
  file.puts "};"
end

#anime�e�[�u�������o��
def print_anime(file,cnt,anm_cnt)
    file.puts("//ANM");
    
    file.puts "static const GFL_G3D_UTIL_ANM anm_table_unit" + sprintf("%02d",cnt) + "[] = {";
    
    for k in 1..anm_cnt-1
        file.puts "\t{ " + k.to_s + ", 0 },"
    end
    
    file.puts("};\n");
end

#obj�e�[�u�������o��
def print_obj(file,dir,cnt,is_anm_tbl)
    file.puts("//OBJ");
    
    str_unit_num = sprintf("%02d",cnt);
                
    file.puts "static const GFL_G3D_UTIL_OBJ obj_" + dir + "_unit" + str_unit_num + "[] = {";
    file.puts "\t{"
    file.puts "\t\t0, 0, 0, // MdlResID, MdlDataID, TexResID"
    if is_anm_tbl == true
      file.puts "\t\tanm_table_unit" + str_unit_num + ", NELEMS(anm_table_unit" + str_unit_num + "), // AnmTbl, AnmTblNum"
    else
      file.puts "\t\tNULL, 0, // AnmTbl, AnmTblNum"
    end
    file.puts "\t}"
    file.puts("};\n");
end


#�V�[������(�t�H���_���̃f�[�^)�������o��
def print_scene(file, dir)
  #�t�H���_���̃t�@�C�����
  scene_dir = Dir::entries( dir )

  #�f��ID�̕�����𐶐�
  demoid = "DEMO3D_ID_" + dir.upcase

  # scene�w�b�_
  file.puts("//=========================================================================\n");
  file.puts("// " + demoid );
  file.puts("//=========================================================================\n");
  
  puts " > " + dir

  #1�I���W��
  cnt = 1; 
  loop do
    is_find = false

    #�t�H���_���̃t�@�C�������J��
    anm_cnt = 0;
    scene_dir.each{|i|
      #�g���q�`�F�b�N
      if check_ext(i)
        # 2�����̐��l�`�F�b�N
        if i.slice(/\d\d/).to_i == cnt
        
          # ����̂݃w�b�_��������
          if is_find == false
            file.puts "//UNIT"
            file.puts "static const GFL_G3D_UTIL_RES res_" + dir + "_unit" + sprintf("%02d",cnt) + "[] = {\n";
          end
          
          line = "\t{ " + ARC_ID + ", NARC_demo3d_" + i.sub(/.i/,"_nsb") + ", GFL_G3D_UTIL_RESARC },";
          anm_cnt += 1
          file.puts line

          is_find = true          
        end
      end
    }
    # �I������
    if is_find == false
      print_setup(file,dir,cnt);
      break
    else
      # �t�b�_��������
      file.puts("};\n");
      
      # �A�j������
      if anm_cnt <= 1
        print_obj(file,dir,cnt,false);
      else
        #�A�j���e�[�u�������o��
        print_anime(file,cnt,anm_cnt)
        print_obj(file,dir,cnt,true);
      end

      # ���̃��j�b�g��
      cnt += 1
    end
  end
end

# �A�N�Z�X�e�[�u���̍��ڂ������o��
def print_access_table(file,dir)
  #�t�H���_���̃t�@�C�����
  scene_dir = Dir::entries( dir )
  #�t�H���_���̃t�@�C�������J��
  scene_dir.each{|i|
    #�g���q�`�F�b�N
    if check_ext(i)
      # 2�����̐��l�`�F�b�N
      if i.slice(/camera/) == "camera"
          line = "\t{ " + dir + "_setup, NELEMS(" + dir + "_setup), NARC_demo3d_" + i.sub(/.ica/,"_bin") + " },";
          file.puts line        
      end
    end
  }
end


#====================================================
# �又�� �J�n
#====================================================

File::open( DST_FILENAME ,"w"){ |file|
    file.puts("//=========================================================================");
    file.puts("/**");
    file.puts(" * @file\t" + DST_FILENAME);
    file.puts(" * @brief\t" + DST_BRIEF);
    file.puts(" * @autor\t" + DST_AUTOR);
    file.puts(" * @note\t" + DST_NOTE);
    file.puts(" */");
    file.puts("//=========================================================================\n");
#    file.puts("#pragma once\n\n");
#    file.puts("#include <gflib.h>\n");
#    file.puts("#include \"arc/demo3d.naix\"\n\n");
    
    #�J�����g�ɂ���t�H���_������݂Ԃ�
    pwd =  Dir::entries( Dir::pwd )
    for i in pwd
      if File::ftype( i ) == "directory" 
        # ����p�X�͔r��
        if i != "." && i != ".." && i != ".svn"
          print_scene(file,i)
        end
      end
    end

    file.puts("\n\n");

    file.puts("//=========================================================================\n");
    file.puts("// �A�N�Z�X�e�[�u��" );
    file.puts("//=========================================================================\n");
    file.puts "static const DEMO3D_SETUP_DATA c_demo3d_setup_data[ DEMO3D_ID_MAX ] = {"
    file.puts "\t{ 0, 0, 0 },"

    #�J�����g�ɂ���t�H���_������݂Ԃ�
    pwd = Dir::entries( Dir::pwd )
    for i in pwd
      if File::ftype( i ) == "directory" 
        # ����p�X�͔r��
        if i != "." && i != ".." && i != ".svn"
          print_access_table(file,i)
        end
      end
    end
  file.puts "};"

};
