#==================================================
# sortlist_make.rb
# �G�N�Z���t�@�C������擾�����\�[�g�ύ�����
# GTS�Ŏg�p����cdat�t�@�C���ɕϊ�����X�N���v�g
#
# 2010/05/07 mori
#
# �����P�F�Q�Ƃ���CSV�t�@�C��
# �����Q�F�o�͂���cdat�t�@�C����
#
# �g�p��j
# ruby sortlist_make.rb aiueo.txt gts_coutrylist.cdat
#
#==================================================
require "csv"
require 'jcode'
$KCODE = "SJIS"

tmpstr=[]		#�ꎞ�����p�z��
coutrylist=[]	#gmm��`��ۑ����Ă����z��
headlist=[]		#������������ۑ����Ă����z��
headcount=Array.new(50,0)	# �������o��񐔂𐔂���z��
headstart=Array.new(50,0)	# headcount�̐��l�𑫂�����Ŏw��̓������̃X�^�[�g�ʒu���擾����z��
searchstr='�����������������������������������ĂƂȂɂʂ˂̂͂Ђӂւق܂݂ނ߂�����������'
dakustr='�������������������������Âłǂς҂Ղ؂ۂ΂тԂׂ�'	#���_�E�����_���O�����߂�
kanastr='�������������������������ĂƂ͂Ђӂւق͂Ђӂւ�'	#�u���p������

#�����`�F�b�N
if (ARGV[0]==nil||ARGV[1]==nil) then
	printf("�������w�肵�Ă�������\nruby sort_listmake.rb <csv�t�@�C��> <�o��cdat�t�@�C��>\n")
	exit
end

#csv�t�@�C���I�[�v��
csv = CSV.open(ARGV[0],"r")

# ����gmm���X�g�̏W�v�B���������Ђ炪�ȉ����Đ擪�̕��������擾����
csv.each{|row|
	# ��s���uID�v�̍s�͏o�͂��Ȃ��悤�ɂ���
	if (row[0].to_a.index("Floor")==nil) then
		coutrylist << row[2].to_a
		tmpstr = row[1].tr('�@-��', '��-��').tr(dakustr,kanastr)
		headlist<<tmpstr.split(//)[0]
#		printf("%s, %s\n", tmpstr.to_a, row[2].to_a)
	end
}

# �������̏o���񐔂𐔂��グ��
headlist.each{|head|
	a = searchstr.index(head)/2+1
	headcount[a] = headcount[a]+1
#	printf("%d,\n",searchstr.index(head))
}

# �����グ���������̏o���񐔂��X�^�[�g�ʒu�ɕϊ�����
50.times{|i|
	0..i.times{|n|
		headstart[i] = headstart[i]+headcount[n];
	}
}

#----------------------------------------------
# .cdat�Ƀt�@�C���o��
#----------------------------------------------

File.open(ARGV[1],"w"){|file|

file.printf("//-----------------------------------------------------------------\n")
file.printf("/*\n")
file.printf("/* @file %s\n",ARGV[1])
file.printf(" */\n")
file.printf("//-----------------------------------------------------------------\n")
file.printf("#ifndef __%s__\n",ARGV[1].tr("a-z\.","A-Z_"))
file.printf("#define __%s__\n",ARGV[1].tr("a-z\.","A-Z_"))

file.printf("\n///�������F50�������X�g
ALIGN4 static const u16 CountryListTbl[] = {\n");
coutrylist.each{|name|
	file.printf("\t%s,\n",name)
}
file.printf("};\nconst u32 CountryListTblNum = NELEMS(CountryListTbl);\n")

searchstr = searchstr.split(//)

file.printf("\n// �A�C�E�G�I���ō���CountryList�����Ԗڂɏo�����邩��
// �i�[���Ă��郊�X�g
// {  CountryList�̓Y����,   �A�C�E�G�I�̓Y�����i�A=0,�J=5,��=35,��=38)
static const u8 CountrySortList[][2]={\n")

50.times{|i|
	if headcount[i]!=0 then
		file.printf("\t{ %2d, %2d }, // %s\n", headstart[i], i, searchstr[i-1])
	end
}
file.printf("\t{ %3d, %3d }, // �ԕ�\n", headstart[49], 255)

file.printf("};\n\n")

file.printf("static const COUNTRY_SORT_HEAD_MAX	= NELEMS(CountrySortList);\n")

file.printf("#endif")
}