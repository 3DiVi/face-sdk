/**
	\file Recognizer.h
	\~English
	\brief Recognizer - Interface object for creating and matching templates.
	\~Russian
	\brief Recognizer - Интерфейсный объект для создания и сравнения шаблонов.
*/

#ifndef __PBIO_API__PBIO__RECOGNIZER_H_
#define __PBIO_API__PBIO__RECOGNIZER_H_


#include <istream>
#include <sstream>
#include <vector>

#include "ComplexObject.h"
#include "Error.h"
#include "RawSample.h"
#include "SmartPtr.h"
#include "Template.h"
#include "TemplatesIndex.h"
#include "stl_wraps_impls/WrapIStreamBufferImpl.h"
#include "stl_wraps_impls/WrapIStreamImpl.h"
#include "stl_wraps_impls/WrapOStreamImpl.h"

namespace pbio
{

class FacerecService;

/** \~English
	\brief Interface object for creating and matching templates.
	\~Russian
	\brief Интерфейсный объект для создания и сравнения шаблонов.
*/
class Recognizer : public ComplexObject
{
public:

	/** \~English
		\brief Alias for the type of a smart pointer to Recognizer.
		\~Russian
		\brief Псевдоним для типа умного указателя на Recognizer.
	*/
	typedef LightSmartPtr<Recognizer>::tPtr Ptr;

	/** \~English
		\brief Result of template matching.
		\~Russian
		\brief Результат сравнения шаблонов.
	*/
	struct MatchResult
	{
		/** \~English
			\brief Distance between the templates.
			\~Russian
			\brief Расстояние между шаблонами.
		*/
		double distance;

		/** \~English
			\brief
				False acceptance rate corresponding to the distance value taken as a threshold
				at the extended LFW test (Labeled Faces in the Wild http://vis-www.cs.umass.edu/lfw/).
			\~Russian
			\brief
				Значение FAR (false acceptance rate), соответствующее расстоянию distance, взятому в качестве порога
				на расширенном LFW-тесте (Labeled Faces in the Wild http://vis-www.cs.umass.edu/lfw/).
		*/
		double fa_r;

		/** \~English
			\brief
				False rejection rate corresponding to the distance value taken as a threshold
				at the extended LFW test (Labeled Faces in the Wild http://vis-www.cs.umass.edu/lfw/).
			\~Russian
			\brief
				Значение FRR (false rejection rate), соответствующее расстоянию distance, взятому в качестве порога
				на расширенном LFW тесте (Labeled Faces in the Wild http://vis-www.cs.umass.edu/lfw/).
		*/
		double fr_r;

		/** \~English
			\brief Score of templates similarity - real number from 0 to 1.
			\~Russian
			\brief Величина сходства шаблонов - действительное число от 0 до 1.
		*/
		double score;
	};

	/** \~English
		\brief Result of searching the Template query in the TemplatesIndex.
		\~Russian
		\brief Результат поиска запросного шаблона (Template) в индексе (TemplatesIndex).
	*/
	struct SearchResult
	{
		/** \~English
			\brief Index in the TemplatesIndex. 
			\~Russian
			\brief Порядковый номер шаблона в индексе.
		*/
		size_t i;

		/** \~English 
			\brief Result of matching the query Template with the i-th Template from the TemplatesIndex. 
			\~Russian
			\brief Результат сравнения запросного шаблона с i-м шаблоном индекса.
		*/
		MatchResult match_result;
	};

	/** \~English
		\brief Types of search acceleration.
		\~Russian
		\brief Типы ускорения поиска.
	*/
	enum SearchAccelerationType
	{
		/** \~English
			\brief
				No acceleration, a standard line search.
				The result is identical to N calls of Recognizer::verifyMatch.
			\~Russian
			\brief
				Без ускорения, стандартный линейный поиск.
				Результат идентичен N вызовам Recognizer::verifyMatch.
		*/
		NO_SEARCH_ACCELERATION = 0,

		/** \~English
			\brief
				Heuristically accelerated linear search.
				Heuristics reduces TAR (true acceptance rate) on
					the 1:N test by no more than 0.1%
			\~Russian
			\brief
				Эвристически ускоренный линейный поиск.
				Эвристика уменьшает TAR (true acceptance rate) на
					тесте 1:N не более чем на 0.1%
		*/
		SEARCH_ACCELERATION_1 = 1,
	};


