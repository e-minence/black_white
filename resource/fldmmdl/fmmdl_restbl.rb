#=======================================================================
# fmmdl_restbl.rb
# フィールド動作モデル リソーステーブル作成
# 
# 引数 fmmdl_restbl.rb
# fname_listcsv fname_restbl res_dir select_res
# fname_prmcsv_ncgimd fname_restbl_ncgimd fname_mkobj_ncgimd
# res_ncg_dir path_conv_ncgimd
# flag_dummy
# fname_listcsv_poke
# 
# fname_listcsv
# 動作モデルリスト表 テキスト変換済みファイル名
# 
# fname_restbl
# 動作モデルリソースファイル名記述先ファイル名
#
# res_dir
# リソースファイルが置かれているディレクトリ
# 
# select_res
# fname_listcsvで指定されている複数のリソースファイルの内、どちらを使うか
# 0=製品反映モデルファイルを使用 1=テスト用を使用
#
# fname_prmcsv_ncgimd
# 動作モデルリスト表 ncg変換対応imd テキスト変換済みファイル名
#
# fname_restbl_ncgimd
# 動作モデル ncg変換imd一覧記述先ファイル名
#
# fname_mkobj_ncgimd
# 動作モデル ncg変換対象make記述先ファイル名
#
# res_ncg_dir
# ncgファイルが置かれているディレクトリ
#
# flag_dummy
# "0" = ファイルが存在していない場合エラー。
# "1" = ファイルが存在していない場合、ダミーファイルから置き換え。
#
# fname_listcsv_poke
# ポケモン用リスト表　テキスト変換済みファイル名
#=======================================================================
$KCODE = "SJIS"
load "rbdefine"

#=======================================================================
#	定数
#=======================================================================
STR_ERROR = "ERROR"
STR_END = "#END"

RET_FALSE = (0)
RET_TRUE = (1)
RET_ERROR = (-1)

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

#=======================================================================
#	異常終了
#=======================================================================
def error_end( path_delfile, file0, file1, file2, file3 )
	file0.close
	file1.close
	file2.close
	file3.close
	File.delete( path_delfile )
end

#=======================================================================
#	ファイルコピー
#=======================================================================
def file_copy( srcpath, copypath )
	open( srcpath ){ |input|
		open( copypath, "w" ){ |output|
			output.write( input.read )
		}
	}
end

#=======================================================================
#	指定番号の","区切り文字列を取得
#	戻り値 nil=無し,STR_ERROR=エラー,STR_END=終了
#=======================================================================
def listcsv_getlinestr( file_listcsv, get_no )
	no = 0
	file_listcsv.pos = 0
	
	line = file_listcsv.gets #一行目飛ばし
  
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
#	指定番号のリソースファイル名を取得
#	戻り値 nil=無し,STR_ERROR=エラー,STR_END=終了
#=======================================================================
def listcsv_get_filename( file_listcsv, no, sel_res )
  xlsline = RBDEF_NUM_RESFILE_NAME_0

  if( sel_res != "0" )
    xlsline = RBDEF_NUM_RESFILE_NAME_1
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
#	指定番号の表示タイプを取得
#	戻り値 nil=無し,STR_ERROR=エラー,STR_END=終了
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
#	指定番号より前リソースファイル名を検索し重複チェック
#	戻り値 RET_TRUE=指定ファイル名が存在している RET_FALSE=無し
#	戻り値 nil=無し,STR_ERROR=エラー,STR_END=終了
#=======================================================================
def csvlist_check_repeat_filename( buf, check_no, check_str )
  no = 0
  max = buf.size

#debug
=begin
  printf( "重複チェック %d以下に%sが在るか探しています\n",
         check_no, check_str )
=end

	while no < check_no
    if( no >= max )
      break
    end
    
    str = buf[no]
    
	  if( str == check_str )
		  return RET_TRUE
		end
		
		no = no + 1
	end
	
	return RET_FALSE
end

