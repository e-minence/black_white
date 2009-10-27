#===================================================================
#
# @brief  �V���b�v�f�[�^�R���o�[�^�[
#
# itemym.h��fs_item.csv��ǂݍ���ŃV���b�v�f�[�^���쐬���܂��B
#
# ex)
# ruby shopdata.rb fs_item.csv itemsym.h > fs_item.cdat
#
# @data   09.10.19
# @author mori
#
#===================================================================

# ITEMSYM.h�̒�`�擾���J�n����s
ITEMSYM_H_START_LINE = 10


# �����R�[�h�w��
$KCODE = 'SJIS'

#CSV�ǂݍ���
require "csv"

#���[�N������
hash       = Hash.new	# �ǂ������X�g
namelist   = Array.new	# �z��
maxlist    = Array.new	# �z��MAX��
line_count = 0



#---------------------------------------
# WB�A�C�e������`�t�@�C���I�[�v��
#---------------------------------------
file = File.open(ARGV[1])


file.each{|line|
	# 10�s�ڂ���ǂݍ���
	if line_count>ITEMSYM_H_START_LINE then
		if line.to_s.slice(/\/\/\s\w*/)!=nil then
			hash[line.to_s.slice(/\/\/\s\w*/).sub(/\/\/\s/,"")] = line.to_s.slice(/ITEM_\w*/)
		end
	end
	line_count = line_count+1
}

file.close

#---------------------------------------
# �V���b�v�f�[�^�t�@�C���I�[�v��
#---------------------------------------
reader = CSV.open( ARGV[0], 'r')

# 3�s�ǂݔ�΂�
reader.shift
reader.shift
reader.shift

# �t�@�C���w�b�_�o��
printf( "//====================================================================\n")
printf( "//    �V���b�v�f�[�^\n")
printf( "//\n")
printf( "//\n")
printf( "// ��fs_item.xls��resource/shopdata/�ŃR���o�[�g���ďo�͂��Ă��܂�\n")
printf( "//\n")
printf( "//====================================================================\n\n")

# CSV�f�[�^����o��
reader.each { |row|
	printf( "// %s\n", row[0] )
	printf( "static const u16 %s[] = {\n", row[1].to_s )
	namelist << row[1].to_s

	# �ǂ����f�[�^�o��
	(row.length-2).times{|i|
		if row[i+2]==nil then
			printf("\t\t0xffff,\n")
		else
			printf("\t\t%s,\n", hash[row[i+2].to_s])
		end
	}
	printf("};\n")
	printf("#define %s_MAX	(NELEMS(%s))\n\n", row[1].to_s.upcase,row[1].to_s)
	maxlist << sprintf( "%s_MAX",row[1].to_s.upcase,row[1].to_s )
}
reader.close

# �ꊇ�A�N�Z�X�p�ɔz��|�C���^�̔z����`
printf("\n\n// �ꊇ�A�N�Z�X�p�z���`\n")
printf("static const u16 *shop_data_table[]={\n")
namelist.each{ |name|
	printf("\t%s,\n", name)
}
printf("};\n")

# �e�V���b�v�̃A�C�e�����̔z��
printf("\n\n// �e�V���b�v�̔��ő吔�擾�p\n")
printf("static const u8 shop_itemnum_table[]={\n")
maxlist.each{ |maxdef|
	printf("\t%s,\n", maxdef)
}
printf("};\n")