	/**
		\~English
		\brief
			Get a method name.
			Thread-safe.

		\return
			The name of the method.

		\~Russian
		\brief
			Получить имя метода.
			Потокобезопасный.

		\return
			Имя метода.
	*/
	std::string getMethodName() const;


	/**
		\~English
		\brief
			Get a point on the ROC curve with a given distance threshold.
			Thread-safe.

		\return
			Point on the ROC curve in the form of a MatchResult struct.

		\~Russian
		\brief
			Получить точку на ROC-кривой с заданным порогом расстояния.
			Потокобезопасный.

		\return
			Точка на ROC-кривой в виде структуры MatchResult.
	*/
	MatchResult getROCCurvePointByDistanceThreshold(const double distance_threshold) const;


	/**
		\~English
		\brief
			Get a point on the ROC curve with a given false acceptance rate (FAR).
			Thread-safe.

		\return
			Point on the ROC curve in the form of a MatchResult struct.

		\~Russian
		\brief
			Получить точку на ROC-кривой с заданным false acceptance rate (FAR).
			Потокобезопасный.

		\return
			Точка на ROC-кривой в виде структуры MatchResult.
	*/
	MatchResult getROCCurvePointByFAR(const double desired_far) const;


	/**
		\~English
		\brief
			Get a point on the ROC curve with a given false rejection rate (FRR).
			Thread-safe.

		\return
			Point on the ROC curve in the form of a MatchResult struct.

		\~Russian
		\brief
			Получить точку на ROC-кривой с заданным false rejection rate (FRR).
			Потокобезопасный.

		\return
			Точка на ROC-кривой в виде структуры MatchResult.
	*/
	MatchResult getROCCurvePointByFRR(const double desired_frr) const;


	/**
		\~English
		\brief
			Get a point on the ROC curve with a given similarity score threshold.
			Thread-safe.

		\return
			Point on the ROC curve in the form of a MatchResult struct.

		\~Russian
		\brief
			Получить точку на ROC-кривой с заданным порогом значения сходства.
			Потокобезопасный.

		\return
			Точка на ROC-кривой в виде структуры MatchResult.
	*/
	MatchResult getROCCurvePointByScoreThreshold(const double score_threshold) const;


	/**
		\~English
		\brief
			Create a template.

		\param[in]  sample
			Face sample.
			At the moment, all methods can only be used with frontal samples.

		\return
			Created template.

		\~Russian
		\brief
			Создать шаблон.

		\param[in]  sample
			Образец лица.
			На данный момент все методы могут работать только с фронтальными образцами.

		\return
			Созданный шаблон.
	*/
	Template::Ptr processing(const RawSample &sample) const;


	/**
		\~English
		\brief
			Load the template.
			The format is platform-independent.
			Only the templates that were created with the same method 
			(i.e. with the same ini_file) can be loaded.

		\param[in] binary_stream
			Input stream object.
			The file stream (std::ifstream) must be opened with the std::ios_base::binary flag.

		\return
			Loaded template.

		\~Russian
		\brief
			Загрузить шаблон.
			Формат платформонезависимый.
			Загружать можно только шаблоны, созданные этим же методом 
			(т.е. с таким же конфигурационным файлом).

		\param[in] binary_stream
			Объект потока.
			Поток файла (std::ifstream) необходимо открывать с флагом std::ios_base::binary.

		\return
			Загруженный шаблон.
	*/
	Template::Ptr loadTemplate(std::istream &binary_stream) const;

	/**
		\~English
		\brief
			Load the template.
			The format is platform-independent.
			Only the templates that were created with the same method 
			(i.e. with the same ini_file) can be loaded.

		\param[in] binary_stream
			Input stream object.
			The file stream (std::ifstream) must be opened with the std::ios_base::binary flag.

		\return
			Loaded template.

		\~Russian
		\brief
			Загрузить шаблон.
			Формат платформонезависимый.
			Загружать можно только шаблоны, созданные этим же методом 
			(т.е. с таким же конфигурационным файлом).

		\param[in] binary_stream
			Объект потока.
			Поток файла (std::ifstream) необходимо открывать с флагом std::ios_base::binary.

		\return
			Загруженный шаблон.
	*/
	Template::Ptr loadTemplate(pbio::stl_wraps::WrapIStream &binary_stream) const;


