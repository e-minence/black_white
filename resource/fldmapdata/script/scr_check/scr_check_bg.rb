def AddBGLabel(ary, str)
  rc = str.index("ev_bg")
  if rc != nil then
    ary << str
  end
end

def SearchCode(code_ary, str)
  code_ary.each{|i|
    target = str.index(i)
    if target != nil then     #��������
      return true
    end
  }
  return false
end

ev_file = open(ARGV[0],"r")

code_ary = Array.new
code_ary << "_TALK_START_SE_PLAY()"
code_ary << "_EASY_BGWIN_"
code_ary << "_LOOK_ELBOARD"
label = Array.new
dump_ary = Array.new
seq = 0
find = false
while line = ev_file.gets
  str = line.chomp("\n").chomp("\r")

  case seq
  when 0 then
    #_EVENT_DATA_END���o
    data_end = str.index("_EVENT_DATA_END")
    if data_end == nil then
      column = str.split " "
      #�u_EVENT_DATA�v��z��
      idx = str.index("_EVENT_DATA")
      if idx != nil then
        AddBGLabel(label, column[1])
      end
    else
      #_EVENT_DATA_END�����o�����̂ŁA���x�������Ɉڂ�
      seq = 1
#      label.each{ |i|
#        p i
#      }
    end
  when 1 then
    #EVENT_START���o
    ev_start = str.index("EVENT_START")
    if ev_start != nil then
      if ev_start == 0 then
        column = str.split " "
        rc = label.include?(column[1])
        if rc == true then
#          printf("%s�𒲍��J�n\n",column[1])
          search_label = column[1]      #���ׂ郉�x�����L��
          #�ΏۃR�[�h�����V�[�P���X��
          seq = 2
          find = false
        end
      end
    end
  when 2 then
    #EVENT_END���o
    ev_end = str.index("EVENT_END")
    if ev_end != nil then
      if find != true then      #�����R�[�h�����ł��Ȃ�����
#        printf("�����R�[�h���݂���Ȃ�\n")
         dump_ary << search_label    #�Ώۃ��x�����v�b�V�� 
      end
      #�C�x���g�G���h�����������̂ŃV�[�P���X��߂�
      seq = 1
    else
      #�����R�[�h���ׂ�
      if find == false then
        find = SearchCode(code_ary, str)
      end
    end
  end
end

if dump_ary.length != 0 then
  printf("========%s========\n",ARGV[0])
  dump_ary.each{|label|
    p label
  }
end

