#version 450

// "It's never right to play ragtime fast!" - Scott Joplin
// ...but here in ShaderToy, we're only allowed 3 minutes of music :)
#define BPM 98.0

#define BEAT (60./BPM)     // length of a quarter note in seconds
#define MEASURE (2.*BEAT)  // length of a measure (2/4 time)

float timeTrack(float time)
{
    time -= .5;  // pickup measure
    if (time >= 16.*MEASURE)
    {
        time -= 16.*MEASURE;
        if (time >= 15.*MEASURE) time += MEASURE;  // 2nd ending
    }
    if (time >= 33.*MEASURE)
    {
        time -= 16.*MEASURE;
        if (time >= 32.*MEASURE) time += MEASURE;  // 2nd ending
    }
    if (time >= 34.*MEASURE)
    {
        time -= 34.*MEASURE;  // back to the beginning
        if (time >= 15.*MEASURE) time = 50.*MEASURE + time - 15.*MEASURE;  // to trio
    }
    if (time >= 67.*MEASURE)
    {
        time = 51.*MEASURE + time - 67.*MEASURE;  // repeat trio
        if (time >= 66.*MEASURE) time = 67.*MEASURE + time - 66.*MEASURE;  // 2nd ending
    }
    if (time >= 84.*MEASURE)
    {
        time = 68.*MEASURE + time - 84.*MEASURE;
        if (time >= 83.*MEASURE) time += MEASURE;  // 2nd ending
    }

    return time;
}

