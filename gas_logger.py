import serial
import time
from datetime import datetime, timezone
import requests

# ---------------- CONFIG ----------------
SERIAL_PORT = "COM4"
BAUD_RATE = 9600

INFLUX_URL = "http://localhost:8086"
INFLUX_ORG = "default"
INFLUX_BUCKET = "hv"
INFLUX_TOKEN = "F2Zpn-m5HnXvNmrcNuT7cN-U1UlteE9kkpUd2EUCu6HWdbzJ5pC6Pvw0At6KCfFQOav8ka1zlBxcao2yMH_j_g=="
# ----------------------------------------

def write_to_influx(lines: str):
    url = f"{INFLUX_URL}/api/v2/write"
    params = {
        "org": INFLUX_ORG,
        "bucket": INFLUX_BUCKET,
        "precision": "ns",
    }
    headers = {
        "Authorization": f"Token {INFLUX_TOKEN}",
        "Content-Type": "text/plain; charset=utf-8",
    }
    r = requests.post(url, params=params, headers=headers, data=lines.encode("utf-8"))
    r.raise_for_status()

def parse_line(line):
    try:
        # T:23.4C|H:45.2%|P:1013.25hPa
        parts = line.split("|")

        t = float(parts[0].split(":")[1].replace("C", ""))
        h = float(parts[1].split(":")[1].replace("%", ""))
        p = float(parts[2].split(":")[1].replace("hPa", ""))

        return t, h, p
    except Exception as e:
        print("Parse error:", e, "on line:", line)
        return None

def main():

    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)
    time.sleep(2)
  
    while True:
        if ser.in_waiting > 0:
            raw = ser.readline().decode().strip()
            parsed = parse_line(raw)

            if parsed:
                temp, hum, pres = parsed
                now = datetime.now(timezone.utc)
                ts_ns = int(now.timestamp() * 1e9)

                line = (
                    f"bme280,host=arduinoUno "
                    f"temperature={temp},humidity={hum},pressure={pres} {ts_ns}"
                )

                print(line)

                try:
                    write_to_influx(line)
                except Exception as e:
                    print("[ERROR] Influx write failed:", e)

        time.sleep(5)

if __name__ == "__main__":
    main()
