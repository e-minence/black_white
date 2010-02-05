#===================================================================
#
# @brief  BP�V���b�v�f�[�^�R���o�[�^�[
#
# itemym.h��bp_item.csv��ǂݍ����BP�V���b�v�f�[�^���쐬���܂��B
#
# ex)
# ruby bpshopdata.rb bp_item.csv itemsym.h > bp_item.cdat bp_item_inde.h
#
# @data   10.02.05
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
lastnamebuf = ""		# �V���b�v��`������񕪊o���Ă���
line_count = 0
shop_count = 0
closeflag  = 0			# ���񂾂��͔z���`������K�v�������̂Ńt���O



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

# 1�s�ǂݔ�΂�
reader.shift

# �V���b�v�f�[�^�w�b�_�����o��
outbuf += sprintf( "//====================================================================\n")
outbuf += sprintf( "// @file  %s\n", ARGV[2])
outbuf += sprintf( "// @brief BP�V���b�v�f�[�^\n")
outbuf += sprintf( "//\n")
outbuf += sprintf( "// ��bp_item.xls��resource/shopdata/�ŃR���o�[�g���ďo�͂��Ă��܂�\n")
outbuf += sprintf( "//\n")
outbuf += sprintf( "//====================================================================\n\n")

# �w�b�_�t�@�C���w�b�_�����o��
headerbuf += sprintf( "//====================================================================\n")
headerbuf += sprintf( "// @file  %s\n",ARGV[3])
headerbuf += sprintf( "// @brief BP�V���b�v�f�[�^�w��p�w�b�_\n")
headerbuf += sprintf( "//\n")
headerbuf += sprintf( "// ��bp_item.xls��resource/shopdata/�ŃR���o�[�g���ďo�͂��Ă��܂�\n")
headerbuf += sprintf( "//\n")
headerbuf += sprintf( "//====================================================================\n\n")
headerbuf += sprintf( "#ifndef __%s__\n",   ARGV[3].to_s.upcase.tr(".","_"))
headerbuf += sprintf( "#define __%s__\n\n", ARGV[3].to_s.upcase.tr(".","_"))


# CSV�f�[�^����o��
reader.each { |row|
	# 1��ڂɖ��O�������Ă���ꍇ��BP�V���b�v�f�[�^��`�J�n
	if row[0].to_s!="" then
	
		#�O�̍s�܂ł̃f�[�^��`���I������
		if closeflag==1 then
#			outbuf += sprintf("\t\t{0xffff, 0xffff, },\n")
			outbuf += sprintf("};\n")
			outbuf += sprintf("#define %s_MAX	(NELEMS(%s))\n\n", lastnamebuf.upcase,lastnamebuf)
			maxlist << sprintf( "%s_MAX",lastnamebuf.upcase,lastnamebuf )
		end

		#�V�����f�[�^��`�̐錾
		outbuf += sprintf( "// %s\n", row[0] )
		outbuf += sprintf( "static const SHOP_ITEM %s[][2] = {\n", row[1].to_s )
		namelist << row[1].to_s
		closeflag = 1
		lastnamebuf = row[1].to_s
		headerbuf += sprintf("#define %s    ( %d )  // %s\n", 
							 row[1].to_s.upcase,shop_count,row[0].to_s)
		shop_count=shop_count+1
					     		

	end

	if row[3].to_s!="" then
		if row[4].to_s=="" then
			STDERR.print( ""+row[3]+"�̉��ɒl�i�������Ă���܂���\n")
			exit(1)
		end
		if hash[row[3].to_s]!=nil then
			outbuf += sprintf("\t\t{ %s,\t%s, },\n", hash[row[3].to_s],row[4].to_s)
		else
			STDERR.print( "�u"+row[3].to_s+"�v�ƌ����ǂ����͑��݂��܂���\n")
			exit(1)
		end

		
	end
}
reader.close

#�Ō�̒�`�͏I�[�������̂ŃR�R�Ŋ���
#outbuf += sprintf("\t\t{0xffff, 0xffff, },\n")
outbuf += sprintf("};\n")
outbuf += sprintf("#define %s_MAX	(NELEMS(%s))\n\n", lastnamebuf.upcase,lastnamebuf)
maxlist << sprintf( "%s_MAX",lastnamebuf.upcase,lastnamebuf )


# �ꊇ�A�N�Z�X�p�ɔz��|�C���^�̔z����`
outbuf += sprintf("\n\n// �ꊇ�A�N�Z�X�p�z���`\n")
outbuf += sprintf("static const SHOP_ITEM *bp_shop_data_table[]={\n")
namelist.each{ |name|
	outbuf += sprintf("\t%s,\n", name)
}
outbuf += sprintf("};\n")


# �w�b�_�t�@�C���̏I�[����
headerbuf += sprintf( "\n\n#endif\n" )

# �e�V���b�v�̃A�C�e�����̔z��
outbuf += sprintf("\n\n// �e�V���b�v�̔��ő吔�擾�p\n")
outbuf += sprintf("static const u8 bp_shop_itemnum_table[]={\n")
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