	/**
		\~English
		\brief
			Load the template.
			The format is platform-independent.
			Only the templates that were created with the same method 
			(i.e. with the same ini_file) can be loaded.

		\param[in] data
			Data buffer.

		\param[in] size
			Number of bytes in the data buffer.

		\return
			Loaded template.

		\~Russian
		\brief
			Загрузить шаблон.
			Формат платформонезависимый.
			Загружать можно только шаблоны, созданные этим же методом 
			(т.е. с таким же конфигурационным файлом).

		\param[in] data
			Буфер данных.

		\param[in] size
			Размер буфера (в байтах).

		\return
			Загруженный шаблон.
	*/
	Template::Ptr loadTemplate(
		const void* const data,
		const int size) const;


	/**
		\~English
		\brief
			Compare two templates.
			The order of templates does not matter.
			Only the templates that were created with the same method 
			(i.e. with the same ini_file) can be loaded.

		\param[in]  template1
			%Template created by the same method.

		\param[in]  template2
			%Template created by the same method.

		\return
			Result of the matching.

		\~Russian
		\brief
			Сравнить два шаблона.
			Порядок шаблонов не важен.
			Сравнивать можно только шаблоны, созданные этим же методом 
			(т.е. с таким же конфигурационным файлом).

		\param[in]  template1
			Шаблон, созданный таким же методом.

		\param[in]  template2
			Шаблон, созданный таким же методом.

		\return
			Результат сравнения.
	*/
	MatchResult verifyMatch(
		const Template &template1,
		const Template &template2) const;


	/**
		\~English
		\brief
			Create the TemplatesIndex for quick search.
			The total size of all indexes is limited by the license.

		\param[in]  templates
			Vector of templates for creating an index.

		\param[in]  search_threads_count
			Count of threads that will be used while searching in this index.

		\param[in]  reserve_queries_count
			Integer passed to TemplatesIndex::reserveSearchMemory, which is called with the created index.

		\return
			Created index.

		\~Russian
		\brief
			Создать индекс (TemplatesIndex) для выполнения быстрого поиска.
			Суммарный размер всех индексов ограничен лицензией.

		\param[in]  templates
			Вектор шаблонов для построения индекса.

		\param[in]  search_threads_count
			Количество потоков для использования во время поиска в этом индексе.

		\param[in]  reserve_queries_count
			Целое число, передаваемое в метод TemplatesIndex::reserveSearchMemory, вызванный от созданного индекса.

		\return
			Созданный индекс.
	*/
	TemplatesIndex::Ptr createIndex(
		const std::vector<pbio::Template::Ptr> &templates,
		const int search_threads_count = 1,
		const int reserve_queries_count = 0) const;


	/**
		\~English
		\brief
			Search for the k nearest Templates in the TemplatesIndex.
			The search time depends on k and the size of the TemplatesIndex.

		\param[in]  query_template
			The Template query.

		\param[in]  templates_index
			TemplatesIndex for search.

		\param[in]  k
			Count of the nearest templates for search.

		\param[in]  acceleration
			Acceleration type.

		\return
			Vector with min(k, templates_index.size()) elements
			in ascending order of distance to the query_template.

		\~Russian
		\brief
			Поиск k ближайших шаблонов в индексе.
			Время поиска зависит от значения k и от размеров индекса.

		\param[in]  query_template
			Запросный шаблон.

		\param[in]  templates_index
			Индекс для поиска.

		\param[in]  k
			Количество ближайших шаблонов для поиска.

		\param[in]  acceleration
			Тип ускорения поиска.

		\return
			Вектор с min(k, templates_index.size()) элементами 
			в порядке возрастания расстояния до запросного шаблона. 
	*/
	std::vector<SearchResult> search(
		const pbio::Template &query_template,
		const pbio::TemplatesIndex &templates_index,
		const size_t k,
		const SearchAccelerationType acceleration = SEARCH_ACCELERATION_1) const;


