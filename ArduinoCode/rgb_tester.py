from colorist import BgColorRGB

def main() -> None:
    celcius: float = 25
    print(f"Temperature: {celcius}°C")
    set_backlight(celcius)
    
def map_value(x: float, in_min: int, in_max: int, out_min: int, out_max: int):
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
    
def set_backlight(celcius: float) -> None:
    r: int = 0
    g: int = 0
    b: int = 0
    
    min_temp: float = -5
    mid_temp: float = 15
    max_temp: float = 35
    
    if (celcius <= min_temp):
        r = 0
        g = 0
        b = 255
    elif (celcius <= mid_temp):
        r = 0
        g = map_value(celcius, min_temp, mid_temp, 0, 255)
        b = 255 - g
    else:
        r = 255
        g = map_value(celcius, mid_temp, max_temp, 255, 0)
        b = 0
    
    col = BgColorRGB(r, g, b)
    print(f"{col} [ {r} {g} {b} ] {col.OFF}")
    
if __name__ == "__main__": 
    main()