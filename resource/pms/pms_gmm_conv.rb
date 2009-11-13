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
def make_row( gmm )
  row_name = sprintf("search_abc_%02d",1)
  row_data = "������"
  gmm.make_row_kanji( row_name, row_data, row_data )
end

#====================================================
# GMM �f���o��
#====================================================
def make_gmm( gmm_name )
  gmm = GMM::new
  gmm.open_gmm( GMM_SOURCE, DST_GMM_PATH + gmm_name )
  make_row( gmm )
  gmm.close_gmm
end


#====================================================
# �f�[�^�ϊ�
#====================================================

#====================================================
# ���\�[�X���當����𒊏o
#====================================================
def line_trim

  
end


$filenum = 0
$data = Array.new

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
          #���ڏo��
          $data[$filenum,row_idx] = line
          row_idx += 1;
          
        end
      elsif line.include?("u16 PMS_InitialTable_")
        #���ڌ���
        #puts line
        output_flag = 1
        row_idx = 0
      end
    }
  }
end



# �J�^�J�i���Ђ炪�Ȃ�
# ���_���O��


#====================================================
# �又�� �J�n
#====================================================

# �f�[�^���
load_resource

#$data = $data.sort_by{ |id,elem|
#  id.downcase
#}




# �o��

#TODO each�ɂ����� 
for i in 1..50
  name = "pms_abc" + sprintf("%02d",i) + ".gmm"
  make_gmm( name )
end