	/**
		\~English
		\brief
			Search for the k nearest Templates in the TemplatesIndex.
			The search time depends on k and the size of the TemplatesIndex.

		\param[in]  queries_templates
			Vector of queries.

		\param[in]  templates_index
			TemplatesIndex for search.

		\param[in]  k
			Count of the nearest templates for search.

		\param[in]  acceleration
			Acceleration type.

		\return
			Vector with queries_templates.size() elements,
			each is a vector with min(k, templates_index.size()) elements
			in ascending order of distance to the query_template.

		\~Russian
		\brief
			Поиск k ближайших шаблонов в индексе.
			Время поиска зависит от значения k и от размеров индекса.

		\param[in]  queries_templates
			Вектор запросных шаблонов.

		\param[in]  templates_index
			Индекс для поиска.

		\param[in]  k
			Количество ближайших шаблонов для поиска.

		\param[in]  acceleration
			Тип ускорения поиска.

		\return
			Вектор с queries_templates.size() элементами, 
			каждый из которых является вектором с min(k, templates_index.size()) 
			элементами в порядке возрастания расстояния до запросного шаблона. 
	*/
	std::vector<std::vector<SearchResult> > search(
		const std::vector<pbio::Template::Ptr> &queries_templates,
		const pbio::TemplatesIndex &templates_index,
		const size_t k,
		const SearchAccelerationType acceleration = SEARCH_ACCELERATION_1) const;


	/**
		\~English
		\brief
			Choose templates set that best represent original templates.

		\param[in]  set_size
			Required set size.

		\param[in]  templates
			Original templates.

		\param[in]  inviolable_templates_indexes
			Indexes of templates, required to be included in the result set.

		\return
			Indexes of templates that make up the result set.

		\~Russian
		\brief
			Выбрать набор шаблонов, представляющий оригинальные шаблоны наилучшим образом.

		\param[in]  set_size
			Требуемый размер набора.

		\param[in]  templates
			Оригинальные шаблоны.

		\param[in]  inviolable_templates_indexes
			Индексы шаблонов, которых необходимо включить в набор.

		\return
			Индексы шаблонов, составляющих результирующий набор.
	*/
	std::vector<size_t> chooseRepresentativeTemplatesSet(
		const size_t set_size,
		const std::vector<pbio::Template::Ptr> &templates,
		const std::vector<size_t> &inviolable_templates_indexes = std::vector<size_t>());

private:

	Recognizer(
		const DHPtr &dll_handle,
		void* impl);

	friend class FacerecService;
	friend class object_with_ref_counter<Recognizer>;
};

}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////

