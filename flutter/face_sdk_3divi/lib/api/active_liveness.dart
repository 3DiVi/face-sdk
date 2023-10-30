part of face_sdk_3divi;


/// Check type for active liveness.
enum ActiveLivenessCheckType {
  NONE,         // 0
  SMILE,        // 1
  BLINK,        // 2
  TURN_UP,      // 3
  TURN_DOWN,    // 4
  TURN_RIGHT,   // 5
  TURN_LEFT,    // 6
  PERSPECTIVE,  // 7
}


/// Active liveness check state.
enum ActiveLiveness {
  ALL_CHECKS_PASSED,     // 0
  CURRENT_CHECK_PASSED,  // 1
  CHECK_FAIL,            // 2
  WAITING_FACE_ALIGN,    // 3
  IN_PROGRESS,           // 4
  NOT_COMPUTED,          // 5
}


/// Object for a complete description of the state of active liveness assessment
class ActiveLivenessStatus {
    ActiveLiveness verdict = ActiveLiveness.NOT_COMPUTED;
    ActiveLivenessCheckType check_type = ActiveLivenessCheckType.NONE;
		double progress_level = 0;
}

