#----------------------------------------------------------------------------
#  overlaytool.rb
#  afetr�𕡐��w��ł���悤�Ƀp�b�`�C��  2009.01.16 k.ohno
#  TWL��p�ɉ���                          2009.04.15 k.ohno
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
MATCH_OVERLAYSRC = /^SRCS_OVERLAY_.*/	# �s������uSRCS_OVERLAY_...�v
MATCH_ENDSRC	 = /[\s\t].*\\\z/				# �s�����u\�v
MATCH_ERROR      = /\\\s/

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

Autoload MCS_EX
{
  Address		0x02400000
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
  Object		* (.ltdmain)
	Object		$(OBJS_LTDAUTOLOAD)
	Library		$(LLIBS_EX) $(GLIBS_EX)
}

BOTTOM_LSFFILE2




overlay_count        =  0
overlay_table_num    =  0
overlay_name		= ["main", "ITCM", "MCS_EX"]
overlay_target		= ["main", "ITCM", "MCS_EX"]
source_name          = Array.new
lib_name          = Array.new
overlay_hash		= { "main" => 1, "ITCM" => 1, "MCS_EX" => 1 }
overlay_flag        = 0

line_get_flag     = 0

line_count        = 1

afterList = []
overlayList = []
afteroverlaycnt = 0

USERNAME_SKIP = 1     #���߂��Ȃ�
USERNAME_INSERT = 0   #���߂���
USERNAME_NONE = -1     #���[�U�[�l�[����`�͂܂��Ȃ�
## ���[�U�[����`���������ꍇ��1 �L�����Ă���
usernamestate = USERNAME_NONE


File.open(OUTPUT_LSFFILE,"w"){|file|
  file.puts(default_lsf)
  file.puts("#�������牺��overlaytool.rb�Ŏ�����������Ă��܂�\n\n")
}


#�^�[�Q�b�g�ɂȂ�\�[�X�R�[�h�̋L�q�t�@�C�����P�s������
File.readlines(MAKE_PROG_FILE).each{ |line|
  line.chomp!

  #�s���́u\�v�̌��ɃX�y�[�X�������Ă���ƌ듮�삷��̂Ń`�F�b�N����
  if line.match(MATCH_ERROR) then
    printf "%d�s�ځF \\�}�[�N�̌��ɃX�y�[�X���͂����Ă��܂��B\n",line_count
    exit 1
  end
  line_count = line_count + 1
  
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
    if afteroverlaycnt != 0 then
      m = MATCH_KEYWORD.match(line)
      afterList[afteroverlaycnt-1] = m.post_match.split(nil)[0].sub(/[\s\t]/,"")
      afteroverlaycnt = afteroverlaycnt + 1
    else
      afteroverlaycnt = 1   ##ohno
      overlay_flag = 1
      m = MATCH_KEYWORD.match(line)
      _str = m.post_match.split(nil)[0].sub(/[\s\t]/,"")

      if overlay_hash.key?(_str) then
        overlay_target << _str
        overlay_count = overlay_count+1
      else
        printf "�u%s�v �Ƃ����I�[�o�[���C�\\�[�X�͒�`����Ă��܂���\n",_str
        exit 1
      end
    end
  end

  #�uSRCS_OVERLAY�v�̌�̓\�[�X�R�[�h�s�Ȃ̂Łu\�v���Ȃ��Ȃ�܂�
  # �ۑ���������
  if line_get_flag==1 then
    if line.match(/[a-zA-Z_0-9]+\.[cs]/) then
      source_name << line
    end

    # �u\�v�������s���łĂ�����I���Ȃ̂ŁA�擾�I��
    # lsf�t�@�C���ɏ������݂��s��
    if line.match(MATCH_ENDSRC)==nil || line=="" then
      #			p source_name
      line_get_flag = 0
      _count = 0
      File.open(OUTPUT_LSFFILE,"a"){|file|


        # �uSRCS_OVERLAY_???�v����SRCS_OVERLAY_������ď��������������O���I�[�o�[���C�̈��
        # �^�[�Q�b�g�ɂ���B�������A�\���Ƃ��āumain,ITCM,DTCM�v�Ƃ��������񂪒�`����Ă���̂�
        # �����͔�����悤�ɂ���
        _targetname = overlay_target[overlay_table_num+2]
        if _targetname != "main" && _targetname!="MCS_EX" && _targetname != "ITCM" then
          _targetname = _targetname.downcase.sub(/srcs_overlay_/,"")
        end
        ##�I�[�o�[���C�̃��X�g��S������Ă���
        overlayList.push(overlay_name[overlay_table_num+2].downcase.sub(/srcs_overlay_/,""))
        
        file.printf("Overlay %s\n{\n\tAfter\t%s\n",
        overlay_name[overlay_table_num+2].downcase.sub(/srcs_overlay_/,""),
        _targetname
        )
        if afteroverlaycnt != 0 then
          afterList.each { |afline|
            file.printf("\tAfter\t%s\n",afline.downcase.sub(/srcs_overlay_/,""))
          }
        end
        file.printf("\tObject")
        source_name.each{ |name|
          filename = name.slice(/[a-zA-Z_0-9]+\./)
          if _count != 0 then
            file.printf("\t")
          end
          file.printf("\t$(OBJDIR)/%so",filename)
          if name.match(/\\\z/) then
            file.printf(" \\")
          end
          file.puts("\n")
          _count = _count+1
        }

        if lib_name.length > 0 then
          file.printf("\tLibrary")
          local_c = 0
          lib_name.each{ |name|
            local_c = local_c + 1
            filename = name
            file.printf("\t%s",filename)
            if local_c != lib_name.length  then
              file.printf(" \\")
            end
            file.puts("\n")
          }
          lib_name.clear
        end
        file.puts("}\n\n")
        afteroverlaycnt = 0   ##ohno
        afterList = []
      }
    end
  end

  #�I�[�o�[���C�\�[�X���̎擾
  if line.match(MATCH_OVERLAYSRC) then
    line_get_flag = 1

    if overlay_flag==0 then
      printf "Error!!\n"
      printf "#===>�ŃI�[�o�[���C�A�h���X���w�肵�Ă��Ȃ��̂ɁA\n"
      printf "�I�[�o�[���C�p�̃\\�[�X�R�[�h���L�q����Ă��܂�\n"
      exit 1
    end

    overlay_name << line.split(nil)[0]
    overlay_hash[line.split(nil)[0]] = 1

    source_name = Array.new(0)
    source_name << line

    overlay_table_num = overlay_table_num + 1
  end

  if line.match(MATCH_LIB) then
    line =~ /(\S+)\s+(\S+)/
    lib_name << $2
  end

}

