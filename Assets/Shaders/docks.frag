#version 450

#define BPM 133.0

#define BEAT (60./BPM)     // length of a quarter note in seconds
#define MEASURE (4.*BEAT)  // length of a measure (4/4 time)

#define THEMEA \
/*13*/ N(.5,G2) CHORD2(.5,D3,D4) N(.5,G4) N(.5,A4) N(.5,B4) N(.5,C5) CHORD2(.5,G3,B4) CHORD2(.5,D3,A4) \
/*14*/ CHORD2(.5,F2,G4) N(.5,C3) CHORD2(1.,F3,D4) N(1.,C4) N(1.,G4) \
/*15*/ CHORD2(.5,Ef2,D4) N(.5,Bf2) N(.5,Ef3) N(1.5,G3) N(.5,Ef3) N(.5,G3) \
/*16*/ N(.5,F2) N(.5,C3) N(1.5,F3) N(.5,C4) N(.5,C3) CHORD2(.5,F3,C4) \
/*17*/ CHORD2(.5,G2,B3) CHORD2(.5,D3,D4) CHORD2(1.5,G3,G4) N(1.,A3) CHORD2(.5,G3,B3) \

#define THEMEB \
/*25*/ N(.5,C3) N(.5,G3) N(.5,C4) N(.5,G4) N(.5,E5) N(.5,D5) N(.5,C5) N(.5,E5) \
/*26*/ CHORD2(.5,B2,D5) N(.5,G3) N(.5,B3) N(1.,G4) N(.5,G3) N(1.,B3) \

#define THEMEA_STRINGS \
/*13*/ CHORD2(4.,B3,D4) \
/*14*/ CHORD2(4.,A3,C4) \
/*15*/ CHORD2(4.,G3,Bf3) \
/*16*/ CHORD2(4.,F3,A3) \
/*17*/ CHORD2(8.,G3,B3) \

#define STRINGS_MUSIC \
/*1*/  N(6.,B4) \
/*2*/  N(2.,D4) \
/*3*/  N(6.,C4) \
/*4*/  N(2.,G4) \
/*5*/  N(6.5,D4) \
/*6*/  N(.5,B4) N(.5,A4) N(4.5,G4) \
/*8*/  N(4.,F4) \
/*9*/  CHORD2(4.,Bf3,D4) \
/*10*/ N(2.,Bf3) N(2.,G3) \
/*11*/ N(4.,C4) \
/*12*/ N(4.,D4) \
/* A section */ \
THEMEA_STRINGS \
THEMEA_STRINGS \
/* B section */ \
CHORD2(4.,C4,E4) \
CHORD2(4.,B3,D4) \
CHORD2(4.,A3,C4) \
CHORD2(3.,G3,B3) CHORD2(1.,G3,D4) \
CHORD2(4.,C4,E4) \
CHORD2(4.,B3,D4) \
CHORD2(4.,Bf3,Df4) \
CHORD2(2.,A3,C4) N(2.,Gf3) \
/* A section */ \
THEMEA_STRINGS \
THEMEA_STRINGS \
/* C section */ \
R(2.) CHORD2(6.,F3,A3) \
CHORD2(8.,Ef3,G3) \
CHORD2(4.,D3,Gf3) \
CHORD2(2.,Ef3,G3) CHORD2(2.,Gf3,A3) \

