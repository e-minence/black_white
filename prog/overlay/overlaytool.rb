#----------------------------------------------------------------------------
#  overlaytool.rb
#  after�𕡐��w��ł���悤�Ƀp�b�`�C��  2009.01.16 k.ohno
#  TWL��p�ɉ���                          2009.04.15 k.ohno
#  �g���������Ή��ׂ̈Ƀ\�[�X����         2010.01.13 tamada
#  �I�[�o�[���C�O���[�v�L�q�ɑΉ�         2010.02.22 tamada
#  CRYPTO�̈ꕔ���풓��                   2010.06.01 k.ohno
#----------------------------------------------------------------------------


#----------------------------------------------------------------------------
#
#       �萔��`
#
#----------------------------------------------------------------------------

#DEFAULT_LSFFILE		= "default.lsf"
OVERLAY_DIR			= "overlay"
MAKE_PROG_FILE  	= "make_prog_files"
OUTPUT_LSFFILE  	= OVERLAY_DIR + "/main.lsf"
OUTPUT_TWL_LSFFILE  	= OVERLAY_DIR + "/main.TWL.HYB.lsf"
OUTPUT_OVERLAYFILES	= OVERLAY_DIR + "/overlay_files"
OUTPUT_OVERLAYTEXT  = OVERLAY_DIR + "/overlaymap.txt"

MATCH_KEYWORD	 = /\A#===>/			# �s�����u#===>�v
MATCH_LIB        = /^#LIB=>/            # ���C�u�������܂߂�ꍇ
MATCH_GROUP     = /^#GROUP_OVERLAY_/
MATCH_OVERLAYSRC = /^SRCS_OVERLAY_.*/	# �s������uSRCS_OVERLAY_...�v
MATCH_ENDSRC	 = /[\s\t].*\\\z/				# �s�����u\�v
MATCH_ERROR      = /\\\s/

DEFAULT_SECTION_NAME		= ["main", "ITCM", "EX_MEM_START", "LCDC_VRAM_H"]

#�f�t�H���g�œǂݍ��ރt�@�C��
#default_lsf = File.read(DEFAULT_LSFFILE)
#
default_lsf = <<DEFAULT_LSFFILE

#----------------------------------------------------------------------------
#
#  Nitro LCF SPEC FILE
#
#----------------------------------------------------------------------------

Static main
{
  Address		$(ADDRESS_STATIC)
  Object		$(OBJS_STATIC)
  Library		$(LLIBS) $(GLIBS) $(CW_LIBS)
  
  #CRYPTO�̈ꕔ���풓��
  Object  OBJECT(CRYPTO_RC4Init, libcrypto.$HYB.a) (.text)
  Object  OBJECT(CRYPTO_RC4Encrypt, libcrypto.$HYB.a) (.text)

  #	�X�^�b�N�T�C�Y�@IRQ�X�^�b�N�T�C�Y
  #	(�ȗ����ɂ̓X�^�b�N�T�C�Y��0(�ő���x�܂Ŋm��)�AIRQ �X�^�b�N�T�C�Y�� 0x800 �o�C�g)
  StackSize	0x1800 0x800
}

Autoload ITCM
{
  Address		0x01ff8000
  Object		* (.itcm)
}

Autoload DTCM
{
  Address		$(ADDRESS_DTCM)
  Object		* (.dtcm)
}

Autoload EX_MEM_START
{
  Address		$EX_MEM_START_ADRS
}

Autoload  LCDC_VRAM_H
{
  Address 0x06898000
}

#----------------------------------------------------------------------------
#  �I�[�o�[���C�ݒ�
#----------------------------------------------------------------------------

DEFAULT_LSFFILE


##
## TWL��`
##
bottom_lsf1 = <<BOTTOM_LSFFILE1


Ltdautoload LTDMAIN
{
	# NITRO/TWL ���L�̃I�[�o�[���C���݂�ꍇ�́A����ɂ��̌��ɔz�u����K�v������܂��B
BOTTOM_LSFFILE1
 
bottom_lsf2 = <<BOTTOM_LSFFILE2
	Address	0x02700000
  Object		* (.ltdmain)
	Object		$(OBJS_LTDAUTOLOAD)
	Library		$(LLIBS_EX) $(GLIBS_EX)
}

BOTTOM_LSFFILE2