#�I�[�o�[���C�ɊY������t�@�C�����R���p���^�[�Q�b�g�ɂȂ�悤�ɂ���
File.open(OUTPUT_OVERLAYFILES,"w"){|file|
  for i in 3..overlay_name.size-1
    if i==3 then
      file.printf("SRCS_OVERLAY\t=\t")
    end
    file.printf("\t\t\t$(%s)",overlay_name[i])
    if i!=overlay_name.size-1 then
      file.printf("\t\\\n",overlay_name[i])
    else
      file.printf("\n")
    end
  end

}

##TWL�p�t�@�C�����쐬���镔��

copycmd = "cp " + OUTPUT_LSFFILE+" "+OUTPUT_TWL_LSFFILE
system(copycmd)

fh = File.open(OUTPUT_TWL_LSFFILE,"a")
fh.puts(bottom_lsf1)
overlayList.each{ |line|
  fh.printf("\tAfter\t%s\n",line )
}
fh.puts(bottom_lsf2)
fh.close



#�I�[�o�[���C�̃^�[�Q�b�g�l�[���Ƃ��̔ԍ����e�L�X�g�ɓf���o��
File.open(OUTPUT_OVERLAYTEXT,"w"){|file|
  for i in 0..overlay_count-1
    file.printf( " ID 0x%x = %s\n",i,overlay_name[i+3].downcase.sub(/srcs_overlay_/,""));
  end
}

#p overlay_count
#p overlay_table_num

if overlay_count != overlay_table_num then
  printf "'Error!!!\n"
  printf "===>            =  %d ��\n",overlay_count
  printf "SRCS_OVERLAY_?  =  %d ��\n",overlay_table_num
  printf "'===>'�Ŏw�肷��I�[�o�[���C�^�[�Q�b�g�̉񐔂�\n�I�[�o�[���C�w��̃\\�[�X�R�[�h�Q�̐��������Ă��܂���\n"
  exit 1
else
  printf "�I�[�o�[���C�c�[������I��\n"
end



#p overlay_name
#p overlay_target

