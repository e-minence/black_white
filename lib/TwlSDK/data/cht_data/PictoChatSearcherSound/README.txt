■■■■■■■■■■■■■■■■■■■■■■■■■■■

	ピクトチャットサーチャー用サウンドデータ
    
■■■■■■■■■■■■■■■■■■■■■■■■■■■

このパッケージ（フォルダ）にはピクトチャットサーチャーで
アイコンが表示される時に鳴らすSEのデータが収められています。
これ以外のSE（ウインドウが開く、チャット起動を選択するなど）に
関しては、指定のSEはありませんので、必要に応じて各ソフトで
使用しているSEを鳴らしてください。


■ファイルの内訳■
BankPCS/sePCS.bnk
使用するインストが定義されたバンクファイル

SeqPCS/sePCS.mus
SEのシーケンスが書かれたシーケンスアーカイブファイル

WavePCS/CelestaPost.32.t.an5.aiff
元波形

sound_data.sarc
サウンドアーカイブ

MakeSound.bat
ReMakeSound.bat
サウンドデータを作るバッチファイル。


■組み込みについて■
各ゲームプロジェクトごとに組み込みやすい方法があると思われます。
その主な方法は下記のとおりです。
（これ以外の方法で組み込んでも構いません。）

※初期状態の各ラベルは以下のとおりです。
シーケンスアーカイブ・・・SEQARC_SE_PCS
バンク・・・・・・・・・・BANK_SE_PCS
波形アーカイブ・・・・・・WAVE_SE_PCS


■組み込み例１
このパッケージで作られたサウンドデータ（sound_data.sdat）を使用する。

MakeSound.batを使用してサウンドデータ（sound_data.sdat）を作成し、
このデータをロム上において、通常のゲームのサウンドデータと併用して鳴らす
方法です。
データが独立している分扱いやすいという利点がありますが、ゲームのSEを含め
鳴らす関数をコールする前に使用するサウンドデータを切り替える必要があるため、
組み込み時にその対応が必要になります。


■組み込み例２
バンク、シーケンスアーカイブを各ゲームのサウンドアーカイブ（sound_data.sarc）に登録する。

各ゲームで使用しているサウンドアーカイブに加えることで、ゲームと同様の
再生、データ管理が可能です。シーケンスアーカイブが使用するバンクは
「sound_data.sbdl」を参照するようになっていますので、このファイルと
シーケンスアーカイブ（sePCS.mus）の相対パスが変更された場合は、
インクルードパスを書き直す必要があります。


■組み込み例３
バンク、シーケンスアーカイブのテキストファイルの内容を使用しているファイルに
コピー＆ペーストする。

「利用例２」ではゲームとは別データで扱うため、全体のサイズが若干増えます。
それを回避する場合は、各データのテキストベースのコピー＆ペースト（移植）を
行うことで対応できます。


■注意■
■パラメータの変更
シーケンスアーカイブにＳＥのプレイヤーナンバーやプライオリティが
指定されていますが、ゲームの状況に応じて変更する必要があります。
ゲームで使用していないプレイヤーナンバーに変更したり、ゲームで
使用しているプライオリティとのバランスを調整したりしてください。

■パスの確認と変更
ファイルのパスを変更する場合は、各テキストファイルの修正が必要です。

■本来のSEの確認
組み込み方を間違うと、本来の音とは違う発音が行われる可能性があります。
必ず、一度は元の音を、バッチファイルで生成される「SoundPlayer.srl」を
起動して確認してください。

