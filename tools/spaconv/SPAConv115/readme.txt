//==============================================================================
//
//	SPAコンバータ
//								2005.08.19(金) matsuda
//==============================================================================
・このコンバータについて
	パーティクルエディタから出力されるsprファイルを、SPLライブラリで使用出来る形(*.spa)に
	コンバートします。


//==============================================================================
//	sprファイルから作成する場合
//==============================================================================
・sprファイルからリソースファイル(*.spa)を作成する場合の手順
	１．sprファイルと、そのsprファイルで使用しているsptファイルを同じフォルダ上に置きます。
	２．sprファイルをSPAコンバータにD&Dするか「開く」ボタンで開きます。

//--------------------------------------------------------------
//	sprファイルから作成した場合に生成されるファイル
//--------------------------------------------------------------
・常に生成されるファイル
　SPAConv.exeのある場所に「生成ファイル出力場所」フォルダが作成され、そこに出力されます。
	リソースファイル(*.spa)			※バイナリファイルです
	リソースファイルのヘッダ(*.h)

・SPAコンバータ上でログ出力のチェックを入れていた場合に出力されるファイル
	SPAConv.exeがあるフォルダ上に「ログファイル出力場所」フォルダが作成されています。
	各種出力ログファイルはその中に生成されています。


//==============================================================================
//	リストファイルから作成する場合
//==============================================================================
・リストファイルとは
	リソースファイル(*.spa)を作成するルールを記入したテキストファイルです。

・リストファイルを用意すると
	１．技エフェクト用のリストファイル、フィールドエフェクト用のリストファイルを別々に
		用意する事で、リソースファイルの状態が把握しやすくなります。
	２．一つにリソースファイルに複数のsprを入れる事が出来ます。
	３．生成するリソースファイルの名前を自由に決められます。
	４．リソースファイルのポインタテーブルを生成する事が出来ます。

・リストファイルから作成する場合の手順
	１．リストファイルを作成します。
	２．リストファイルとリストファイルに記入されているsprファイル、
		そのsprファイルで使用されているsptファイルを全て同じフォルダ上に置きます。
	３．リストファイルをSPAコンバータにD&Dするか「開く」ボタンで開きます。
	
//--------------------------------------------------------------
//	リストファイルの書き方
//--------------------------------------------------------------
effect_lst.txtファイルを作成(ファイル名は一例です)

---- effect_lst.txt の中身 ----

TABLE_ON		この文字列を書いて置くとリソースファイルのポインタテーブルが生成されます
TABLE_PATH_HEAD = "wazaeffect/effectdata/"	※補足参照
INC_PATH_HEAD = "effectdata/"	※補足参照
NNSARC_LIST = "waza_particle.lst"	※補足参照
SPR_PATH = ".\spr" or "c:\test\spr"		※補足参照
SPT_PATH = ".\spt" or "c:\test\spt"		※補足参照

aaa.spa			出力spa名
	aaa.spr		読み込むsprファイル名

bbb.spa
	bbb.spr
	fff.spr		一つのリソースファイルに複数のsprを入れる場合は連続してspr名を記入します。

ccc.spa
	ddd.spr
	eee.spr
	ggg.spr

hhh.spa
	kkk.spr

上記がリストファイルの作成例です。

※TABLE_PATH_HEAD の補足
ポインタテーブルを生成する場合、
通常(リストファイル名 = effect_lst.txt)
const char * const effect_lst_tbl[] = {
	"aaa.spa",
	"bbb.spa",
	"ccc.spa",
	"hhh.spa",
};
のように出力されます。
TABLE_PATH_HEADに
TABLE_PATH_HEAD = "wazaeffect\effectdata\"
のように書いて置くと、ポインタテーブルは
const char * const effect_lst_tbl[] = {
	"wazaeffect\effectdata\aaa.spa",
	"wazaeffect\effectdata\bbb.spa",
	"wazaeffect\effectdata\ccc.spa",
	"wazaeffect\effectdata\hhh.spa",
};
のように出力されます。
TABLE_PATH_HEADを記入しなかった場合は最初の例のように出力されます。
TABLE_PATH_HEADが不要ならば省略する事も可能です。
(省略する場合は"TABLE_PATH_HEAD"自体を書かなければOKです)

※INC_PATH_HEAD の補足
こちらはeffect_lst.hの方にincludeされるファイルの頭に付くパスです。
INC_PATH_HEADが不要ならば省略する事も可能です。
(省略する場合は"INC_PATH_HEAD"自体を書かなければOKです)

