import serial
import serial.tools.list_ports
import math
import sys

# クォータニオンからオイラー角への変換
def quaternion_to_euler(i, j, k, r):
    # i, j, k, r はそれぞれ qx, qy, qz, qw に対応
    # ロール角 (X軸周り) の計算
    sinr_cosp = 2.0 * (r * i + j * k)
    cosr_cosp = 1.0 - 2.0 * (i * i + j * j)
    roll = math.atan2(sinr_cosp, cosr_cosp)

    # ピッチ角 (Y軸周り) の計算
    sinp = 2.0 * (r * j - k * i)
    if abs(sinp) >= 1.0:
        pitch = math.copysign(math.pi / 2.0, sinp)
    else:
        pitch = math.asin(sinp)

    # ヨー角 (Z軸周り) の計算
    siny_cosp = 2.0 * (r * k + i * j)
    cosy_cosp = 1.0 - 2.0 * (j * j + k * k)
    yaw = math.atan2(siny_cosp, cosy_cosp)

    # ラジアンから度に変換
    return math.degrees(roll), math.degrees(pitch), math.degrees(yaw)

def main():
    # 引数からポート名を取得
    port_name = None
    if len(sys.argv) > 1:
        port_name = sys.argv[1]

    ports = list(serial.tools.list_ports.comports())
    if not ports:
        print("利用可能なシリアルポートが見つかりません。")
        sys.exit(1)

    # 引数がない場合、ST-Linkポートを自動探索
    if not port_name:
        for port in ports:
            desc = port.description.lower()
            if "stlink" in desc or "st-link" in desc or "virtual com" in desc:
                port_name = port.device
                print(f"ST-Linkポートを自動検出しました: {port.device} ({port.description})")
                break

    if not port_name:
        print("利用可能なポート:")
        for idx, port in enumerate(ports):
            print(f"[{idx}] {port.device} - {port.description}")

        # デフォルトで最初のポートを選択
        selected_idx = 0
        if len(ports) > 1:
            try:
                val = input(f"接続するポートの番号を入力してください [デフォルト: 0]: ")
                if val.strip():
                    selected_idx = int(val)
            except ValueError:
                print("入力が無効なためデフォルトのポートを使用します。")
        port_name = ports[selected_idx].device
    baud_rate = 115200

    print(f"ポート {port_name} に {baud_rate} bps で接続します...")

    try:
        ser = serial.Serial(port_name, baud_rate, timeout=1.0)
        # 受信バッファをクリア
        ser.reset_input_buffer()
        print("接続成功。データ受信待ちです... (Ctrl+C で終了)")

        while True:
            if ser.in_waiting > 0:
                try:
                    # 1行読み取り、デコード
                    line = ser.readline().decode('utf-8', errors='ignore').strip()
                    if not line:
                        continue

                    # データの種別ごとにパース
                    parts = line.split(',')
                    header = parts[0]

                    if header == 'O' and len(parts) == 4:
                        # PAA5160E1 (OTOS) データ
                        # O, x, y, h
                        x = float(parts[1])
                        y = float(parts[2])
                        h = float(parts[3])
                        # ラジアンを度に変換
                        h_deg = math.degrees(h)
                        print(f"[OTOS] X: {x:8.4f} m, Y: {y:8.4f} m, Heading: {h_deg:8.3f} deg")

                    elif header == 'B' and len(parts) == 5:
                        # BNO085 データ
                        # B, i, j, k, r
                        i = float(parts[1])
                        j = float(parts[2])
                        k = float(parts[3])
                        r = float(parts[4])
                        
                        # オイラー角の計算
                        roll, pitch, yaw = quaternion_to_euler(i, j, k, r)
                        print(f"[BNO ] Roll: {roll:8.3f} deg, Pitch: {pitch:8.3f} deg, Yaw: {yaw:8.3f} deg (i:{i:6.3f}, j:{j:6.3f}, k:{k:6.3f}, r:{r:6.3f})")
                    
                    else:
                        # その他の文字列 (デバッグログなど)
                        print(f"[LOG ] {line}")

                except Exception as e:
                    # パースエラーなどが発生した場合も処理を継続
                    pass

    except serial.SerialException as e:
        print(f"シリアルエラーが発生しました: {e}")
    except KeyboardInterrupt:
        print("\nプログラムを終了します。")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("シリアルポートを閉じました。")

if __name__ == '__main__':
    main()