#ファイル検索タイプ　激遅
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
    
		if( no < check_no )	#指定位置より前
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
# 指定されたモデルサイズ、アニメーションIDから
# NCG変換時に必要なパラメタをハッシュで返す
# listfile ncg対応表txtファイル
# mdlstr モデルサイズ文字列 MDLSIZE_32x32等
# anmstr アニメID文字列 ANMID_NON等
# 戻り値 nil=なし
#=======================================================================
def ncgparam_get( listfile, mdlstr, anmstr )
  listfile.pos = 0
  listfile.gets #一行抜かし
  listfile.gets #一行抜かし
  
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
# DRAWTYPE_BLACT処理
# 戻り値 RET_TRUE=リソースを追加した。
#=======================================================================
def conv_drawtype_blact(
  file_listcsv, file_restbl, no, dir_res, sel_res, flag_dummy,
  file_prmcsv_ncgimd, file_restbl_ncgimd,
  file_make_ncgimd, dir_res_ncgimd, path_conv_ncgimd,
  strbuf_res, data_no )
	
  imdname = listcsv_get_filename( file_listcsv, no, sel_res )
  
	if( imdname == STR_ERROR )
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
  
  if( csvlist_check_repeat_filename(
      strbuf_res,data_no,imdname) == RET_TRUE )
    return RET_FALSE
  end

	file_restbl.printf( " \\\n" )
  
  str = listcsv_getlinestr( file_listcsv, no )
  mdltype = str[RBDEF_NUM_MDLSIZE]
  anmtype = str[RBDEF_NUM_ANMID]
  param = ncgparam_get( file_prmcsv_ncgimd, mdltype, anmtype )
  
  imdpath = sprintf( "%s\/%s", dir_res, imdname )
  
  #ncg変換対象ではない
  if( param == nil )
		if( FileTest.exist?(imdpath) != true )
		  if( flag_dummy == 0 )
				printf( "ERROR %s がありません\n", imdname )
        return RET_ERROR
      end
      
      #ダミーファイル作成 現状非対応
		  #file_copy( flag_dummy, imdpath )
		  #printf( "%sをダミーファイルから作成しました\n", imdpath )
		  printf( "ERROR %s がありません\n", imdname )
      return RET_ERROR
    end
    
    strbuf_res[data_no] = imdname
    
		file_restbl.printf( "\t%s", str )
    return RET_TRUE
  end
  
  #ncg変換対象
  file_restbl_ncgimd.printf( " \\\n" )
  
  name = imdname.gsub( "\.imd", "" )
  ncgname = name + "\.ncg"
  nclname = name + "\.ncl"
  
  ncgpath = sprintf( "%s\/%s", dir_res_ncgimd, ncgname )
  
	if( FileTest.exist?(ncgpath) != true )
    if( flag_dummy == 0 )
		  printf( "ERROR %s がありません\n", ncgpath )
      return RET_ERROR
    end
    
    dummy_file = sprintf(
      "%s/%s.ncg", dir_res_ncgimd, param['str_flag_dummy'] )
    file_copy( dummy_file, ncgpath )
		printf( "%sをダミーファイルから作成しました\n", ncgpath )
  end
	
  nclpath = sprintf( "%s\/%s", dir_res_ncgimd, nclname )

  if( FileTest.exist?(nclpath) != true )
    if( flag_dummy == 0 )
		  printf( "ERROR %s がありません\n", nclpath )
      return RET_ERROR
    end
    
    dummy_file = sprintf(
      "%s/%s.ncl", dir_res_ncgimd, param['str_flag_dummy'] )
    file_copy( dummy_file, nclpath )
		printf( "%sをダミーファイルから作成しました\n", nclpath )
  end
  
  file_restbl.printf( "\t%s", imdname )
  file_restbl_ncgimd.printf( "\t%s", imdname )
  
  #make object
  dmyimdpath = sprintf( "%s\/%s", dir_res, param['str_imdfile'] )
  
  file_make_ncgimd.printf( "#%s\n", imdname )
  file_make_ncgimd.printf(
    "%s: %s %s\n\t@echo convert ncg_imd %s\n\t@ruby %s %s %s %s %s %s %s\n\n",
    imdpath, ncgpath, nclpath, name,
    path_conv_ncgimd, ncgpath, dir_res, dmyimdpath, 
    param['str_width'], param['str_height'], param['str_count'] )
  
  strbuf_res[data_no] = imdname
  return RET_TRUE
end

