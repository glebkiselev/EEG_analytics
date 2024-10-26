//
// Created by ZONT_ on 10/25/2024.
//

#include "ml.hpp"

#include <iostream>

#include "all_ops_resolver.h"

#include "model.hpp"

uint8_t ml::tensor_arena[kTensorArenaSize];
tflite::MicroInterpreter* ml::interpreter;
TfLiteTensor* ml::input;
TfLiteTensor* ml::output;

void ml::setup() {
    using namespace std;
    cout << "Initializing TFLite..." << endl;

    // Set up model
    const tflite::Model* model = tflite::GetModel(model_bytes);
    static tflite::AllOpsResolver resolver;

    // Set up interpreter
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize);
    interpreter = &static_interpreter;

    if (interpreter == nullptr) {
        throw std::runtime_error("Failed to initialize TFLite");
    }

    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        throw std::runtime_error("Failed to allocate tensors");
    }

    // Get pointers to the model's input and output tensors
    input = interpreter->input(0);
    output = interpreter->output(0);

    cout << "TFLite done." << endl;
}

float* ml::inference(const float* x) {
    for (int i = 0; i < 24; ++i) {
        input->data.f[i] = x[i];
    }
    if (interpreter->Invoke() != kTfLiteOk) {
        throw std::runtime_error("Error during inference");
    }
    return output->data.f;
}