※NNSARC_LIST の補足
これを記入しておくと、nnsarc.exe用のリストファイルを作成します。
NNSARC_LIST = "???" の???部分は生成するリストファイル名になります。
リストファイルを生成する必要がないならば省略することも可能です。
(省略する場合は"NNSARC_LIST"自体を書かなければOKです)

※SPR_PATH, SPT_PATH の補足
通常spr,sptはリストファイルと同じフォルダ上に存在している必要がありますが、
ここにパスを記入しておけばそちらを見に行きます。
絶対パス、相対パス、どちらの指定も可能です。


//--------------------------------------------------------------
//	リストファイルから作成した場合に生成されるファイル
//--------------------------------------------------------------
読み込んだリストファイル名がeffect_lst.txtの場合
・常に生成されるファイル
　SPAConv.exeのある場所に「生成ファイル出力場所」フォルダが作成され、そこに出力されます。
	リソースファイル(*.spa)			※バイナリファイルです
	リソースファイルのヘッダ(*.h)
	
・TABLE_ONを定義していた場合のみ出力されるファイル
	effect_lst.dat		リソースファイルのポインタテーブル
	effect_lst.h		各リソースファイルのヘッダーをまとめてincludeしているヘッダファイル

・NNSARC_LISTを定義していた場合のみ出力されるファイル
	???					NNSARC_LISTで定義したファイル名でリストファイルが作成されます。
   ※※NNSARC_LISTを使用する場合、TABLE_ONを定義した時に出来るeffect_lst.dat(仮)は
	　必要なくなりますが、プログラムの都合上修正が面倒なので、どちらか片方のみ生成、
	　のような対応はしていません。使わなくてもTABLE_ONを定義しないとNNSARC_LISTの定義は
	　有効になりません。
	　(使用しないeffect_lst.datはプログラムでincludeしなければいいだけなので、
	　 そういう方向でお願いします)

・SPAコンバータ上でログ出力のチェックを入れていた場合に出力されるファイル
	SPAConv.exeがあるフォルダ上に「ログファイル出力場所」フォルダが作成されています。
	各種出力ログファイルはその中に生成されています。



//==============================================================================
//	変更履歴
//==============================================================================
1.15: 2009.03.17(火)
	1.14で追加したSPR_PATH, SPT_PATHのバグ修正。
	技エフェクト表の形をWBフォーマットに変更。
	※このバージョン以降ではプラチナまでの技エフェクト表はコンバート出来なくなるので注意！
	
1.14: 2008.12.24(水)
	SPTの項目に複数行ファイル名が書かれていても正常に読めるようにした。
	SPR_PATH, SPT_PATH の対応

1.13: 2006.06.15(木)
	SPRを22まで増やした。
	
1.12: 2006.06.12(月)
	SPRを19まで増やした。
	
1.11: 2006.05.18(木)
	技エフェクトのエクセル表を読み込んだ時、ListWork作成時に、行移動する時、きちんと
	改行コードが来るまでfpを進めるようにした。
	
1.10: 2006.05.17(水)
	SPRを13まで増やした。
	
1.09: 2006.05.13(土)
	SPRを12まで増やした。
	
1.08: 2006.02.06(月)
	リストファイル読み込みした時に作られるspr.hをまとめてincludeするファイルが、
	あまりにもincludeするものが多いと、Cygwinでエラーになってしまうので、
	includeするファイルの中身を直接コピーしてきて、まとめてincludeファイルの方に
	直接定義するようにした。
	
1.07: 2006.01.23(月)
	SPR7,8が追加されたので対応。

1.06: 2006.01.18(水)
	エクセルファイル読み込みの時、同じSPA内に、同名称のSPRが2つ以上ある場合、
	そのままSPRが書かれている分だけリストに書いていたのを、1つしか書かないようにした。
	
1.05: 2006.01.16(月)
	SPR6,SPT6の項目が追加されたので対応。
	
1.04: 2005.11.16(水)
	エフェクト一覧.xlsに項目が追加されたので対応。
	
1.03: 2005.10.31(月)
	再読み込みボタンをつけた。
	技エフェクトモードでエクセルファイルから直接読み込めるようにした。
	
1.02: 2005.10.11(火)
	各リソースのヘッダファイルに、対象のspaファイル内のリソース個数をdefine定義で書くようにした。
	(spaファイル名_SPAMAX)

1.01: 2005.10.05(水)
	NNSARC_LISTの書式を書けるようにした。

1.00：2005.08.19(金)
	作成