// Right hand
#define CHANNEL1 \
DYN(F) \
/*1*/  R(.25) N(.25,Gs4) CHORD2(.25,Ds4,Ds5) N(.25,Gs4) N(.25,C5) CHORD2(.5,Ds4,Ds5) N(.25,G4) \
/*2*/  CHORD2(.25,Ds4,Ds5) N(.25,G4) N(.25,As4) CHORD2(1.25,Ds4,Ds5) \
/*3*/  R(.25) N(.25,Gs4) CHORD2(.25,Ds4,Ds5) N(.25,Gs4) N(.25,C5) CHORD2(.5,Ds4,Ds5) N(.25,G4) \
/*4*/  CHORD2(.25,Ds4,Ds5) N(.25,G4) N(.25,As4) CHORD2(.5,Ds4,Ds5) R(.5) CHORD2(.25,Ds4,Ds5) \
/*5*/  R(.25) N(.25,Gs4) N(.25,B4) CHORD2(.25,E4,E5) R(.25) CHORD2(.25,Ds4,Ds5) R(.25) CHORD2(.25,Ds4,Ds5) \
/*6*/  R(.25) N(.25,Gs4) N(.25,B4) CHORD2(.25,E4,E5) R(.25) CHORD2(.25,Ds4,Ds5) R(.5) \
/*7*/  DYN(P) R(2.) \
/*8*/  R(.25) N(.25,Gs4) N(.25,B4) N(.25,Gs5) R(.25) N(.25,Gs5) N(.25,B5) N(.25,Gs6) \
/*9*/  CRESC(2.,MF,F) CHORD2(.5,Gs5,Gs6) CHORD2(.5,Gs5,Gs6) CHORD2(.5,Gs5,Gs6) CHORD2(.25,Gs5,Gs6) CHORD2(.5,Gs5,Gs6) \
/*10*/ N(.25,Ds6) N(.25,F6) N(.25,C6) N(.25,Ds6) CHORD2(.5,Gs5,F6) CHORD2(.5,E5,Gs5) \
/*11*/ N(.25,As5) CHORD2(.25,E5,B5) N(.25,Gs5) N(.25,As5) CHORD2(.5,Ds5,C6) N(.25,Gs5) \
/*12*/ CHORD2(.25,Ds5,C6) N(.25,Gs5) CHORD2(.5,Ds5,As5) CHORD2(.5,Ds5,Gs5) R(.25) CHORD2(.75,Gs4,Gs5) \
/*13*/ CHORD2(.5,Gs4,Gs5) CHORD2(.5,Gs4,Gs5) CHORD2(.25,Gs4,Gs5) CHORD2(.5,Gs4,Gs5) \
/*14*/ N(.25,Ds5) CHORD2(.25,Gs4,F5) N(.25,C5) N(.25,Ds5) CHORD2(.5,Gs4,F5) CHORD2(.5,E4,Gs4) \
/*15*/ N(.25,As4) CHORD2(.25,E4,B4) N(.25,Gs4) N(.25,As4) CHORD2(.5,Ds4,C5) N(.25,Gs4) \
/*16 - 1st ending*/ CHORD2(.25,Ds4,C5) N(.25,Gs4) CHORD2(.5,Ds4,As4) CHORD2(.5,Ds4,Gs4) R(.5) \
/*17 - 2nd ending*/ CHORD2(.25,Ds4,C5) N(.25,Gs4) CHORD2(.5,Ds4,As4) CHORD2(.5,Ds4,Gs4) R(.5) \
/*18*/ DYN(F) R(.25) N(.25,G5) CHORD2(.25,Ds5,Ds6) N(.25,G5) N(.25,As5) CHORD2(.5,D5,D6) N(.25,G5) \
/*19*/ CRESC(2.,F,MF) CHORD2(.25,Cs5,Cs6) N(.25,G5) N(.25,As5) CHORD2(.5,C5,C6) N(.25,Ds5) CHORD2(.25,As4,As5) N(.25,Ds5) \
/*20*/ R(.25) N(.25,C5) CHORD2(.25,Gs4,Gs5) N(.25,C5) N(.25,Ds5) CHORD2(.5,F4,F5) N(.25,C5) \
/*21*/ CHORD2(.25,Gs4,Gs5) N(.25,C5) N(.25,Ds5) CHORD2(.5,F4,F5) N(.25,C5) CHORD2(.5,F4,F5) \
/*22*/ DYN(F) R(.25) N(.25,Ds5) CHORD2(.25,G4,G5) N(.25,As4) N(.25,Cs5) CHORD2(.5,F4,F5) N(.25,Ds5) \
/*23*/ CRESC(2.,F,MF) CHORD2(.25,G4,G5) N(.25,As4) N(.25,Cs5) CHORD2(.5,F4,F5) N(.25,Cs5) CHORD2(.5,F4,F5) \
/*24*/ R(.25) N(.25,C5) CHORD2(.25,Gs4,Gs5) N(.25,C5) N(.25,Ds5) CHORD2(.5,F4,F5) N(.25,C5) \
/*25*/ CHORD2(.25,Gs4,Gs5) N(.25,C5) N(.25,Ds5) CHORD2(.5,F4,F5) N(.25,C5) CHORD2(.5,F4,F5) \
/*26*/ DYN(F) R(.25) N(.25,G5) CHORD2(.25,Ds5,Ds6) N(.25,G5) N(.25,As5) CHORD2(.5,D5,D6) N(.25,G5) \
/*27*/ CRESC(2.,F,MP) CHORD2(.25,Cs5,Cs6) N(.25,G5) N(.25,As5) CHORD2(.5,C5,C6) N(.25,Ds5) CHORD2(.25,As4,As5) N(.25,Ds5) \
/*28*/ R(.25) N(.25,C5) CHORD2(.25,Gs4,Gs5) N(.25,C5) N(.25,Ds5) CHORD2(.5,F4,F5) N(.25,C5) \
/*29*/ CRESC(2.,MP,F) CHORD2(.5,Gs4,Gs5) CHORD2(.5,Gs4,Gs5) CHORD2(.5,G4,G5) CHORD2(.5,Fs4,Fs5) \
/*30*/ R(.25) N(.25,F4) N(.25,A4) N(.25,C5) N(.25,F5) N(.25,C5) N(.25,A4) N(.25,F4) \
/*31*/ R(.25) N(.25,F4) N(.25,As4) N(.25,Cs5) CHORD2(.5,F4,F5) CHORD3(.5,F4,As4,Cs5) \
/*32*/ CHORD4(.5,D4,F4,Gs4,C5) R(.25) CHORD4(.25,D4,F4,Gs4,C5) R(.25) CHORD2(.5,Cs4,As4) N(.25,Ds4) \
/*33 - 1st ending*/ CHORD2(.5,C4,Gs4) CHORD2(.5,Ds4,Ds5) CHORD2(.5,Ds4,Ds5) CHORD2(.5,Ds4,Ds5) \
/*34 - 2nd ending*/ CRESC(2.,F,FF) R(.25) N(.25,Gs4) N(.25,C5) N(.25,Ds5) CHORD2(.5,Gs4,Gs5) R(.5) \
AT(50.*2.) \
/*50*/ DYN(F) CHORD2(.25,Ds4,C5) N(.25,Gs4) CHORD2(.5,Ds4,As4) CHORD2(.5,Ds4,Gs4) CHORD2(.5,Gs4,Gs5) \
/* Trio */ \
/*51*/ DYN(MF) CHORD4(.75,Gs4,C5,Ds5,Gs5) CHORD4(.5,As4,C5,Ds5,Gs5) CHORD2(.25,C5,Ds5) N(.5,As4) \
/*52*/ CRESC(2.,MF,MP) CHORD4(.75,Gs4,C5,Ds5,Gs5) CHORD4(.5,As4,C5,Ds5,Gs5) CHORD2(.25,C5,Ds5) N(.25,As4) CHORD2(.25,C5,Ds5) \
/*53*/ N(.25,Gs4) N(.25,Cs5) N(.25,As4) N(.25,Cs5) N(.25,F5) N(.25,Gs4) N(.25,Cs5) N(.25,F5) \
/*54*/ CRESC(2.,MP,MF) N(.25,As4) N(.25,Cs5) N(.25,F5) N(.5,Gs4) N(.25,F5) N(.25,As4) N(.25,F5) \
/*55*/ CHORD4(.75,Gs4,C5,Ds5,Gs5) CHORD4(.5,As4,C5,Ds5,Gs5) CHORD2(.25,C5,Ds5) N(.5,As4) \
/*56*/ CRESC(2.,MF,MP) CHORD4(.75,Gs4,C5,Ds5,Gs5) CHORD4(.5,As4,C5,Ds5,Gs5) CHORD2(.25,C5,Ds5) N(.25,As4) CHORD2(.25,C5,Ds5) \
/*57*/ N(.25,Gs4) N(.25,Cs5) N(.25,As4) N(.25,Cs5) N(.25,F5) N(.25,Gs4) N(.25,Cs5) N(.25,F5) \
/*58*/ CRESC(2.,MP,MF) N(.25,As4) N(.25,Cs5) N(.25,F5) N(.5,Gs4) N(.25,F5) N(.25,A4) CHORD2(.25,Ds5,F5) \
/*59*/ CHORD4(.75,As4,D5,F5,As5) CHORD4(.5,C5,D5,F5,As5) CHORD2(.25,D5,F5) N(.5,C5) \
/*60*/ CHORD4(.75,As4,D5,F5,As5) CHORD4(.5,C5,D5,F5,As5) CHORD2(.25,D5,F5) N(.25,C5) CHORD2(.25,D5,F5) \
/*61*/ CRESC(4.,MF,F) R(.25) N(.25,Ds5) CHORD2(.25,As4,As5) N(.25,Ds5) N(.25,Fs5) CHORD2(.5,C5,C6) N(.25,Ds5) \
/*62*/ CHORD2(.25,As4,As5) N(.25,Ds5) N(.25,Fs5) CHORD2(.5,C5,C6) N(.25,Ds5) CHORD2(.5,As4,As5) \
/*63*/ DYN(FF) CHORD3(.5,Cs5,E5,Cs6) CHORD3(.5,Cs5,E5,Cs6) CRESC(1.,FF,F) CHORD3(.5,C5,E5,C6) CHORD3(.5,As4,E5,As5) \
/*64*/ CHORD2(.25,Gs4,F5) N(.25,Cs5) N(.25,Ds5) CHORD2(.5,As4,Fs5) N(.25,As4) CHORD2(.5,Gs4,F5) \
/*65*/ CRESC(2.,F,MF) CHORD2(.25,G4,F5) N(.25,Cs5) N(.25,Ds5) CHORD2(.5,Fs4,F5) N(.25,C5) CHORD2(.25,G4,Ds5) CHORD2(.5,F4,Cs5) \
/*66 - 1st ending*/ DYN(FF) CHORD2(.5,Gs4,Gs5) N(.25,F5) CHORD2(.25,Gs4,Gs5) N(.25,F5) CHORD2(.25,Gs4,Gs5) N(.25,F5) \
/*67 - 2nd ending*/ /*CHORD2(.25,F4,Cs5)*/ R(.25) N(.25,Cs5) N(.25,F5) N(.25,Gs5) CHORD2(.5,Cs5,Cs6) R(.5) \
/*68*/ DYN(F) CHORD2(.5,Gs4,Gs5) CHORD2(.5,Gs4,F5) CHORD2(.5,Gs4,Gs5) CHORD2(.5,Gs4,F5) \
/*69*/ CHORD2(.5,F5,Gs5) CHORD2(.25,F5,As5) CHORD2(.5,F5,C6) N(.25,As5) N(.25,Gs5) N(.25,F5) \
/*70*/ N(.25,Ds5) N(.5,F5) CHORD2(1.25,Gs4,C5) \
/*71*/ R(.25) N(.25,Ds5) CHORD2(.25,Gs4,F5) N(.25,C5) N(.25,Ds5) CHORD2(.5,Gs4,F5) N(.25,C5) \
/*72*/ CHORD2(.5,G4,Ds5) N(.25,F5) CHORD2(1.25,G4,As4) \
/*73*/ R(.25) N(.25,Cs5) CHORD2(.25,G4,F5) N(.25,As4) N(.25,Cs5) CHORD2(.5,G4,F5) CRESC(4.25,P,F) CHORD2(.5,Gs4,C5) \
/*74*/ N(.25,Ds5) CHORD2(.25,Gs4,F5) N(.25,C5) N(.25,Ds5) CHORD2(.5,Gs4,F5) CHORD2(.5,Gs4,C5) \
/*75*/ N(.25,Ds5) CHORD2(.25,Gs4,F5) N(.25,C5) N(.25,Ds5) CHORD2(.5,Gs4,F5) N(.25,Ds5) \
/*76*/ CHORD2(.5,Gs4,Gs5) CHORD2(.5,Gs4,F5) CHORD2(.5,Gs4,Gs5) CHORD2(.5,Gs4,F5) \
/*77*/ CHORD2(.5,F5,Gs5) CHORD2(.25,F5,As5) CHORD2(.5,F5,C6) N(.25,As5) N(.25,Gs5) N(.25,F5) \
/*78*/ N(.5,Gs5) N(.5,F5) N(.25,Ds5) N(.5,Gs5) CHORD2(.5,Gs4,C5) \
/*79*/ N(.25,Ds5) CHORD2(.25,Gs4,F5) N(.25,C5) N(.25,Ds5) CHORD2(.5,Gs4,F5) CHORD2(.5,F4,Gs4) \
/*80*/ N(.25,As4) CHORD2(.5,F4,Gs4) CHORD2(.5,E4,Gs4) CHORD2(.25,E4,As4) CHORD2(.5,Ds4,Gs4) \
/*81*/ N(.25,As4) CHORD2(.25,Ds4,C5) N(.25,Gs4) N(.25,As4) CHORD2(.5,Ds4,C5) CHORD2(.5,D4,Gs4) \
/*82*/ N(.25,As4) CHORD2(.25,D4,C5) N(.25,Gs4) R(.25) CHORD2(.5,Cs4,As4) N(.25,Ds4) \
/*83 - 1st ending*/ CHORD2(.5,C4,Gs4) CHORD2(.5,Ds4,Ds5) CHORD2(.5,Ds4,Ds5) CHORD2(.5,Ds4,Ds5) \
/*84 - 2nd ending*/ CHORD2(.5,C4,Gs4) DYN(FF) CHORD3(.5,G4,Cs5,Ds5) CHORD4(.5,Gs4,C5,Ds5,Gs5) \

