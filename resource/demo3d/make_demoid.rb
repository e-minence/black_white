#!/usr/bin/ruby
#===================================================================
#
# @brief  �f���Đ��A�v���p�f��ID����
#
# @data   09.12.04
# @author genya_hosaka
#
#===================================================================

#====================================================
# �ݒ�
#====================================================
# �����R�[�h�w��
$KCODE = 'SJIS'

#====================================================
# �萔�錾
#====================================================
#�w�b�_�t�@�C���o��
DST_FILENAME = "demo3d_demoid.h"  #�o�̓t�@�C����
DST_BRIEF = "�f���Đ��A�v���Ŏg���f��ID�ł��Bresource\demo3d���̃t�H���_���玩���������Ă��܂��B"
DST_AUTOR = "genya_hosaka"
DST_NOTE = "���̃t�@�C����demo3d/make_demoid.rb�ɂ���Ď����������ꂽ���̂ł��B"

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
    file.puts("#ifndef __DEMO3D_DEMOID_H__\n");
    file.puts("#define __DEMO3D_DEMOID_H__\n\n");

    file.puts "#define DEMO3D_ID_NULL (0)\n\n"

    #�J�����g�ɂ���t�@�C�������X�g�A�b�v
    pwd =  Dir::entries( Dir::pwd )

    # �z��
    val = pwd.to_a;
    # �t�H���_�Ɍ��I�A����t�H���_��r��
    val = val.select{ |i| File::ftype(i) == "directory" && i != "." && i != ".." && i != ".svn" }

    #���ׂă`�F�b�N
    count = 1
    for i in val
      #imd�`�F�b�N > imd���t�H���_���Ɉ���Ȃ��ꍇ��ID�𐶐����Ȃ�
      imd = Dir::entries(i).find{|elem| elem.slice(/...\z/m) == "imd" }
      if imd != nil
        file.puts "#define DEMO3D_ID_" + i.upcase + " (" + count.to_s + ")" 
        count += 1
      end
    end
    
    file.puts "\n#define DEMO3D_ID_MAX (" + count.to_s + ")\n\n"
    
    file.puts("#endif // __DEMO3D_DEMOID_H__");

}
