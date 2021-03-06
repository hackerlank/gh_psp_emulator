RIN -GB/GBC Emulator-

■ 概要
Windows用エミュレータ「TGB Dual」をベースとした、PSP用の
GB/GBCエミュレータです。ただし、ファームv1.0のPSPでしか動作しません。
（ファームv1.5のPSPでも特殊な方法で起動できるそうです）

■ 使用方法
メモリースティックに /PSP/GAME/RIN という形のフォルダを作り、
同封の EBOOT.PBP を置いてください。次に、実行させたいROM
イメージを好きなフォルダに一つ以上コピーしてください。
ZIPかGZIPで圧縮されたROMイメージも使用可能です。
ゲームのセーブファイルはRINのSAVEフォルダに自動保存されます。

デフォルト操作：
　○：　Aボタン
　×：　Bボタン
　△：　Aボタン（連射）
　□：　Bボタン（連射）
　L：　メニュー
　START：　スタートボタン
　SELECT：　セレクトボタン
　R+START：　クイックロード
　R+SELECT：　クイックセーブ

■ チート機能
GB-PAR（GameShark）のチートコードを使用できます。
チートファイル（拡張子tch）をCHEATフォルダにコピーして、
ゲーム起動後にメニューからロードしてください。
チートファイルの書式は同封のファイルを参考にしてください。
また、TGB Dual拡張コードには対応していません。

■ オンメモリステートセーブ
TEMPスロットへのステートセーブはメモリ（RAM）上に保存されます。
メモステへのアクセスが発生しないため高速にセーブ・ロードできますが、
PSPの電源を切ったり、別のROMをロードすれば消えてしまいます
（スリープでは消えません）。

■ セーブファイルについて
圧縮されたセーブファイルはそのままではTGB Dualで使用できません。
GZIPに対応した解凍ソフトで解凍してください。

ステートファイルはRIN独自の拡張が加えられているため、
TGB Dualで使用できない場合があります。その場合は、
ステートファイルの1byte目を 0x02 から 0x01 に書き換えてください。

■ 免責
自己責任で使用してください。配布自由。

■ 謝辞
TGB Dual作者のHii氏、VisualBoyAdvance作者のForgotten氏、
GEST作者のTM氏、Hello Worldのnem氏、PS2DEV、UoRIN制作者の方々。
多大な助力に感謝いたします。


/* ------------------------------------------------
　みらきち
　http://mirakichi.hp.infoseek.co.jp/software/
------------------------------------------------- */
