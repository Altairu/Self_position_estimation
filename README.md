# 自己位置・姿勢推定プロジェクト

PAA5160E1 (OTOS) センサーおよび Adafruit BNO085 センサーから取得した位置・姿勢データを STM32 (NUCLEO-F446RE) で処理し、PCのWebブラウザ（HTML GUI）へリアルタイムに送信・同期・リセットするシステムです。

## システム構成

* マイコン: STM32F446RE (NUCLEO-F446RE)
* センサー1: PAA5160E1 (OTOS 自己位置オドメトリセンサー) - I2C1 接続
* センサー2: Adafruit BNO085 (9軸IMU/姿勢センサー) - I2C3 接続
* PC側可視化: Web Serial API と Three.js を用いた HTML アプリケーション

## 環境設定

開発およびビルドを行うために、以下のツールをインストールし、環境変数 (PATH) に追加してください。

* GCC Arm Toolchain
  * STM32マイコン用のクロスコンパイラです。`arm-none-eabi-gcc` がコマンドラインから実行できる必要があります。
* CMake (バージョン 3.15 以上)
  * プロジェクトのビルド設定・管理ツールです。
* Ninja または Make
  * ビルドジェネレータです。CMake でビルドを行う際に必要です。
* STM32CubeProgrammer
  * マイコンへの書き込みを行うために、CLI ツール（`STM32_Programmer_CLI.exe`）が PATH に通っている必要があります。
* Web Serial API 対応ブラウザ
  * PCとマイコン間のシリアル通信を表示するため、Google Chrome または Microsoft Edge を使用します。

## ビルド方法

CMakeプリセットを使用して、デバッグビルドを実行します。

* ビルドの構成（初期化）
  ```powershell
  cmake --preset Debug
  ```
* ファームウェアのビルド
  ```powershell
  cmake --build --preset Debug
  ```
  実行後、`build/Debug/Self_position_estimation.elf` が生成されます。

## 書き込み方法

STM32CubeProgrammer CLI を使用して、ST-Link経由でビルドしたファームウェアをマイコンに書き込みます。

* ST-Link デバイスの接続確認
  ```powershell
  STM32_Programmer_CLI.exe -l
  ```
* ファームウェアの書き込みと実行開始
  ```powershell
  STM32_Programmer_CLI.exe -c port=SWD sn=<ST-Linkのシリアル番号> -w build/Debug/Self_position_estimation.elf -v -rst
  ```
  ※ シリアル番号が `066EFF545383564867203313` の場合は以下のようになります。
  ```powershell
  STM32_Programmer_CLI.exe -c port=SWD sn=066EFF545383564867203313 -w build/Debug/Self_position_estimation.elf -v -rst
  ```

## 使い方

* 可視化アプリの起動
  * プロジェクトルートにある [gui_visualizer.html](gui_visualizer.html) を、Google Chrome もしくは Microsoft Edge で直接開きます。
* シリアルポートへの接続
  * 画面右上にある「シリアル接続」ボタンを押します。
  * ポートの選択ダイアログが表示されるので、マイコンが接続されている仮想 COM ポート（例: STLink Virtual COM Port）を選択し、接続します。
* データの可視化
  * 接続されると、マイコンから送信される OTOS の自己位置データ（X, Y座標）および BNO085 の姿勢データ（クォータニオン）がリアルタイムにデコードされ、画面に反映されます。
  * 3D姿勢プレビューおよび2D移動軌跡マップが自動的に更新されます。
* 位置・姿勢リセット機能
  * 画面右上にある「位置・姿勢リセット」ボタンを押すと、PC側で保持されている軌跡データがクリアされ、BNO085の現在の姿勢を基準（ゼロ）とするオフセットが適用されます。
  * 同時に、シリアル経由でマイコンへリセットシグナル `'R'` が送信され、マイコンがOTOSセンサーの内部オドメトリ座標を `(0, 0, 0)` にリセットします。

## ライセンス

このプロジェクトは [MIT ライセンス](LICENSE) のもとで公開されています。詳細は LICENSE ファイルを参照してください。
