#=======================================================================
#  fmmdl_mdllist.rb
#  �t�B�[���h���샂�f���@���f�����X�g�쐬
#  ���� fmmdl_mdllist.rb xlstxt residx file_binname file_codename file_codestrname
#  xlstxt ���샂�f�����X�g�\ �e�L�X�g�ϊ��ς݃t�@�C����
#  residx ���\�[�X�A�[�J�C�u�f�[�^�C���f�b�N�X�t�@�C����
#  file_binname �쐬����o�C�i���t�@�C����
#  file_codename �쐬����OBJ�R�[�h�t�@�C����
#  file_codestrname �쐬����OBJ�R�[�h������t�@�C����
#  symbolfilename �ǂݍ��ރV���{���t�@�C����
#=======================================================================
$KCODE = "SJIS"
load "rbdefine"

#=======================================================================
#  �p�����^�t�H�[�}�b�g
#  0-3 ���f������
#  4-  �ȉ��������̃��f���p�����[�^
#
#  �p�����[�^�[�t�H�[�}�b�g
#  0-1 OBJ�R�[�h
#  2-3 ���\�[�X�A�[�J�C�u�C���f�b�N�X
#  4  �\���^�C�v
#  5  �����֐�
#  6  �e�\��
#  7  ���Վ��
#  8  �f�荞��
#  9  ���f���T�C�Y
#  10  �e�N�X�`���T�C�Y
#  11  �A�j��ID
#  12 ����
#  13-15 4bit�����p�_�~�[
#=======================================================================

#=======================================================================
#  �쐬�����OBJ�R�[�h������t�@�C���\��
#  32byte�P�ʂŃR�[�h�̐����i�[����Ă���B
#  0..31  �R�[�h0�Ԃ̃R�[�h������ 32byte ASCII �I�[�k������
#  32..63 �R�[�h1�ԕ�����
#=======================================================================

#=======================================================================
#  �萔
#=======================================================================
#�߂�l
RET_TRUE = (1)
RET_FALSE = (0)
RET_ERROR = (0xfffffffe)

#���X�g�ϊ��p�w�b�_�[�t�@�C��
FLDMMDL_LIST_H = "fldmmdl_list.h"

#�Œ蕶����
STR_NULL = "" #NULL����
STR_END = "#END" #�I�[������

STR_CODESTART_BBD = "OBJCODESTART_BBD" #�R�[�h�J�n �r���{�[�h
STR_CODEEND_BBD = "OBJCODEEND_BBD" #�R�[�h�I�� �r���{�[�h

STR_CODESTART_POKE = "OBJCODESTART_TPOKE" #�R�[�h�J�n �|�P����
STR_CODEEND_POKE = "OBJCODEEND_TPOKE" #�R�[�h�I�� �|�P����

STR_CODETOTAL = "OBJCODETOTAL" #�R�[�h����
STR_CODEMAX = "OBJCODEMAX" #�R�[�h�ő�

STR_DRAWTYPE_NON = "DRAWTYPE_NON" #�\���^�C�v����

#�R�[�h�J�n�ʒu
CODE_STARTNUM_BBD = 0x0000 #�r���{�[�h
CODE_STARTNUM_POKE = 0x1000 #�|�P����
CODE_MAX = 0xffff #�ő�R�[�h

#�\���R�[�h������ �ꕶ����Œ� �k�������܂�
CODESTRBUF = (16)

#����
ARGVNO_FNAME_LISTCSV = 0 #���X�gcsv�t�@�C���l�[��
ARGVNO_FNAME_RESIDX = 1 #���\�[�X�A�[�J�C�u�C���f�b�N�X�t�@�C���l�[��
ARGVNO_FNAME_BIN = 2 #�o�C�i���t�@�C���l�[��
ARGVNO_FNAME_CODE = 3 #�R�[�h�t�@�C���l�[��
ARGVNO_FNAME_CODESTR = 4 #�R�[�h������t�@�C���l�[��
ARGVNO_FNAME_SYMNBOL = 5 #�V���{���t�@�C���l�[��
ARGVNO_FLAG_SELRES = 6 #���\�[�X�I��
ARGVNO_FNAME_LISTCSV_POKE = 7 #���X�gcsv�t�@�C���l�[���@�|�P����

#=======================================================================
#  �֐�
#=======================================================================
#-----------------------------------------------------------------------
#  �ُ�I��
#-----------------------------------------------------------------------
def error_end(
  path_delfile0, path_delfile1, path_delfile2,
  file0, file1, file2, file3, file4, file5, file6 )
  printf( "ERROR fldmmdl list convert\n" )
  file0.close
  file1.close
  file2.close
  file3.close
  file4.close
  file5.close
  File.delete( path_delfile0 )
  File.delete( path_delfile1 )
  File.delete( path_delfile2 )
end

#-----------------------------------------------------------------------
#  ���f�����X�g�p�w�b�_�[�t�@�C��������
#  search ����������
#  �߂�l �w�蕶����̐��l RET_ERROR=�q�b�g����
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
      
      if( str[1] == search )  #1 �V���{����
        str_num = str[2]  #2 ���l
        
        if( str_num =~ /\A0x/ ) #16�i�\�L
          /([\A0x0-9a-fA-F]+)/ =~ str_num
          str_num = $1
          num = str_num.hex
        else          #10�i�\�L
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
#  ���f�����X�g�p�A�[�J�C�u�C���f�b�N�X�t�@�C��������
#  search ����������
#  �߂�l �w�蕶����̐��l RET_ERROR=�q�b�g����
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
        if( line.index(check) != nil )
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
#  �R�[�h�t�@�C���쐬
#=======================================================================
#-----------------------------------------------------------------------
#  �R�[�h�t�@�C���L�q�@���C��
#  �߂�l�@RET_FALSE=�ُ�I��
#-----------------------------------------------------------------------
def file_code_write_main(
  file_code, file_codestr, file_listcsv, code_no, count )
  start_code = code_no
  
  pos = file_listcsv.pos
  file_listcsv.pos = 0 #�擪��
  line = file_listcsv.gets #��s��΂�
  
  loop{
    line = file_listcsv.gets
    
    if( line == nil )
      return nil
    end
    
    if( line.include?(STR_END) )
      break
    end
    
    str = line.split( "," )
    
    file_code.printf( "#define %s (0x%x) //%d(total %d) %s\n",
      str[RBDEF_NUM_CODE], code_no, code_no, count, str[RBDEF_NUM_CODENAME] );
    
    codestr = Array.new( CODESTRBUF );  #������o�b�t�@
    codestr.fill( "\000".unpack('C*'), 0..CODESTRBUF ) #�k�������Ŗ��ߐs����
    
    i = 0
    strbuf = str[RBDEF_NUM_CODE]
    while strbuf[i]
      codestr[i] = strbuf[i]
      i = i + 1
      if( i >= (CODESTRBUF-1) )
        break
      end
    end
    
    i = 0
    while i < CODESTRBUF    #�o�C�g�P�ʂŃo�b�t�@������������
      c = Array( codestr[i] )
      file_codestr.write( c.pack("C*") )
      i = i + 1
    end
    
    count = count + 1
    code_no = code_no + 1
  }
  
  if( code_no == start_code )
    printf( "OBJ�R�[�h����`����Ă��܂���\n" );
    return nil
  end

  file_listcsv.pos = pos
  
  param = Hash.new
  param.store( "str_code_no", code_no )
  param.store( "str_count", count )
  return param
end  

