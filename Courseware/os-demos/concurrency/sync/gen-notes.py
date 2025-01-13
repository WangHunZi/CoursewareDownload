# Author: GPT-4-turbo

import numpy as np
import soundfile as sf

# Constants
SAMPLE_RATE = 44100  # Sample rate in Hz
DURATION = 1  # Duration in seconds
VOLUME = 0.1  # Volume, scale from 0 to 1

# Function to generate the frequency for each piano key
def piano_key_frequency(n):
    """
    Calculate the frequency of the nth key on a piano
    n is the number of the key, with 1 being A0 and 88 being C8
    """
    return 2 ** ((n - 49) / 12) * 440.0

# Function to generate a slightly richer tone than a pure sine wave
def generate_tone(frequency):
    t = np.linspace(0, DURATION, int(SAMPLE_RATE * DURATION), False)
    # Fundamental frequency
    tone = np.sin(frequency * 2 * np.pi * t)
    # Add harmonics
    for harmonic in range(2, 5):  # First few harmonics
        harmonic_freq = frequency * harmonic
        harmonic_amplitude = VOLUME / harmonic  # Decreasing amplitude for each harmonic
        tone += harmonic_amplitude * np.sin(harmonic_freq * 2 * np.pi * t)
    tone *= VOLUME / np.max(np.abs(tone))  # Normalize volume
    return tone

# Generate and save the tones for all 88 piano keys
for n in range(1, 89):
    frequency = piano_key_frequency(n)
    tone = generate_tone(frequency)
    filename = f"piano_key_{n}.wav"
    sf.write(filename, tone, SAMPLE_RATE)
    print(f"Generated {filename} with frequency {frequency:.2f} Hz")

print("All piano key tones generated.")
