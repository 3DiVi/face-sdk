#ifndef ___TEST_SDK__RECOGNITION_TEST_11_H___
#define ___TEST_SDK__RECOGNITION_TEST_11_H___

#include <fstream>

#include "common.h"
#include "progress.h"


namespace recognition_test11{

// test(...) --> make_recognition_test11(...) --> recognition_11_worker(...)

template<typename Comparator, typename T>
struct recognition_11_context
{
	const double min_distance;
	const double max_distance;
	const std::vector<std::vector<T> > &persons;
	const Comparator &comparator;
	const int max_closest_mismatch_count;
	int match_persons;
	int mismatch_persons;

	const int bins_count;
	const double bin_size;
	const double inv_bin_size;

	size_t current_person_id;
	size_t persons_batch_size;
	size_t done_count;

	Progress progress;
	mutex_type mutex;
	
	double tar_sum;
	double far_sum;
	std::vector<double> roc_tar;
	std::vector<double> roc_far;
	std::vector<std::pair<float, std::pair<T, T> > > closest_mismatches;

	
	recognition_11_context(
		const double min_distance,
		const double max_distance,
		const std::vector<std::vector<T> > &persons,
		const Comparator &comparator,
		const int max_closest_mismatch_count,
		const std::string runtime_log_file);
};


template<typename A, typename B>
bool first_less(
	const std::pair<A, B> &a,
	const std::pair<A, B> &b);


void test(
	const std::string dll_path,
	const std::string sdk_config_dir,
	const std::string recognizer_config,
	const std::string dataset_config,
	const std::vector<std::string> processing_result_files,
	const std::string result_roc_file,
	const std::string result_closest_mismatches_file,
	const uint64_t begin_image_id,
	uint64_t end_image_id,
	const int use_cpu_cores_count,
	const std::string runtime_log_file);


// template comparator pass to make_recognition_test11
class TemplateComparator
{
public:
	TemplateComparator(
		const std::vector<pbio::Template::Ptr> &image_id2templates,
		const int threads_count,
		const pbio::FacerecService::Ptr &service,
		const std::string recognizer_config);

	float operator()(
		const int thread_id,
		const uint64_t im_id1,
		const uint64_t im_id2) const;

private:	
	const std::vector<pbio::Template::Ptr> image_id2templates;
	std::vector<pbio::Recognizer::Ptr> recognizers;
};


template<typename Comparator, typename T>
class recognition_11_worker{
public:
	recognition_11_worker(
		recognition_11_context<Comparator, T> &context,
		const int thread_id);

