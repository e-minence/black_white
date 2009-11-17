#=======================================================================
# fmmdl_restbl.rb
# �t�B�[���h���샂�f�� ���\�[�X�e�[�u���쐬
# 
# ���� fmmdl_restbl.rb
# fname_listcsv fname_restbl res_dir select_res
# fname_prmcsv_ncgimd fname_restbl_ncgimd fname_mkobj_ncgimd
# res_ncg_dir path_conv_ncgimd
# flag_dummy
# fname_listcsv_poke
# 
# fname_listcsv
# ���샂�f�����X�g�\ �e�L�X�g�ϊ��ς݃t�@�C����
# 
# fname_restbl
# ���샂�f�����\�[�X�t�@�C�����L�q��t�@�C����
#
# res_dir
# ���\�[�X�t�@�C�����u����Ă���f�B���N�g��
# 
# select_res
# fname_listcsv�Ŏw�肳��Ă��镡���̃��\�[�X�t�@�C���̓��A�ǂ�����g����
# 0=���i���f���f���t�@�C�����g�p 1=�e�X�g�p���g�p
#
# fname_prmcsv_ncgimd
# ���샂�f�����X�g�\ ncg�ϊ��Ή�imd �e�L�X�g�ϊ��ς݃t�@�C����
#
# fname_restbl_ncgimd
# ���샂�f�� ncg�ϊ�imd�ꗗ�L�q��t�@�C����
#
# fname_mkobj_ncgimd
# ���샂�f�� ncg�ϊ��Ώ�make�L�q��t�@�C����
#
# res_ncg_dir
# ncg�t�@�C�����u����Ă���f�B���N�g��
#
# flag_dummy
# "0" = �t�@�C�������݂��Ă��Ȃ��ꍇ�G���[�B
# "1" = �t�@�C�������݂��Ă��Ȃ��ꍇ�A�_�~�[�t�@�C������u�������B
#
# fname_listcsv_poke
# �|�P�����p���X�g�\�@�e�L�X�g�ϊ��ς݃t�@�C����
#=======================================================================
$KCODE = "SJIS"
load "rbdefine"

#=======================================================================
#  �萔
#=======================================================================
STR_ERROR = "ERROR"
STR_END = "#END"

RET_FALSE = (0)
RET_TRUE = (1)
RET_ERROR = (-1)

#���f���p�_�~�[�t�@�C����
FPATH_DUMMY_IMD = "mmdl_res/dummy.imd"

ARGVNO_FNAME_RESCSV = (0)
ARGVNO_FNAME_RESTBL = (1)
ARGVNO_DIR_RES = (2)
ARGVNO_FLAG_SELECT_RES = (3)
ARGVNO_FNAME_NCGIMDCSV = (4)
ARGVNO_FNAME_NCGIMDLIST = (5)
ARGVNO_FNAME_MKNCGIMD = (6)
ARGVNO_DIR_NCG = (7)
ARGVNO_PATH_NCGIMDCV = (8)
ARGVNO_FLAG_CREATE_DUMMY = (9)
ARGVNO_FNAME_RESCSV_POKE = (10)
ARGVNO_FNAME_RESCSV_MDL = (11)

#=======================================================================
#  �ُ�I��
#=======================================================================
def error_end(
  path_delfile, file0, file1, file2, file3, file4, file5, file6 )
  file0.close
  file1.close
  file2.close
  file3.close
  file4.close
  file5.close
  file6.close
  File.delete( path_delfile )
end

#=======================================================================
#  �t�@�C���R�s�[
#=======================================================================
def file_copy( srcpath, copypath )
  open( srcpath ){ |input|
    open( copypath, "w" ){ |output|
      output.write( input.read )
    }
  }
end

#=======================================================================
#  �z��Ƀf�[�^�ǉ�
#=======================================================================
def buf_push( buf, data )
  buf << data
end