#=======================================================================
#	リソーステーブル作成
#=======================================================================
#---------------------------------------------------------------
# 引数取得
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
# ファイルオープン
#---------------------------------------------------------------
file_listcsv = File.open( fname_listcsv, "r" )
file_listcsv_poke = File.open( fname_listcsv_poke, "r" )
file_restbl = File.open( fname_restbl, "w" )
file_prmcsv_ncgimd = File.open( fname_prmcsv_ncgimd, "r" )
file_restbl_ncgimd = File.open( fname_restbl_ncgimd, "w" )
file_make_ncgimd = File.open( fname_mkobj_ncgimd, "w" )

#---------------------------------------------------------------
# 初期化
#---------------------------------------------------------------
no = 0
data_no = 0
flag = 0
ret = RET_FALSE
strbuf_res = Array.new()

#---------------------------------------------------------------
# ビルボード変換
#---------------------------------------------------------------
file_restbl.printf( "FMMDL_RESLIST =" )
file_restbl_ncgimd.printf( "FMMDL_RESLIST_NCGIMD =" )

loop{
  type = listcsv_get_drawtype( file_listcsv, no )
  
  case type
  when "DRAWTYPE_NON"
    #現状無し
  when "DRAWTYPE_BLACT"
    ret = conv_drawtype_blact(
      file_listcsv, file_restbl, no, dir_res, sel_res, flag_dummy,
      file_prmcsv_ncgimd, file_restbl_ncgimd,
      file_make_ncgimd, dir_res_ncgimd, path_conv_ncgimd,
      strbuf_res, data_no )
  when "DRAWTYPE_MDL"
    #現状無し
  when STR_END
    break
  else
    ret = RET_ERROR
  end
  
  if( ret == RET_TRUE ) #リソース存在
    flag = 1
    data_no = data_no + 1
  elsif( ret == RET_ERROR ) #エラー
		printf( "ERROR: fmmdl_restbl.rb %s error text\n", fname_listcsv )
		error_end( fname_restbl, file_listcsv, file_restbl,
              file_prmcsv_ncgimd, file_restbl_ncgimd )
		exit 1
  end
  
	no = no + 1
}

if( flag == 0 ) #リソースが一つも存在していない
	printf( "fmmdl_restbl.rb リソース対象がありません\n" )
  error_end( fname_restbl, file_listcsv, file_restbl,
            file_prmcsv_ncgimd, file_restbl_ncgimd )
	exit 1
end

#---------------------------------------------------------------
# ポケモン変換
#---------------------------------------------------------------
no = 0
flag = 0
ret = RET_FALSE

loop{
  type = listcsv_get_drawtype( file_listcsv_poke, no )
  
  case type
  when "DRAWTYPE_NON"
    #現状無し
  when "DRAWTYPE_BLACT"
    ret = conv_drawtype_blact(
      file_listcsv_poke, file_restbl, no, dir_res, sel_res, flag_dummy,
      file_prmcsv_ncgimd, file_restbl_ncgimd,
      file_make_ncgimd,
      dir_res_ncgimd + "\/pokemon\/",
      path_conv_ncgimd,
      strbuf_res, data_no )
  when "DRAWTYPE_MDL"
    #現状無し
  when STR_END
    break
  else
    ret = RET_ERROR
  end
  
  if( ret == RET_TRUE ) #リソース存在
    flag = 1
    data_no = data_no + 1
  elsif( ret == RET_ERROR ) #エラー
		printf( "ERROR: fmmdl_restbl.rb %s error text\n", fname_listcsv )
		error_end( fname_restbl, file_listcsv_poke, file_restbl,
              file_prmcsv_ncgimd, file_restbl_ncgimd )
		exit 1
  end
  
	no = no + 1
}

if( flag == 0 ) #リソースが一つも存在していない
	printf( "fmmdl_restbl.rb リソース対象がありません\n" )
  error_end( fname_restbl, file_listcsv_poke, file_restbl,
            file_prmcsv_ncgimd, file_restbl_ncgimd )
	exit 1
end

#---------------------------------------------------------------
# デバッグ
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
# 終了
#---------------------------------------------------------------
file_listcsv.close
file_listcsv_poke.close
file_restbl.close
file_prmcsv_ncgimd.close
file_restbl_ncgimd.close
file_make_ncgimd.close
