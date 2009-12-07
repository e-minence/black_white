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


def check_ext(file)
  ext = file.slice(/...\z/m)
  
  return ( ext == "imd" || ext == "ica" || ext == "ita" ||
           ext == "ima" || ext == "itp" || ext == "iva" );
end


def print_scene(file, dir)
  #�t�H���_���̃t�@�C�����
  scene_dir = Dir::entries( dir )

  #�f��ID�̕�����𐶐�
  demoid = "DEMO3D_ID_" + dir.upcase

  # scene�w�b�_
  file.puts("//=========================================================================\n");
  file.puts("// " + demoid );
  file.puts("//=========================================================================\n");

  
  #1�I���W��
  cnt = 1; 
  loop do

    puts cnt

    is_find = false

    #�t�H���_���̃t�@�C�������J��
    scene_dir.each{|i|
      #�g���q�`�F�b�N
      if check_ext(i)
        # 2�����̐��l�`�F�b�N
        if i.slice(/\d\d/).to_i == cnt
        
          # ����̂݃w�b�_��������
          if is_find == false
            line = "static const GFL_G3D_UTIL_RES res_unit_" + sprintf("%02d",cnt) + "[] =\n";
            file.puts( line );
            file.puts("{\n");
          end
          
          puts i
          puts 
          line = "\t{ " + ARC_ID + ", NARC_demo3d_" + i.sub(/.i/,"_nsb") + ", GFL_G3D_RESARC },";
          file.puts line

          is_find = true          
        end
      end
    }
    if is_find == false
      break
    else
      file.puts("};\n");
      cnt += 1
    end
  end
end

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
    file.puts("#pragma once\n\n");
    file.puts("#include <gflib.h>\n");
    file.puts("#include \"arc/demo3d.naix\"\n\n");
    
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
    file.puts "static const DEMO3D_SETUP_DATA c_demo3d_setup_Data[ DEMO3D_ID_MAX ] = {"
    file.puts "\t{ 0, 0, 0 },"

    #�J�����g�ɂ���t�H���_������݂Ԃ�
    pwd =  Dir::entries( Dir::pwd )
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