// Left hand
#define CHANNEL2 \
AT(-0.5) \
DYN(F) \
CHORD2(.5,Ds2,Ds3) \
/*1*/  CHORD2(.5,Gs2,Gs3) CHORD3(.5,Ds3,Gs3,C3) CHORD3(.5,Ds3,Gs3,C3) CHORD2(.5,A2,A3) \
/*2*/  CHORD2(.5,As2,As3) CHORD3(.5,Ds3,G3,Cs3) CHORD3(.5,Ds3,G3,Cs3) CHORD2(.5,Ds2,Ds3) \
/*3*/  CHORD2(.5,Gs2,Gs3) CHORD3(.5,Ds3,Gs3,C3) CHORD3(.5,Ds3,Gs3,C3) CHORD2(.5,A2,A3) \
/*4*/  CHORD2(.5,As2,As3) CHORD3(.5,Ds3,G3,Cs3) CHORD3(.5,Ds3,G3,Cs3) CHORD2(.5,Ds2,Ds3) \
/*5*/  CHORD2(1.,E2,E3) CHORD2(.5,Ds2,Ds3) CHORD2(.5,Ds2,Ds3) \
/*6*/  CHORD2(1.,E2,E3) CHORD2(.5,Ds2,Ds3) R(.5) \
/*7*/  DYN(P) N(.25,Gs1) N(.25,Gs2) N(.25,B2) N(.25,Gs3) N(.25,Gs2) N(.25,Gs3) N(.25,B3) N(.25,Gs4) \
/*8*/  CRESC(2.,P,MF) N(.25,Gs3) R(.75) N(.25,Gs4) R(.75) \
/*9*/  CRESC(2.,MF,F) CHORD4(.5,D4,F4,Gs4,B4) CHORD4(.5,D4,F4,Gs4,B4) CHORD4(.5,D4,F4,Gs4,B4) CHORD4(.5,D4,F4,Gs4,B4) \
/*10*/ CHORD3(.5,Ds4,Gs4,C5) CHORD3(.5,Ds4,Gs4,C5) CHORD3(.5,Ds4,Gs4,C5) CHORD3(.5,Ds4,Gs4,C5) \
/*11*/ CHORD3(.5,E4,Gs4,B4) CHORD3(.5,E4,Gs4,B4) CHORD3(.5,Ds4,Gs4,C5) CHORD3(.5,Ds4,Gs4,C5) \
/*12*/ CHORD3(.5,Ds4,Gs4,C5) CHORD3(.5,Ds4,G4,Cs5) CHORD2(.5,Gs4,C5) R(.5) \
/*13*/ CHORD4(.5,D3,F3,Gs3,B3) CHORD4(.5,D3,F3,Gs3,B3) CHORD4(.5,D3,F3,Gs3,B3) CHORD4(.5,D3,F3,Gs3,B3) \
/*14*/ CHORD3(.5,Ds3,Gs3,C4) CHORD3(.5,Ds3,Gs3,C4) CHORD3(.5,Ds3,Gs3,C4) CHORD3(.5,Ds3,Gs3,C4) \
/*15*/ CHORD3(.5,E3,Gs3,B3) CHORD3(.5,E3,Gs3,B3) CHORD3(.5,Ds3,Gs3,C4) CHORD3(.5,Ds3,Gs3,C4) \
/*16 - 1st ending*/ CHORD3(.5,Ds3,Gs3,C4) CHORD3(.5,Ds3,G3,Cs4) CHORD2(.5,Gs3,C4) CHORD2(.5,Ds2,Ds3) \
/*17 - 2nd ending*/ CHORD3(.5,Ds3,Gs3,C4) CHORD3(.5,Ds3,G3,Cs4) CHORD2(.5,Gs3,C4) CHORD2(.5,A2,A3) \
/*18*/ DYN(F) CHORD2(.5,As2,As3) CHORD3(.5,Ds3,G3,Ds4) CHORD2(.5,Ds2,Ds3) CHORD3(.5,Ds3,G3,Ds4) \
/*19*/ CRESC(2.,F,MF) CHORD2(.5,As2,As3) CHORD3(.5,Ds3,G3,Ds4) CHORD2(.5,Ds2,Ds3) CHORD2(.5,G2,G3) \
/*20*/ CHORD2(.5,Gs2,Gs3) CHORD3(.5,Ds3,Gs3,C4) CHORD2(.5,Ds3,Ds3) CHORD3(.5,Ds3,Gs3,C4) \
/*21*/ CHORD2(.5,Gs2,Gs3) CHORD3(.5,Ds3,Gs3,C4) CHORD2(.5,Gs2,Gs3) CHORD2(.5,A2,A3) \
/*22*/ DYN(F) CHORD2(.5,As2,As3) CHORD3(.5,Ds3,G3,Ds4) CHORD2(.5,Ds2,Ds3) CHORD3(.5,Ds3,G3,Ds4) \
/*23*/ CRESC(2.,F,MF) CHORD2(.5,As2,As3) CHORD3(.5,Ds3,G3,Ds4) CHORD2(.5,As2,As3) CHORD2(.5,B2,B3) \
/*24*/ CHORD2(.5,C3,C4)   CHORD3(.5,Ds3,Gs3,C4) CHORD2(.5,Ds2,Ds3) CHORD3(.5,Ds3,Gs3,C4) \
/*25*/ CHORD2(.5,Gs2,Gs3) CHORD3(.5,Ds3,Gs3,C4) CHORD2(.5,Gs2,Gs3) CHORD2(.5,A2,A3) \
/*26*/ DYN(F) CHORD2(.5,As2,As3) CHORD3(.5,Ds3,G3,Ds4) CHORD2(.5,Ds2,Ds3) CHORD3(.5,Ds3,G3,Ds4) \
/*27*/ CRESC(2.,F,MP) CRESC(2.,F,MF) CHORD2(.5,As2,As3) CHORD3(.5,Ds3,G3,Ds4) CHORD2(.5,Ds2,Ds3) CHORD2(.5,G2,G3) \
/*28*/ CHORD2(.5,Gs2,Gs3) CHORD3(.5,Ds3,Gs3,C4) CHORD2(.5,Ds3,Ds3) CHORD3(.5,Ds3,Gs3,C4) \
/*29*/ CRESC(2.,MP,F) CHORD2(.5,Gs2,Gs3) CHORD2(.5,Gs2,Gs3)    CHORD2(.5,G2,G3)   CHORD2(.5,Fs2,Fs3) \
/*30*/ CHORD2(.5,F2,F3)   CHORD2(.5,F2,F3)      CHORD2(.5,A2,A3)   CHORD2(.5,A2,A3) \
/*31*/ CHORD2(.5,As2,As3) CHORD3(.5,F3,As3,Cs4) CHORD3(.5,F3,As3,Cs4) CHORD3(.5,F3,As3,Cs4) \
/*32*/ CHORD3(.5,As2,F3,As3) CHORD3(.5,As2,F3,As3) CHORD2(.5,Ds2,Ds3) CHORD2(.5,G2,G3) \
/*33 - 1st ending*/ CHORD2(.5,Gs2,Gs3) CHORD3(.5,Ds3,Gs3,C4) CHORD3(.5,Ds3,Gs3,C4) CHORD2(.5,A2,A3) \
/*34 - 2nd ending*/ CRESC(2.,F,FF) CHORD2(.5,Gs2,Gs3) CHORD3(.5,Gs3,C4,Ds4) CHORD3(.5,Gs3,C4,Ds4) CHORD2(.5,Ds2,Ds3) \
AT(50.*2.) \
/*50*/ DYN(F) CHORD3(.5,Ds3,Gs3,C4) CHORD3(.5,Ds3,G3,Cs4) CHORD2(.5,Gs3,C4) R(.5) \
/* Trio */ \
/*51*/ DYN(MF) CHORD2(.5,Ds2,Ds3) CHORD3(.5,Gs3,C4,Fs4) CHORD2(.5,Fs2,Fs3) CHORD3(.5,Gs3,C4,Fs4) \
/*52*/ CRESC(2.,MF,MP) CHORD2(.5,Fs2,Fs3) CHORD3(.5,Gs3,C4,Fs4) CHORD2(.5,Ds2,Ds3) CHORD2(.5,C2,C3) \
/*53*/ CHORD2(.5,Cs2,Cs3) CHORD3(.5,Gs3,Cs4,F4) CHORD2(.5,Gs2,Gs3) CHORD3(.5,Gs3,Cs4,F4) \
/*54*/ CRESC(2.,MP,MF) CHORD2(.5,F2,F3)   CHORD3(.5,Gs3,Cs4,F4) CHORD2(.5,Cs2,Cs3) CHORD2(.5,D2,D3) \
/*55*/ CHORD2(.5,Ds2,Ds3) CHORD3(.5,Gs3,C4,Fs4) CHORD2(.5,Fs2,Fs3) CHORD3(.5,Gs3,C4,Fs4) \
/*56*/ CRESC(2.,MF,MP) CHORD2(.5,Fs2,Fs3) CHORD3(.5,Gs3,C4,Fs4) CHORD2(.5,Ds2,Ds3) CHORD2(.5,C2,C3) \
/*57*/ CHORD2(.5,Cs2,Cs3) CHORD3(.5,Gs3,Cs4,F4) CHORD2(.5,Gs2,Gs3) CHORD3(.5,Gs3,Cs4,F4) \
/*58*/ CRESC(2.,MP,MF) CHORD2(.5,F2,F3)   CHORD3(.5,Gs3,Cs4,F4) CHORD2(.5,Cs2,Cs3) CHORD2(.5,C2,C3) \
/*59*/ CHORD2(.5,As1,As2) CHORD3(.5,As3,D4,Gs4) CHORD2(.5,D2,D3)   CHORD3(.5,As3,D4,Gs4) \
/*60*/ CHORD2(.5,F2,F3)   CHORD3(.5,As3,D4,Gs4) CHORD2(.5,As2,As3) CHORD3(.5,As3,D4,Gs4) \
/*61*/ CRESC(4.,MF,F) CHORD2(.5,Ds2,Ds3) CHORD3(.5,As3,Ds4,Fs4) CHORD2(.5,Fs2,Fs3) CHORD3(.5,As3,Ds4,Fs4) \
/*62*/ CHORD2(.5,Ds2,Ds3) CHORD3(.5,As3,Ds4,Fs4) CHORD2(.5,Fs2,Fs3) CHORD3(.5,As3,Ds4,Fs4) \
/*63*/ DYN(FF) N(.25,G3) N(.5,As3) N(.25,Cs4) CRESC(1.,FF,F) N(.25,E4) N(.25,Cs4) N(.25,As3) N(.25,G3) \
/*64*/ CHORD2(.5,Gs2,Gs3) CHORD3(.5,Gs3,Cs4,F4) N(.5,As2) CHORD3(.5,Gs3,As3,D4) \
/*65*/ CRESC(2.,F,MF) CHORD3(.5,Ds3,As3,Cs4) CHORD3(.5,Ds3,As3,Cs4) CHORD2(.5,Gs3,C4) CHORD2(.5,Gs3,C4) \
/*66 - 1st ending*/ CHORD2(.5,Cs3,Cs4) CHORD3(.5,Gs3,Cs4,F4) DYN(FF) CHORD3(.5,Gs3,Cs4,F4) CHORD2(.5,D2,D3) \
/*67 - 2nd ending*/ CHORD2(.5,Cs3,Cs4) CHORD2(.5,Gs2,Gs3) CHORD2(.5,Cs2,Cs3) CHORD2(.5,C3,C4) \
/*68*/ DYN(F) CHORD2(.5,Cs3,Cs4) CHORD3(.5,Gs3,Cs4,F4) CHORD3(.5,Gs3,Cs4,F4) CHORD2(.5,C3,C4) \
/*69*/ CHORD2(.5,Cs3,Cs4) CHORD3(.5,Gs3,Cs4,F4) CHORD2(.5,As2,As3) CHORD2(.5,B2,B3) \
/*70*/ CHORD2(.5,C3,C4)   CHORD3(.5,Gs3,C4,Ds4) CHORD3(.5,Gs3,C4,Ds4) CHORD2(.5,Ds2,Ds3) \
/*71*/ CHORD2(.5,Gs2,Gs3) CHORD3(.5,Ds3,Gs3,C4) CHORD2(.5,Ds2,Ds3) CHORD2(.5,A2,A3) \
/*72*/ CHORD2(.5,As2,As3) CHORD3(.5,Ds3,G3,Ds4) CHORD2(.5,Ds2,Ds3) CHORD2(.5,A2,A3) \
/*73*/ CHORD2(.5,As2,As3) CHORD3(.5,Ds3,G3,Cs4) CHORD2(.5,Ds2,Ds3) CHORD2(.5,G2,G3) \
/*74*/ CRESC(4.,P,F) CHORD2(.5,Gs2,Gs3) CHORD3(.5,Ds3,Gs3,C4) CHORD2(.5,Ds2,Ds3) CHORD3(.5,Ds3,Gs3,C4) \
/*75*/ CHORD2(.5,Gs2,Gs3) CHORD3(.5,Ds3,Gs3,C4) CHORD2(.5,As2,As3) CHORD2(.5,C3,C4) \
/*76*/ CHORD2(.5,Cs3,Cs4) CHORD3(.5,Gs3,Cs4,F4) CHORD3(.5,Gs3,Cs4,F4) CHORD2(.5,C3,C4) \
/*77*/ CHORD2(.5,Cs3,Cs4) CHORD3(.5,Gs3,Cs4,F4) CHORD2(.5,As2,As3) CHORD2(.5,B2,B3) \
/*78*/ CHORD2(.5,C3,C4)   CHORD3(.5,Gs3,C4,Ds4) CHORD3(.5,Gs3,C4,Ds4) CHORD2(.5,Ds2,Ds3) \
/*79*/ CHORD2(.5,Gs2,Gs3) CHORD3(.5,Ds3,Gs3,C4) CHORD2(.5,Ds2,Ds3) CHORD3(.5,Ds3,Gs3,C4) \
/*80*/ CHORD2(.5,Cs2,Cs3) CHORD2(.5,Cs2,Cs3)    CHORD2(.5,As1,As2) CHORD2(.5,D2,D3) \
/*81*/ CHORD2(.5,Ds2,Ds3) CHORD3(.5,Ds3,Gs3,C4) CHORD2(.5,Ds2,Ds3) CHORD2(.5,E2,E3) \
/*82*/ CHORD2(.5,F2,F3)   CHORD2(.5,F2,F3)      CHORD2(.5,G2,G3)   CHORD2(.5,G2,G3) \
/*83 - 1st ending*/ CHORD2(.5,Gs2,Gs3) CHORD3(.5,Ds3,Gs3,C4) CHORD2(.5,As2,As3) CHORD2(.5,Cs3,Cs4) \
/*84 - 2nd ending*/ CHORD2(.5,Gs2,Gs3) DYN(FF) CHORD2(.5,Ds2,Ds3) CHORD2(.5,Gs1,Gs2) \
R(1000.) N(.5,C3)  // Notes get stuck with some Intel graphics cards. Adding a random note at the end fixes this problem somehow?

