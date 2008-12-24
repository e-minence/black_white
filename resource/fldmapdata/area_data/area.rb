#------------------------------------------------------------------------------
#	//�G���A�Ǘ��\���J��
#	//�w�b�_�s�ǂݔ�΂�
#	//�G���A�R�[�h�A�z�u���f���f�[�^�A�e�N�X�`���Z�b�g�A�l���f�[�^�A���C�g�t���O�����[�h
#	//���ꂼ����x�N�^�o�^
#	//�o�^�̍ہA�o�^�C���f�b�N�X��Ԃ��悤�ɂ���
#	//�Ԃ��Ă����C���f�b�N�X���G���A�f�[�^�Ƃ��ĕۑ�
#	//���C�g�t���O�ۑ�
#	//�G���AID���J��Ԃ��A�I��������A�e�x�N�^���A�[�J�C�u�p�X�N���v�g�t�@�C���Ƃ��ďo��
#
#
#	2008.12.18	tamada	wb���ɈڐA�J�n
#
#------------------------------------------------------------------------------

load 'area_common.def'
require 'area_common.rb'	#COL_�`�̒�`�Ȃ�

#�z��ւ̃G���g��
def EntryVec(vec, item)
	#���łɂ��邩���ׂ�
	index = vec.index(item)
	if index == nil then
		#�o�^
		vec << item
		return (vec.length-1)
	else
		return index
	end
end
#�z��ւ̃G���g��(�w�蕶����͏Ȃ�)
def EntryVec2(vec, item, omit)
	if omit == item then
		return 0xffff	#�o�^�Ȃ��R�[�h
	end
	#���łɂ��邩���ׂ�
	index = vec.index(item)
	if index == nil then
		#�o�^
		vec << item
		return (vec.length-1)
	else
		return index
	end
end

#�����O�ݒ�擾
def GetInnerOuter(str)
	fix_str = str.chomp("\n").chomp("\r")
	if "OUT" == fix_str then
		return 1;
	elsif "IN" == fix_str then
		return 0;
	end
	#�w��s���̏ꍇ��0���Z�b�g
	return 0;
end

#���C�g�ݒ�擾
def Getlight(str)
	fix_str = str.chomp("\n").chomp("\r")
	if "ON" == fix_str then
		return 1;
	elsif "OFF" == fix_str then
		return 0;
	elsif "TOUDAI" == fix_str then
		return 2;
	end
	#�w��s���̏ꍇ��0���Z�b�g
	return 0;
end

#�g���q�ύX
def ChangeExtend(inStr, inExtStr)
	idx = inStr.index(".")+1
	last = inStr.length-1

	str = inStr.dup
	str[idx..last] = inExtStr
	return str
end

#�X�N���v�g�t�@�C���쐬
def MakeScript(vec, inFileName, inExt)
	script_file = File.open(inFileName,"w")
	vec.each do | item |
		file_name = ChangeExtend(item, inExt);
		script_file.printf("\"%s\"\n",file_name)
	end
end
def MakeDependFile(vec, inFileName, inExt, symbolName)
	dep_file = File.open(inFileName, "w")
	dep_file.printf("%s = \\\n", symbolName)
	vec.each do |item|
		file_name = ChangeExtend(item, inExt)
		dep_file.printf("\t%s \\\n", file_name)
	end
end

#�t�@�C���p�b�N���C�g
def FileWrite(bin_file, data, code)
	ary = Array(data)
	pack_str = ary.pack(code)
	bin_file.write(pack_str)
end

#------------------------------------------------------------------------------
#	�R���o�[�g�{��
#------------------------------------------------------------------------------

area_tbl_file = File.open(ARGV[0],"r")
area_id_h = File.open(TARGET_HEADER_FILENAME, "w")

area_id_h.printf("//���̃t�@�C���̓R���o�[�^�ɂ�萶������܂�\n")
area_id_h.printf("enum {\n")

area_vec =[]
build_vec = []
tex_vec = []
g_anm_vec = []
marge_tex_before = []

total_bin_file = File.open(TARGET_BIN_FILENAME, "wb")

#1�s�ǂݔ�΂�
#line = area_tbl_file.gets
read_through area_tbl_file

area_count = 0
while line = area_tbl_file.gets
	column = line.split "\t"

	#�G���A�h�c��
	area_id_h.printf("\t%-24s = %3d,\n",column[COL_AREANAME].upcase, area_count)
	area_count += 1

	#��������
	bin_file_name = "#{column[COL_AREANAME]}.bin".downcase
	#�G���g��
	EntryVec(area_vec, bin_file_name)
	#bin�t�@�C���쐬
	#�f�[�^��������
	data = EntryVec(build_vec,column[COL_BMNAME])	#���f��
	FileWrite(total_bin_file,data, "S")
	data = EntryVec(tex_vec,column[COL_TEXNAME])		#�e�N�X�`���Z�b�g
	FileWrite(total_bin_file,data, "S")
	data = EntryVec2(g_anm_vec,column[COL_ANMNAME],"none")	#�n�`�A�j���t�@�C��
	FileWrite(total_bin_file,data, "S")
	data = GetInnerOuter(column[COL_INOUT])			#INNER/OUTER
	FileWrite(total_bin_file,data, "C")
	data = Getlight(column[COL_LIGHTTYPE])				#���C�g
	FileWrite(total_bin_file,data, "C")

	#�}�[�W�O�e�N�X�`�������W
	EntryVec(marge_tex_before,column[COL_TEXPART1])	#�}�[�W�O�e�N�X�`��1
	EntryVec(marge_tex_before,column[COL_TEXPART2])	#�}�[�W�O�e�N�X�`��2
	#"dummy"�͏Ȃ�
	marge_tex_before.delete("dummy")
	
	
end

total_bin_file.close
#�e�C���쐬
area_id_h.printf("\tAREA_ID_MAX\n");
area_id_h.printf("};")

#	//�z�u
MakeScript(build_vec, "build_list.txt", "dat" );
MakeScript(build_vec, "build_xls_list.txt", "xls" );
#	//�n�`�A�j��
MakeScript(g_anm_vec, ARC_ANM_LIST_FILENAME, "nsbta" );



