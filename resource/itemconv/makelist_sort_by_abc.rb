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
OUTPUT_ARC = 0    # 0=�w�b�_�o��, 1=ARC�ŏo��(u16���Ƃɐ��f�[�^)

if OUTPUT_ARC == 1
  DST_ARCNAME = "itemsort_abc.dat"
else
  DST_FILENAME = "itemsort_abc_def.h"  #�o�̓t�@�C����
  DST_BRIEF = "50�����֗p�̃f�[�^�e�[�u���ł��B"
  DST_AUTOR = "genya_hosaka"
  DST_NOTE = "���̃t�@�C����makelist_sort_by_abc.rb�ɂ���Ď����������ꂽ���̂ł��B"
  DST_TBL_HEADER = "//50�������̗D��x �Y��:item_id \nstatic const u16 ItemSortByAbc[] ="
end

CSV_ROW_ID = 0 #ID�̗�
CSV_ROW_NAME = 2; #���O�f�[�^�̗�

#===================================================================
# �又��
#===================================================================
#----------------------------------------------------
# �O���[�o���ϐ�
#----------------------------------------------------
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

#CSV �N���[�Y
reader.close

# �����\�[�g
# "�@-��"��"��-��"�Ƃ��Ĉ���
abc_sort = tbl.sort_by{|name, id| name.tr("�@-��","��-��") }

#id�ƃ\�[�g���ʂ����т����n�b�V���𐶐�
count = 0;
abc_sort.each{|name,id|
  prio[id] = count;
  count += 1;
}

#NO���ɕ��ׂ�
no_sort = tbl.sort_by{|name, id| id }

#----------------------------------------------------
# �o��
#----------------------------------------------------

if OUTPUT_ARC == 1
  #���f�[�^�ŏo��
  dstFile = File.open( DST_ARCNAME, "wb" ); 
  #�f�[�^�e�[�u���o��
  no_sort.each{ |name,id|
    dstFile.write( [prio[id]].pack("S") );
  }
  dstFile.close;
else
  #�w�b�_�t�@�C���ŏo��
  File.open( DST_FILENAME ,"w"){ |file|

    #�w�b�_�o��
    file.puts("//=========================================================================");
    file.puts("/**");
    file.puts(" * @file\t" + DST_FILENAME);
    file.puts(" * @brief\t" + DST_BRIEF);
    file.puts(" * @autor\t" + DST_AUTOR);
    file.puts(" * @note\t" + DST_NOTE);
    file.puts(" */");
    file.puts("//=========================================================================\n\n");
    file.puts(DST_TBL_HEADER);

    #�f�[�^�e�[�u���o��
    no_sort.each{|name,id|
      file.print("\t");
      file.print(prio[id]);
      file.print(",\t//No.");
      file.print(id);
      file.puts( " "+name )
    }

    file.puts("}");

    #��r�ł���悤�ɂ��Ă������߂ɂ��������������ʂ��o��
    file.puts("\n/*\n\t *** 50�������X�g ***\n\n");

    count=0;
    abc_sort.each{|name,id|
      file.print("  ");
      file.print(count);
      file.print("  ItemNo.");
      file.print(id);
      file.puts( "  "+name )
      count += 1;
    }
    file.puts("*/");
  }

end



