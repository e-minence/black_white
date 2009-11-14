#!/usr/bin/ruby
#===================================================================
#
# @brief  �ȈՉ�b50��GMM �f�[�^����
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
# GMM�����N���X
require File.dirname(__FILE__) + '/../../tools/gmm_make/gmm_make'

#====================================================
# �萔�錾
#====================================================
GMM_SOURCE = "../message/template.gmm"
SRC_RESOURCE = "pms_input.res"
DST_GMM_PATH = "../message/src/pms/abc/"

#====================================================
# GMM �s��������
#====================================================
def make_row( gmm, data )
  cnt  =0;
  
  data.each{|i|
    #����������甲����
    if(i == nil )
      break
    end
    
    row_name = sprintf("search_abc_%02d", cnt)
    row_data = i
    gmm.make_row_kanji( row_name, row_data, row_data )
    
    cnt += 1
  }
end

#====================================================
# GMM �f���o��
#====================================================
def make_gmm( gmm_name, data )
  gmm = GMM::new
  gmm.open_gmm( GMM_SOURCE, DST_GMM_PATH + gmm_name )
  make_row( gmm, data )
  gmm.close_gmm
end

#====================================================
# ���\�[�X���當����𒊏o
#====================================================
def str_trim( str )
  start = str.index("//") + 2
  finish = str.index("\n") - 1

  str = str[start..finish]

  # �J�^�J�i���Ђ炪�Ȃ�
  # ���_���O��
  origin = "��-���`-�y�@-���O-�X�����������������������������������Âłǂ΂тԂׂڂς҂Ղ؂ۂ������@�B�D�F�H�K�M�O�Q�S�U�W�Y�[�]�_�a�d�f�h�o�r�u�x�{�p�s�v�y�|�������b��"
  normalize = "a-zA-Z��-��0-9�����������������������������������ĂƂ͂Ђӂւق͂Ђӂւق���킠���������������������������������ĂƂ͂Ђӂւق͂Ђӂւق����"
  str = str.tr(origin,normalize);

  return str
end


#�������z��
def make_nm_array(n,m)
  (0...n).map { Array.new(m) }
end

$filenum = 0
$data = make_nm_array( 50, 200 )

#====================================================
# ���\�[�X �ǂݍ���
#====================================================
def load_resource
  output_flag = 0
  row_idx = 0
  
  File::open( SRC_RESOURCE ) {|f|
    f.each { |line|
      if output_flag == 1
        if line.include?("PMS_WORDID_END")
          #�o�͏I������
          output_flag = 0
          $filenum += 1
        else
          #�]���ȕ����������
          line = str_trim( line )
          #���ڏo��
          $data[$filenum][row_idx] = line
          #�v�f��i�߂�
          row_idx += 1;
          
        end
      elsif line.include?("u16 PMS_InitialTable_")
        #���ڌ���
        output_flag = 1
        row_idx = 0
      end
    }
  }
end

#====================================================
# �又�� �J�n
#====================================================

# �f�[�^���
load_resource

# GMM�o��
cnt = 0
$data.each{|data|
  #����������甲����
  if(data[0] == nil )
    break
  end

  name = "pms_abc" + sprintf("%02d",cnt) + ".gmm"
  make_gmm( name, data )
  
  cnt += 1
}

#�w�b�_�t�@�C���o��
DST_FILENAME = "pms_abc_gmm_def.h"  #�o�̓t�@�C����
DST_BRIEF = "50�����֗p�̃f�[�^�e�[�u���ł��B"
DST_AUTOR = "genya_hosaka"
DST_NOTE = "���̃t�@�C����pms_gmm_conv.rb�ɂ���Ď����������ꂽ���̂ł��B"
DST_MAX_DEF = "PMS_ABC_GMMTBL_MAX"

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
    file.puts("#include \"message.naix\"\n\n");

    file.puts("#define " + DST_MAX_DEF + " (" + $filenum.to_s + ") // �ő吔\n\n" );

    file.puts("static const u16 pms_abc_gmmtbl[ " + DST_MAX_DEF + " ] = {")
    
    #�f�[�^�e�[�u���o��

    for cnt in 0..$filenum-1
      line = "\tNARC_message_pms_abc" + sprintf("%02d",cnt) + "_dat,";
      file.puts( line );
    end
    
    file.puts("};");
    
}
