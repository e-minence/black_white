#------------------------------------------------------------------------------
#
#   �z�u���f���A�j���f�[�^�R���o�[�g�p�X�N���v�g
#
#   2009.05.11  tamada
#
#------------------------------------------------------------------------------

INPUT_FILE = ARGV[0]
OUTPUT_FILE = ARGV[1]
OUTPUT_HEADER = OUTPUT_FILE.sub(/\.bin/,"\.h")

lib_path_naixread = ENV["PROJECT_ROOT"] + "tools/naixread"
require lib_path_naixread

class InputFileError < Exception; end

#------------------------------------------------------------------------------
#
#   �z�u���f���A�j���f�[�^�N���X
#
#------------------------------------------------------------------------------
class BmAnime

  @@MAX_FILE = 4

  @@COL_SYMBOL  = 1
  @@COL_ANMTYPE = 2
  @@COL_PROGTYPE = 3
  @@COL_ANMCOUNT = 4
  @@COL_SETCOUNT = 5
  @@COL_FILES = 6

  #�A�j���^�C�v�w��(�Ȃ�тɒ��ӁI�I�j
  @@anime_type = ["NO_ANIME", "ETERNAL", "EVENT", "TIMEZONE"]

  #����^�C�v�w��(�Ȃ�тɒ��ӁI�I�j
  @@program_type = ["NONE", "ELBOARD1", "ELBOARD2"]

  #�g���q�ϊ��e�[�u��
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
  attr :anime_count
  attr :set_count
  attr :files

  #������
	def initialize(line)
    column = line.split(",")

    @symbol = column[@@COL_SYMBOL]

    @anm_type = @@anime_type.index(column[@@COL_ANMTYPE])
    if @anm_type == nil
      raise InputFileError, "�A�j��ID #{@symbol} �̃A�j���^�C�v #{column[@@COL_ANMTYPE]} �͔�Ή��ł�"
    end

    @prog_type = @@program_type.index(column[@@COL_PROGTYPE])
    if @prog_type == nil
      raise InputFileError, "�A�j��ID #{@symbol} �̓���^�C�v \"#{column[@@COL_PROGTYPE]}\" �͔�Ή��ł�"
    end

    @anime_count = Integer(column[@@COL_ANMCOUNT])

    @set_count = Integer(column[@@COL_SETCOUNT])

    @files = column[@@COL_FILES .. (@@COL_FILES + @@MAX_FILE - 1)].find_all{|item|item != ""}.map{|item|
      ext = File.extname(item)
      unless @@ext_tbl.has_key?(ext) then
        raise InputFileError, "#{item} �̊g���q #{ext} �ɂ͔�Ή��ł�"
      end
      item.sub(ext, @@ext_tbl[ext])   #�g���q���R���o�[�g��̂��̂ɕϊ�
    }
	end


  #�ȉ��̓N���X���\�b�h��`
  #------------------------------------------------------------------------------

  #�A�j���w��t�@�C���̍ő吔
  def BmAnime.MAX_FILE
    @@MAX_FILE
  end

  #�t�@�C������ǂݍ����BmAnime�̔z���Ԃ�
  def BmAnime.readFile(filename)
    bmarray = Array.new
    names = Hash.new

    # �t�@�C������ǂݍ���
    File.open(filename){|file|
      file.gets #1�s�ڍ폜
      file.each{|line|
        anm = BmAnime.new(line.chomp!)
        if names.has_key?(anm.symbol) then
          raise InputFileError, "�A�j��ID #{anm.symbol}��2���`����Ă��܂�"
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
#     ���C������
#
#------------------------------------------------------------------------------
begin
  #�A�j���[�V�����t�@�C���C���f�b�N�X�ǂݍ���
  anime_naix = NAIXReader.read("anime_files/buildmodel_anime.naix")

  #�z�u���f���A�j���[�V�����e�[�u���ǂݍ���
  bmarray = BmAnime.readFile(INPUT_FILE)

  #�Q�Ɨp�t�@�C�������F�V���{�����`���ŏo��
  File.open(OUTPUT_HEADER,"w"){|file|
    bmarray.each_index{|index|
      file.printf("%-20s %3d\n", bmarray[index].symbol, index)
    }
  }

  #�o�C�i���t�@�C�������F�o�̓f�[�^�쐬
  str = ""
  bmarray.each{|anm|
    #�A�j���^�C�v�A����w��A�A�j���J�E���g�A�Z�b�g�J�E���g
    arr = [anm.anm_type, anm.prog_type, anm.anime_count, anm.set_count]
    #�A�j���w��t�@�C��
    anm.files.each{|file|
      index = anime_naix.getIndex(file.sub(/\./,"_"))
      if index == nil then
          raise InputFileError, "#{anm.symbol} �̃t�@�C�� #{file} �����݂��܂���"
      else
        arr << index
      end
    }
    #�A�j���w��t�@�C��������Ȃ��������_�~�[�Œǉ�
    (BmAnime.MAX_FILE - anm.files.length).times do |i|
      arr << 0xffff
    end
  
    str += arr.pack("C C C C S4")
  }
  #�o�C�i���t�@�C�������F�o��
  File.open(OUTPUT_FILE,"wb"){|file| file.write(str)}

end




