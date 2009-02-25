#=======================================================================
#	fmmdl_mdllist.rb
#	�t�B�[���h���샂�f���@���f�����X�g�쐬
#	���� fmmdl_mdllist.rb xlstxt residx binfilename codefilename
#	xlstxt ���샂�f�����X�g�\ �e�L�X�g�ϊ��ς݃t�@�C����
#	residx ���\�[�X�A�[�J�C�u�f�[�^�C���f�b�N�X�t�@�C����
#	binfilename �쐬����o�C�i���t�@�C����
#	codefilename �쐬����OBJ�R�[�h�t�@�C����
#=======================================================================
$KCODE = "SJIS"

#=======================================================================
#	�t�H�[�}�b�g
#	0-3 ���f������
#	4-  �ȉ��������̃��f���p�����[�^
#
#	�p�����[�^�[�t�H�[�}�b�g
#	0-1 OBJ�R�[�h
#	2-3 ���\�[�X�A�[�J�C�u�C���f�b�N�X 
#	4	�\���^�C�v
#	5	�����֐�
#	6	�e�\��
#	7	���Վ��
#	8	�f�荞��
#	9	���f���T�C�Y
#	10	�e�N�X�`���T�C�Y
#	11	�A�j��ID
#=======================================================================

#=======================================================================
#	�萔
#=======================================================================
#�߂�l
RET_TRUE = (1)
RET_FALSE = (0)
RET_ERROR = (0xfffffffe)

#���X�g�ϊ��p�w�b�_�[�t�@�C��
FLDMMDL_LIST_H = "fldmmdl_list.h"

#�Œ蕶����
STR_NULL = ""
STR_END = "#END"
STR_CODEMAX = "OBJCODEMAX"

#�Ǘ��\�����ʒu ����
STRPRMNO_CODENAME = (3)
#�Ǘ��\�����ʒu �\���R�[�h
STRPRMNO_CODE = (7)
#�Ǘ��\�����ʒu ���f���t�@�C����
STRPRMNO_MDLFILENAME = (8)
#�Ǘ��\�����ʒu �\���^�C�v
STRPRMNO_DRAWTYPE = (9)
#�Ǘ��\�����ʒu �����֐�
STRPRMNO_DRAWPROC = (10)
#�Ǘ��\�����ʒu �e�\��
STRPRMNO_SHADOW = (11)
#�Ǘ��\�����ʒu ���Վ��
STRPRMNO_FOOTMARK = (12)
#�Ǘ��\�����ʒu �f�荞��
STRPRMNO_REFLECT = (13)
#�Ǘ��\�����ʒu ���f���T�C�Y
STRPRMNO_MDLSIZE = (14)
#�Ǘ��\�����ʒu �e�N�X�`���T�C�Y
STRPRMNO_TEXSIZE = (15)
#�Ǘ��\�����ʒu �A�j���[�V����ID
STRPRMNO_ANMID = (16)

#=======================================================================
#	�֐�
#=======================================================================
#-----------------------------------------------------------------------
#	�ُ�I��
#-----------------------------------------------------------------------
def error_end(
	path_delfile0, path_delfile1, file0, file1, file2, file3, file4 )
	printf( "ERROR fldmmdl list convert\n" )
	file0.close
	file1.close
	file2.close
	file3.close
	file4.close
	File.delete( path_delfile0 )
	File.delete( path_delfile1 )
end