	void operator()();

private:
	recognition_11_context<Comparator, T> &context;
	const int thread_id;
};


template<typename Comparator, typename T>
void make_recognition_test11(
	const double min_distance,
	const double max_distance,
	const std::vector<std::vector<T> > &persons,
	const Comparator &comparator,
	const std::string save_roc_filepath,
	const int max_closest_mismatch_count = 0,
	// out
	std::vector<std::pair<float, std::pair<T, T> > > *closest_mismatches_ptr = NULL,
	const int use_cpu_cores_count = 1,
	const std::string runtime_log_file="");


////////////////////////
/////IMPLEMENTATION/////
////////////////////////


template<typename Comparator, typename T>
inline
recognition_11_context<Comparator, T>::recognition_11_context(
	const double min_distance,
	const double max_distance,
	const std::vector<std::vector<T> > &persons,
	const Comparator &comparator,
	const int max_closest_mismatch_count,
	const std::string runtime_log_file):
min_distance(min_distance),
max_distance(max_distance),
persons(persons),
comparator(comparator),
max_closest_mismatch_count(max_closest_mismatch_count),
match_persons(0),
mismatch_persons(0),
bins_count(10000),
bin_size( (max_distance - min_distance) / bins_count ),
inv_bin_size( 1 / bin_size ),
current_person_id(0),
persons_batch_size(1),
done_count(0),
progress(persons.size(), persons.size() / 1000, runtime_log_file),
tar_sum(0),
far_sum(0)
{
	TASSERT(max_distance > min_distance);

	// count match and mismatch persons
	for(size_t i = 0; i < persons.size(); ++i)
	{
		if(persons[i].size() >= 2)
			match_persons++;

		if(persons[i].size() >= 1)
			mismatch_persons++;
	}

	std::cout << "match_persons: " << match_persons << std::endl;
	std::cout << "mismatch_persons: " << mismatch_persons << std::endl;
}


template<typename A, typename B>
inline
bool first_less(
	const std::pair<A, B> &a,
	const std::pair<A, B> &b)
{
	return a.first < b.first;
}


template<typename Comparator, typename T>
inline
recognition_11_worker<Comparator, T>::recognition_11_worker(
	recognition_11_context<Comparator, T> &context,
	const int thread_id):
context(context),
thread_id(thread_id)
{
	// nothing
}


template<typename Comparator, typename T>
inline
void recognition_11_worker<Comparator, T>::operator()() try
{
	for(;;)
	{
		size_t begin_p1 = 0;
		size_t end_p1 = 0;

		{  // under mutex
			const lock_guard guard(context.mutex);

			begin_p1 = context.current_person_id;
			end_p1 = std::min<size_t>(context.current_person_id + context.persons_batch_size, context.persons.size());
			context.current_person_id = end_p1;
		}

		if( begin_p1 == end_p1 ) break;

		for(size_t p1 = begin_p1; p1 < end_p1; ++p1)
		{
			std::vector<std::pair<float, std::pair<T, T> > > cur_closest_mismatches;

			std::vector<double> cur_roc_tar;
			std::vector<double> cur_roc_far;

			double cur_tar_sum = 0;
			double cur_far_sum = 0;

			const std::vector<T> &person1 = context.persons[p1];

			// compute true pairs
			for(size_t i = 0; i < person1.size(); ++i)
			{
				const int s = person1.size();
				const double w = 1. / context.match_persons / (s * (s-1) / 2);
				for(size_t j = i + 1; j < person1.size(); ++j)
				{
					const float t = context.comparator(thread_id, person1[i], person1[j]);
					TASSERT(t >= 0);

					const int ti = std::max<double>(t - context.min_distance, 0) * context.inv_bin_size;

					if(ti >= cur_roc_tar.size())
						cur_roc_tar.resize(ti + 1, 0.0);

					cur_roc_tar[ti] += w;

					cur_tar_sum += w;
				}
			}


			// comptue false pairs
			for(size_t i = 0; i < person1.size(); ++i)
			{
				const double w1 = 1. / ( context.mismatch_persons * (context.mismatch_persons - 1) / 2 )
					/ person1.size();

				for(size_t p2 = p1 + 1; p2 < context.persons.size(); ++p2)
				{
					const std::vector<T> &person2 = context.persons[p2];

					const double w = w1 / person2.size();

					for(size_t j = 0; j < person2.size(); ++j)
					{
						const float t = context.comparator(thread_id, person1[i], person2[j]);
						TASSERT(t >= 0);

						if(t < 1e8)
						{
							const int ti = std::max<double>(t - context.min_distance, 0) * context.inv_bin_size;

							if(ti >= cur_roc_far.size())
								cur_roc_far.resize(ti + 1, 0.0);

							cur_roc_far[ti] += w;
						}

						cur_far_sum += w;

						if(context.max_closest_mismatch_count > 0)
						{
							cur_closest_mismatches.push_back(
								std::make_pair(t, std::make_pair(person1[i], person2[j])));

							if(cur_closest_mismatches.size() > context.max_closest_mismatch_count * 10)
							{
								std::nth_element(
									cur_closest_mismatches.begin(),
									cur_closest_mismatches.begin() + context.max_closest_mismatch_count - 1,
									cur_closest_mismatches.end(),
									first_less<float, std::pair<T, T> >);
								cur_closest_mismatches.resize(context.max_closest_mismatch_count);
							}
						}
					}
				}
			}


			{  // under mutex
				const lock_guard guard(context.mutex);

				if(context.max_closest_mismatch_count > 0)
				{
					for(size_t i = 0; i < cur_closest_mismatches.size(); ++i)
						context.closest_mismatches.push_back(cur_closest_mismatches[i]);

					if(context.closest_mismatches.size() > context.max_closest_mismatch_count)
					{
						std::nth_element(
							context.closest_mismatches.begin(),
							context.closest_mismatches.begin() + context.max_closest_mismatch_count - 1,
							context.closest_mismatches.end(),
							first_less<float, std::pair<T, T> >);
						context.closest_mismatches.resize(context.max_closest_mismatch_count);
					}
				}

				const int rsize = std::max<size_t>(
					std::max<size_t>(context.roc_tar.size(), context.roc_far.size()),
					std::max<size_t>(cur_roc_far.size(), cur_roc_tar.size()));

				context.roc_tar.resize(rsize, 0.0);
				context.roc_far.resize(rsize, 0.0);

				for(size_t i = 0; i < cur_roc_far.size(); ++i)
					context.roc_far[i] += cur_roc_far[i];

				for(size_t i = 0; i < cur_roc_tar.size(); ++i)
					context.roc_tar[i] += cur_roc_tar[i];

				context.tar_sum += cur_tar_sum;
				context.far_sum += cur_far_sum;

				++context.done_count;
				context.progress.tick(context.done_count);
			}  // under mutex
		}
	}  // for(;;)
}
catch(const std::exception &e)
{
	const lock_guard guard(context.mutex);
	std::cout << "\n\nexception catched in recognition 1:1 worker thread: '" << e.what() << "'\n" << std::endl;
	std::cerr << "\n\nexception catched in recognition 1:1 worker thread: '" << e.what() << "'\n" << std::endl;
}
catch(...)
{
	const lock_guard guard(context.mutex);
	std::cout << "\n\nexception ... catched in recognition 1:1 worker thread\n" << std::endl;
	std::cerr << "\n\nexception ... catched in recognition 1:1 worker thread\n" << std::endl;
}


template<typename Comparator, typename T>
inline
void make_recognition_test11(
	const double min_distance,
	const double max_distance,
	const std::vector<std::vector<T> > &persons,
	const Comparator &comparator,
	const std::string save_roc_filepath,
	const int max_closest_mismatch_count,
	// out
	std::vector<std::pair<float, std::pair<T, T> > > *closest_mismatches_ptr,
	const int use_cpu_cores_count,
	const std::string runtime_log_file)
{
	TASSERT(max_closest_mismatch_count == 0 || closest_mismatches_ptr);
	TASSERT(max_distance > min_distance);

	std::vector<shared_ptr<recognition_11_worker<Comparator, T> > > workers(use_cpu_cores_count);
	std::vector<shared_ptr<thread> > threads(use_cpu_cores_count);

	recognition_11_context<Comparator, T> context(
		min_distance,
		max_distance,
		persons,
		comparator,
		max_closest_mismatch_count,
		runtime_log_file);

	for(int i = 0; i < use_cpu_cores_count; i++)
	{
		workers[i] = shared_ptr<recognition_11_worker<Comparator, T> >(
			new recognition_11_worker<Comparator, T>(context, i) );
		threads[i] = shared_ptr<thread>( new thread(*workers[i]) );
	}

	for(int i = 0; i < use_cpu_cores_count; i++)
	{
		threads[i]->join();
	}

	if(max_closest_mismatch_count > 0)
	{
		std::sort(
			context.closest_mismatches.begin(),
			context.closest_mismatches.end(),
			first_less<float, std::pair<T, T> >);


		closest_mismatches_ptr->assign(
			context.closest_mismatches.begin(),
			context.closest_mismatches.end());
	}

	std::cout << "tar_sum : " << context.tar_sum << std::endl;
	std::cout << "far_sum : " << context.far_sum << std::endl;
	TASSERT(context.roc_tar.size() == context.roc_far.size());

	// save roc
	std::cout << "saving roc" << std::endl;
	std::ofstream file(save_roc_filepath.c_str());
	TASSERT(file.is_open());

	double r_far = 0;
	double r_tar = 0;

	for(size_t i = 0; i < context.roc_far.size(); ++i)
	{
		r_far += context.roc_far[i];
		r_tar += context.roc_tar[i];

		const double threshold = (i + 1) * context.bin_size;

		file << r_far << " " << r_tar << " " << threshold << "\n";
	}

	file.flush();
	TASSERT(file.is_open() && file.good());

	std::cout << "roc saved in '" << save_roc_filepath << "'" << std::endl;
}


} // recognition_test11 namespace


#endif  // ___TEST_SDK__RECOGNITION_TEST_11_H___

