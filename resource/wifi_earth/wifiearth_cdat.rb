#--------------------------------------------------------------------------
# CountryList.xls�̂T�O�����̃��X�g����S���E�p�̓o�^�\�����X�g��
# .cdat�t�@�C���ŏo�͂���R���o�[�^�[
#
# 2010.06.02 by mori
#--------------------------------------------------------------------------
require "csv"
require 'jcode'
$KCODE = "SJIS"

countrylist=[]	# ���ԍ���ۑ����Ă����z��
countryname=[]	# ������ۑ����Ă����z��

#�����`�F�b�N
if (ARGV[0]==nil||ARGV[1]==nil) then
	printf("�������w�肵�Ă�������\nruby sort_listmake.rb <csv�t�@�C��> <�o��cdat�t�@�C��>\n")
	exit
end

#csv�t�@�C���I�[�v��
csv = CSV.open(ARGV[0],"r")

# ����gmm���X�g�̏W�v�B���������Ђ炪�ȉ����Đ擪�̕��������擾����
csv.each{|row|
	if (row[0].to_a.index("Floor")==nil) then
		countrylist << row[2].to_a
		countryname << row[1].to_a
	end
}

File.open(ARGV[1],"w"){|file|

file.printf("//-----------------------------------------------------------------\n")
file.printf("/*\n")
file.printf("/* @file %s\n",ARGV[1])
file.printf(" */\n")
file.printf("//-----------------------------------------------------------------\n")
file.printf("#ifndef __%s__\n",ARGV[1].tr("a-z\.","A-Z_"))
file.printf("#define __%s__\n",ARGV[1].tr("a-z\.","A-Z_"))

file.printf("\n/// �o�^�\�����X�g�i�S�Ă̍��ł͂Ȃ����Ƃɒ��Ӂj���₵�����ꍇ��
///CountryList.xls�̂T�O�����̗���ύX���Ă��������B
ALIGN4 static const int EnableCountryList[] = {\n");
countrylist.zip(countryname).each{|index,name|
	file.printf("\t%s,\t// %s\n",index, name)
}
file.printf("};\n\n")

file.printf("static const COUNTRY_ENABLE_MAX	= NELEMS(EnableCountryList);\n")


file.printf("#endif")
}