// Note frequencies

#define FREQ_C(oct)  (16.35*exp2(float(oct)))
#define FREQ_Cs(oct) (17.32*exp2(float(oct)))
#define FREQ_D(oct)  (18.35*exp2(float(oct)))
#define FREQ_Ds(oct) (19.45*exp2(float(oct)))
#define FREQ_E(oct)  (20.60*exp2(float(oct)))
#define FREQ_F(oct)  (21.83*exp2(float(oct)))
#define FREQ_Fs(oct) (23.12*exp2(float(oct)))
#define FREQ_G(oct)  (24.50*exp2(float(oct)))
#define FREQ_Gs(oct) (25.96*exp2(float(oct)))
#define FREQ_A(oct)  (27.50*exp2(float(oct)))
#define FREQ_As(oct) (29.14*exp2(float(oct)))
#define FREQ_B(oct)  (30.87*exp2(float(oct)))

#define PI 3.14159265359

float attenuate(float t)
{
    return exp2(-8.0*t);
}

// The built-in sin function has some precision issues at large x values, which results in
// annoying high pitched buzzing sounds. Keeping x within 0 and 2pi fixes that.
float better_sin(float x)
{
    return sin(mod(x, 2.0*PI));
}

// This doesn't sound exactly like a piano, but it's good enough for me!
float piano(float time,float freq)
{
    float sampleX = 0.;
    int maxHarm = 7;
    if (freq > 800.0) maxHarm = 5;  // make high notes less bright
    if (freq > 1600.0) maxHarm = 3;
    for (int i = 1; i <= maxHarm; i++)
        sampleX += better_sin(PI*2.0 * freq * float(i) * time) / float(i);
    return sampleX;
}