#define PIANO_MUSIC \
/*1*/  N(.5,G2) N(.5,D3) N(.5,G3) N(1.,A3) N(1.,B3) N(.5,D4) \
/*2*/  CHORD2(.5,G2,G4) N(.5,D3) N(.5,G3) CHORD2(2.5,D5,G5) \
/*3*/  N(.5,F2) N(.5,C3) N(.5,F3) N(1.,A3) N(1.,B3) N(.5,D4) \
/*4*/  CHORD2(.5,F2,G4) N(.5,C3) N(.5,F3) CHORD2(1.5,D5,G5) N(1.,C3) \
/*5*/  N(.5,G2) N(.5,D3) N(.5,G3) N(1.,A3) N(1.,B3) N(.5,D4) \
/*6*/  CHORD2(.5,G2,G4) N(.5,D3) N(.5,G3) CHORD2(2.5,D5,G5) \
/*7*/  N(.5,F2) N(.5,C3) N(.5,F3) N(1.,A3) N(1.,B3) N(.5,D4) \
/*8*/  CHORD2(.5,F2,G4) N(.5,C3) N(.5,F3) CHORD2(2.5,D5,G5) \
/*9*/  N(.5,Ef2) N(.5,Bf2) N(.5,Ef3) N(1.,A3) N(1.,Bf3) N(.5,D4) \
/*10*/ CHORD2(.5,Ef2,G4) N(.5,Bf2) N(.5,Ef3) CHORD2(2.5,D5,G5) \
/*11*/ N(.5,F2) N(.5,C3) N(.5,F3) N(1.,A3) N(1.,B3) N(.5,D4) \
/*12*/ CHORD2(.5,F2,G4) N(.5,C3) N(.5,F3) CHORD2(1.5,D5,G5) N(1.,C3) \
/* A section */ \
THEMEA \
/*18*/ N(.5,G2) N(.5,D3) N(.5,G3) N(1.,B3) N(1.5,G4) \
THEMEA \
/*24*/ N(.5,G2) N(.5,D3) N(.5,G3) N(2.5,B3) \
/* B section */ \
THEMEB \
/*27*/ N(.5,A2) N(.5,E3) N(.5,A3) N(.5,E4) N(.5,D5) N(.5,C5) CHORD2(.5,E3,B4) N(.5,C5) \
/*28*/ CHORD2(.5,G2,B4) N(.5,D3) N(.5,G3) N(1.5,G4) N(1.,D3) \
THEMEB \
/*31*/ N(.5,Bf2) N(.5,G3) N(1.,Bf3) N(.5,C5) N(.5,D5) CHORD2(.5,Bf3,C5) N(.5,B4) \
/*32*/ N(.5,A2) N(.5,A3) N(1.,G4) CHORD2(.5,D3,E4) N(1.5,A3) \
/* A section */ \
THEMEA \
/*18*/ N(.5,G2) N(.5,D3) N(.5,G3) N(1.,B3) N(1.5,G4) \
THEMEA \
/*24*/ N(.5,G2) N(.5,D3) N(.5,G3) N(1.5,B3) CHORD2(1.,B3,G4) \
/* C section */ \
/*45*/ N(.5,F2) N(.5,C3) N(.5,F3) CHORD2(1.,A3,F4) CHORD2(.5,A3,F4) CHORD2(.5,A3,F4) CHORD2(.5,A3,F4) \
/*46*/ N(.5,F2) CHORD2(.5,A3,F4) CHORD2(.5,A3,F4) CHORD2(.5,A3,F4) N(.5,C3) CHORD2(.5,A3,F4) CHORD3(.5,F3,A3,F4) CHORD2(.5,A3,F4) \
/*47*/ N(.5,Ef2) N(.5,Bf2) N(.5,Ef3) CHORD2(1.,Bf3,G4) CHORD2(.5,Bf3,G4) N(.5,Ef3) CHORD2(.5,Bf3,G4) \
/*48*/ N(.5,Ef2) CHORD3(.5,Bf2,Bf3,G4) N(.5,Ef3) CHORD2(.5,Bf3,G4) CHORD2(.5,Bf3,G4) CHORD2(.5,Bf3,G4) CHORD3(.5,Bf2,Bf3,G4) CHORD2(.5,Bf3,G4) \
/*49*/ N(.5,D2) N(.5,A2) N(.5,D3) CHORD2(1.,C4,G4) CHORD2(.5,C4,G4) N(.5,D3) CHORD2(.5,C4,G4) \
/*50*/ N(.5,D2) CHORD3(.5,A2,C4,A4) N(.5,D3) CHORD2(1.,C4,A4) CHORD2(1.,C4,A4) CHORD2(.5,C4,A4) \

