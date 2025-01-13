enum {
    Cs = 41,
    D = 42,
    A = 49,
    B = 51,
    E = 44,
    Fs = 46,
    G = 47,

    A0 = A - 12,
    B0 = B - 12,
    D0 = D - 12,
    F0s = Fs - 12,
    G0 = G - 12,

    C1s = Cs + 12,
    D1 = D + 12,
    F1s = Fs + 12,
    A1 = A + 12,
    B1 = B + 12,
};

int NOTES[4][8] = {
    {D, A0, B0, F0s, G0, D0, G0, A0},
    {A, E, Fs, Cs, D, A0, D, E},
    {D1, A, B, Fs, G, D, G, A},
    {F1s, C1s, D1, D, B, Fs, B, C1s},
};

#define N LENGTH(NOTES[0])
#define T LENGTH(NOTES)

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))
