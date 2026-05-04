# Tune

Enable creating and sending a tune to be played on the system.

## Enums

### `SongElement`

An element of the tune

| Value | Description |
|-------|-------------|
| `STYLE_LEGATO` (0) | |
| `STYLE_NORMAL` (1) | |
| `STYLE_STACCATO` (2) | |
| `DURATION_1` (3) | |
| `DURATION_2` (4) | |
| `DURATION_4` (5) | |
| `DURATION_8` (6) | |
| `DURATION_16` (7) | |
| `DURATION_32` (8) | |
| `NOTE_A` (9) | |
| `NOTE_B` (10) | |
| `NOTE_C` (11) | |
| `NOTE_D` (12) | |
| `NOTE_E` (13) | |
| `NOTE_F` (14) | |
| `NOTE_G` (15) | |
| `NOTE_PAUSE` (16) | |
| `SHARP` (17) | |
| `FLAT` (18) | |
| `OCTAVE_UP` (19) | |
| `OCTAVE_DOWN` (20) | |

### `TuneResult`

Possible results returned for tune requests.

| Value | Description |
|-------|-------------|
| `UNKNOWN` (0) | |
| `SUCCESS` (1) | |
| `INVALID_TEMPO` (2) | |
| `TUNE_TOO_LONG` (3) | |
| `ERROR` (4) | |
| `NO_SYSTEM` (5) | |

## `TuneDescription`

Tune description, containing song elements and tempo.

## `Tune`

Enable creating and sending a tune to be played on the system.

### `play_tune_async`

```python
def play_tune_async(tune_description, callback: Callable, user_data: Any = None)
```

Send a tune to be played by the system.

### `play_tune`

```python
def play_tune(tune_description)
```

Get play_tune (blocking)
