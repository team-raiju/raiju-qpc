#ifndef SIGNALS_H
#define SIGNALS_H

/* define the event signals used in the application ------------------------*/
enum SumoHSMSignals {
    TIMEOUT_SIG = Q_USER_SIG, /* offset the first signal by Q_USER_SIG */
    TIMEOUT_2_SIG,
    STOP_BUZZER_SIG,
    START_SIG,
    STOP_SIG,
    CHANGE_STATE_EVT_SIG,
    CHANGE_STRATEGY_EVT_SIG,
    CHANGE_PRE_STRATEGY_EVT_SIG,
    LINE_CHANGED_FL_SIG,
    LINE_CHANGED_FR_SIG,
    LINE_CHANGED_BL_SIG,
    LINE_CHANGED_BR_SIG,
    DIST_SENSOR_CHANGE_SIG,
    RADIO_DATA_SIG,
    BUTTON_SIG,
    BLE_DATA_UPDATE_SIG,
    BLE_DATA_REQUEST_SIG,
    LOW_BATTERY_SIG,
    TIMEOUT_SEND_BLE_SIG,
    FAILSAFE_SIG,
    STUCK_SIG,
    STUCK_END_SIG,
    BLE_ATTACK_NEAR_SIG,
    START_MODULE_TEST_SIG,
    IMU_POLL_SIG,
    MAX_SIG, /* keep last (the number of signals) */
};

#endif /* SIGNALS_H */
