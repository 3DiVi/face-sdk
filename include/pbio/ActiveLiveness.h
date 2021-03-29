#ifndef __LIBFACEREC_ACTIVELIVENESS_H
#define __LIBFACEREC_ACTIVELIVENESS_H
namespace pbio {


class ActiveLiveness {
public:

	/** \~English
		\brief Check type for active liveness.
		\~Russian
		\brief Тип проверки для активной (сценарной) оценки принадлежности лица живому человеку.
	*/
	enum CheckType{
		NONE = 0,
		SMILE = 1,
		BLINK = 2,
		TURN_UP = 3,
		TURN_DOWN = 4,
		TURN_RIGHT = 5,
		TURN_LEFT = 6,
		PERSPECTIVE = 7
	};

	//! @cond IGNORED
	static std::string CheckTypeToString(const CheckType& check){
		switch (check) {
			case NONE: return "none";
			case SMILE: return "smile";
			case BLINK: return "blink";
			case TURN_UP: return "turn_up";
			case TURN_DOWN: return "turn_down";
			case TURN_RIGHT: return "turn_right";
			case TURN_LEFT: return "turn_left";
			case PERSPECTIVE: return "perspective";
			default: return "";
		}
	}
	//! @endcond


	/** \~English
		\brief Active liveness check state.
		\~Russian
		\brief Состояние проверки активной (сценарной) оценки принадлежности лица живому человеку.
	*/
	enum Liveness {
		ALL_CHECKS_PASSED = 0,
		CURRENT_CHECK_PASSED = 1,
		CHECK_FAIL = 2,
		WAITING_FACE_ALIGN = 3,
		IN_PROGRESS = 4,
		NOT_COMPUTED = 5
	};

	/** \~English
		\brief Active liveness check state.
		\~Russian
		\brief Объект для полного описания состояния активной (сценарной) оценки принадлежности лица живому человеку.
	*/
	struct ActiveLivenessStatus {
		Liveness verdict;
		CheckType check_type;
		float progress_level;

		ActiveLivenessStatus(){
			verdict = NOT_COMPUTED;
			check_type = NONE;
			progress_level = 0;
		}
	};

};

}
#endif //__LIBFACEREC_ACTIVELIVENESS_H
