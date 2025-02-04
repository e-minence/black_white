###################################################################################
# ■brief:  gmm生成関数
# ■file    gmm_generator.rb
# ■author: obata
# ■date:   2010.02.12
################################################################################### 
require 'kconv'  # for String.toutf8


RETURN_CODE = "\n" # 置換対象の改行コード
SPACE_CODE  = "　" # 置換対象の空白コード
DUMMY_RETURN_CODE  = "@"   # 改行コードとして埋め込むダミーコード
DUMMY_SPACE_CODE   = "□"  # 空白コードとして埋め込むダミーコード


#==================================================================================
# □brief:  gmmヘッダ部を取得する
# □return: gmmヘッダ部の文字列
#==================================================================================
def GetGMMHeader

  header = ' 

<?xml version="1.0" encoding="UTF-8" ?>
<?xml-stylesheet type="text/xsl" href="" ?>

<gmml version="4.5">

<head>
  <create user="obata" host="W00207" date="2010-02-12T11:11:29"/>
  <generator name="ResearchRadarConverter" version="4.9.2.5"/>
</head>

<body language="japanese">
  <color>
    <list entry="0" r="00" g="00" b="00" a="FF"/>
    <list entry="1" r="FF" g="00" b="00" a="FF"/>
    <list entry="2" r="00" g="00" b="FF" a="FF"/>
    <list entry="3" r="00" g="8C" b="00" a="FF"/>
  </color>

  <tag-table>
    <group entry="0" japanese="タググループ" english="タググループ">
    </group>

    <group entry="1" japanese="ポケモンＷＢ（単語）" english="PokemonDP" color="#c0c0c0">
      <tag entry="00" japanese="トレーナー名" english="トレーナー名" view="EP" width="60" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="01" japanese="ポケモン種族名" english="ポケモン種族名" view="EP" width="60" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">ポケモンの名前。
ピカチュウとかフシギダネとか。</comment>
        <comment language="english">ポケモンの名前。
ピカチュウとかフシギダネとか。</comment>
      </tag>
      <tag entry="02" japanese="ポケモンニックネーム" english="ポケモンニックネーム" view="EP" width="60" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">ポケモンのニックネーム</comment>
        <comment language="english">ポケモンのニックネーム</comment>
      </tag>
      <tag entry="03" japanese="ポケモンタイプ" english="ポケモンタイプ" view="EP" width="48" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">みず、くさ、ほのお…とか。</comment>
        <comment language="english">みず、くさ、ほのお…とか。</comment>
      </tag>
      <tag entry="04" japanese="ポケモン分類" english="ポケモン分類" view="EP" width="60" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">「DNAポケモン」など。</comment>
        <comment language="english">「DNAポケモン」など。</comment>
      </tag>
      <tag entry="05" japanese="マップ名" english="マップ名" view="EP" width="120" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="06" japanese="とくせい名" english="とくせい名" view="EP" width="84" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="07" japanese="わざ名" english="わざ名" view="EP" width="84" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="08" japanese="せいかく" english="せいかく" view="EP" width="60" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="09" japanese="どうぐ名" english="どうぐ名" view="EP" width="96" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="10" japanese="グッズ名" english="グッズ名" view="EP" length="8" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="11" japanese="ボックス名" english="ボックス名" view="EP" width="96" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="13" japanese="ステータス名" english="ステータス名" view="EP" width="74" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">たいりょく、こうげきりょく、ぼうぎょりょく、とくこう、とくぼう、すばやさ、かいひりつ、めいちゅう</comment>
        <comment language="english">たいりょく、こうげきりょく、ぼうぎょりょく、とくこう、とくぼう、すばやさ、かいひりつ、めいちゅう</comment>
      </tag>
      <tag entry="14" japanese="トレーナー種別名" english="トレーナー種別名" view="EP" width="120" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">「たんぱんこぞう」「エリートトレーナー」など</comment>
        <comment language="english">「たんぱんこぞう」「エリートトレーナー」など</comment>
      </tag>
      <tag entry="17" japanese="特殊状態名" english="特殊状態名" view="EP" width="48" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">ポケモンの特殊状態です。</comment>
        <comment language="english">ポケモンの特殊状態です。</comment>
      </tag>
      <tag entry="18" japanese="バッグのポケット名" english="バッグのポケット名" view="EP" width="132" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">バッグのポケットの名前。「ポケット」の部分も含む。</comment>
        <comment language="english">バッグのポケットの名前。「ポケット」の部分も含む。</comment>
      </tag>
      <tag entry="26" japanese="カップ名" english="カップ名" view="EP" width="72" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">対戦のレギュレーション名</comment>
        <comment language="english">対戦のレギュレーション名</comment>
      </tag>
      <tag entry="28" japanese="簡易会話単語" english="簡易会話単語" view="EP" width="84" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="31" japanese="アクセサリー名" english="アクセサリー名" view="EP" width="96" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">ポケモンにつけるアクセサリー</comment>
        <comment language="english">ポケモンにつけるアクセサリー</comment>
      </tag>
      <tag entry="32" japanese="ジム名" english="ジム名" view="EP" width="72" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="33" japanese="時間帯" english="時間帯" view="EP" width="48" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">ぼうけんノートなどで使う</comment>
        <comment language="english">ぼうけんノートなどで使う</comment>
      </tag>
      <tag entry="34" japanese="コンテスト名称" english="コンテスト名称" view="EP" width="120" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">コンテストのジャンル
