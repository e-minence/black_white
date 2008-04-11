#=============================================================================================
#
#	隠しアイテム管理表コンバータ
#
#	05.11.16 Satoshi Nohara
#
#	07.06.04 プラチナ用にオフセットを変更
#
#	●_HIDE_ITEM_EVENT記述をhide_item.evに出力する
#
#=============================================================================================
BEGIN{

	#=================================================================
	#
	#アイテム定義ファイル読み込み
	#
	#=================================================================
	ITEMSYM_H = "../../../include/itemtool/itemsym.h"

	#getline = １行読み込み
	#１は戻り値
	while ( 1 == getline < ITEMSYM_H ) {
		if ( NF == 7 && $1 =="#define" ) {
			ItemName[$7] = $2
		}
	}
	close(ITEMSYM_H)

	file1 = "hide_item.dat"

	print "//====================================================================" > file1
	print "//						隠しアイテムデータ" >> file1
	print "//" >> file1
	print "// 05.11.17 Satoshi Nohara" >> file1
	print "//" >> file1
	print "// ●hide_item.xlsをコンバートして出力しています" >> file1
	print "//" >> file1
	print "//====================================================================" >> file1
	print "" >> file1

	print "const HIDE_ITEM_DATA hide_item_data[] = {" >> file1
	print "\t//アイテムナンバー 個数 反応 特殊 フラグインデックス" >> file1
}

NR >= 4{

	#項目数チェック
	if( NF < 9 ) next
	if( $1 == "マップ名" )	next
	if( $1 ~ "アイテム" )	next

	#-----------------------------------------------------------------
	#
	#フラグマックスを超えていたらエラー(07/06/04 MAX = 284 )
	#
	#-----------------------------------------------------------------
	if( $9 > 283 ){
		printf("フラグの最大数を超えています！\n") >> "/dev/stderr"
		exit				#途中で終了
	}

	#追加
	print "\t{ " ItemName[$5] ",\t" $6 ",\t" $7 ",\t" $8 ",\t" $9 " }," >> file1
}

END{
	print "};" >> file1
	print "#define HIDE_ITEM_DATA_MAX\t( NELEMS(hide_item_data) )" >> file1
}