// note
#define N(len, note) note=(t>=x)?x:note;x+=len*60.0/BPM;
// rest
#define R(len) x+=len*(60.0/BPM);

#define GRACE(a)        x-=0.05*(60.0/BPM);N(0.05,a)
#define GRACE2(a, b)    x-=0.10*(60.0/BPM);N(0.05,a)N(0.05,b)
#define GRACE3(a, b, c) x-=0.15*(60.0/BPM);N(0.05,a)N(0.05,b)N(0.05,c)
#define CHORD2(len, a, b)       N(len, a) x-=len*60.0/BPM; N(len, b)
#define CHORD3(len, a, b, c)    N(len, a) x-=len*60.0/BPM; N(len, b) x-=len*60.0/BPM; N(len, c)
#define CHORD4(len, a, b, c, d) N(len, a) x-=len*60.0/BPM; N(len, b) x-=len*60.0/BPM; N(len, c) x-=len*60.0/BPM; N(len, d)

#define AT(time) x=time*60.0/BPM;

// Dynamics
#define PP 0.1
#define P  0.2
#define MP 0.25
#define MF 0.5
#define F  0.65
#define FF 1.0
#define DYN(d) if(t>=x)volume=d;
#define CRESC(len, a, b) if (t>=x)volume=mix(a, b, min(1.0,(t-x)/((len)*60.0/BPM)));