「○○○○○コンテスト」</comment>
        <comment language="english">コンテストのジャンル
「○○○○○コンテスト」</comment>
      </tag>
      <tag entry="35" japanese="コンテストランク" english="コンテストランク" view="EP" width="84" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">○○○○ランク
「ランク」も含む</comment>
        <comment language="english">○○○○ランク
「ランク」も含む</comment>
      </tag>
      <tag entry="36" japanese="国名" english="国名" view="EP" width="216" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">プレイヤーが入力する、自分が住んでいる国名</comment>
        <comment language="english">プレイヤーが入力する、自分が住んでいる国名</comment>
      </tag>
      <tag entry="37" japanese="地域名" english="地域名" view="EP" width="216" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">プレイヤーが入力する、自分が住んでいる地域名</comment>
        <comment language="english">プレイヤーが入力する、自分が住んでいる地域名</comment>
      </tag>
      <tag entry="39" japanese="リボンの名前" english="リボンの名前" view="EP" width="168" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">リボンの名前</comment>
        <comment language="english">リボンの名前</comment>
      </tag>
      <tag entry="40" japanese="広場ミニゲーム" english="広場ミニゲーム" view="EP" width="120" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">WiFi広場のミニゲーム名</comment>
        <comment language="english">WiFi広場のミニゲーム名</comment>
      </tag>
      <tag entry="41" japanese="広場イベント名" english="広場イベント名" view="EP" width="60" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">WiFi広場イベント名</comment>
        <comment language="english">WiFi広場イベント名</comment>
      </tag>
      <tag entry="42" japanese="広場アイテム名" english="広場アイテム名" view="EP" width="120" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">WiFi広場のアイテム名</comment>
        <comment language="english">WiFi広場のアイテム名</comment>
      </tag>
      <tag entry="43" japanese="日本の挨拶" english="日本の挨拶" view="EP" width="143" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">日本の挨拶</comment>
        <comment language="english">日本の挨拶</comment>
      </tag>
      <tag entry="44" japanese="英語の挨拶" english="英語の挨拶" view="EP" width="143" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">英語の挨拶</comment>
        <comment language="english">英語の挨拶</comment>
      </tag>
      <tag entry="45" japanese="フランス語の挨拶" english="フランス語の挨拶" view="EP" width="143" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">フランス語の挨拶</comment>
        <comment language="english">フランス語の挨拶</comment>
      </tag>
      <tag entry="46" japanese="ドイツ語の挨拶" english="ドイツ語の挨拶" view="EP" width="143" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">ドイツ語の挨拶</comment>
        <comment language="english">ドイツ語の挨拶</comment>
      </tag>
      <tag entry="47" japanese="イタリア語の挨拶" english="イタリア語の挨拶" view="EP" width="143" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">イタリア語の挨拶</comment>
        <comment language="english">イタリア語の挨拶</comment>
      </tag>
      <tag entry="48" japanese="スペイン語の挨拶" english="スペイン語の挨拶" view="EP" width="143" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">スペイン語の挨拶</comment>
        <comment language="english">スペイン語の挨拶</comment>
      </tag>
    </group>

    <group entry="2" japanese="ポケモンＷＢ（数値）" english="ポケモンＷＢ（数値）" color="#c0c0c0">
      <tag entry="00" japanese="１桁" english="１桁" view="EP" width="8" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="01" japanese="２桁" english="２桁" view="EP" width="16" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="02" japanese="３桁" english="３桁" view="EP" width="24" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="03" japanese="４桁" english="４桁" view="EP" width="32" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="04" japanese="５桁" english="５桁" view="EP" width="40" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="05" japanese="６桁" english="６桁" view="EP" width="48" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
        <comment language="japanese">