#=======================================================================
#  �w��ԍ���","��؂蕶������擾
#  �߂�l nil=����,STR_ERROR=�G���[,STR_END=�I��
#=======================================================================
def listcsv_getlinestr( file_listcsv, get_no )
  no = 0
  file_listcsv.pos = 0
  
  line = file_listcsv.gets #��s�ڔ�΂�
  
  if( line == nil )
    return RET_FALSE
  end
  
  while no < get_no
    line = file_listcsv.gets

    if( line == nil )
      return STR_ERROR
    end
  
    str = line.split( "," )
  
    if( str[0] == STR_END )
      return STR_END
    end
    
    no = no + 1
  end
  
  line = file_listcsv.gets
  line = line.strip
  
  if( line == nil )
    return STR_ERROR
  end
  
  str = line.split( "," )
  
  if( str[0] == STR_END )
    return STR_END
  end
  
  return str
end

#=======================================================================
#  �w��ԍ��̃��\�[�X�t�@�C�������擾
#  �߂�l nil=����,STR_ERROR=�G���[,STR_END=�I��
#=======================================================================
def listcsv_get_bbd_filename( file_listcsv, no, sel_res )
  xlsline = RBDEF_NUM_BBD_RESFILE_NAME_0
  
  if( sel_res != "0" )
    xlsline = RBDEF_NUM_BBD_RESFILE_NAME_1
  end
  
  str = listcsv_getlinestr( file_listcsv, no )
  
  if( str == STR_ERROR )
    return STR_ERROR
  end
  
  if( str == STR_END )
    return STR_END
  end
  
  if( str[xlsline] != "" && str[xlsline] != nil )
    return str[xlsline]
  end
  
  return nil
end

#=======================================================================
#  �w��ԍ��̕\���^�C�v���擾
#  �߂�l nil=����,STR_ERROR=�G���[,STR_END=�I��
#=======================================================================
def listcsv_get_drawtype( file_listcsv, no )
  str = listcsv_getlinestr( file_listcsv, no )
  
  if( str == STR_ERROR )
    return STR_ERROR
  end
  
  if( str == STR_END )
    return STR_END
  end

  type = str[RBDEF_NUM_DRAWTYPE]
      
  if( type != "" && type != nil )
    return type
  end
  
  return nil
end

#=======================================================================
#  �w��ԍ����O���\�[�X�t�@�C�������������d���`�F�b�N
#  �߂�l RET_TRUE=�w��t�@�C���������݂��Ă��� RET_FALSE=����
#  �߂�l nil=����,STR_ERROR=�G���[,STR_END=�I��
#=======================================================================
def csvlist_check_repeat_filename( buf, check_str )
  no = 1 #�ŏ��̃}�N����`���΂�
  max = buf.size
  
#debug
=begin
  printf( "�d���`�F�b�N %d�ȉ���%s���݂邩�T���Ă��܂�\n",
         check_no, check_str )
=end

  while no < max
    str = buf[no]
    str = str.strip
    
    if( str == check_str )
      return RET_TRUE
    end
    
    no = no + 1
  end
  
  return RET_FALSE
end

#�t�@�C�������^�C�v�@���x
=begin
def csvlist_check_repeat_filename(
  file_listcsv, check_no, check_str, sel_res )
  no = 0
  file_listcsv.pos = 0
  
  xlsline = RBDEF_NUM_RESFILE_NAME_0
  
  if( sel_res != "0" )
    xlsline = RBDEF_NUM_RESFILE_NAME_1
  end
  
  while no < check_no
    str = listcsv_getlinestr( file_listcsv, no )
    
    if( str == STR_ERROR )
      break
    end
  
    if( str == STR_END )
      break
    end
    
    if( no < check_no )  #�w��ʒu���O
      if( str[xlsline] != "" && str[xlsline] != nil )
        if( str[xlsline] == check_str )
          return RET_TRUE
        end
      end
    end
    
    no = no + 1
  end
  
  return RET_FALSE
