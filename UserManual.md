# 計測アプリ使い方

## 手順

1. 受信器ESP32をパソコンに接続する．このとき，ESP32用のドライバをインストールしておかないとCOM認識が上手くいかないので，事前にインストールする(https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)
2. ESP32がどのCOMに対応しているか確認する．
3. MATLAB上でcheeseApplication.mを開く．
4. これからの走者の番号をrunner_numberの値に代入．
5. serialInitialize関数の引数のCOMポートを自分の環境用に変更する．
6. mファイルを実行する．
7. アプリを操作して計測を行う．
8. 終了し，saveをしてアプリを閉じたら，コンソール上で何かのキーを押して，pauseを解除する．
9. その後，今の走者のデータがwebにアップされる．

## 備考
WebAPIは時々Timeoutなどによって接続に失敗する．
GUI起動始めに失敗した時は，もう一度m fileを実行すればよい．
GUI終了時のsaveで失敗した時は，uploadToWeb(api_accessor, runner_number);という風に，コマンドラインから実行すればよい．(データ自体はMATLAB上のmatファイルに先に記録されているので，WebへのUPのみやり直せばよい)
