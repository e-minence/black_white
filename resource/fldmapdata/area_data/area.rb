#	//�G���A�Ǘ��\���J��
#	//2�s�ǂݔ�΂�
#	//�G���A�R�[�h�A�z�u���f���f�[�^�A�e�N�X�`���Z�b�g�A�l���f�[�^�A���C�g�t���O�����[�h
#	//���ꂼ����x�N�^�o�^
#	//�o�^�̍ہA�o�^�C���f�b�N�X��Ԃ��悤�ɂ���
#	//�Ԃ��Ă����C���f�b�N�X���G���A�f�[�^�Ƃ��ĕۑ�
#	//���C�g�t���O�ۑ�
#	//1�s���ƂɃo�C�i�����쐬
#	//�G���AID���J��Ԃ��A�I��������A�e�x�N�^���A�[�J�C�u�p�X�N���v�g�t�@�C���Ƃ��ďo��

COL_AREANAME	=	0
COL_BMNAME		=	1
COL_TEXNAME		=	2
COL_TEXPART1	=	3
COL_TEXPART2	=	4
COL_ANMNAME		=	5
COL_INOUT		=	6
COL_LIGHTTYPE	=	7
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

#�t�@�C���p�b�N���C�g
def FileWrite(bin_file, data, code)
	ary = Array(data)
	pack_str = ary.pack(code)
	bin_file.write(pack_str)
end

#area_tbl_file = File.open("area_table.txt","r")
area_tbl_file = File.open(ARGV[0],"r")
area_id_h = File.open("area_id.h","w")

area_id_h.printf("//���̃t�@�C���̓R���o�[�^�ɂ�萶������܂�\n")
area_id_h.printf("enum {\n")

area_vec =[]
build_vec = []
tex_vec = []
g_anm_vec = []
marge_tex_before = []

total_bin_file = File.open("area_data.bin","wb")

#1�s�ǂݔ�΂�
line = area_tbl_file.gets
while line = area_tbl_file.gets
	column = line.split "\t"

	#�G���A�h�c��
	area_id_h.printf("\t%s,\n",column[COL_AREANAME])

	#�G���A�o�C�i���쐬
	bin_file_name = "#{column[COL_AREANAME]}.bin"
	#��������
	bin_file_name.downcase!
	#�G���g��
	EntryVec(area_vec, bin_file_name)
	#bin�t�@�C���쐬
#	bin_file = File.open(bin_file_name,"wb")
	#�f�[�^��������
	data = EntryVec(build_vec,column[COL_BMNAME])	#���f��
#	FileWrite(bin_file,data, "S")
	FileWrite(total_bin_file,data, "S")
	data = EntryVec(tex_vec,column[COL_TEXNAME])		#�e�N�X�`���Z�b�g
#	FileWrite(bin_file,data, "S")
	FileWrite(total_bin_file,data, "S")
	data = EntryVec2(g_anm_vec,column[COL_ANMNAME],"none")	#�n�`�A�j���t�@�C��
#	FileWrite(bin_file,data, "S")
	FileWrite(total_bin_file,data, "S")
	data = GetInnerOuter(column[COL_INOUT])			#INNER/OUTER
#	FileWrite(bin_file,data, "C")
	FileWrite(total_bin_file,data, "C")
	data = Getlight(column[COL_LIGHTTYPE])				#���C�g
#	FileWrite(bin_file,data, "C")
	FileWrite(total_bin_file,data, "C")

	#�}�[�W�O�e�N�X�`�������W
	EntryVec(marge_tex_before,column[COL_TEXPART2])	#�}�[�W�O�e�N�X�`��1
	EntryVec(marge_tex_before,column[COL_TEXPART2])	#�}�[�W�O�e�N�X�`��2
	#"dummy"�͏Ȃ�
	marge_tex_before.delete("dummy")
	
	
end

total_bin_file.close
#�e�C���쐬
area_id_h.printf("};")

#�x�N�^���g�p���āA�A�[�J�C�u�p�X�N���v�g�t�@�C�����쐬
#	//�G���A
MakeScript(area_vec, "area_list.txt", "bin" );
#	//�z�u
MakeScript(build_vec, "build_list.txt", "dat" );
MakeScript(build_vec, "build_xls_list.txt", "xls" );
#	//�e�N�X�`��
MakeScript(tex_vec, "tex_list.txt", "nsbtx" );
#	//�}�[�W�O�e�N�X�`��
MakeScript(marge_tex_before, "tex_imd_list.txt", "imd" );
#	//�n�`�A�j��
MakeScript(g_anm_vec, "g_anm_list.txt", "nsbta" );



