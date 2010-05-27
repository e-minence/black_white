require "wazaoboe_hash.rb"
require "wazaoboe_lv_hash.rb"
require "machinewaza_hash.rb"
require "kowaza_hash.rb"

# ���x���A�b�v�ŋZ���o���邩�H
def CheckWazaEnable_byLv( mons_name, waza_name )
  wazaoboe_list = $wazaoboe_hash[ mons_name ]
  if wazaoboe_list.include?( waza_name ) then
    return true
  else
    return false
  end
end 

# �����x���Ŋo���邩�H
def GetWazaOboeLv( mons_name, waza_name )
  key = mons_name + "_" + waza_name
  return $wazaoboe_lv_hash[ key ]
end

# �f�t�H���g�Z �擾
def GetDefaultWazaList( mons_name, level )
  waza_list = Array.new # �ŏI�I�ȋZ���X�g
  wazaoboe_list = $wazaoboe_hash[ mons_name ]
  if wazaoboe_list == nil then return waza_list end # �f�t�H���g�Z�Ȃ�
  wazaoboe_list.compact! # nil���폜
  wazaoboe_list.each do |waza_name|
    waza_lv = GetWazaOboeLv( mons_name, waza_name )
    if waza_lv <= level then waza_list << waza_name end
  end
  while 4 < waza_list.size
    waza_list.slice!(0)
  end
  return waza_list
end

# �킴�}�V���ŏK���\���H
def CheckWazaEnable_byMachine( mons_name, waza_name )
  machinewaza_list = $machinewaza_hash[ mons_name ]
  if machinewaza_list.include?( waza_name ) then
    return true
  else
    return false
  end
end

# �q�Z�Ƃ��ďK���\���H
def CheckWazaEnable_byKowaza( mons_name, waza_name )
  kowaza_list = $kowaza_hash[ mons_name ]
  if kowaza_list.include?( waza_name ) then
    return true
  else
    return false
  end
end
