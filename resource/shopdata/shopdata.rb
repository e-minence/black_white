#===================================================================
#
# @brief  �V���b�v�f�[�^�R���o�[�^�[
#
# itemym.h��fs_item.csv��ǂݍ���ŃV���b�v�f�[�^���쐬���܂��B
#
# ex)
# ruby shopdata.rb fs_item.csv itemsym.h > fs_item.cdat fs_item_index.h
#
# @data   09.10.19
# @author mori
#
#===================================================================

# ITEMSYM.h�̒�`�擾���J�n����s
ITEMSYM_H_START_LINE = 10
FIXSHOP_START_LINE	 = 7

# �����R�[�h�w��
$KCODE = 'SJIS'

#CSV�ǂݍ���
require "csv"

#���[�N������
hash       = Hash.new	# �ǂ������X�g
namelist   = Array.new	# �z��
maxlist    = Array.new	# �z��MAX��
outbuf	   = ""			# �o�̓V���b�v�f�[�^�Q
headerbuf  = ""			# �X�N���v�g�Ŏw�肷�邽�߂̃w�b�_
line_count = 0
shop_count = 0

#STDERR.print("�G���[\n")
#exit(1)


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

# �V���b�v�f�[�^�w�b�_�����o��
outbuf += sprintf( "//====================================================================\n")
outbuf += sprintf( "// @file  %s\n", ARGV[2])
outbuf += sprintf( "// @brief �V���b�v�f�[�^\n")
outbuf += sprintf( "//\n")
outbuf += sprintf( "// ��fs_item.xls��resource/shopdata/�ŃR���o�[�g���ďo�͂��Ă��܂�\n")
outbuf += sprintf( "//\n")
outbuf += sprintf( "//====================================================================\n\n")

# �w�b�_�t�@�C���w�b�_�����o��
headerbuf += sprintf( "//====================================================================\n")
headerbuf += sprintf( "// @file  %s\n",ARGV[3])
headerbuf += sprintf( "// @brief �V���b�v�f�[�^�w��p�w�b�_\n")
headerbuf += sprintf( "//\n")
headerbuf += sprintf( "// ��fs_item.xls��resource/shopdata/�ŃR���o�[�g���ďo�͂��Ă��܂�\n")
headerbuf += sprintf( "//\n")
headerbuf += sprintf( "//====================================================================\n\n")
headerbuf += sprintf( "#ifndef __%s__\n",   ARGV[3].to_s.upcase.tr(".","_"))
headerbuf += sprintf( "#define __%s__\n\n", ARGV[3].to_s.upcase.tr(".","_"))


# CSV�f�[�^����o��
reader.each { |row|
	if row[0].to_s=="" then
		break
	end
	outbuf += sprintf( "// %s\n", row[0] )
	outbuf += sprintf( "static const u16 %s[] = {\n", row[1].to_s )
	namelist << row[1].to_s

	# �ǂ����f�[�^�o��
	(row.length-2).times{|i|
		if row[i+2]!=nil then
			if hash[row[i+2].to_s]!=nil then
				outbuf += sprintf("\t\t%s,\n", hash[row[i+2].to_s])
			else
				STDERR.print( ""+row[0]+"�̍s�A�u"+row[i+2].to_s+"�v�ƌ����ǂ����͑��݂��܂���\n")
				exit(1)
			end
		end
	}
	outbuf += sprintf("};\n")
	outbuf += sprintf("#define %s_MAX	(NELEMS(%s))\n\n", row[1].to_s.upcase,row[1].to_s)
	maxlist << sprintf( "%s_MAX",row[1].to_s.upcase,row[1].to_s )
	shop_count=shop_count+1

	if shop_count>=FIXSHOP_START_LINE then
		headerbuf += sprintf("#define %s    ( %d )  // %s\n", row[1].to_s.upcase,
		shop_count-FIXSHOP_START_LINE,row[0].to_s)
	end
}
reader.close

# �ꊇ�A�N�Z�X�p�ɔz��|�C���^�̔z����`
outbuf += sprintf("\n\n// �ꊇ�A�N�Z�X�p�z���`\n")
outbuf += sprintf("static const u16 *shop_data_table[]={\n")
namelist.each{ |name|
	outbuf += sprintf("\t%s,\n", name)
}
outbuf += sprintf("};\n")


# �w�b�_�t�@�C���̏I�[����
headerbuf += sprintf( "\n\n#endif\n" )

# �e�V���b�v�̃A�C�e�����̔z��
outbuf += sprintf("\n\n// �e�V���b�v�̔��ő吔�擾�p\n")
outbuf += sprintf("static const u8 shop_itemnum_table[]={\n")
maxlist.each{ |maxdef|
	outbuf += sprintf("\t%s,\n", maxdef)
}
outbuf += sprintf("};\n")


#���߂��V���v�f�[�^���o��(.cdat)
file = File.open(ARGV[2],"w")
file.printf(outbuf)
file.close

#���߂��w�b�_�f�[�^���o��(.h)
file = File.open(ARGV[3],"w")
file.printf(headerbuf)
file.close

exit(0)