end
=end

#=======================================================================
# �w�肳�ꂽ���f���T�C�Y�A�A�j���[�V����ID����
# NCG�ϊ����ɕK�v�ȃp�����^���n�b�V���ŕԂ�
# listfile ncg�Ή��\txt�t�@�C��
# mdlstr ���f���T�C�Y������ MDLSIZE_32x32��
# anmstr �A�j��ID������ ANMID_NON��
# �߂�l nil=�Ȃ�
#=======================================================================
def ncgparam_get( listfile, mdlstr, anmstr )
  listfile.pos = 0
  listfile.gets #��s������
  listfile.gets #��s������
  
  while line = listfile.gets
    line = line.strip
    str = line.split( "," )
    
    if( str[0] == STR_END )
      break
    end
    
    if( str[0] != nil && str[0] == anmstr )
      i = 1
      while data = str[i]
        param = data.split( "/" )
        
        if( param[0] != nil && param[0] == mdlstr )
          h = Hash.new
          h.store( "str_width", param[1] )
          h.store( "str_height", param[2] )
          h.store( "str_count", param[3] )
          h.store( "str_imdfile", param[4] )
          h.store( "str_flag_dummy", param[5] )
          return h
        end
        
        i = i + 1
      end
    end
  end
  
  return nil
end

#=======================================================================
#=======================================================================
def buf_add_convname( name,
  buf_imdtex, buf_itp, buf_ica, buf_ita, buf_ima, buf_iva )
  extname = File.extname( name )
  
  if( extname == "\.imd" )
    buf_imdtex << " \\\n"
    buf_imdtex << sprintf( "\t%s", name )
  elsif( extname == "\.itp" )
    buf_itp << " \\\n"
    buf_itp << sprintf( "\t%s", name )
  elsif( extname == "\.ica" )
    buf_ica << " \\\n"
    buf_ica << sprintf( "\t%s", name )
  elsif( extname == "\.ima" )
    buf_ima << " \\\n"
    buf_ica << sprintf( "\t%s", name )
  elsif( extname == "\.ita" )
    buf_ita << " \\\n"
    buf_ita << sprintf( "\t%s", name )
  elsif( extname == "\.iva" )
    buf_iva << " \\\n"
    buf_iva << sprintf( "\t%s", name )
  end
end

#=======================================================================
# DRAWTYPE_BLACT����
# �߂�l RET_TRUE=���\�[�X��ǉ������B
#=======================================================================
def conv_drawtype_blact(
  file_listcsv, no, dir_res, sel_res, flag_dummy,
  file_prmcsv_ncgimd,
  file_make_ncgimd, dir_res_ncgimd, path_conv_ncgimd,
  buf_imdtex, buf_ncgimd )
  
  imdname = listcsv_get_bbd_filename( file_listcsv, no, sel_res )
  
  if( imdname == STR_ERROR )
    return RET_ERROR
  end
  
  if( imdname == STR_END )
    return RET_ERROR
  end

  if( imdname == nil )
    return RET_FALSE
  end
  
=begin  
  if( csvlist_check_repeat_filename(
    file_listcsv,no,imdname,sel_res) == RET_TRUE )
    return RET_FALSE
  end
