#=======================================================================
# trtype_bgm.rb
# �g���[�i�[�^�C�v�ʂɍĐ����鎋��BGM�\���쐬
#
# �����@tr_bgm.rb fpath_csv fpath_cdat fpath_def
# fpath_csv �Ǘ��\csv�t�@�C���p�X
# fpath_cdat �쐬����cdat�t�@�C���p�X
# fpath_def �Q�Ƃ���g���[�i�[�^�C�v��`�t�@�C��
# print_error "1"=�G���[�o�͂��s�� "0"=�s��Ȃ�
#=======================================================================
$KCODE = "SJIS"

#=======================================================================
# �萔
#=======================================================================
STR_TRTYPE_MAX = "TRTYPE_MAX"

RET_TRUE = (1)
RET_FALSE = (0)

STR_TRUE = "1"
STR_FALSE = "0"

#=======================================================================
# �T�u���[�`��
#=======================================================================
#-----------------------------------------------------------------------
#	�w�b�_�[�t�@�C��������
#	search ����������
#	�߂�l RET_TRUE=���݂���
#-----------------------------------------------------------------------
def hfile_search( hfile, search )
	hfile.pos = 0
	
	search = search.strip #�擪�����̋󔒁A���s�폜
	
	while line = hfile.gets
		if( line =~ /\A#define/ )
			len = line.length
			str = line.split() #�󔒕����ȊO����
			
			if( str[1] == search )	#1 �V���{����
        return RET_TRUE
      end
		end
	end
  
	return RET_FALSE
end

#=======================================================================
# �ϊ�
#=======================================================================
fpath_csv = ARGV[0]
fpath_cdat = ARGV[1]
fpath_def = ARGV[2]
print_error = ARGV[3]
file_csv = File.open( fpath_csv, "r" )
file_cdat = File.open( fpath_cdat, "w" )
file_def = File.open( fpath_def, "r" )

file_cdat.printf( "//%s\n", fpath_cdat )
file_cdat.printf( "//���̃t�@�C���̓R���o�[�^����o�͂���܂���\n\n" )
file_cdat.printf( "//[][trainer type, trainer eye bgm]\n" )
file_cdat.printf( "unsigned short DATA_TrainerTypeToEyeBGMNo[][2] = {\n" )

line = file_csv.gets #�f�[�^�J�n�ʒu�܂Ői�߂�

i = 0

while line = file_csv.gets
  line = line.strip
  str = line.split( "," )
  
  if( str[0] != nil && str[0] != "" &&
      str[1] != nil && str[1] != "" )
    type = "TRTYPE_" + str[0].upcase
    
    if( hfile_search(file_def,type) == RET_FALSE )
      if( print_error == STR_TRUE )
        printf( "�g���[�i�[����BGM�Ή��\ %s �͑��݂��Ă��܂���\n", type )
      end
      file_cdat.printf( "//" )
    end
    
    file_cdat.printf( "  {%s,%s},", type, str[1] )
    
    if( str[2] != nil && str[2] != "" )
      file_cdat.printf( " //%s\n", str[2] )
    else
      file_cdat.printf( "\n" )
    end
  end
  
  i = i + 1
end

file_cdat.printf( "  {%s,0}, //end\n", STR_TRTYPE_MAX )
file_cdat.printf( "};" )
file_def.close
file_cdat.close
file_csv.close
