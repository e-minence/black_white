============================================================================
 overlaymap.exe
 DSプロジェクトのxMAPファイルを解析してオーバーレイ状態を図視させる
============================================================================

●「overlaymap.exe」の説明
オーバーレイの配置状態を画像で確認できるツールのα版です。
xMapは一回しか読み込めません。

読み込ませるxMapファイルは「main.nef.xMap」か「main.tef.xMap」です。
直接解析してオーバーレイ情報を出力します。
ただし、main.nef.xMapは12万行にもおよぶ巨大ファイルですので、
少し重いです。（5秒ぐらい？）


●使用方法

  bash上で「sh overlaymap.sh」
  もしくはＤＯＳプロンプト上で「overlaymap.bat」
　と実行することで、「overlay_graph.png」が生成されます。
　プログラムが上に伸びるものだとした時のプログラムのサイズ
　とオーバーレイの分布状態を表しています。
　同時にdotファイルも生成されます。（dotファイルについては下で説明）

●ボタンの説明

※ここに書いてあるのはウインドウプログラムとして「Overlaymap.exe」を
　実行した時の説明です。
機能は
「ｏｐｅｎ」　→　xMapファイルを読み込んで解析してpngとして出力します。
　　　　　　　　　また出力結果を別ウインドウに表示します。
　　　　　　　　　Graphviz用のdotファイルも出力します。
「WindowOff」 →　結果表示ウインドウが邪魔になったら押すと消えます。



●「dot」ファイルについて

.dotファイルはオープンソースのソフトウェア「Graphviz」という
ツールに対応したテキストファイルです。
http://www.graphviz.org/

「Graphviz」は名前の通り、グラフ描画に特化したソフトで
名前と名前の連結情報・名前と数値の対応表などを渡すと自動的に
グラフ画像を生成してくれます。

Overlaymap.exeはアドレスから各オーバーレイの「サイズと位置」を図示しますが、
「Graphviz」で出力するグラフは「オーバーレイがどのオーバーレイの後ろに連結されているのか？」
を家系図のように表示します。

overlaymap.exeを実行した後で
dot -Tpng overlaymap.dot >overlaymap.png
というコマンドを実行するとフローチャートのような画像が生成されます。
overlaymap.sh・overlaymap.batの中には記述されています。


●サンプル「rom.xMap」
rom.xMapはマスター時のマップファイルです。
サンプルとして実行できます。
軽くするためにカットしてあります。
実際のmain.nef.xMapの読み込み後はもっと重くなります。


●更新情報
　2010/06 xMapファイルの下からオーバーレイの定義が５００行までの間に見つかったら解析するように
　　　　　していたのだが、５００行よりも上に存在するファイルが出力されるようになってしまったので
　　　　　対応した。
　2010/04 xMapファイルのサイズが初期設定の配列のサイズを超えて読み込めなくなっていたので、
　　　　　配列サイズをファイルから可変させるようにした
　2010/04 オーバーレイ解析できる最大数を２００個に設定していたら超えてしまったという報告を
　　　　　小幡くんから受けたので４００個に増やした。
　2010/02 名木橋君君からの依頼でオーバーレイの描画範囲を広くした。PNGの大きさも大きくなった。
　　　　　左端に、グリッドラインが示している0x10000ごとの数値を書き込んだ。
  2008/01 松田君からの要望で生成するビットマップの幅を右に広げた
　2007/08 松田君からの要望でコマンドラインからのみの使用もできるようにした。
　　　　　コマンドラインから実行した場合は画像とgraphviz用ファイルを出力してすぐに終了します。

2007/02/09
AkitoMori