#define PLAYNOTES(note, freq) if(note>-10.)a+=piano(t-note,freq)*volume*attenuate(t-note);

float channel1(float t)
{
    float x = 0.0;
    float a = 0.0;
    // I tried using an array here, but it was too slow, and this managed to be faster somehow.
    float A3=-10.,Ds3=-10.,
          C4=-10.,Cs4=-10.,D4=-10.,Ds4=-10.,E4=-10.,F4=-10.,Fs4=-10.,G4=-10.,Gs4=-10.,A4=-10.,As4=-10.,B4=-10.,
          C5=-10.,Cs5=-10.,D5=-10.,Ds5=-10.,E5=-10.,F5=-10.,Fs5=-10.,G5=-10.,Gs5=-10.,A5=-10.,As5=-10.,B5=-10.,
          C6=-10.,Cs6=-10.,D6=-10.,Ds6=-10.,E6=-10.,F6=-10.,Fs6=-10.,G6=-10.,Gs6=-10.,A6=-10.,As6=-10.,B6=-10.,
          C7=-10.,D7=-10.,E7=-10.;
    float volume = 0.0;

    CHANNEL1

    PLAYNOTES(A3,  FREQ_A(3))
    PLAYNOTES(Ds3, FREQ_Ds(3))
    PLAYNOTES(C4,  FREQ_C(4))
    PLAYNOTES(Cs4, FREQ_Cs(4))
    PLAYNOTES(D4,  FREQ_D(4))
    PLAYNOTES(Ds4, FREQ_Ds(4))
    PLAYNOTES(E4,  FREQ_E(4))
    PLAYNOTES(F4,  FREQ_F(4))
    PLAYNOTES(Fs4, FREQ_Fs(4))
    PLAYNOTES(G4,  FREQ_G(4))
    PLAYNOTES(Gs4, FREQ_Gs(4))
    PLAYNOTES(A4,  FREQ_A(4))
    PLAYNOTES(As4, FREQ_As(4))
    PLAYNOTES(B4,  FREQ_B(4))
    PLAYNOTES(C5,  FREQ_C(5))
    PLAYNOTES(Cs5, FREQ_Cs(5))
    PLAYNOTES(D5,  FREQ_D(5))
    PLAYNOTES(Ds5, FREQ_Ds(5))
    PLAYNOTES(E5,  FREQ_E(5))
    PLAYNOTES(F5,  FREQ_F(5))
    PLAYNOTES(Fs5, FREQ_Fs(5))
    PLAYNOTES(G5,  FREQ_G(5))
    PLAYNOTES(Gs5, FREQ_Gs(5))
    PLAYNOTES(A5,  FREQ_A(5))
    PLAYNOTES(As5, FREQ_As(5))
    PLAYNOTES(B5,  FREQ_B(5))
    PLAYNOTES(C6,  FREQ_C(6))
    PLAYNOTES(Cs6, FREQ_Cs(6))
    PLAYNOTES(D6,  FREQ_D(6))
    PLAYNOTES(Ds6, FREQ_Ds(6))
    PLAYNOTES(E6,  FREQ_E(6))
    PLAYNOTES(F6,  FREQ_F(6))
    PLAYNOTES(Fs6, FREQ_Fs(6))
    PLAYNOTES(G6,  FREQ_G(6))
    PLAYNOTES(Gs6, FREQ_Gs(6))
    PLAYNOTES(A6,  FREQ_A(6))
    PLAYNOTES(As6, FREQ_As(6))
    PLAYNOTES(B6,  FREQ_B(6))
    PLAYNOTES(C7,  FREQ_C(7))
    PLAYNOTES(D7,  FREQ_D(7))
    PLAYNOTES(E7,  FREQ_E(7))

    return a;
}

