#include <bunsan/filesystem/operations.hpp>

#include <bunsan/enable_error_info.hpp>
#include <bunsan/filesystem/error.hpp>

#include <deque>

namespace bunsan{namespace filesystem
{
    void reset_dir(const boost::filesystem::path &dir)
    {
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            if (boost::filesystem::exists(dir))
                boost::filesystem::remove_all(dir);
            boost::filesystem::create_directory(dir);
        }
        BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO_EXCEPT(error::path(dir), boost::filesystem::filesystem_error)
    }

    boost::filesystem::path keep_in_root(const boost::filesystem::path &path,
                                         const boost::filesystem::path &root)
    {
        std::deque<boost::filesystem::path> stack;
        for (const boost::filesystem::path &i: path)
        {
            if (i == "..")
            {
                if (!stack.empty())
                    stack.pop_back();
            }
            else if (i != ".")
            {
                stack.push_back(i);
            }
        }
        boost::filesystem::path stripped;
        for (const boost::filesystem::path &i: stack)
            stripped /= i;
        return root / stripped;
    }

#define BUNSAN_EXCEPTIONS_WRAP_END_COPY_TREE() \
    BUNSAN_EXCEPTIONS_WRAP_END_ERROR_INFO_EXCEPT(error::source_path(from) << error::destination_path(to), \
        boost::filesystem::filesystem_error)

    void copy_tree(const boost::filesystem::path &from,
                   const boost::filesystem::path &to)
    {
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            copy_tree(from, to, copy_option::fail_if_exists);
        }
        BUNSAN_EXCEPTIONS_WRAP_END_COPY_TREE()
    }

    void copy_tree(const boost::filesystem::path &from,
                   const boost::filesystem::path &to,
                   boost::system::error_code &ec)
    {
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            copy_tree(from, to, copy_option::fail_if_exists, ec);
        }
        BUNSAN_EXCEPTIONS_WRAP_END_COPY_TREE()
    }

    void copy_tree(const boost::filesystem::path &from,
                   const boost::filesystem::path &to,
                   const BOOST_SCOPED_ENUM(copy_option) option)
    {
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            boost::system::error_code ec;
            copy_tree(from, to, option, ec);
            if (ec)
                BOOST_FILESYSTEM_THROW(boost::filesystem::filesystem_error(
                                       "bunsan::filesystem::copy_tree", from, to, ec));
        }
        BUNSAN_EXCEPTIONS_WRAP_END_COPY_TREE()
    }

    void copy_tree(const boost::filesystem::path &from,
                   const boost::filesystem::path &to,
                   const BOOST_SCOPED_ENUM(copy_option) option,
                   boost::system::error_code &ec)
    {
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            switch (option)
            {
            case copy_option::fail_if_exists:
                if (boost::filesystem::exists(to))
                    ec.assign(EEXIST, boost::system::system_category());
                break;
            case copy_option::overwrite_if_exists:
                boost::filesystem::remove_all(to, ec);
                break;
            }
            if (ec)
                return;
            const boost::filesystem::file_status s(boost::filesystem::symlink_status(from, ec));
            if (ec)
                return;
            boost::filesystem::copy(from, to, ec);
            if (ec)
                return;
            if (is_directory(from, ec) && !ec)
            {
                for (boost::filesystem::directory_iterator i(from), end; !ec && i != end; ++i)
                    copy_tree(i->path(), to / i->path().filename(), ec);
            }
        }
        BUNSAN_EXCEPTIONS_WRAP_END_COPY_TREE()
    }
}}
