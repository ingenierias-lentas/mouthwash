import pretty_midi as pm
import itertools

# middle C
baseNote = 60

chordDuration = .1
noteDuration = chordDuration*.8

pmObj = pm.PrettyMIDI()

synthVoice = pm.instrument_name_to_program('Whistle')
pmObj.instruments.append(pm.Instrument(synthVoice))

currTime = 0.0
velocity = 100

for offset1, offset2 in itertools.combinations(range(1, 12), 2):
    pmObj.instruments[0].notes.append(pm.Note(
        velocity, baseNote, currTime, currTime + noteDuration))
    pmObj.instruments[0].notes.append(pm.Note(
        velocity, baseNote + offset1, currTime, currTime + noteDuration))
    pmObj.instruments[0].notes.append(pm.Note(
        velocity, baseNote + offset2, currTime, currTime + noteDuration))
    currTime += chordDuration

midiFilename = "all_chords.mid"
pmObj.write(midiFilename)
