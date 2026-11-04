#pragma once
#include <Arduino.h>

struct BambuState {
    bool connected = false;
    String gcode_state = "OFFLINE"; // IDLE, RUNNING, PAUSE, FINISH, FAILED, OFFLINE
    int mc_percent = 0;             // 0 - 100%
    int mc_remaining_time = 0;      // Remaining time in minutes
    float nozzle_temper = 0.0f;
    float nozzle_target_temper = 0.0f;
    float bed_temper = 0.0f;
    float bed_target_temper = 0.0f;
    float chamber_temper = 0.0f;
    int current_layer = 0;
    int total_layers = 0;
    String subtask_name = "Idle";   // 3D model / gcode filename
    int fan_gear = 0;               // Cooling fan speed (0-15)
    uint32_t last_update_ms = 0;
};