</comment>
        <comment language="english">
</comment>
      </tag>
      <tag entry="06" japanese="７桁" english="７桁" view="EP" width="56" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
      <tag entry="07" japanese="８桁" english="８桁" view="EP" width="64" line="1">
        <parameter number="0" japanese="番号" english="番号" type="int8"/>
      </tag>
    </group>

    <group entry="189" japanese="汎用コントロール" english="汎用コントロール" color="#c0c0c0">
      <tag entry="00" japanese="カラー変更" english="カラー変更" view="EP" length="0" line="1">
        <parameter number="0" japanese="文字色番号" english="文字色番号" type="int8"/>
        <parameter number="1" japanese="影色番号" english="影色番号" type="int8"/>
        <parameter number="2" japanese="背景色番号" english="背景色番号" type="int8"/>
        <comment language="japanese">システムのカラータグより細かい設定が可能な低レベル版。
ほとんどの場合、システムタグを使えば問題ないと思います。</comment>
        <comment language="english">システムのカラータグより細かい設定が可能な低レベル版。
ほとんどの場合、システムタグを使えば問題ないと思います。</comment>
      </tag>
      <tag entry="01" japanese="カラー変更リセット" english="カラー変更リセット" view="EP" length="0" line="1"/>
      <tag entry="02" japanese="Ｘセンタリング" english="Ｘセンタリング" view="EP" length="0" line="1"/>
      <tag entry="03" japanese="Ｘ右寄せ" english="Ｘ右寄せ" view="EP" length="0" line="1">
        <parameter number="0" japanese="右スペース" english="右スペース" type="int8"/>
        <comment language="japanese">当該行の文字列を
ウィンドウ内で右寄せします。</comment>
        <comment language="english">当該行の文字列を