#-----------------------------------------------------------------------
#  �R�[�h�t�@�C���L�q
#  �߂�l�@RET_FALSE=�ُ�I��
#-----------------------------------------------------------------------
def file_code_write( file_code, file_codestr,
      file_listcsv, file_listcsv_poke )
  pos = file_listcsv.pos
  file_listcsv.pos = 0 #�擪��
  line = file_listcsv.gets #��s��΂�
  
  file_code.printf( "//���샂�f�� OBJ�R�[�h��`\n" )
  file_code.printf( "//���̃t�@�C���̓R���o�[�^����쐬����܂���\n" )
  
  path = file_code.path
  name = File.basename( path, "\.*" )
  name = name.upcase
  name = sprintf( "_%s_", name )
  file_code.printf( "#ifndef %s\n#define %s\n\n", name, name )
  
  count = 0

  #�r���{�[�h�R�[�h�L�q
  code_no = CODE_STARTNUM_BBD
  
  file_code.printf( "//�r���{�[�h\n" )
  file_code.printf( "#define %s (0x%x)\n", STR_CODESTART_BBD, code_no )
  
  ret = file_code_write_main(
    file_code, file_codestr, file_listcsv, code_no, count )
  
  if( ret == nil )
    return RET_FALSE
  end
  
  code_no = ret['str_code_no']
  count = ret['str_count']
  
  file_code.printf(
    "#define %s (0x%x) //%d(total %d) %s\n\n",
    STR_CODEEND_BBD, code_no, code_no, count, "�r���{�[�h�R�[�h�I�[" );

  #�|�P�����R�[�h�L�q
  code_no = CODE_STARTNUM_POKE
  
  file_code.printf( "//�A������|�P����\n" )
  file_code.printf( "#define %s (0x%x)\n", STR_CODESTART_POKE, code_no )
  
  ret = file_code_write_main(
    file_code, file_codestr, file_listcsv_poke, code_no, count )
  
  if( ret == nil )
    return RET_FALSE
  end
  
  code_no = ret['str_code_no']
  count = ret['str_count']
  
  file_code.printf(
    "#define %s (0x%x) //%d(total %d) %s\n\n",
    STR_CODEEND_POKE, code_no, code_no, count, "�|�P�����R�[�h�I�[" );
  
  #�I�[�L�q
  file_code.printf(
    "#define %s (0x%x) //%d %s\n", STR_CODETOTAL, count, count, "����" )
  file_code.printf(
    "#define %s (0x%x) //%d %s\n\n", STR_CODEMAX, CODE_MAX, CODE_MAX, "�ő�" )
  
  file_code.printf( "#endif //_%s_", name );
  return RET_TRUE
end