#define DRUMS_MUSIC \
/*1*/ N(1.,B) N(.25,S) N(.25,B) N(.25,B) N(1.25,B) N(.75,S) N(.25,B) \
/*2*/ N(1.,B) N(.75,S) N(.25,B) N(.5,B) N(.5,B) N(.25,S) N(.5,B) N(.25,B) \
/*3*/ N(1.,B) N(.75,S) N(.25,B) N(1.,B) N(.75,S) N(.25,B) \
/*4*/ N(1.,B) N(.75,S) N(.25,B) N(.5,B) N(.5,B) N(.25,S) N(.5,B) N(.25,B) \

// Note frequencies
#define FREQ_C(oct)  (16.35*exp2(float(oct)))
#define FREQ_Df(oct) (17.32*exp2(float(oct)))
#define FREQ_D(oct)  (18.35*exp2(float(oct)))
#define FREQ_Ef(oct) (19.45*exp2(float(oct)))
#define FREQ_E(oct)  (20.60*exp2(float(oct)))
#define FREQ_F(oct)  (21.83*exp2(float(oct)))
#define FREQ_Gf(oct) (23.12*exp2(float(oct)))
#define FREQ_G(oct)  (24.50*exp2(float(oct)))
#define FREQ_Af(oct) (25.96*exp2(float(oct)))
#define FREQ_A(oct)  (27.50*exp2(float(oct)))
#define FREQ_Bf(oct) (29.14*exp2(float(oct)))
#define FREQ_B(oct)  (30.87*exp2(float(oct)))

#define PI 3.14159265359

float attenuate(float t) { return exp2(-2.5*t); }

// The built-in sin function has some precision issues at large x values, which results in
// annoying high pitched buzzing sounds. Keeping x within 0 and 2pi fixes that.
float better_sin(float x) { return sin(mod(x, 2.0*PI)); }

float db_to_amp(float db) { return pow(10.0f, db/20.0f); }

// Imitation of the Yamaha DX7 electric piano.
float e_piano(float time,float freq)
{
    float x = 2.0*PI*freq*time;
    return
        better_sin(x*1.0)*db_to_amp(-15.7) +
        better_sin(x*1.99)*db_to_amp(-33.9) +
        better_sin(x*2.99)*db_to_amp(-22.1) +
        better_sin(x*3.99)*db_to_amp(-24.9) +
        better_sin(x*4.98)*db_to_amp(-34.8) +
        better_sin(x*5.98)*db_to_amp(-42.1) +
        better_sin(x*6.96)*db_to_amp(-55.8) +
        better_sin(x*7.95)*db_to_amp(-64.7) +
        better_sin(x*13.0)*db_to_amp(-37.7) +
        better_sin(x*15.01)*db_to_amp(-37.9);
}

float strings(float time, float freq)
{
    float x = 2.0*PI*freq*time;
    return
        better_sin(x*1.0)*db_to_amp(-10.) +
        better_sin(x*2.0)*db_to_amp(-18.3) +
        better_sin(x*3.0)*db_to_amp(-21.3) +
        better_sin(x*4.0)*db_to_amp(-20.1) +
        better_sin(x*5.0)*db_to_amp(-22.2) +
        better_sin(x*6.0)*db_to_amp(-24.5) +
        better_sin(x*7.0)*db_to_amp(-29.5) +
        better_sin(x*8.0)*db_to_amp(-31.3) +
        better_sin(x*9.0)*db_to_amp(-35.8) +
        better_sin(x*10.0)*db_to_amp(-38.1) +
        better_sin(x*11.0)*db_to_amp(-42.3) +
        better_sin(x*12.0)*db_to_amp(-44.8) +
        better_sin(x*13.0)*db_to_amp(-46.5) +
        better_sin(x*14.0)*db_to_amp(-48.0) +
        better_sin(x*15.0)*db_to_amp(-46.3);
}

float chorus_strings(float time, float freq)
{
    float freq1 = freq*1.006;
    float freq2 = freq*1.01;
    float freq3 = freq*.999;
    float y =
        .25*strings(time,freq1) +
        .25*strings(time,freq2) +
        .25*strings(time,freq3);
    return y;
}

// note
#define N(len, note) note=(t>=x)?x:note;x+=len*60.0/BPM;
// rest
#define R(len) x+=len*(60.0/BPM);

