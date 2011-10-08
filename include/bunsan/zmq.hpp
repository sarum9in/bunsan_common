#ifndef BUNSAN_ZMQ_HPP
#define BUNSAN_ZMQ_HPP

#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <zmq.hpp>

namespace bunsan{namespace zmq
{
	/// autoclosing socket
	using ::zmq::message_t;
	using ::zmq::context_t;
	using ::zmq::error_t;
	class socket_t: public ::zmq::socket_t
	{
		boost::optional<int> linger;
	public:
		socket_t(zmq::context_t &context, int type);
		void set_linger(int linger_);
		using ::zmq::socket_t::send;
		using ::zmq::socket_t::recv;
		// send overloads
		bool send(const std::string &msg, int opt=0);
		bool send(const char *msg, int opt=0);
		bool send(const std::vector<unsigned char> &msg, int opt=0);
		//TODO all char overloads
		bool send(const std::vector<std::string> &msg, int opt=0);
		template <typename T>
		bool send_more(const T &msg)
		{
			return send(msg, ZMQ_SNDMORE);
		}
		// receive overloads
		bool recv(zmq::message_t &msg, int opt=0);
		bool recv(std::string &msg, int opt=0);
		bool recv(std::vector<unsigned char> &msg, int opt=0);
		bool recv(std::vector<std::string> &msg, int opt=0);
		template <typename T>
		bool recv_more(T &msg, int &more, int opt=0)
		{
			bool ret = false;
			if (more)
			{
				ret = recv(msg, opt);
				getsockopt(ZMQ_RCVMORE, &more);
			}
			return ret;
		}
		//TODO all char overloads
		// sizeof incapsulation
		using ::zmq::socket_t::setsockopt;
		using ::zmq::socket_t::getsockopt;
		template <typename T>
		inline void setsockopt(int option, const T *value)
		{
			::zmq::socket_t::setsockopt(option, value, sizeof(T));
		}
		template <typename T>
		inline void setsockopt(int option, const T &value)
		{
			setsockopt(option, &value);
		}
		template <typename T>
		inline void getsockopt(int option, T &value)
		{
			getsockopt(option, &value);
		}
		template <typename T>
		inline void getsockopt(int option, T *value)
		{
			size_t len = sizeof(T);
			::zmq::socket_t::getsockopt(option, value, &len);
		}
		~socket_t();
	};
	typedef socket_t socket;
}}

#endif //BUNSAN_ZMQ_HPP