#=======================================================================
#  ���샂�f���p�����^�R���o�[�g
#=======================================================================
#-----------------------------------------------------------------------
#  ���샂�f���f�[�^��s�R���o�[�g
#-----------------------------------------------------------------------
def convert_line( no, line, wfile, idxfile, file_symbol, flag_selres )
  str = line.split( "," )
  
  #OBJ�R�[�h 2
  ary = Array( no )
  wfile.write( ary.pack("S*") )
  
  #�A�[�J�C�u�C���f�b�N�X �e�N�X�`�� 2 (4)
  word = str[RBDEF_NUM_DRAWTYPE]
  if( word == STR_DRAWTYPE_NON ) #�\���^�C�v�@����
    ret = 0
  else
    xlsline = RBDEF_NUM_RESFILE_NAME_0
    
    if( flag_selres != "0" )
      xlsline = RBDEF_NUM_RESFILE_NAME_1
    end
    
    word = str[xlsline]
    
    #���K�\���ŕ�����".imd"���폜����B���A���̂�'m'�����܂ō폜���Ă��܂�
    #/(\A.*[^\.imd])/ =~ word
    #mdlname = $1
    #
    #����Ƃ���gsub��".imd"���폜����B
    mdlname = word.gsub( "\.imd", "" )
    
    ret = arcidx_search( idxfile, mdlname )
    if( ret == RET_ERROR )
      printf( "ERROR ���f���t�@�C�����ُ� %s\n", word )
      return RET_FALSE
    end
  end
  ary = Array( ret )
  wfile.write( ary.pack("S*") )
  
  #�\���^�C�v 1 (5)
  word = str[RBDEF_NUM_DRAWTYPE]
  ret = hfile_search( file_symbol, word )
  if( ret == RET_ERROR )
    printf( "ERROR �\���^�C�v�ُ� %s\n", word )
    return RET_FALSE
  end
  ary = Array( ret )
  wfile.write( ary.pack("C*") )
  
  #�����֐� 1 (6)
  word = str[RBDEF_NUM_DRAWPROC]
  ret = hfile_search( file_symbol, word )
  if( ret == RET_ERROR )
    printf( "ERROR �����֐��ُ�ُ� %s\n", word )
    return RET_FALSE
  end
  ary = Array( ret )
  wfile.write( ary.pack("C*") )
  
  #�e�\�� 1 (7)
  word = str[RBDEF_NUM_SHADOW]
  if( word != "��" )
    ret = 0 
  else
    ret = 1
  end
  ary = Array( ret )
  wfile.write( ary.pack("C*") )
  
  #���Վ�� 1 (8)
  word = str[RBDEF_NUM_FOOTMARK]
  ret = hfile_search( file_symbol, word )
  if( ret == RET_ERROR )
    printf( "ERROR ���Վ�ވُ� %s\n", word )
    return RET_FALSE
  end
  ary = Array( ret )
  wfile.write( ary.pack("C*") )
  
  #�f�荞�� 1 (9)
  word = str[RBDEF_NUM_REFLECT]
  if( word != "��" )
    ret = 0
  else
    ret = 1
  end
  ary = Array( ret )
  wfile.write( ary.pack("C*") )
  
  #���f���T�C�Y 1 (10)
  word = str[RBDEF_NUM_MDLSIZE]
  ret = hfile_search( file_symbol, word )
  if( ret == RET_ERROR )
    printf( "ERROR ���f���T�C�Y�ُ� %s\n", word )
    return RET_FALSE
  end
  ary = Array( ret )
  wfile.write( ary.pack("C*") )
  
  #�e�N�X�`���T�C�Y 1 (11)
  word = str[RBDEF_NUM_TEXSIZE]
  ret = hfile_search( file_symbol, word )
  if( ret == RET_ERROR )
    printf( "ERROR �e�N�X�`���T�C�Y�ُ� %s\n", word )
    return RET_FALSE
  end
  ary = Array( ret )
  wfile.write( ary.pack("C*") )
  
  #�A�j��ID 1 (12)
  word = str[RBDEF_NUM_ANMID]
  ret = hfile_search( file_symbol, word )
  if( ret == RET_ERROR )
    printf( "ERROR �A�j��ID�ُ� _%s_\n", word )
    return RET_FALSE
  end
  ary = Array( ret )
  wfile.write( ary.pack("C*") )
  
  #���� 1 (13)
  word = str[RBDEF_NUM_SEX]
  if( word == "�j" )
    ret = 0
  elsif( word == "��" )
    ret = 1
  else
    ret = 2
  end
  ary = Array( ret )
  wfile.write( ary.pack("C*") )
  
  #�S�r�b�g���E�p�_�~�[�f�[�^ 3 (14-16)
  ret = 0
  ary = Array( ret )
  wfile.write( ary.pack("C*") )
  wfile.write( ary.pack("C*") )
  wfile.write( ary.pack("C*") )
  
  return RET_TRUE
end

#-----------------------------------------------------------------------
#  ���샂�f���@�f�[�^�R���o�[�g�@���C��
#-----------------------------------------------------------------------
def convert_code_param_main( start_code,
  file_bin, file_listcsv, file_residx, file_symbol, flag_selres )
  count = 0
  no = start_code
  
  file_listcsv.pos = 0 #�擪�s��
  line = file_listcsv.gets #�擪�s������
  
  while line = file_listcsv.gets
    if( line.include?(STR_END) )
      break
    end
    
    ret = convert_line(
      no, line, file_bin, file_residx, file_symbol, flag_selres )
    
    if( ret == RET_FALSE )
      return RET_ERROR #ERROR
    end
    
    no = no + 1
    count = count + 1
  end
  
  return count
end

#-----------------------------------------------------------------------
#  ���샂�f���@�f�[�^�R���o�[�g�@���C��
#-----------------------------------------------------------------------
def convert_code_param( file_bin,
    file_listcsv, file_listcsv_poke,
    file_residx, file_symbol, flag_selres )
  count = 0 #�_�~�[�����L�q
  ary = Array( count )
  file_bin.pos = 0
  file_bin.write( ary.pack("I*") )
  
  #�r���{�[�h
  ret = convert_code_param_main( CODE_STARTNUM_BBD,
    file_bin, file_listcsv, file_residx, file_symbol, flag_selres )
  
  if( ret == RET_ERROR )
    return RET_FALSE
  end
  
  count = count + ret
  
  #�|�P����
  ret = convert_code_param_main( CODE_STARTNUM_POKE,
    file_bin, file_listcsv_poke, file_residx, file_symbol, flag_selres )
  
  if( ret == RET_ERROR )
    return RET_FALSE
  end
  
  count = count + ret
  
  file_bin.pos = 0 #�����L�q
  ary = Array( count )
  file_bin.write( ary.pack("I*") )
  return RET_TRUE