float channel2(float t)
{
    float x = 0.0;
    float a = 0.0;
    float volume = 0.0;

    // I tried using an array here, but it was too slow, and this managed to be faster somehow.
    float Gs1=-10.,A1=-10.,As1=-10.,
          C2=-10.,Cs2=-10.,D2=-10.,Ds2=-10.,E2=-10.,F2=-10.,Fs2=-10.,G2=-10.,Gs2=-10.,A2=-10.,As2=-10.,B2=-10.,
          C3=-10.,Cs3=-10.,D3=-10.,Ds3=-10.,E3=-10.,F3=-10.,Fs3=-10.,G3=-10.,Gs3=-10.,A3=-10.,As3=-10.,B3=-10.,
          C4=-10.,Cs4=-10.,D4=-10.,Ds4=-10.,E4=-10.,F4=-10.,Fs4=-10.,G4=-10.,Gs4=-10.,B4=-10.,
          C5=-10.,Cs5=-10.,Ds5=-10.,E5=-10.;

    CHANNEL2

    PLAYNOTES(Gs1, FREQ_Gs(1))
    PLAYNOTES(A1,  FREQ_A(1))
    PLAYNOTES(As1, FREQ_As(1))
    PLAYNOTES(C2,  FREQ_C(2))
    PLAYNOTES(Cs2, FREQ_Cs(2))
    PLAYNOTES(D2,  FREQ_D(2))
    PLAYNOTES(Ds2, FREQ_Ds(2))
    PLAYNOTES(E2,  FREQ_E(2))
    PLAYNOTES(F2,  FREQ_F(2))
    PLAYNOTES(Fs2, FREQ_Fs(2))
    PLAYNOTES(G2,  FREQ_G(2))
    PLAYNOTES(Gs2, FREQ_Gs(2))
    PLAYNOTES(A2,  FREQ_A(2))
    PLAYNOTES(As2, FREQ_As(2))
    PLAYNOTES(B2,  FREQ_B(2))
    PLAYNOTES(C3,  FREQ_C(3))
    PLAYNOTES(Cs3, FREQ_Cs(3))
    PLAYNOTES(D3,  FREQ_D(3))
    PLAYNOTES(Ds3, FREQ_Ds(3))
    PLAYNOTES(E3,  FREQ_E(3))
    PLAYNOTES(F3,  FREQ_F(3))
    PLAYNOTES(Fs3, FREQ_Fs(3))
    PLAYNOTES(G3,  FREQ_G(3))
    PLAYNOTES(Gs3, FREQ_Gs(3))
    PLAYNOTES(A3,  FREQ_A(3))
    PLAYNOTES(As3, FREQ_As(3))
    PLAYNOTES(B3,  FREQ_B(3))
    PLAYNOTES(C4,  FREQ_C(4))
    PLAYNOTES(Cs4, FREQ_Cs(4))
    PLAYNOTES(D4,  FREQ_D(4))
    PLAYNOTES(Ds4, FREQ_Ds(4))
    PLAYNOTES(E4,  FREQ_E(4))
    PLAYNOTES(F4,  FREQ_F(4))
    PLAYNOTES(Fs4, FREQ_Fs(4))
    PLAYNOTES(G4,  FREQ_G(4))
    PLAYNOTES(Gs4, FREQ_Gs(4))
    PLAYNOTES(B4,  FREQ_B(4))
    PLAYNOTES(C5,  FREQ_C(5))
    PLAYNOTES(Cs5, FREQ_Cs(5))
    PLAYNOTES(Ds5, FREQ_Ds(5))
    PLAYNOTES(E5,  FREQ_E(5))

    return a;
}

vec2 mainSound( int samp, float time )
{
    time = timeTrack(time);

    vec2 sound = vec2(0);

    float right = channel1(time) * 0.25;
    float left = channel2(time) * 0.25;

    // mix the channels
    sound += left * vec2(0.7, 0.3);
    sound += right * vec2(0.3, 0.7);
    
    return sound;
}

layout(location = 0) out vec4 outColor;

const ivec2 size = ivec2(4096,2048);

void main()
{
	ivec2 c = ivec2(gl_FragCoord.xy);
	int samp = c.x + c.y * size.x;
	float time = float(samp) / 44100.0;
    vec2 res = mainSound(samp, time);
	outColor = vec4(res, 0., 1.);
}