#define CHORD2(len, a, b)       N(len, a) x-=len*60.0/BPM; N(len, b)
#define CHORD3(len, a, b, c)    N(len, a) x-=len*60.0/BPM; N(len, b) x-=len*60.0/BPM; N(len, c)
#define CHORD4(len, a, b, c, d) N(len, a) x-=len*60.0/BPM; N(len, b) x-=len*60.0/BPM; N(len, c) x-=len*60.0/BPM; N(len, d)

#define PLAYNOTES(note, freq) if(note>-10.)a+=e_piano(t-note,freq)*attenuate(t-note);

float piano_music(float t)
{
    float x = 0.0;
    float a = 0.0;

    // I tried using an array here, but it was too slow, and this managed to be faster somehow.
    float
        D2=-10.,Ef2=-10.,F2=-10.,G2=-10.,A2=-10.,Bf2=-10.,B2=-10.,
        C3=-10.,D3=-10.,Ef3=-10.,E3=-10.,F3=-10.,G3=-10.,A3=-10.,Bf3=-10.,B3=-10.,
        C4=-10.,D4=-10.,E4=-10.,F4=-10.,G4=-10.,A4=-10.,B4=-10.,
        C5=-10.,D5=-10.,E5=-10.,G5=-10.;

    PIANO_MUSIC

    PLAYNOTES(D2,  FREQ_D(2))
    PLAYNOTES(Ef2, FREQ_Ef(2))
    PLAYNOTES(F2,  FREQ_F(2))
    PLAYNOTES(G2,  FREQ_G(2))
    PLAYNOTES(A2,  FREQ_A(2))
    PLAYNOTES(Bf2, FREQ_Bf(2))
    PLAYNOTES(B2,  FREQ_B(2))
    
    PLAYNOTES(C3,  FREQ_C(3))
    PLAYNOTES(D3,  FREQ_D(3))
    PLAYNOTES(Ef3, FREQ_Ef(3))
    PLAYNOTES(E3,  FREQ_E(3))
    PLAYNOTES(F3,  FREQ_F(3))
    PLAYNOTES(G3,  FREQ_G(3))
    PLAYNOTES(A3,  FREQ_A(3))
    PLAYNOTES(Bf3, FREQ_Bf(3))
    PLAYNOTES(B3,  FREQ_B(3))

    PLAYNOTES(C4,  FREQ_C(4))
    PLAYNOTES(D4,  FREQ_D(4))
    PLAYNOTES(E4,  FREQ_E(4))
    PLAYNOTES(F4,  FREQ_F(4))
    PLAYNOTES(G4,  FREQ_G(4))
    PLAYNOTES(A4,  FREQ_A(4))
    PLAYNOTES(B4,  FREQ_B(4))

    PLAYNOTES(C5,  FREQ_C(5))
    PLAYNOTES(D5,  FREQ_D(5))
    PLAYNOTES(E5,  FREQ_E(5))
    PLAYNOTES(G5,  FREQ_G(5))

    return a;
}

float strings_asdr(float t, float dur)
{
    float over = t-dur*60.0/BPM;
    return (over >= 0.0) ? max(1.0-over,0.0) : min(t*5., 1.0);
}

#undef N
#define N(len,note) if(t>=x)note=strings_asdr(t-x,len);x+=len*60.0/BPM;
#undef CHORD2
#define CHORD2(len, n1, n2) if(t>=x){n1=n2=strings_asdr(t-x,len);}x+=len*60.0/BPM;

#undef PLAYNOTES
#define PLAYNOTES(note, freq) if(note>0.1)a+=note*chorus_strings(t,freq);

