#ifndef __PBIO_API__PBIO__SAMPLE_CHECK_STATUS_H_
#define __PBIO_API__PBIO__SAMPLE_CHECK_STATUS_H_

namespace pbio
{

/**
	\~English
	\brief
		The result of some kind of sample checking.
	\~Russian
	\brief
		Результат некоторой проверки сэмпла.
*/
struct SampleCheckStatus
{
	enum Verdict
	{
		/**
			\~English
			\brief
				Sample passed the checking.
			\~Russian
			\brief
				Сэмпл прошел проверку.
		*/
		PASSED          = 0,

		/**
			\~English
			\brief
				Sample did not pass the checking.
			\~Russian
			\brief
				Сэмпл не прошел проверку.
		*/
		FAILED          = 1,

		/**
			\~English
			\brief
				Sample was not checked.
			\~Russian
			\brief
				Сэмпл не проверялся.
		*/
		NOT_COMPUTED    = 2
	};
};

} // pbio pbio

#endif // __PBIO_API__PBIO__SAMPLE_CHECK_STATUS_H_
