//
// Created by ZONT_ on 10/25/2024.
//

#pragma once

#include "tensorflow/lite/micro/micro_interpreter.h"

namespace ml {
    constexpr size_t kTensorArenaSize = 2 * 1024;
    extern uint8_t tensor_arena[kTensorArenaSize];
    extern tflite::MicroInterpreter* interpreter;
    extern TfLiteTensor* input;
    extern TfLiteTensor* output;

    void setup();
    float* inference(const float* x);
}

