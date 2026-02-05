import serial
import keyboard
import time

# Nastavi COM port
ser = serial.Serial('COM9', 115200)

# Tipke za premikanje
red_keys   = ['w', 'a', 's', 'd']           
blue_keys  = ['up', 'left', 'down', 'right']  

# Prejšnje stanje strelskih tipk (rising edge)
r_prev = False
p_prev = False

# Cooldown timerji (v sekundah)
r_cooldown = 0
p_cooldown = 0
COOLDOWN_TIME = 3.0  # 3 sekunde

last_time = time.time()

while True:
    now = time.time()
    dt = now - last_time
    last_time = now

    # zmanjšaj cooldown timerje
    r_cooldown = max(0, r_cooldown - dt)
    p_cooldown = max(0, p_cooldown - dt)

    bits = 0

    # --- premikanje ---
    for i, k in enumerate(red_keys):
        if keyboard.is_pressed(k):
            bits |= 1 << i

    for i, k in enumerate(blue_keys):
        if keyboard.is_pressed(k):
            bits |= 1 << (i + 5)

    # --- FIRE z cooldown ---
    # Rdeči tank 'r'
    r_current = keyboard.is_pressed('r')
    if r_current and not r_prev and r_cooldown == 0:
        bits |= 1 << 4
        r_cooldown = COOLDOWN_TIME  # reset cooldown
    r_prev = r_current

    # Modri tank '1'
    p_current = keyboard.is_pressed('1')
    if p_current and not p_prev and p_cooldown == 0:
        bits |= 1 << 9
        p_cooldown = COOLDOWN_TIME
    p_prev = p_current

    # --- DEBUG ---
    p1_up    = (bits >> 0) & 1
    p1_left  = (bits >> 1) & 1
    p1_down  = (bits >> 2) & 1
    p1_right = (bits >> 3) & 1
    p1_fire  = (bits >> 4) & 1

    p2_up    = (bits >> 5) & 1
    p2_left  = (bits >> 6) & 1
    p2_down  = (bits >> 7) & 1
    p2_right = (bits >> 8) & 1
    p2_fire  = (bits >> 9) & 1

    print(f"P1: W={p1_up} A={p1_left} S={p1_down} D={p1_right} Fire={p1_fire} | "
          f"P2: Up={p2_up} Left={p2_left} Down={p2_down} Right={p2_right} Fire={p2_fire}")

    # --- POŠLJI UART ---
    data = bits.to_bytes(2, 'little')
    ser.write(data)

    time.sleep(0.02)