ウィンドウ内で右寄せします。</comment>
      </tag>
    </group>

    <group entry="190" japanese="動的コントロール" english="Word" color="#c0c0c0">
      <tag entry="00" japanese="ページ切り替え（行送り）" english="ページ切り替え（行送り）" view="EP" length="0" line="1"/>
      <tag entry="01" japanese="ページ切り替え（クリア）" english="ページ切り替え（クリア）" view="EP" length="0" line="1"/>
      <tag entry="02" japanese="ウェイト変更（単発）" english="ウェイト変更（単発）" view="EP" length="0" line="1">
        <parameter number="0" japanese="フレーム数" english="フレーム数" type="int16"/>
      </tag>
      <tag entry="03" japanese="ウェイト変更（継続）" english="ウェイト変更（継続）" view="EP" length="0" line="1">
        <parameter number="0" japanese="フレーム数" english="フレーム数" type="int16"/>
      </tag>
      <tag entry="04" japanese="ウェイトリセット" english="ウェイトリセット" view="EP" length="0" line="1"/>
      <tag entry="05" japanese="コールバック引数変更（単発）" english="コールバック引数変更（単発）" view="EP" length="0" line="1">
        <parameter number="0" japanese="引数" english="引数" type="int16"/>
      </tag>
      <tag entry="06" japanese="コールバック引数変更（継続）" english="コールバック引数変更（継続）" view="EP" length="0" line="1">
        <parameter number="0" japanese="引数" english="引数" type="int16"/>
      </tag>
      <tag entry="07" japanese="コールバック引数リセット" english="コールバック引数リセット" view="EP" length="0" line="1"/>
      <tag entry="08" japanese="描画領域クリア" english="描画領域クリア" view="EP" length="0" line="1"/>
    </group>

    <group entry="254" japanese="文字列展開" english="Word">
    </group>

    <set japanese="500%" english="500%" value="FF:01:サイズ:500"/>
    <set japanese="400%" english="400%" value="FF:01:サイズ:400"/>
    <set japanese="300%" english="300%" value="FF:01:サイズ:300"/>
    <set japanese="200%" english="200%" value="FF:01:サイズ:200"/>
    <set japanese="150%" english="150%" value="FF:01:サイズ:150"/>
    <set japanese="100%" english="100%" value="FF:01:サイズ:100"/>
    <set japanese="75%" english="75%" value="FF:01:サイズ:75"/>
    <set japanese="50%" english="50%" value="FF:01:サイズ:50"/>
    <set japanese="カラーデフォルト" english="Black" value="FF:00:色:0"/>
    <set japanese="カラー１" english="Red" value="FF:00:色:1"/>
    <set japanese="カラー２" english="Green" value="FF:00:色:2"/>
    <set japanese="カラー３" english="Blue" value="FF:00:色:3"/>
  </tag-table>

  <columns>
    <id position="0" width="151"/>
    <group default=" 0:デフォルト" position="-1" width="100">
      <list entry="0" japanese="デフォルト" english="Default" colorID="0"/>
    </group>
    <number position="-1" width="100"/>
    <comment position="3" width="288"/>
    <erase position="-1" width="100"/>
    <language japanese="JPN" english="JPN">
      <message position="1" width="274"/>
      <font default=" 0:large" position="4" width="109">
        <list entry="0" file="W:\font\large.nftr"/>
        <list entry="1" file="W:\font\small.nftr"/>
      </font>
      <size position="-1" width="100"/>
      <space position="-1" width="100"/>
      <width position="6" width="70"/>
      <line position="-1" width="100"/>
    </language>
    <language japanese="JPN_KANJI" english="JPN_KANJI">
      <message position="2" width="246"/>
      <font default=" 0:large" position="5" width="106">
        <list entry="0" file="W:\font\large.nftr"/>
        <list entry="1" file="W:\font\small.nftr"/>
      </font>
      <size position="-1" width="100"/>
      <space position="-1" width="100"/>
      <width position="7" width="75"/>
      <line position="-1" width="100"/>
    </language>
  </columns>

  <dialog left="32" top="207" right="684" bottom="827" maximized="no" rayout="Vertical2">
    <comment-input left="733" top="589" right="1080" bottom="739" visible="no"/>
    <tag-palette left="736" top="133" right="1209" bottom="531" visible="no"/>
    <band id="0" index="0" style="0000" width="477"/>
    <band id="1" index="1" style="0000" width="165"/>
    <band id="2" index="2" style="0000" width="644"/>
    <band id="3" index="3" style="0000" width="644"/>
    <edit entry="0" language="JPN_KANJI" mode="Edit" spell-check="-1"/>
    <edit entry="1" language="JPN" mode="Preview" spell-check="-1"/>
    <edit entry="2" language="JPN" mode="Source" spell-check="-1"/>
    <edit entry="3" language="JPN" mode="Source" spell-check="-1"/>
  </dialog>

  <output default="デフォルト">
    <target japanese="デフォルト" english="default" endian="big" encoding="Shift_JIS" letter-list-encoding="Shift_JIS" header-file-encoding="Shift_JIS" language="JPN">
      <struct name="JMSMesgEntry" id="" message="mesgOffset">
      </struct>
      <entry sort="id" id-block="no" flowlabel-block="no"/>
      <header divide="no" id-prefix="JMS_" query-prefix="JMS_QUERY_" flownode-prefix="JMS_FLOWNODE_" id="" struct="" query=""/>
    </target>
  </output>

