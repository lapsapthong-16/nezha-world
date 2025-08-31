#ifndef MODEL_HPP
#define MODEL_HPP

// global proportions (same as your struct)
struct ModelScale {
    // torso: slimmer & taller
    float torsoH = 1.18f;   // was 1.02
    float torsoTopR = 0.62f;   // was 0.70
    float torsoBotR = 0.74f;   // was 0.82

    // head (kept off for now) – unchanged
    float headLift = 0.92f;
    float headR = 0.95f;

    // shoulders: a touch higher & slightly narrower
    float shoulderX = 0.84f;   // was 0.92
    float shoulderY = 0.74f;   // was 0.66

    // arms: a bit thinner & shorter to feel childlike
    float upperArmR = 0.22f;   // was 0.26
    float upperArmH = 0.50f;   // was 0.56
    float lowerArmR = 0.20f;   // was 0.24
    float lowerArmH = 0.44f;   // was 0.48
    float jointR = 0.16f;   // was 0.18

    // shorts/hip
    float shortSleeveDrop = 0.88f; // was 0.86
    float shortSleeveLen = 0.38f; // was 0.44 (shorter)
    float hipX = 0.30f; // was 0.34 (legs closer)
    float upperLegH = 0.38f; // was 0.40
    float lowerLegH = 0.40f; // was 0.42
};

// single global instance (declared here, defined in model.cpp)
extern ModelScale MS;

#endif
