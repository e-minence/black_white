
$monsname_convert_hash = {
  "�p�u���[" => "���[�e���["
}

def ConvertMonsName( mons_name )
  if $monsname_convert_hash.include?( mons_name ) then
    return $monsname_convert_hash[ mons_name ]
  else
    return mons_name
  end
end