=end
  
  if( csvlist_check_repeat_filename(buf_imdtex,imdname) == RET_TRUE )
    return RET_TRUE #���\�[�X���ɒǉ��ς�
  end

  buf_imdtex << " \\\n"
  
  str = listcsv_getlinestr( file_listcsv, no )
  mdltype = str[RBDEF_NUM_BBD_MDLSIZE]
  anmtype = str[RBDEF_NUM_BBD_ANMID]
  param = ncgparam_get( file_prmcsv_ncgimd, mdltype, anmtype )
  
  imdpath = sprintf( "%s\/%s", dir_res, imdname )
  
  #ncg�ϊ��Ώۂł͂Ȃ�
  if( param == nil )
    if( FileTest.exist?(imdpath) != true )
      if( flag_dummy == 0 )
        printf( "ERROR %s ������܂���\n", imdname )
        return RET_ERROR
      end
      
      #�_�~�[�t�@�C���쐬 �����Ή�
      file_copy( FPATH_DUMMY_IMD, imdpath )
      printf( "%s���_�~�[�t�@�C������쐬���܂���\n", imdpath )
      RET_TRUE
    end
    
    buf_imdtex << sprintf( "\t%s", imdname )
    return RET_TRUE
  end
  
  #ncg�ϊ��Ώ�
  buf_ncgimd <<= sprintf( " \\\n" )
  
  name = imdname.gsub( "\.imd", "" )
  ncgname = name + "\.ncg"
  nclname = name + "\.ncl"
  
  ncgpath = sprintf( "%s\/%s", dir_res_ncgimd, ncgname )
  
  if( FileTest.exist?(ncgpath) != true )
    if( flag_dummy == 0 )
      printf( "ERROR %s ������܂���\n", ncgpath )
      return RET_ERROR
    end
    
    dummy_file = sprintf(
      "%s/%s.ncg", dir_res_ncgimd, param['str_flag_dummy'] )
    file_copy( dummy_file, ncgpath )
    printf( "%s���_�~�[�t�@�C������쐬���܂���\n", ncgpath )
  end
  
  nclpath = sprintf( "%s\/%s", dir_res_ncgimd, nclname )

  if( FileTest.exist?(nclpath) != true )
    if( flag_dummy == 0 )
      printf( "ERROR %s ������܂���\n", nclpath )
      return RET_ERROR
    end
    
    dummy_file = sprintf(
      "%s/%s.ncl", dir_res_ncgimd, param['str_flag_dummy'] )
    file_copy( dummy_file, nclpath )
    printf( "%s���_�~�[�t�@�C������쐬���܂���\n", nclpath )
  end
  
  buf_imdtex << sprintf( "\t%s", imdname )
  buf_ncgimd << sprintf( "\t%s", imdname )
  
  #make object
  dmyimdpath = sprintf( "%s\/%s", dir_res, param['str_imdfile'] )
  
  file_make_ncgimd.printf( "#%s\n", imdname )
  file_make_ncgimd.printf(
    "%s: %s %s\n\t@echo convert ncg_imd %s\n\t@ruby %s %s %s %s %s %s %s\n\n",
    imdpath, ncgpath, nclpath, name,
    path_conv_ncgimd, ncgpath, dir_res, dmyimdpath, 
    param['str_width'], param['str_height'], param['str_count'] )
  return RET_TRUE
end

