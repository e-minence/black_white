#!/usr/bin/ruby
#===================================================================
#
# @brief  WB�A�C�e�������X�g�����i50���\�[�g�p�f�[�^)
#
# @data   09.10.10
# @author genya_hosaka
#
#===================================================================

# �����R�[�h�w��
$KCODE = 'SJIS'

#CSV�ǂݍ���
require "csv"
# String �N���X�� tr ���\�b�h�œ��{����g�p�\�ɂ���B
require "jcode"

#===================================================================
# �ݒ�
#===================================================================
DST_FILENAME = "itemsort_def.h"  #�o�̓t�@�C����

DST_BRIEF = "50���\�[�g�p�̃f�[�^�e�[�u���ł��B"
DST_AUTOR = "genya_hosaka"
DST_NOTE = "���̃t�@�C����makelist_sort_by_abc.rb�ɂ���Ď����������ꂽ���̂ł��B"
DST_TBL_HEADER = "//�Y��:item_id \nstatic const u16 ItemSortByAbc[] ="

CSV_ROW_ID = 0 #ID�̗�
CSV_ROW_NAME = 2; #���O�f�[�^�̗�


#===================================================================
# �又��
#===================================================================
#�擾�f�[�^�ێ��p�n�b�V���e�[�u��
tbl = Hash.new
#id�Ə����\�[�g���ʂ����т���n�b�V���e�[�u��
prio = Hash.new

#----------------------------------------------------
# �ǂݍ���
#----------------------------------------------------

# CSV�I�[�v��
reader = CSV.open( 'wb_item.csv', 'r')

#��s�X�L�b�v
reader.shift

#��������o��
reader.each { |row|
  tbl[ row[ CSV_ROW_NAME ].to_s ] = row[ CSV_ROW_ID ].to_i
}

reader.close

# �����\�[�g
abc_sort = tbl.sort_by{|name, id| name.tr("�@-��","��-��") }

#id�ƃ\�[�g���ʂ����т����n�b�V���𐶐�
count = 0;
abc_sort.each{|name,id|
  prio[id] = count;
  count += 1;
}

#----------------------------------------------------
# �o��
#----------------------------------------------------
File.open( DST_FILENAME ,"w"){ |file|

 #�w�b�_�o��
file.puts("//=========================================================================");
file.puts("/**");
file.puts(" * @file " + DST_FILENAME);
file.puts(" * @brief" + DST_BRIEF);
file.puts(" * @autor" + DST_AUTOR);
file.puts(" * @note" + DST_NOTE);
file.puts(" */");
file.puts("//=========================================================================\n\n");
file.puts(DST_TBL_HEADER);

#NO���ɕ��ׂ�
no_sort = tbl.sort_by{|name, id| id }

#�f�[�^�e�[�u���o��
count = 0;
no_sort.each{|name,id|
  file.print("\t");
  file.print(prio[id]);
  file.print(",\t//No.");
  file.print(id);
  file.puts( " "+name )
  count += 1;
}

file.puts("}");

#��r�ł���悤�ɂ��Ă������߂ɂ��������������ʂ��o��
file.puts("\n/*\n\t *** 50�������X�g ***\n\n");

count=0;
abc_sort.each{|name,id|
  file.print("\t");
  file.print(count);
  file.print(",\t//No.");
  file.print(id);
  file.puts( " "+name )
  count += 1;
}
file.puts("*/");


}
