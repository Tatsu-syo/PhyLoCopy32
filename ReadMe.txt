PhyLoCopy32 source file package
By Tatsuhiko Shoji(Tatsu) 1995-2000,2021

[本プログラムは何か]
  このプログラムは、MS-DOSの3.5インチ2DD9セクタフォーマットフロッピー
ディスクを元にMSXエミュレーターで使うフロッピーディスクイメージファイルを
作る、また、逆に、フロッピーディスクイメージファイルの内容を本物のディスクに
コピーするプログラムです。

  原作となったMS-DOS汎用のPhyLoCopyは16ビットMS-DOSコマンドラインプロンプトで
動作するプログラムであり、64ビット版Windowsでは動作しないので64bit版Windowsでも
動作するよう32ビット版Windowsコンソールプログラムに移植したのが
本プログラムです。

　ですので、32ビット版Windowsでは16ビット版プログラムが動作するので16ビット版
PhyLoCopyのほうが機能としては上回ります。ただし、ロングファイルネーム対応の
ような16ビット版では対応しない機能もあります。

  また、このプログラムは、何も内容の無い、まっさらなフロッピーディスクイ
メージファイルを他のフロッピーイメージファイルから作る機能や、フロッピー
イメージファイルを利用したブートセクタの更新といった物も用意しており、
MSX&MSXエミュレーターユーザーにとって便利な機能を提供しております。
逆にいうと、MSXエミュレーター及び、本物のMSXの利用に便利な機能に
特化しているので、2HD非対応など、機能的に劣る部分があるのをご了承ください。

[必要となるプログラム]
・Visual Studio 2003(VC++)

[動作環境]
　Windows 2000以降もしくは95以降となります。
ただし、Windows 95/98/Meは作者の手元に動作環境がないので各自ご確認ください。

[ファイル]
ReadMe.txt:本ファイルです。
LICENSE.txt : MITライセンスの条文です。
PhyLoCP32.sln : Visual Studio 2003用ソリューションファイルです。
PhyLoCP32.vcproj : Visual Studio 2003用プロジェクトファイルです。
*.cpp,*.h : ソースファイルです。
PhyLoCP32.rc : リソースファイルです。

[使い方]
1.まず、本アーカイブを展開します。
2.PhyLoCP32.slnを開きます。
3.ビルドを行うと実行ファイルが生成されます。

[ライセンス]
MITライセンス(http://opensource.org/licenses/mit-license.php)に従います。

[作者]
庄子 達彦(Tatsu)
E-mail Address rxk16664@nifty.ne.jp
My Web page http://tatsu.life.coocan.jp/index.html
------------------------------------------------------------------------
