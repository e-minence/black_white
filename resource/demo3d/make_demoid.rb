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
    file.puts("#pragma once\n\n");

    file.puts "typedef enum {"
    file.puts "\tDEMO3D_ID_NULL = 0,\n\n"

    #�J�����g�ɂ���t�H���_������݂Ԃ�
    pwd =  Dir::entries( Dir::pwd )
    for i in pwd
      if File::ftype( i ) == "directory" 
        # ����p�X�͔r��
        if i != "." && i != ".." && i != ".svn"
          file.puts "\tDEMO3D_ID_" + i.upcase + ","
        end
      end
    end
    
    file.puts "\n\tDEMO3D_ID_MAX,"
    
    file.puts "} DEMO3D_ID;"

}