#----------------------------------------------------------------------------
#
#
#
#----------------------------------------------------------------------------
class OverlayInfo

  attr_reader :name, :put_name, :afterList, :source_name, :lib_name, :group_name

  def initialize( name, afterList, source_name, lib_name, group_name )
    @name = name
    @put_name = @name.downcase.sub(/srcs_overlay_/,"")
    @afterList = afterList
    @source_name = source_name
    @lib_name = lib_name
    @group_name = group_name
  end

  def listup_group_sections( sections, group_name )
    sections.map{|section|
      #p section.group_name
      if section.group_name == group_name then
        #puts "HIT! #{group_name} in #{section.put_name}"
        section.put_name
      end
    }
  end

  def putAfters( afterList )
    output = ""
    afterList.each{|afline|
      if afline == nil then next end
      if DEFAULT_SECTION_NAME.include?( afline ) == false then
        afline = afline.downcase.sub(/srcs_overlay_/,"")
      end
      output += sprintf( "\tAfter\t%s\n", afline )
    }
    return output
  end

  def putOverlaySection( sections )
    
    output = sprintf("Overlay %s\n{\n", @put_name )

    # �uSRCS_OVERLAY_???�v����SRCS_OVERLAY_������ď��������������O���I�[�o�[���C�̈��
    # �^�[�Q�b�g�ɂ���B�������A�\���Ƃ��āumain,ITCM,DTCM�v�Ƃ��������񂪒�`����Ă���̂�
    # �����͔�����悤�ɂ���
    @afterList.each { |afline|
      #�O���[�v���̏ꍇ�A��͂���
      if afline =~/^GROUP_OVERLAY_/ then
        lists =listup_group_sections( sections, afline )
        #p afline
        #p lists
        output += putAfters( lists )
      else
        if DEFAULT_SECTION_NAME.include?( afline ) == false then
          afline = afline.downcase.sub(/srcs_overlay_/,"")
        end
        output += sprintf( "\tAfter\t%s\n", afline )
      end
    }
    output += sprintf("\tObject")
    @source_name.each_with_index{ | name, index |
      filename = name.slice(/[a-zA-Z_0-9]+\./)
      if index != 0 then
        output += sprintf("\t")
      end
      output += sprintf("\t$(OBJDIR)/%so",filename)
      if name.match(/\\\z/) then
        output += sprintf(" \\")
      end
      output += sprintf("\n")
    }

    if @lib_name.length > 0 then
      output += sprintf("\tLibrary")
      @lib_name.each_with_index{ |name, index|
        filename = name
        output += sprintf("\t%s",filename)
        if index + 1 != @lib_name.length  then
          output += sprintf(" \\")
        end
        output += sprintf("\n")
      }
    end
    output += sprintf("}\n\n")
    return output
  end

end




#----------------------------------------------------------------------------
#
#�^�[�Q�b�g�ɂȂ�\�[�X�R�[�h�̋L�q�t�@�C�����P�s������
#
#----------------------------------------------------------------------------
sections = Array.new

section_name  = DEFAULT_SECTION_NAME.dup
source_name   = nil
group_name    = nil
lib_name      = Array.new
afterList     = []

USERNAME_SKIP = 1     #���߂��Ȃ�
USERNAME_INSERT = 0   #���߂���
USERNAME_NONE = -1     #���[�U�[�l�[����`�͂܂��Ȃ�
## ���[�U�[����`���������ꍇ��1 �L�����Ă���
usernamestate = USERNAME_NONE



