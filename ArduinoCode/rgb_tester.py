from colorist import BgColorRGB
from scipy.interpolate import interp1d


def main() -> None:
    # Testing
    celsius: float = 70
    print(f"Temperature: {celsius}°C")
    print_colour(celsius, -15, 18, 25, 50)


def map_value(x: float, in_min: float, in_max: float, out_min: float, out_max: float) -> int:
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)


def print_colour(value: float, min_val: float, mid_val: float, mid_max_val: float, max_val: float) -> None:
    if value < min_val:
        r, g, b = 0, 0, 255

    if value > max_val:
        r, g, b = 255, 0, 0

    else:
        x: list[float] = [min_val, mid_val, mid_max_val, max_val]

        y_r: list[int] = [0, 0, 255, 255]
        y_g: list[int] = [0, 255, 255, 0]
        y_b: list[int] = [255, 0, 0, 0]

        f_r: interp1d = interp1d(x, y_r, kind='cubic')
        f_g: interp1d = interp1d(x, y_g, kind='cubic')
        f_b: interp1d = interp1d(x, y_b, kind='cubic')

        r: int = int(max(min(f_r(value), 255), 0))
        g: int = int(max(min(f_g(value), 255), 0))
        b: int = int(max(min(f_b(value), 255), 0))

    col = BgColorRGB(r, g, b)
    print(f"{col} [ {r} {g} {b} ] {col.OFF}")


if __name__ == "__main__":
    main()
