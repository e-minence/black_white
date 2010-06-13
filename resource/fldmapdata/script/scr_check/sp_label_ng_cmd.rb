def AddLabel(ary, str, target_label)
  rc = str.index(target_label)
  if rc != nil then
    ary << str
  end
end

def SearchCode(code_ary, str, find_code_ary)
  code_ary.each{|i|
    target = str.index(i)
    if target != nil then     #��������
      find_code_ary << i
      return true
    end
  }
  return false
end

def MakeSearchCodeList(list_txt, code_ary)
  list_file = open(list_txt,"r")
  while line = list_file.gets
    str = line.chomp("\n").chomp("\r")
    code_ary << str
  end
end

ev_file = open(ARGV[0],"r")

list_txt = ARGV[1]

target_label = ARGV[2]

#�����������R�[�h�̃��X�g�쐬
code_ary = Array.new
MakeSearchCodeList(list_txt, code_ary)

label = Array.new
dump_ary = Array.new
find_code_ary = Array.new

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
        AddLabel(label, column[1], target_label)
      end
    else
      #_EVENT_DATA_END�����o�����̂ŁA���x�������Ɉڂ�
      seq = 1
#      label.each{ |i|
#        p i
#      }
    end
  when 1 then
    #INIT_EVENT_START���o
    ev_start = str.index("INIT_EVENT_START")
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
    #INIT_EVENT_END���o
    ev_end = str.index("INIT_EVENT_END")
    if ev_end != nil then
      if find != false then      #�����R�[�h��������
#        printf("�����R�[�h���݂�����\n")
         dump_ary << search_label    #�Ώۃ��x�����v�b�V��
      end
      #�C�x���g�G���h�����������̂ŃV�[�P���X��߂�
      seq = 1
    else
      #�����R�[�h���ׂ�
      if find == false then
        find = SearchCode(code_ary, str, find_code_ary)
      end
    end
  end
end

if dump_ary.length != 0 then
  printf("========%s========\n",ARGV[0])
  dump_ary.each_with_index{|label, idx|
    printf("%s ==> %s\n",label, find_code_ary[idx])
  }
end


