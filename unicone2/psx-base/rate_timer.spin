{{
  Time the frequency at which an event occurs.

  This object takes samples of the system counter, and keeps
  a running average of the interval between samples.

  -- Micah Dowty <micah@navi.cx>
}}

CON
  NUM_SAMPLES = 64


VAR
  long  samples[NUM_SAMPLES]
  long  head, sum
  long  n_samples
  long  last_cnt
  byte  string_buf[11]


PUB init
  '' Reset the rate timer. Clears all stored samples.

  longfill(@samples, 0, NUM_SAMPLES + 2)
  n_samples := -1


PUB sample | now
  '' Take a sample at the current time

  now := cnt

  n_samples := (n_samples + 1) <# NUM_SAMPLES
  if n_samples > 0
    sum -= samples[head]
    sum += samples[head] := now - last_cnt
    head++
    if head == NUM_SAMPLES
      head := 0
  last_cnt := now


PUB average_interval : interval
  '' Get the average interval, in clock ticks, between events.
  '' If too few samples have been taken, returns zero.

  if n_samples <> 0
    interval := sum / n_samples

PUB average_hz : chz | interval 
  '' Get the average event rate, in centihertz (Hz / 100).
  '' If too few samples have been taken, returns zero.

  interval := average_interval
  if interval <> 0
    chz := clkfreq / (interval / 100)


PUB hz_string : str | chz
  '' Get a string describing the average event rate.
  '' If enough samples have been taken, returns a
  '' string of the form "####.## Hz". If not, returns
  '' the string "? Hz".

  chz := average_hz                                
  if chz == 0
    str := string("? Hz")
  else
    string_buf[0] := ((chz / 100000) // 10) + "0"
    string_buf[1] := ((chz / 10000) // 10) + "0"
    string_buf[2] := ((chz / 1000) // 10) + "0"
    string_buf[3] := ((chz / 100) // 10) + "0"
    string_buf[4] := "."
    string_buf[5] := ((chz / 10) // 10) + "0"
    string_buf[6] := ( chz      // 10) + "0"    
    string_buf[7] := " "
    string_buf[8] := "H"
    string_buf[9] := "z"
    string_buf[10] := 0
    str := @string_buf

           