end

#=======================================================================
#  ���f�����X�g�쐬
#=======================================================================
#---------------------------------------------------------------
# �����擾
#---------------------------------------------------------------
fname_listcsv = ARGV[ARGVNO_FNAME_LISTCSV]

if( fname_listcsv == nil )
  printf( "ERROR fmmdl_mdllist xlstxt filename\n" )
  exit 1
end

fname_listcsv_poke = ARGV[ARGVNO_FNAME_LISTCSV_POKE]

if( fname_listcsv_poke == nil )
  printf( "ERROR fmmdl_mdllist xlstxt poke filename\n" )
  exit 1
end

fname_residx = ARGV[ARGVNO_FNAME_RESIDX]

if( fname_residx == nil )
  printf( "ERROR fmmdl_mdllist residx filename\n" )
  exit 1
end

fname_bin = ARGV[ARGVNO_FNAME_BIN]

if( fname_bin == nil )
  printf( "ERROR fmmdl_mdllist bin filename\n" )
  exit 1
end

fname_code = ARGV[ARGVNO_FNAME_CODE]

if( fname_code == nil )
  printf( "ERROR fmmdl_mdllist code filename\n" )
  exit 1
end

fname_codestr = ARGV[ARGVNO_FNAME_CODESTR]

if( fname_codestr == nil )
  printf( "ERROR fmmdl_mdllist code str filename\n" )
  exit 1
end

fname_symbol = ARGV[ARGVNO_FNAME_SYMNBOL]

if( fname_symbol == nil )
  printf( "ERROR fmmdl_mdllist symbol filename\n" )
  exit 1
end

flag_selres = ARGV[ARGVNO_FLAG_SELRES]

if( flag_selres != "0" && flag_selres != "1" )
  printf( "ERROR!! fmmdl_mdllist.rb flag_selres\n" )
  exit 1
end

#---------------------------------------------------------------
# �t�@�C���I�[�v��
#---------------------------------------------------------------
file_listcsv = File.open( fname_listcsv, "r" );
file_listcsv_poke = File.open( fname_listcsv_poke, "r" );
file_residx = File.open( fname_residx, "r" );
file_bin = File.open( fname_bin, "wb" );
file_code = File.open( fname_code, "w" );
file_codestr = File.open( fname_codestr, "wb" );
file_symbol = File.open( fname_symbol, "r" );

#---------------------------------------------------------------
# �\���R�[�h�w�b�_�[�t�@�C���쐬
#---------------------------------------------------------------
ret = file_code_write(
  file_code, file_codestr, file_listcsv, file_listcsv_poke )

if( ret == RET_FALSE )
  error_end( fname_bin, fname_code, fname_codestr,
     file_listcsv, file_residx, file_bin, file_code,
     file_codestr, file_symbol, file_listcsv_poke )
  exit 1
end

#---------------------------------------------------------------
# �R�[�h�p�����^�t�@�C���쐬
#---------------------------------------------------------------
ret = convert_code_param( file_bin,
  file_listcsv, file_listcsv_poke,
  file_residx, file_symbol, flag_selres )

if( ret == RET_FALSE )
  error_end( fname_bin, fname_code, fname_codestr,
     file_listcsv, file_residx, file_bin, file_code,
     file_codestr, file_symbol, file_listcsv_poke )
  exit 1
end

#---------------------------------------------------------------
# �t�@�C���N���[�Y
#---------------------------------------------------------------
file_listcsv.close
file_listcsv_poke.close
file_residx.close
file_bin.close
file_code.close
file_codestr.close
file_symbol.close
