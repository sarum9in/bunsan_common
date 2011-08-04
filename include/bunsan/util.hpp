#ifndef BUNSAN_UTIL_HPP
#define BUNSAN_UTIL_HPP

#include <iostream>
#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>

#define SLOG(M) do\
		{\
			std::stringstream buf;\
			buf<<"["<<__FILE__<<":"<<__LINE__<<":"<<__func__<<":"<<boost::posix_time::second_clock::local_time()<<"] "<<M<<std::endl;\
			std::cerr<<buf.str()<<std::flush;\
		} while (false)
#define DLOG(...) SLOG(#__VA_ARGS__)

#include <locale>
#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

namespace bunsan
{
	/*!
	 * \brief recreate directory
	 */
	void reset_dir(const boost::filesystem::path &dir);
	/*!
	 * \brief read info from filename with relative path bug fix
	 * 
	 * it is not safe function, it should be used only from single thread
	 */
	template <typename Ptree>
	void read_info(const std::string &filename, Ptree &pt, const std::locale &loc = std::locale())
	{
		boost::filesystem::initial_path();
		boost::filesystem::current_path(boost::filesystem::absolute(boost::filesystem::path(filename)).parent_path());
		boost::property_tree::info_parser::read_info(boost::filesystem::path(filename).filename().native(), pt);
		boost::filesystem::current_path(boost::filesystem::initial_path());
	}
}

#endif //BUNSAN_UTIL_HPP