'

  # 先頭の空白文字を削除して返す
  return header.lstrip

end


#==================================================================================
# □brief:  gmmフッタ部を取得する
# □return: gmmフッタ部の文字列
#==================================================================================
def GetGMMFooter

  footer = '

  <lock>
    <operation add_message="no" header_setting="no" tag_setting="no" output_setting="no" display="no" sort="no" flowchart_window="no"/>
  </lock>

  <flowchart-group-list>
    <flowchart-group japanese="root" english="root" group-id="0" parent-id="-1"/>
  </flowchart-group-list>
</body>

</gmml> 

'

  # 末尾の空白を削除して返す
  return footer.rstrip

end


#==================================================================================
# □brief: gmmファイルを出力する
# □param: directory       出力先ディレクトリ
# □param: fileName        出力ファイル名
# □param: stringLavel     テキストのラベル名配列
# □param: stringJPN       文字列データ配列 ( ひらがな )
# □param: stringJPN_KANJI 文字列データ配列 ( 漢字 )
#==================================================================================
def GenerateGMM( directory, fileName, stringLavel, stringJPN, stringJPN_KANJI )

  # 引数の文字列を整形
  0.upto( stringLavel.size - 1 ) do |strIdx|
    # 両端の空白を削除
    stringLavel    [ strIdx ].strip!
    stringJPN      [ strIdx ].strip!
    stringJPN_KANJI[ strIdx ].strip!
    # 改行ダミーコードを復元
    if stringJPN[ strIdx ].include?( DUMMY_RETURN_CODE ) then
      stringJPN[ strIdx ].gsub!( DUMMY_RETURN_CODE, RETURN_CODE )
    end
    if stringJPN_KANJI[ strIdx ].include?( DUMMY_RETURN_CODE ) then
      stringJPN_KANJI[ strIdx ].gsub!( DUMMY_RETURN_CODE, RETURN_CODE )
    end
    # 空白ダミーコードを復元
    if stringJPN[ strIdx ].include?( DUMMY_SPACE_CODE ) then
      stringJPN[ strIdx ].gsub!( DUMMY_SPACE_CODE, SPACE_CODE )
    end
    if stringJPN_KANJI[ strIdx ].include?( DUMMY_SPACE_CODE ) then
      stringJPN_KANJI[ strIdx ].gsub!( DUMMY_SPACE_CODE, SPACE_CODE )
    end
  end

  # gmm テキスト列挙部分を作成
  gmmRows = ""
  0.upto( stringLavel.size - 1 ) do |strIdx|
    row = '
      <row id="%s">
        <language name="JPN" width="256">%s</language>
        <language name="JPN_KANJI" width="256">%s</language>
      </row>
      ' % [ stringLavel[strIdx], stringJPN[strIdx], stringJPN_KANJI[strIdx] ]
      row.strip!
      gmmRows += row
  end

  # gmm ファイルを構築
  gmmHeader = GetGMMHeader()
  gmmFooter = GetGMMFooter()
  gmmData   = gmmHeader + gmmRows + gmmFooter

  # gmm ファイル出力
  file = File.open( directory + "/" + fileName, "w" )
  file.puts( gmmData.toutf8 )  # UTF-8 に変換して出力
  file.close

end 