File.readlines(MAKE_PROG_FILE).each_with_index{ |line, line_count|
  line.chomp!

  #�s���́u\�v�̌��ɃX�y�[�X�������Ă���ƌ듮�삷��̂Ń`�F�b�N����
  if line.match(MATCH_ERROR) then
    printf "%d�s�ځF \\�}�[�N�̌��ɃX�y�[�X���͂����Ă��܂��B\n",line_count + 1
    exit 1
  end
  
  #�v���v���Z�b�T���ߕ���
  if usernamestate == USERNAME_NONE
    if line =~ /^#if\s+(.*)$/
      $1.split('|').each{|usernameline|
        if usernameline == ENV["USERNAME"]  #
          usernamestate = USERNAME_INSERT
        end
      }
      if usernamestate == USERNAME_NONE
        usernamestate = USERNAME_SKIP
      end
    end
  else
    if line =~ /^#else$/
      usernamestate = 1 - usernamestate  # ���]
    elsif line =~ /^#endif$/
      usernamestate = USERNAME_NONE
    end
  end
  if usernamestate == USERNAME_SKIP  ## ���̒�`�̏ꍇ�������Ȃ�
    next
  end
  
  #�I�[�o�[���C��`�̎擾
  if line.match(MATCH_KEYWORD) then
    m = MATCH_KEYWORD.match(line)
    _str = m.post_match.split.first.sub(/[\s\t]/,"")
    if section_name.include?(_str) == true then
      afterList << m.post_match.split.first.sub(/[\s\t]/,"")
    elsif _str =~/^GROUP_OVERLAY_/
      afterList << _str
    else
      p _str
      printf "�u%s�v �Ƃ����I�[�o�[���C�\\�[�X�͒�`����Ă��܂���\n",_str
      p section_name
      exit 1
    end
    next
  end

  #�I�[�o�[���C�z�u���C�u�������̎擾
  if line.match(MATCH_LIB) then
    line =~ /(\S+)\s+(\S+)/
    lib_name << $2
    next
  end

  #�O���[�v������`�̎擾
  if line.match(MATCH_GROUP) then
    name = line.chomp.sub(MATCH_GROUP, "GROUP_OVERLAY_")
    if group_name == nil then
      group_name = name
      next
    else
      printf "Error!!\n"
      printf "�ЂƂ̃I�[�o�[���C�Z�N�V�����ɕ����̃O���[�v��`������܂��I\n"
      printf "(%s) (%s)\n", group_name, name 
      exit 1
    end
  end

  #��L�ӊO�́�����n�܂�s�̓R�����g�Ƃ݂Ȃ�
  if line =~/^#/ then
    next
  end

  #�I�[�o�[���C�Z�N�V�������̎擾
  if line.match(MATCH_OVERLAYSRC) then

    if afterList.length == 0 then
      printf "Error!!\n"
      printf "#===>�ŃI�[�o�[���C�A�h���X���w�肵�Ă��Ȃ��̂ɁA\n"
      printf "�I�[�o�[���C�p�̃\\�[�X�R�[�h���L�q����Ă��܂�\n"
      exit 1
    end

    section_name << line.split.first
    source_name = [ line ]
    next
  end

  #�uSRCS_OVERLAY�v�̌�̓\�[�X�R�[�h�s�Ȃ̂Łu\�v���Ȃ��Ȃ�܂�
  # �ۑ���������
  if source_name != nil then
    if line.match(/[a-zA-Z_0-9]+\.[cs]/) then
      source_name << line
    end

    # �u\�v�������s���łĂ�����I���Ȃ̂ŁA�擾�I��
    if line.match(MATCH_ENDSRC)==nil || line=="" then
      sections << OverlayInfo.new( section_name.last, afterList, source_name, lib_name, group_name )
      source_name = nil
      group_name = nil
      lib_name = []
      afterList = []
    end
  end

}


#----------------------------------------------------------------------------
#
#  �ǂݍ��݃f�[�^�����߂��ďo��
#
#----------------------------------------------------------------------------
total_output = ""
total_output += default_lsf
total_output += "#�������牺��overlaytool.rb�Ŏ�����������Ă��܂�\n\n"
sections.each{|overlay_info|
  total_output += overlay_info.putOverlaySection( sections )
}
total_output.gsub!("$EX_MEM_START_ADRS","0x02400000")

#NITRO�p�t�@�C�����쐬���镔��
File.open(OUTPUT_LSFFILE, "w" ){|file|
  #NITRO��LSF�̃t�@�C����$LIB��ǂݑւ���
  file.puts total_output.gsub(".$HYB","")
}

##TWL�p�t�@�C�����쐬���镔��
File.open(OUTPUT_TWL_LSFFILE, "w" ) {|file|
  #TWL��LSF�t�@�C����$LIB��ǂݑւ���
  file.puts total_output.gsub("$HYB","TWL.HYB")
  file.puts(bottom_lsf1)
  sections.each{|section|
    if section.afterList.include?("EX_MEM_START") == false && section.afterList.include?("LCDC_VRAM_H") == false then
#      file.printf("\tAfter\t%s\n",section.put_name )
    end
  }
  file.puts(bottom_lsf2)
}

#�I�[�o�[���C�ɊY������t�@�C�����R���p�C���^�[�Q�b�g�ɂȂ�悤�ɂ���
File.open(OUTPUT_OVERLAYFILES,"w"){|file|
  file.printf("SRCS_OVERLAY\t=\t")
  sections.each{|section|
    file.printf( "\t\t\t$(%s)",section.name )
    if section != sections.last then
      file.printf("\t\\\n")
    else
      file.printf("\n")
    end

  }
}

#�I�[�o�[���C�̃^�[�Q�b�g�l�[���Ƃ��̔ԍ����e�L�X�g�ɓf���o��
File.open(OUTPUT_OVERLAYTEXT,"w"){|file|
  sections.each_with_index{|section, index|
    file.printf( " ID 0x%x = %s\n",index, section.put_name )
  }
}

