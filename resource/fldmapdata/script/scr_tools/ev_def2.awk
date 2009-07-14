#===========================================================================================
#
#	050826 Satoshi Nohara
#
#	batファイルにawk処理を書き込む
#
#===========================================================================================
{
	name = $1

	#例外
	#"sp_"特殊スクリプトファイルは対象から外す！
	if( name ~ "sp_" ){
		next
	}

	#例外
	#".dat"を対象にする！
	#if( (name ~ "fld") && (name ~ "item") ){
		#sub(/\..*$/,"",name)										#"."以降を削除(例 fld_item)
		#print "awk -f ev_def.awk " name ".dat"
		#next
	#}

	print "gawk -f ./scr_tools/ev_def.awk " name
}


