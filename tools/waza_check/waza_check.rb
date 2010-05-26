require "wazaoboe_hash.rb"
require "wazaoboe_lv_hash.rb"

# ���x���A�b�v�ŋZ���o���邩�H
def CheckWazaOboe_byLv( mons_name, waza_name )
  wazaoboe_table = $wazaoboe_hash[ mons_name ]
  if wazaoboe_table.include?( waza_name ) then
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
def GetDefaultWaza( mons_name, level )
  waza_list = Array.new # �ŏI�I�ȋZ���X�g
  wazaoboe_list = $wazaoboe_hash[ mons_name ]
  wazaoboe_list.each do |waza_name|
    waza_lv = GetWazaOboeLv( mons_name, waza_name )
    if waza_lv <= level then waza_list << waza_name end
  end
  while 4 < waza_list.size
    waza_list.slice!(0)
  end
  return waza_list
end