namespace pbio
{

inline
Recognizer::Recognizer(
	const DHPtr &dll_handle,
	void* impl):
ComplexObject(dll_handle, impl)
{
	// nothing else
}


inline
std::string Recognizer::getMethodName() const
{
	std::ostringstream name_stream;
	pbio::stl_wraps::WrapOStreamImpl name_stream_wrap(name_stream);

	void* exception = NULL;

	_dll_handle->Recognizer_getMethodName(
		_impl,
		&name_stream_wrap,
		pbio::stl_wraps::WrapOStream::write_func,
		&exception);

	checkException(exception, *_dll_handle);

	return name_stream.str();
}


inline
Template::Ptr Recognizer::processing(
	const RawSample &sample) const
{
	void* exception = NULL;

	void* const result_impl = _dll_handle->Recognizer_processing(
		_impl,
		(const pbio::facerec::RawSampleImpl*) sample._impl,
		&exception);

	checkException(exception, *_dll_handle);

	return Template::Ptr::make(_dll_handle, result_impl);
}


inline
Template::Ptr Recognizer::loadTemplate(std::istream &binary_stream) const
{
	pbio::stl_wraps::WrapIStreamImpl binary_stream_wrap(binary_stream);

	return loadTemplate(binary_stream_wrap);
}

inline
Template::Ptr Recognizer::loadTemplate(pbio::stl_wraps::WrapIStream &binary_stream) const
{
	void* exception = NULL;

	void* const result_impl = _dll_handle->Recognizer_loadTemplate(
		_impl,
		&binary_stream,
		pbio::stl_wraps::WrapIStream::read_func,
		&exception);

	checkException(exception, *_dll_handle);

	return Template::Ptr::make(_dll_handle, result_impl);
}


inline
Template::Ptr Recognizer::loadTemplate(
	const void* const data,
	const int size) const
{
	void* exception = NULL;

	pbio::stl_wraps::WrapIStreamBufferImpl input_stream((const char*) data, size);

	void* const result_impl = _dll_handle->Recognizer_loadTemplate(
		_impl,
		&input_stream,
		pbio::stl_wraps::WrapIStream::read_func,
		&exception);

	checkException(exception, *_dll_handle);

	return Template::Ptr::make(_dll_handle, result_impl);
}


inline
Recognizer::MatchResult Recognizer::verifyMatch(
	const Template &template1,
	const Template &template2) const
{
	MatchResult result;

	void* exception = NULL;

	_dll_handle->Recognizer_verifyMatch_v2(
		_impl,
		(const pbio::facerec::TemplateImpl*) template1._impl,
		(const pbio::facerec::TemplateImpl*) template2._impl,
		&result.distance,
		&result.fa_r,
		&result.fr_r,
		&result.score,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline
TemplatesIndex::Ptr Recognizer::createIndex(
	const std::vector<pbio::Template::Ptr> &templates,
	const int search_threads_count,
	const int reserve_queries_count) const
{
	std::vector<const pbio::facerec::TemplateImpl*> templates_impls(templates.size());

	for(size_t i = 0; i < templates.size(); ++i)
		templates_impls[i] = (const pbio::facerec::TemplateImpl*) templates[i]->_impl;

	void* exception = NULL;

	void* const result_impl =
		_dll_handle->Recognizer_createIndex(
			_impl,
			templates.size(),
			&templates_impls[0],
			search_threads_count,
			&exception);

	checkException(exception, *_dll_handle);

	const TemplatesIndex::Ptr result = TemplatesIndex::Ptr::make(_dll_handle, result_impl);

	result->reserveSearchMemory(reserve_queries_count);

	return result;
}


inline
std::vector<Recognizer::SearchResult> Recognizer::search(
	const pbio::Template &query_template,
	const pbio::TemplatesIndex &templates_index,
	const size_t k,
	const SearchAccelerationType acceleration) const
{
	std::vector<int64_t> result_i(k);
	std::vector<float> result_distance(k);
	std::vector<float> result_far(k);
	std::vector<float> result_frr(k);
	std::vector<float> result_score(k);

	void* exception = NULL;

	_dll_handle->Recognizer_search_v2(
		_impl,
		acceleration,
		1,  // queries_count
		(const pbio::facerec::TemplateImpl* const*) &query_template._impl,
		(const pbio::facerec::TemplatesIndexImpl*) templates_index._impl,
		k,
		&result_i[0],
		&result_distance[0],
		&result_far[0],
		&result_frr[0],
		&result_score[0],
		&exception);

	checkException(exception, *_dll_handle);

	std::vector<SearchResult> result(k);
	result.reserve(k);

	for(size_t i = 0; i < k; ++i)
	{
		if(result_i[i] < 0)
		{
			result.resize(i);
			break;
		}

		result[i].i = result_i[i];
		result[i].match_result.distance = result_distance[i];
		result[i].match_result.fa_r = result_far[i];
		result[i].match_result.fr_r = result_frr[i];
		result[i].match_result.score = result_score[i];
	}

	return result;
}


inline
std::vector<std::vector<Recognizer::SearchResult> > Recognizer::search(
	const std::vector<pbio::Template::Ptr> &queries_templates,
	const pbio::TemplatesIndex &templates_index,
	const size_t k,
	const SearchAccelerationType acceleration) const
{
	const size_t queries_count = queries_templates.size();

	std::vector<const pbio::facerec::TemplateImpl*> queries_impls(queries_count);

	for(size_t i = 0; i < queries_count; ++i)
	{
		queries_impls[i] = (const pbio::facerec::TemplateImpl*) queries_templates[i]->_impl;
	}

	std::vector<int64_t> result_i(k * queries_count);
	std::vector<float> result_distance(k * queries_count);
	std::vector<float> result_far(k * queries_count);
	std::vector<float> result_frr(k * queries_count);
	std::vector<float> result_score(k * queries_count);

	void* exception = NULL;

	_dll_handle->Recognizer_search_v2(
		_impl,
		acceleration,
		queries_count,
		&queries_impls[0],
		(const pbio::facerec::TemplatesIndexImpl*) templates_index._impl,
		k,
		&result_i[0],
		&result_distance[0],
		&result_far[0],
		&result_frr[0],
		&result_score[0],
		&exception);

	checkException(exception, *_dll_handle);

	std::vector<std::vector<SearchResult> > result(
		queries_count,
		std::vector<SearchResult>(k));

	for(size_t i = 0; i < queries_count; ++i)
	{
		for(size_t j = 0; j < k; ++j)
		{
			const size_t h = i * k + j;

			if(result_i[h] < 0)
			{
				result[i].resize(j);
				break;
			}

			result[i][j].i = result_i[h];
			result[i][j].match_result.distance = result_distance[h];
			result[i][j].match_result.fa_r = result_far[h];
			result[i][j].match_result.fr_r = result_frr[h];
			result[i][j].match_result.score = result_score[h];
		}
	}

	return result;
}





inline
Recognizer::MatchResult Recognizer::getROCCurvePointByDistanceThreshold(
	const double distance_threshold) const
{
	MatchResult result;

	void* exception = NULL;

	_dll_handle->Recognizer_getROCCurvePointByDistanceThreshold_v2(
		_impl,
		distance_threshold,
		&result.distance,
		&result.fa_r,
		&result.fr_r,
		&result.score,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline
Recognizer::MatchResult Recognizer::getROCCurvePointByFAR(
	const double desired_far) const
{
	MatchResult result;

	void* exception = NULL;

	_dll_handle->Recognizer_getROCCurvePointByFAR_v2(
		_impl,
		desired_far,
		&result.distance,
		&result.fa_r,
		&result.fr_r,
		&result.score,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline
Recognizer::MatchResult Recognizer::getROCCurvePointByFRR(
	const double desired_frr) const
{
	MatchResult result;

	void* exception = NULL;

	_dll_handle->Recognizer_getROCCurvePointByFRR_v2(
		_impl,
		desired_frr,
		&result.distance,
		&result.fa_r,
		&result.fr_r,
		&result.score,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}


inline
Recognizer::MatchResult Recognizer::getROCCurvePointByScoreThreshold(
	const double score_threshold) const
{
	MatchResult result;

	void* exception = NULL;

	_dll_handle->Recognizer_getROCCurvePointByScoreThreshold(
		_impl,
		score_threshold,
		&result.distance,
		&result.fa_r,
		&result.fr_r,
		&result.score,
		&exception);

	checkException(exception, *_dll_handle);

	return result;
}

inline
std::vector<size_t> Recognizer::chooseRepresentativeTemplatesSet(
	const size_t set_size,
	const std::vector<pbio::Template::Ptr> &templates,
	const std::vector<size_t> &inviolable_templates_indexes)
{
	std::vector<pbio::facerec::TemplateImpl const*> template_impls(templates.size());

	for(size_t i = 0; i < templates.size(); ++i)
		template_impls[i] = (pbio::facerec::TemplateImpl const*) templates[i]->_impl;

	std::vector<int8_t> inv_flags_8(templates.size(), 0);

	for(size_t i = 0; i < inviolable_templates_indexes.size(); ++i)
	{
		const size_t j = inviolable_templates_indexes[i];

		if(j >= templates.size())
			throw pbio::Error(
				0x63156958,
				"Error: bad index in chooseRepresentativeTemplatesSet, error code: 0x63156958");

		inv_flags_8[j] = 1;
	}

	std::vector<int32_t> result(set_size);

	void* exception = NULL;

	_dll_handle->Recognizer_chooseRepresentativeTemplatesSet(
		_impl,
		template_impls.empty() ? NULL : template_impls.data(),
		inv_flags_8.empty() ? NULL : inv_flags_8.data(),
		templates.size(),
		result.empty() ? NULL : result.data(),
		set_size,
		&exception);

	checkException(exception, *_dll_handle);

	return std::vector<size_t>(result.begin(), result.end());
}


}  // pbio namespace


#endif  // __PBIO_API__PBIO__RECOGNIZER_H_