#=======================================================================
# DRAWTYPE_MDL����
# �߂�l RET_TRUE=���\�[�X��ǉ������B
#=======================================================================
def conv_drawtype_mdl(
  file_listcsv, no, dir_res, sel_res, flag_dummy,
  buf_imd, buf_imdtex, buf_itp, buf_ica, buf_ita, buf_ima, buf_iva )
  
  str = listcsv_getlinestr( file_listcsv, no )
  
  if( str == STR_ERROR )
    return RET_ERROR
  end
  
  if( str == STR_END )
    return RET_ERROR
  end
   
  #imd
  name = str[RBDEF_NUM_MDL_RESFILE_NAME_IMD]
  
  if( name == nil || name =="" )
    return RET_FALSE
  end
  
  if( csvlist_check_repeat_filename(buf_imd,name) == RET_TRUE )
    return RET_TRUE
  end
  
  buf_imd << " \\\n"

  path = sprintf( "%s\/%s", dir_res, name )
  
  if( FileTest.exist?(path) != true ) #�t�@�C�������݂��Ȃ�
    if( flag_dummy == 0 )
      printf( "ERROR %s ������܂���\n", name )
      return RET_ERROR
    end
     
    #�_�~�[�t�@�C���쐬
    imdpath = sprintf( "%s\/%s", dir_res, name )
    file_copy( FPATH_DUMMY_IMD, imdpath )
    buf_imd << sprintf( "\t%s", name )
    
    printf( "%s���_�~�[�t�@�C������쐬���܂���\n", imdpath )
    return RET_TRUE
  end
   
  buf_imd << sprintf( "\t%s", name )
  
  #tex
  name = str[RBDEF_NUM_MDL_RESFILE_NAME_TEX]
  
  if( name != "" && name != nil )
    if( csvlist_check_repeat_filename(buf_imdtex,name) != RET_TRUE )
      path = sprintf( "%s\/%s", dir_res, name )
      p path
      
      if( FileTest.exist?(path) != true ) #�t�@�C�������݂��Ȃ�
        printf( "ERROR %s ������܂���\n", name )
        return RET_ERROR
      end
   
      buf_add_convname( name,
        buf_imdtex, buf_itp, buf_ica, buf_ita, buf_ima, buf_iva )
    end
  end
  
  #anime 0
  name = str[RBDEF_NUM_MDL_RESFILE_NAME_ANM0]
  
  if( name != "" && name != nil )
    if( csvlist_check_repeat_filename(buf_itp,name) != RET_TRUE &&
        csvlist_check_repeat_filename(buf_ica,name) != RET_TRUE &&
        csvlist_check_repeat_filename(buf_ita,name) != RET_TRUE &&
        csvlist_check_repeat_filename(buf_ima,name) != RET_TRUE &&
        csvlist_check_repeat_filename(buf_iva,name) != RET_TRUE )
      path = sprintf( "%s\/%s", dir_res, name )
      
      if( FileTest.exist?(path) != true ) #�t�@�C�������݂��Ȃ�
        printf( "ERROR %s ������܂���\n", name )
        return RET_ERROR
      end
   
      buf_add_convname( name,
        buf_imdtex, buf_itp, buf_ica, buf_ita, buf_ima, buf_iva )
    end
  end
 
  #anime 1
  name = str[RBDEF_NUM_MDL_RESFILE_NAME_ANM1]
  
  if( name != "" && name != nil )
    if( csvlist_check_repeat_filename(buf_itp,name) != RET_TRUE &&
        csvlist_check_repeat_filename(buf_ica,name) != RET_TRUE &&
        csvlist_check_repeat_filename(buf_ita,name) != RET_TRUE &&
        csvlist_check_repeat_filename(buf_ima,name) != RET_TRUE &&
        csvlist_check_repeat_filename(buf_iva,name) != RET_TRUE )
      path = sprintf( "%s\/%s", dir_res, name )
    
      if( FileTest.exist?(path) != true ) #�t�@�C�������݂��Ȃ�
        printf( "ERROR %s ������܂���\n", name )
        return RET_ERROR
      end
   
      buf_add_convname( name,
        buf_imdtex, buf_itp, buf_ica, buf_ita, buf_ima, buf_iva )
    end
  end

  #anime 2
  name = str[RBDEF_NUM_MDL_RESFILE_NAME_ANM2]
  
  if( name != "" && name != nil )
    if( csvlist_check_repeat_filename(buf_itp,name) != RET_TRUE &&
        csvlist_check_repeat_filename(buf_ica,name) != RET_TRUE &&
        csvlist_check_repeat_filename(buf_ita,name) != RET_TRUE &&
        csvlist_check_repeat_filename(buf_ima,name) != RET_TRUE &&
        csvlist_check_repeat_filename(buf_iva,name) != RET_TRUE )
      path = sprintf( "%s\/%s", dir_res, name )
    
      if( FileTest.exist?(path) != true ) #�t�@�C�������݂��Ȃ�
        printf( "ERROR %s ������܂���\n", name )
        return RET_ERROR
      end
   
      buf_add_convname( name,
        buf_imdtex, buf_itp, buf_ica, buf_ita, buf_ima, buf_iva )
    end
  end
  
  return RET_TRUE
