#define __MA20

#define COLOR_LCD

#ifdef  COLOR_LCD
    #define MALYAN_LCD
#else
    #define MAKRPANEL
#endif

#define MANUAL_FEEDRATE { 50*60, 50*60, 20*60, 10*60 }
#define FIL_RUNOUT_VAL  100000

#ifdef LEVELSENSOR
    #define RESTORE_LEVELING_AFTER_G28
    #define Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN
    #define FIX_MOUNTED_PROBE
    #define UBL_DEVEL_DEBUGGING
    #define DEBUG_LEVELING_FEATURE
    #define AUTO_BED_LEVELING_UBL
    #define Z_SAFE_HOMING
#endif

//MA20 config
#ifdef  __MA20
    //#define SDIO_4BIT   true
    #define POWER_LOSS_RECOVERY
    #define HEATER_BED_INVERTING    false
    #define DEFAULT_AXIS_STEPS_PER_UNIT    { 94.2, 263.8, 2026.52, 110 }
    #define DEFAULT_MAX_FEEDRATE           { 300, 40, 5, 65 }           //M203
    #define DEFAULT_MAX_ACCELERATION       { 1000, 1000, 1000, 10000 }  //M201
    #define Z_MIN_ENDSTOP_INVERTING true
    #define X_BED_SIZE 210
    #define Y_BED_SIZE 99999
    #define ENDSTOPPULLUP_ZMIN
    #define PWM_MOTOR_CURRENT { 850, 800, 800 }
    #define NOZZLE_TO_PROBE_OFFSET { 0, 0, 0 }
    #define DEFAULT_AXIS_DIRECTION  0x0D
#endif

