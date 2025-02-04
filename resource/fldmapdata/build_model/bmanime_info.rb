#------------------------------------------------------------------------------
#
#   配置モデルアニメデータコンバート用スクリプト
#
#   2009.05.11  tamada
#
#------------------------------------------------------------------------------

INPUT_FILE = ARGV[0]
OUTPUT_FILE = ARGV[1]
OUTPUT_HEADER = OUTPUT_FILE.sub(/\.bin/,"\.h")

ANMDATA_OUTPUT_DIR = ARGV[2]

lib_path_naixread = ENV["PROJECT_ROOT"] + "tools/naixread"
require lib_path_naixread

class InputFileError < Exception; end

#------------------------------------------------------------------------------
#
#   配置モデルアニメデータクラス
#
#------------------------------------------------------------------------------
class BmAnime

  @@MAX_FILE = 4

  @@COL_SYMBOL  = 1
  @@COL_ANMTYPE = 2
  @@COL_PROGTYPE = 3
  @@COL_ANMSET = 4
  @@COL_PATTERN_COUNT = 5
  @@COL_FILES = 6

  #アニメタイプ指定(ならびに注意！！）
  @@anime_type = ["NO_ANIME", "ETERNAL", "EVENT", "TIMEZONE"]

  #動作タイプ指定(ならびに注意！！）
  @@program_type = ["NONE", "ELBOARD1", "ELBOARD2"]

  #拡張子変換テーブル
  @@ext_tbl = {
    ".ita" => ".nsbta",
    ".itp" => ".nsbtp",
    ".ica" => ".nsbca",
    ".iva" => ".nsbva",
    ".ima" => ".nsbma"
  }

	attr :symbol
	attr :anm_type
  attr :prog_type
  attr :anmset_num
  attr :pattern_count
  attr :files

  #初期化
	def initialize(line)
    column = line.split(",")

    @symbol = column[@@COL_SYMBOL]

    @anm_type = @@anime_type.index(column[@@COL_ANMTYPE])
    if @anm_type == nil
      raise InputFileError, "アニメID #{@symbol} のアニメタイプ #{column[@@COL_ANMTYPE]} は非対応です"
    end

    @prog_type = @@program_type.index(column[@@COL_PROGTYPE])
    if @prog_type == nil
      raise InputFileError, "アニメID #{@symbol} の動作タイプ \"#{column[@@COL_PROGTYPE]}\" は非対応です"
    end

    @anmset_num = Integer(column[@@COL_ANMSET])

    @pattern_count = Integer(column[@@COL_PATTERN_COUNT])

    @files = column[@@COL_FILES .. (@@COL_FILES + @@MAX_FILE - 1)].find_all{|item|item != ""}.map{|item|
      ext = File.extname(item)
      unless @@ext_tbl.has_key?(ext) then
        raise InputFileError, "#{item} の拡張子 #{ext} には非対応です"
      end
      item.sub(ext, @@ext_tbl[ext])   #拡張子をコンバート後のものに変換
    }
    if @files.length != @anmset_num * @pattern_count then
      raise InputFileError, "#{@symbol}の指定ファイル数（#{@files.length}) != #{@anmset_num} x #{@pattern_count} です"
    end
    
	end


  #以下はクラスメソッド定義
  #------------------------------------------------------------------------------

  #アニメ指定ファイルの最大数
  def BmAnime.MAX_FILE
    @@MAX_FILE
  end

  #ファイルから読み込んでBmAnimeの配列を返す
  def BmAnime.readFile(filename)
    bmarray = Array.new
    names = Hash.new

    # ファイルから読み込み
    File.open(filename){|file|
      file.gets #1行目削除
      file.each{|line|
        anm = BmAnime.new(line.chomp!)
        if names.has_key?(anm.symbol) then
          raise InputFileError, "アニメID #{anm.symbol}が2回定義されています"
        else
          bmarray << anm
        end
      }
    }
    return bmarray
  end

end


#------------------------------------------------------------------------------
#
#     メイン処理
#
#------------------------------------------------------------------------------
begin
  #アニメーションファイルインデックス読み込み
  anime_naix = NAIXReader.read("anime_files/buildmodel_anime.naix")

  #配置モデルアニメーションテーブル読み込み
  bmarray = BmAnime.readFile(INPUT_FILE)

  #参照用ファイル生成：シンボルを定義順で出力
  File.open(OUTPUT_HEADER,"w"){|file|
    bmarray.each_index{|index|
      file.printf("%-20s %3d\n", bmarray[index].symbol, index)
    }
  }

  #バイナリファイル生成：出力データ作成
  str = ""
  count = 0;
  bmarray.each{|anm|

    offset = (4) + (4*BmAnime.MAX_FILE);

    #アニメタイプ、動作指定、アニメカウント、セットカウント
    arr = [anm.anm_type, anm.prog_type, anm.anmset_num, anm.pattern_count]
    #アニメ指定ファイル
    anm.files.each{|file|
      index = anime_naix.getIndex(file.sub(/\./,"_"))
      if index == nil then
          raise InputFileError, "#{anm.symbol} のファイル #{file} が存在しません"
      else
        arr << offset

        binaryfile = file.sub( /ica/, "nsbca" )
        binaryfile = binaryfile.sub( /iva/, "nsbva" )
        binaryfile = binaryfile.sub( /ima/, "nsbma" )
        binaryfile = binaryfile.sub( /itp/, "nsbtp" )
        binaryfile = binaryfile.sub( /ita/, "nsbta" )
        
        #そのファイルのサイズからオフセットを求める。
        offset += File.size( "anime_files/#{binaryfile}" )
      end
    }
    #アニメ指定ファイルが足りない部分をダミーで追加
    (BmAnime.MAX_FILE - anm.files.length).times do |i|
      arr << 0xffffffff
    end

    #1アニメデータをanmdatに出力
    File.open( "#{ANMDATA_OUTPUT_DIR}/anmdat_#{count}.dat", "wb" ){|write_file|

      write_file.write(arr.pack("C C C C L4"))

      #各アニメbinaryを配置
      anm.files.each{|anime_file|

        binaryfile = anime_file.sub( /ica/, "nsbca" )
        binaryfile = binaryfile.sub( /iva/, "nsbva" )
        binaryfile = binaryfile.sub( /ima/, "nsbma" )
        binaryfile = binaryfile.sub( /itp/, "nsbtp" )
        binaryfile = binaryfile.sub( /ita/, "nsbta" )

        #ファイル名チェック
        if binaryfile =~ /nsbca/ then
        elsif binaryfile =~ /nsbva/ then
        elsif binaryfile =~ /nsbma/ then
        elsif binaryfile =~ /nsbtp/ then
        elsif binaryfile =~ /nsbta/ then
        else
          puts( "pack #{binaryfile}\n" )
          exit(1)
        end
        

        File.open( "anime_files/#{binaryfile}", "rb" ){|anime_readfile|
          write_file.write( anime_readfile.read() )
        }
      }
    }
    
    count += 1
    str += arr.pack("C C C C L4")
  }
  #バイナリファイル生成：出力
  File.open(OUTPUT_FILE,"wb"){|file| file.write(str)}

end