end


#=======================================================================
#  ���\�[�X�e�[�u���쐬
#=======================================================================
#---------------------------------------------------------------
# �����擾
#---------------------------------------------------------------
fname_listcsv = ARGV[ARGVNO_FNAME_RESCSV]

if( fname_listcsv == nil )
  printf( "ERROR!! fmmdl_restbl.rb fname_restbl\n" )
  exit 1
end

fname_listcsv_poke = ARGV[ARGVNO_FNAME_RESCSV_POKE]

if( fname_listcsv_poke == nil )
  printf( "ERROR!! fmmdl_restbl.rb fname_restbl\n" )
  exit 1
end

fname_listcsv_mdl = ARGV[ARGVNO_FNAME_RESCSV_MDL]

if( fname_listcsv_mdl == nil )
  printf( "ERROR!! fmmdl_restbl.rb fname_restbl\n" )
  exit 1
end

fname_restbl = ARGV[ARGVNO_FNAME_RESTBL]

if( fname_restbl == nil )
  printf( "ERROR!! fmmdl_restbl.rb fname_restbl\n" )
  exit 1
end

dir_res = ARGV[ARGVNO_DIR_RES] 

if( dir_res == nil )
  printf( "ERROR!! fmmdl_restbl.rb resdir\n" )
  exit 1
end

if( FileTest.directory?(dir_res) != true )
  printf( "ERROR!! fmmdl_restbl.rb resdir\n" )
  exit 1
end

sel_res = ARGV[ARGVNO_FLAG_SELECT_RES]

if( sel_res != "0" && sel_res != "1" )
  printf( "ERROR!! fmmdl_restbl.rb sel_res\n" )
  exit 1
end

fname_prmcsv_ncgimd = ARGV[ARGVNO_FNAME_NCGIMDCSV]

if( fname_prmcsv_ncgimd == nil )
  printf( "ERROR!! fmmdl_restbl.rb fname_prmcsv_ncgimd\n" )
  exit 1
end

fname_restbl_ncgimd = ARGV[ARGVNO_FNAME_NCGIMDLIST]

if( fname_restbl_ncgimd == nil )
  printf( "ERROR!! fmmdl_restbl.rb fname_restbl_ncgimd\n" )
  exit 1
end

fname_mkobj_ncgimd = ARGV[ARGVNO_FNAME_MKNCGIMD]

if( fname_mkobj_ncgimd == nil )
  printf( "ERROR!! fmmdl_restbl.rb fname_mkobj_ncgimd\n" )
  exit 1
end

dir_res_ncgimd = ARGV[ARGVNO_DIR_NCG]

if( dir_res_ncgimd == nil )
  printf( "ERROR!! fmmdl_restbl.rb dir_res_ncgimd\n" )
  exit 1
end

path_conv_ncgimd = ARGV[ARGVNO_PATH_NCGIMDCV]

if( path_conv_ncgimd == nil )
  printf( "ERROR!! fmmdl_restbl.rb path_conv_ncgimd\n" )
  exit 1
end

flag_dummy = ARGV[ARGVNO_FLAG_CREATE_DUMMY]

if( ARGV[ARGVNO_FLAG_CREATE_DUMMY] == "0" )
  flag_dummy = 0
else
  flag_dummy = 1
end

#---------------------------------------------------------------
# �t�@�C���I�[�v��
#---------------------------------------------------------------
file_listcsv = File.open( fname_listcsv, "r" )
file_listcsv_poke = File.open( fname_listcsv_poke, "r" )
file_listcsv_mdl = File.open( fname_listcsv_mdl, "r" )
file_restbl = File.open( fname_restbl, "w" )
file_prmcsv_ncgimd = File.open( fname_prmcsv_ncgimd, "r" )
file_restbl_ncgimd = File.open( fname_restbl_ncgimd, "w" )
file_make_ncgimd = File.open( fname_mkobj_ncgimd, "w" )

