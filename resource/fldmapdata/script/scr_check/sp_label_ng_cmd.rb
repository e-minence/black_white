require "call_label.rb"

$KCODE = "SJIS"

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
      if find_code_ary != nil then
        find_code_ary << i
      end  
      return true
    end
  }
  return false
end

def MakeSearchCodeList(list_txt, code_ary)
  list_file = open(list_txt,"r")
  while line = list_file.gets
    base_str = line.chomp("\n").chomp("\r")
    rc = base_str.include?("\(")
    if rc == true then
      m = /\(/.match(base_str)
      str = m.pre_match
    else
      str = base_str
    end
    code_ary << str
  end

#  code_ary.each{|i|
#    p i
#  }
end



ev_file_name = ARGV[0]
s = SubLabelHash.new
hash = s.MakeHash(ev_file_name)

ev_file = open(ev_file_name,"r")

list_txt = ARGV[1]

target_label = ARGV[2]

label_type = ARGV[3]
if label_type == "SP" then
  EventStart = "INIT_EVENT_START"
  EventEnd = "INIT_EVENT_END"
  NgEnd = "EVENT_END"
else
  EventStart = "EVENT_START"
  EventEnd = "EVENT_END"
  NgEnd = "INIT_EVENT_END"
end


#�����������R�[�h�̃��X�g�쐬
code_ary = Array.new
MakeSearchCodeList(list_txt, code_ary)

label = Array.new
dump_ary = Array.new
find_code_ary = Array.new
call_find_code_ary = Array.new
seq = 0
find = false
endjump = false
while line = ev_file.gets
  str = line.chomp("\n").chomp("\r")

  case seq
  when 0 then
    #_EVENT_DATA_END���o
    data_end = str.index("_EVENT_DATA_END")
    if data_end == nil then
      column = str.split "\s"
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
    #START���o
    ev_start = str.index(EventStart)
    fix_str = Marshal.load(Marshal.dump(str))
    fix_str.gsub!(/\s/,"")
    ev_ng_end = fix_str.index(EventEnd)
    if ev_ng_end != nil then
      if ev_ng_end == 0 then
        if endjump == false then
          printf("START���o����END�����o�@file:%s label:%s\n",ev_file_name, column[1])
          exit(-1)
        else
          #�W�����v����
          endjump = false
#          printf("%s �́@��ΏۂȂ̂ŃW�����v\n",column[1])
        end
      end
    end

    if ev_start != nil then
      if ev_start == 0 then
        column = str.split "\s"
        rc = label.include?(column[1])
        if rc == true then
#          printf("%s�𒲍��J�n\n",column[1])
          search_label = column[1]      #���ׂ郉�x�����L��
          #�ΏۃR�[�h�����V�[�P���X��
          seq = 2
          find = false
        else
          #��Ώۃ��x��
          endjump = true
        end
      end
    end
  when 2 then
    fix_str = Marshal.load(Marshal.dump(str))
    fix_str.gsub!(/\s/,"")
    ev_ng_end = fix_str.index(NgEnd)
    if ev_ng_end != nil then
      if ev_ng_end == 0 then
        printf("�Ή����Ă��Ȃ��R�[�h�����o�@%s\n",search_label)
        exit(-1)
      end
    end
    
    #END���o
    ev_end = str.index(EventEnd)
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
        if find == true then
          CallFuncParent(hash, code_ary, str, call_find_code_ary)
        end
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

if call_find_code_ary.length != 0 then
  printf("��-------CALL-------\n")

  call_find_code_ary.each{|i|
    p i
  }
end


