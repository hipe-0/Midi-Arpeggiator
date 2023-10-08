# Midi-Arpeggiator

Hello to everyone, I share this fun project that makes me very creative musically. It can be used as arpeggiator and I use it as composition tool, to express out ideas.
Demo here https://www.youtube.com/watch?v=DeB5YvRfC6c

The design is by me. 
No reference to existing devices. 
Inspiration from software midi VST like Instachord, Melodicflow, Nora, Chtullu or Captain Chord.
3D Made with Fusion 360 in 3h.
The body is 3D printed in PETG plastic, in 2 pieces : orange face (6h printing time) and translucent walls (3h printing time) glued together. A plug on the back secures the usb cable.
Buttons are low cost arcade machine buttons
7 buttons left hand presets for chords I, II, III, IV, V, VI and VII of the mode and the root key
  Pressing a chord button twice inverts the chord (minor gets major and major gets minor)
  Pressing a chord 3 times pushes all notes +1 semitone, to reach out-of-harmony chords and accidentals

8 buttons for individual notes 1-7 in a chord, mimicking a bit like a piano 
  + Inclusive a bass note for the right thumb, 1 octave down

A single record button. Performing :
  1. record (press continuously, it plays a loop immediately after button release), 
  2. play (short press), 
  3. overdub (press continuously while playing) 
  4. and stop (short press) 

3 potentiometers for (8x) Mode, (12x) Root Keys and (5x) Octave
  Modes I choose are 
  1. Major
  2. Minor harmonic
  3. Minor melodic
  4. Minor natural (eolian)
  5. Dorian (celtic)
  6. Mixolydian (folk blues)
  7. Phrygian (metal)
  8. Phrygian Dominant (metal)

  Root Keys are 
    in order of fiths starting with C, then G, D, A, E, B, F#, ...

  Octaves are 
    -2, -1, 0, +1, +2

Recording led : green
The "brain" of the system is an Arduino micro, using midiusb library
Coding in C++ by me
I used a prototype board and soldered only some basic resistors and the wires.
Midi synchronisation as slave. 
Midi Velocity is fixed value (100) only
Midi Chanel 1 only
Latency is in my opinion negligible. At least on my Focusrite scarlett solo ASIO sound card. Some Latency can be seen on midi note editor.

It took me roughly about 12 h to build the prototype from start to finish. (I'm learning !)

Arduino pins:
pin A0 connects to mode pot
pin A1 connects to Root pot
pin A2 connects to octave pot
pin A3 connects to chord buttons voltage read with Resistors in parallel for each button to differentiate voltages + 1 single 1K resitor in serie + 1 x 0.22uF capacitor in parallel for smooting the values, optional
  button for chord I is connected directly with no resistor
  button for chord II is in serie with 100 Ohm resistor
  button for chord III is in serie with 320 Ohm resistor
  button for chord IV is in serie with 560 Ohm resistor
  button for chord V is in serie with 1K Ohm resistor
  button for chord VI is in serie with 2.2K Ohm resistor
  button for chord VII is in serie with 10K Ohm resistor

all pots connected to pin AREF and GND
pin 2 is connected to button note 1 
pin 3 is connected to button note 2
pin 4 is connected to button note 3
pin 5 is connected to button note 4
pin 6 is connected to button note 5
pin 7 is connected to button note 6
pin 8 is connected to button note 7
pin 10 is connected to LED in serie with resistor (probably 1K)
pin 11 is connected to button "bass" note
pin 12 in connected to record button



Hippolyte