#---------------------------------------------------------------
# �t�@�C���o�b�t�@������
#---------------------------------------------------------------
buf_imd = Array.new()
buf_imd << "FMMDL_RESLIST_IMD ="
buf_imdtex = Array.new()
buf_imdtex << "FMMDL_RESLIST_IMDTEX ="
buf_itp = Array.new()
buf_itp << "FMMDL_RESLIST_ITP ="
buf_ica = Array.new()
buf_ica << "FMMDL_RESLIST_ICA ="
buf_ita = Array.new()
buf_ita << "FMMDL_RESLIST_ITA ="
buf_ima = Array.new()
buf_ima << "FMMDL_RESLIST_IMA ="
buf_iva = Array.new()
buf_iva << "FMMDL_RESLIST_IVA ="
buf_ncgimd = Array.new()
buf_ncgimd << "FMMDL_RESLIST_NCGIMD ="

#---------------------------------------------------------------
# �ϐ�������
#---------------------------------------------------------------
no = 0
flag = 0
ret = RET_FALSE

#---------------------------------------------------------------
# �r���{�[�h�ϊ�
#---------------------------------------------------------------
loop{
  type = listcsv_get_drawtype( file_listcsv, no )
  
  case type
  when "DRAWTYPE_NON"
    #���󖳂�
  when "DRAWTYPE_BLACT"
    ret = conv_drawtype_blact(
      file_listcsv, no, dir_res, sel_res, flag_dummy,
      file_prmcsv_ncgimd,
      file_make_ncgimd, dir_res_ncgimd, path_conv_ncgimd,
      buf_imdtex, buf_ncgimd )
  when "DRAWTYPE_MDL"
    #���󖳂�
  when STR_END
    break
  else
    ret = RET_ERROR
  end
  
  if( ret == RET_TRUE ) #���\�[�X����
    flag = 1
  elsif( ret == RET_ERROR ) #�G���[
    printf( "ERROR: fmmdl_restbl.rb %s error text\n", fname_listcsv )
    error_end( fname_restbl, file_restbl,
              file_listcsv, file_listcsv_poke, file_listcsv_mdl,
              file_prmcsv_ncgimd, file_restbl_ncgimd, file_make_ncgimd )
    exit 1
  end
  
  no = no + 1
}

if( flag == 0 ) #���\�[�X��������݂��Ă��Ȃ�
  printf( "fmmdl_restbl.rb ���\�[�X������܂���\n" )
  error_end( fname_restbl, file_restbl,
            file_listcsv, file_listcsv_poke, file_listcsv_mdl,
            file_prmcsv_ncgimd, file_restbl_ncgimd, file_make_ncgimd )
  exit 1
end

#---------------------------------------------------------------
# �|�P�����ϊ�
#---------------------------------------------------------------
no = 0
flag = 0
ret = RET_FALSE

loop{
  type = listcsv_get_drawtype( file_listcsv_poke, no )
  
  case type
  when "DRAWTYPE_NON"
    #���󖳂�
  when "DRAWTYPE_BLACT"
    ret = conv_drawtype_blact(
      file_listcsv_poke, no, dir_res, sel_res, flag_dummy,
      file_prmcsv_ncgimd,
      file_make_ncgimd,
      dir_res_ncgimd + "\/pokemon\/",
      path_conv_ncgimd,
      buf_imdtex, buf_ncgimd )
  when "DRAWTYPE_MDL"
    #���󖳂�
  when STR_END
    break
  else
    ret = RET_ERROR
  end
  
  if( ret == RET_TRUE ) #���\�[�X����
    flag = 1
  elsif( ret == RET_ERROR ) #�G���[
    printf( "ERROR: fmmdl_restbl.rb %s error text\n", fname_listcsv )
    error_end( fname_restbl, file_restbl,
              file_listcsv, file_listcsv_poke, file_listcsv_mdl,
              file_prmcsv_ncgimd, file_restbl_ncgimd, file_make_ncgimd )
    exit 1
  end
  
  no = no + 1
}