float strings_music(float t)
{
    float a = 0.0f;
    float x = 0.0f;
    float
        D3=-10.,Ef3=-10.,F3=-10.,Gf3=-10.,G3=-10.,A3=-10.,Bf3=-10.,B3=-10.,
        C4=-10.,Df4=-10.,D4=-10.,E4=-10.,F4=-10.,G4=-10.,A4=-10.,B4=-10.;
    STRINGS_MUSIC
    
    PLAYNOTES(D3,  FREQ_D(3))
    PLAYNOTES(Ef3, FREQ_Ef(3))
    PLAYNOTES(F3,  FREQ_F(3))
    PLAYNOTES(Gf3, FREQ_Gf(3))
    PLAYNOTES(G3,  FREQ_G(3))
    PLAYNOTES(A3,  FREQ_A(3))
    PLAYNOTES(Bf3, FREQ_Bf(3))
    PLAYNOTES(B3,  FREQ_B(3))

    PLAYNOTES(C4,  FREQ_C(4))
    PLAYNOTES(Df4, FREQ_Df(4))
    PLAYNOTES(D4,  FREQ_D(4))
    PLAYNOTES(E4,  FREQ_E(4))
    PLAYNOTES(F4,  FREQ_F(4))
    PLAYNOTES(G4,  FREQ_G(4))
    PLAYNOTES(A4,  FREQ_A(4))
    PLAYNOTES(B4,  FREQ_B(4))

    return a;
}

float rand(float n){return fract(sin(n) * 43758.5453123);}

float bass(float t)
{
    float y = 
        sin(FREQ_Df(1)*2.*PI*t)
       +sin(FREQ_F(2)*2.*PI*t)
       +sin(FREQ_B(2)*2.*PI*t);
       +sin(FREQ_Gf(4)*2.*PI*t);
       +sin(FREQ_C(6)*2.*PI*t);
    //y += rand(t*15.)*.2;
    return y * exp2(-24.*t);
}

float snare(float t)
{
    return rand(t*500.)*exp2(-30.*t);
}

float hihat(float t)
{
    return rand(t*50.)*exp2(-50.*t)*.25;
}

float ridebell(float t)
{
    float y = 
        sin(541.0*2.*PI*t)*db_to_amp(-51.0)
       +sin(710.0*2.*PI*t)*db_to_amp(-30.8)
       +sin(986.0*2.*PI*t)*db_to_amp(-45.5)
       +sin(1062.0*2.*PI*t)*db_to_amp(-49.0)
       +sin(1434.0*2.*PI*t)*db_to_amp(-46.2)
       +sin(2924.0*2.*PI*t)*db_to_amp(-35.9)
       +sin(3301.0*2.*PI*t)*db_to_amp(-45.2)
       +sin(3459.0*2.*PI*t)*db_to_amp(-40.4)
       +sin(5584.0*2.*PI*t)*db_to_amp(-25.5)
       +sin(7124.0*2.*PI*t)*db_to_amp(-40.5)
       ;
    y*=rand(t*50.);
    return y*exp2(-10.*t)*10.;
}

#undef N
#define N(len,dr) if(t>=x)a+=dr(t-x);x+=len*60./BPM;
#define B bass
#define S snare

float drums_music(float time)
{
    float t = mod(time, MEASURE*4.);

    float x = 0.;
    float a = 0.;
    
    DRUMS_MUSIC
    
    // ride bell
    t = mod(t, MEASURE);
    x = (t >= BEAT*3.) ? BEAT*.25 : BEAT*.5;
    t = mod(t,BEAT);
    if (t >= x && time >= MEASURE*24. && time < MEASURE*32.)
        a += ridebell(t-x);
    
    // hi-hat
    t = mod(t,.25*60./BPM);
    a += hihat(t);
    return a;
}

vec2 mainSound( int samp, float time )
{
    float a = piano_music(time)*.5;
    if (time >= MEASURE*50.0)
        a += piano_music(time - MEASURE*50.0)*.5  // repeat back to beginning
        + .5*strings_music(time - MEASURE*50.0);  // add strings second time
   
    // Gradually fade drums in at the end of intro when strings come in
    float drumfade = 0.;
    if (time >= MEASURE*50.0)
        drumfade = min(1., (time - MEASURE*50.0) / (MEASURE*12.));
    //drumfade = 0.;
    //a = drums_music(time);
    a += drums_music(time - MEASURE*50.0) * drumfade * .2;
    if (time >= 170.) a *= (180.-time)/10.;  // fade out
    return vec2(a);
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