from pathlib import Path

import keyboard
from pylsl import StreamInlet, resolve_byprop

markers = {
    1: 'Extension',
    2: 'Flexion',
    3: 'Palm Forward',
    4: 'Pause',
}


def read_lsl():
    print("Looking for an LSL stream...")
    streams = resolve_byprop('name', 'EEG_Stream')

    # Шаг 2: Подключение к потоку
    if not streams:
        print("No stream found. Check the LSL configuration in EventIDE.")
    else:
        inlet = StreamInlet(streams[0])
        print("Connected to the LSL stream!")
        try:
            while True:
                sample, timestamp = inlet.pull_sample()  # Blocking operation
                print(f"Timestamp: {timestamp}, Sample: {repr(sample)}")
                yield sample, timestamp

        except KeyboardInterrupt:
            print("Interrupted by user.")

    return None, None


def format_csv_line(sample, timestamp, marker=None):
    return f'\n{timestamp},{",".join(sample)},{"[Marker]" if marker else ""},{timestamp if marker else ""},{marker if marker else ""},'


def main(path='data_lsl.csv'):
    path = Path(path)
    path.parent.mkdir(exist_ok=True, parents=True)

    lsl_gen = read_lsl()
    sample, timestamp = next(lsl_gen)
    if sample is None or timestamp is None:
        print('No data fetched.')
        return

    print('Keys for markers:')
    print(markers)

    key_pressed = False
    with open(path, 'w') as f:
        f.write(
            'Timestamp,Channel 1,Channel 2,Channel 3,Channel 4,Channel 5,Channel 6,Channel 7,Channel 8,Channel 9,Channel 10,Channel 11,Channel 12,Channel 13,Channel 14,Channel 15,Channel 16,Channel 17,Channel 18,Channel 19,Channel 20,Channel 21,Channel 22,Channel 23,Channel 24,Channel 25,Channel 26,Channel 27,Marker tag,Marker timestamp,Marker value,')
        while sample is not None and timestamp is not None:
            marker = None
            for key in ['1', '2', '3', '4']:
                if keyboard.is_pressed(key):
                    if not key_pressed:
                        marker = markers[int(key)]
                        print(f"Marker {marker} added at timestamp {timestamp}.")
                    key_pressed = True
                    break
            else:
                key_pressed = False
            f.write(format_csv_line(sample, timestamp, marker))
            sample, timestamp = next(lsl_gen, (None, None))


if __name__ == '__main__':
    main()