if( flag == 0 ) #���\�[�X��������݂��Ă��Ȃ�
  printf( "fmmdl_restbl.rb ���\�[�X�Ώۂ�����܂���\n" )
  error_end( fname_restbl, file_restbl,
            file_listcsv, file_listcsv_poke, file_listcsv_mdl,
            file_prmcsv_ncgimd, file_restbl_ncgimd, file_make_ncgimd )
  exit 1
end

#---------------------------------------------------------------
# ���f���ϊ�
#---------------------------------------------------------------
no = 0
flag = 0
ret = RET_FALSE

loop{
  type = listcsv_get_drawtype( file_listcsv_mdl, no )
  
  case type
  when "DRAWTYPE_NON"
    #���󖳂�
  when "DRAWTYPE_BLACT"
    #�{�����肦�Ȃ�
    printf( "ERROR: fmmdl_restbl.rb %s error text\n", fname_listcsv_mdl )
    error_end( fname_restbl, file_restbl,
            file_listcsv, file_listcsv_poke, file_listcsv_mdl,
            file_prmcsv_ncgimd, file_restbl_ncgimd, file_make_ncgimd )
  when "DRAWTYPE_MDL"
    ret = conv_drawtype_mdl(
      file_listcsv_mdl, no, dir_res, sel_res, flag_dummy,
      buf_imd, buf_imdtex, buf_itp, buf_ica, buf_ita, buf_ima, buf_iva )
  when STR_END
    break
  else
    ret = RET_ERROR
  end
  
  if( ret == RET_TRUE ) #���\�[�X����
    flag = 1
  elsif( ret == RET_ERROR ) #�G���[
    printf( "ERROR: fmmdl_restbl.rb %s error text\n", fname_listcsv )
    error_end( fname_restbl, file_restbl,
            file_listcsv, file_listcsv_poke, file_listcsv_mdl,
            file_prmcsv_ncgimd, file_restbl_ncgimd, file_make_ncgimd )
    exit 1
  end
  
  no = no + 1
}

if( flag == 0 ) #���\�[�X��������݂��Ă��Ȃ�
  printf( "fmmdl_restbl.rb ���\�[�X�Ώۂ�����܂���\n" )
  error_end( fname_restbl, file_restbl,
          file_listcsv, file_listcsv_poke, file_listcsv_mdl,
          file_prmcsv_ncgimd, file_restbl_ncgimd, file_make_ncgimd )
  exit 1
end

#---------------------------------------------------------------
# �f�o�b�O
#---------------------------------------------------------------
=begin
no = 0
max = strbuf_res.size

while no < max
  printf( "buf[%d] = %s\n", no, strbuf_res[no] )
  no = no + 1
end
=end

#---------------------------------------------------------------
# ���X�g��������
#---------------------------------------------------------------
file_restbl.printf( "%s\n\n", buf_imd )
file_restbl.printf( "%s\n\n", buf_imdtex )
file_restbl.printf( "%s\n\n", buf_ica )
file_restbl.printf( "%s\n\n", buf_ima )
file_restbl.printf( "%s\n\n", buf_ita )
file_restbl.printf( "%s\n\n", buf_itp )
file_restbl.printf( "%s\n", buf_iva )
file_restbl_ncgimd.printf( "%s\n", buf_ncgimd )

#---------------------------------------------------------------
# �I��
#---------------------------------------------------------------
file_listcsv.close
file_listcsv_poke.close
file_listcsv_mdl.close
file_restbl.close
file_prmcsv_ncgimd.close
file_restbl_ncgimd.close
file_make_ncgimd.close