#-----------------------------------------------------------------------
#	���f�����X�g�p�w�b�_�[�t�@�C��������
#	search ����������
#	�߂�l �w�蕶����̐��l RET_ERROR=�q�b�g����
#-----------------------------------------------------------------------
def hfile_search( hfile, search )
	pos = hfile.pos
	hfile.pos = 0
	num = RET_ERROR
	
	search = search.strip #�擪�����̋󔒁A���s�폜
	
	while line = hfile.gets
		if( line =~ /\A#define/ )
			len = line.length
			str = line.split() #�󔒕����ȊO����
			
			if( str[1] == search )	#1 �V���{����
				str_num = str[2]	#2 ���l
				
				if( str_num =~ /\A0x/ ) #16�i�\�L
					/([\A0x0-9a-fA-F]+)/ =~ str_num
					str_num = $1
					num = str_num.hex
				else					#10�i�\�L
					/([\A0-9]+)/ =~ str_num
					str_num = $1
					num = str_num.to_i
				end
				
				break
			end
		end
	end
	
	hfile.pos = pos
	return num
end

#-----------------------------------------------------------------------
#	���f�����X�g�p�A�[�J�C�u�C���f�b�N�X�t�@�C��������
#	search ����������
#	�߂�l �w�蕶����̐��l RET_ERROR=�q�b�g����
#-----------------------------------------------------------------------
def arcidx_search( idxfile, search )
	pos = idxfile.pos
	idxfile.pos = 0
	num = RET_ERROR
	
	search = search.strip #�擪�����̋󔒁A���s�폜
	check = sprintf( "_%s_", search )
	
	while line = idxfile.gets
		if( line =~ /^enum.*\{/ )
			while line = idxfile.gets
				if( line.index(check) != @nil )
					/(\s[0-9]+)/ =~ line
					str = $1
					num = str.to_i
					break
				end
			end
			
			break
		end
	end
	
	idxfile.pos = pos
	return num
end

#=======================================================================
#	�R�[�h�t�@�C���쐬
#=======================================================================
#-----------------------------------------------------------------------
#	�R�[�h�t�@�C���L�q
#	�߂�l�@RET_FALSE=�ُ�I��
#-----------------------------------------------------------------------
def codefile_write( codefile, txtfile )
	no = 0
	pos = txtfile.pos
	txtfile.pos = 0 #�擪��
	line = txtfile.gets #��s��΂�
	
	codefile.printf( "//���샂�f�� OBJ�R�[�h��`\n\n" );
	
	loop{
		line = txtfile.gets
		
		if( line == @nil )
			return RET_FALSE
		end
		
		if( line.include?(STR_END) )
			break
		end
		
		str = line.split( "," )
		
		codefile.printf( "#define %s (0x%x) //%d %s\n",
			str[STRPRMNO_CODE], no, no, str[STRPRMNO_CODENAME] );
		
		no = no + 1
	}
	
	if( no == 0 )
		printf( "OBJ�R�[�h����`����Ă��܂���\n" );
		return RET_FALSE
	end
	
	codefile.printf(
		"#define %s (0x%x) //%d %s", STR_CODEMAX, no, no, "�ő�" );
	
	txtfile.pos = pos
	return RET_TRUE
end

#=======================================================================
#	���샂�f���p�����^�R���o�[�g
#=======================================================================
#-----------------------------------------------------------------------
#	���샂�f���f�[�^��s�R���o�[�g
#-----------------------------------------------------------------------
def convert_line( no, line, wfile, idxfile, symfile )
	str = line.split( "," )
	
	#OBJ�R�[�h 2
	ary = Array( no )
	wfile.write( ary.pack("S*") )
	
	#�A�[�J�C�u�C���f�b�N�X �e�N�X�`�� 2 (4)
	word = str[STRPRMNO_DRAWTYPE]
	if( word == "DRAWTYPE_NON" ) #�\���^�C�v�@����
		ret = 0
	else
		word = str[STRPRMNO_MDLFILENAME]
		/(\A.*[^\.imd])/ =~ word
		mdlname = $1
		ret = arcidx_search( idxfile, mdlname )
		if( ret == RET_ERROR )
			printf( "ERROR ���f���t�@�C�����ُ� %s\n", word )
			return RET_FALSE
		end
	end
	ary = Array( ret )
	wfile.write( ary.pack("S*") )
	
	#�\���^�C�v 1 (5)
	word = str[STRPRMNO_DRAWTYPE]
	ret = hfile_search( symfile, word )
	if( ret == RET_ERROR )
		printf( "ERROR �\���^�C�v�ُ� %s\n", word )
		return RET_FALSE
	end
	ary = Array( ret )
	wfile.write( ary.pack("C*") )
	
	#�����֐� 1 (6)
	word = str[STRPRMNO_DRAWPROC]
	ret = hfile_search( symfile, word )
	if( ret == RET_ERROR )
		printf( "ERROR �����֐��ُ�ُ� %s\n", word )
		return RET_FALSE
	end
	ary = Array( ret )
	wfile.write( ary.pack("C*") )
	
	#�e�\�� 1 (7)
	word = str[STRPRMNO_SHADOW]
	if( word != "��" )
		ret = 1
	else
		ret = 0
	end
	ary = Array( ret )
	wfile.write( ary.pack("C*") )
	
	#���Վ�� 1 (8)
	word = str[STRPRMNO_FOOTMARK]
	ret = hfile_search( symfile, word )
	if( ret == RET_ERROR )
		printf( "ERROR ���Վ�ވُ� %s\n", word )
		return RET_FALSE
	end
	ary = Array( ret )
	wfile.write( ary.pack("C*") )
	
	#�f�荞�� 1 (9)
	word = str[STRPRMNO_REFLECT]
	if( word != "��" )
		ret = 1
	else
		ret = 0
	end
	ary = Array( ret )
	wfile.write( ary.pack("C*") )
	
	#���f���T�C�Y 1 (10)
	word = str[STRPRMNO_MDLSIZE]
	ret = hfile_search( symfile, word )
	if( ret == RET_ERROR )
		printf( "ERROR ���f���T�C�Y�ُ� %s\n", word )
		return RET_FALSE
	end
	ary = Array( ret )
	wfile.write( ary.pack("C*") )
	
	#�e�N�X�`���T�C�Y 1 (11)
	word = str[STRPRMNO_TEXSIZE]
	ret = hfile_search( symfile, word )
	if( ret == RET_ERROR )
		printf( "ERROR �e�N�X�`���T�C�Y�ُ� %s\n", word )
		return RET_FALSE
	end
	ary = Array( ret )
	wfile.write( ary.pack("C*") )
	
	#�A�j��ID 1 (12)
	word = str[STRPRMNO_ANMID]
	ret = hfile_search( symfile, word )
	if( ret == RET_ERROR )
		printf( "ERROR �A�j��ID�ُ� _%s_\n", word )
		return RET_FALSE
	end
	ary = Array( ret )
	wfile.write( ary.pack("C*") )
	
	return RET_TRUE
end

#=======================================================================
#	���f�����X�g�쐬
#=======================================================================
xlstxt_filename = ARGV[0]

if( xlstxt_filename == @nil )
	printf( "ERROR fmmdl_mdllist xlstxt filename\n" )
	exit 1
end

residx_filename = ARGV[1]

if( xlstxt_filename == @nil )
	printf( "ERROR fmmdl_mdllist residx filename\n" )
	exit 1
end

bin_filename = ARGV[2]

if( bin_filename == @nil )
	printf( "ERROR fmmdl_mdllist bin filename\n" )
	exit 1
end

code_filename = ARGV[3]

if( code_filename == @nil )
	printf( "ERROR fmmdl_mdllist code filename\n" )
	exit 1
end

symbol_filename = ARGV[4]

if( symbol_filename == @nil )
	printf( "ERROR fmmdl_mdllist symbol filename\n" )
	exit 1
end

txtfile = File.open( xlstxt_filename, "r" );
residxfile = File.open( residx_filename, "r" );
binfile = File.open( bin_filename, "wb" );
codefile = File.open( code_filename, "w" );
symfile = File.open( symbol_filename, "r" );

ret = codefile_write( codefile, txtfile ) #�\���R�[�h�w�b�_�[�t�@�C���쐬

if( ret == RET_FALSE )
	error_end( bin_filename, code_filename,
		 txtfile, residxfile, binfile, codefile, symfile )
	exit 1
end

no = 0
ary = Array( no )
binfile.write( ary.pack("I*") )

ret = RET_FALSE
line = txtfile.gets #�擪�s������

while line = txtfile.gets			#�p�����^�R���o�[�g
	if( line.include?(STR_END) )
		break
	end
	
	ret = convert_line( no, line, binfile, residxfile, symfile )
	
	if( ret == RET_FALSE )
		error_end( bin_filename, code_filename,
			 txtfile, residxfile, binfile, codefile, symfile )
		exit 1
	end

	no = no + 1
end

binfile.pos = 0
ary = Array( no )
binfile.write( ary.pack("I*") ) #����

txtfile.close
residxfile.close
binfile.close
codefile.close
symfile.close
