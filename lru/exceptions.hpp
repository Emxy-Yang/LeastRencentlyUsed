#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP

#include <cstddef>
#include <cstring>
#include <string>

namespace sjtu {

class exception {
protected:
	const std::string variant = "";
	std::string detail = "";
public:
	exception() {}
	exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
	exception(const std::string &v, const std::string &d) : variant(v), detail(d) {}
	virtual std::string what() {
		return variant + " " + detail;
	}
};

class index_out_of_bound : public exception {
public:
	index_out_of_bound()
		: exception("index_out_of_bound", "") {}
	index_out_of_bound(const std::string &msg)
		: exception("index_out_of_bound", msg) {}
	/* __________________________ */
};

class runtime_error : public exception {
public:
	runtime_error()
		: exception("runtime_error", "") {}
	runtime_error(const std::string &msg)
		: exception("runtime_error", msg) {}
	/* __________________________ */
};

class invalid_iterator : public exception {
public:
	invalid_iterator()
		: exception("invalid_iterator", "") {}
	invalid_iterator(const std::string &msg)
		: exception("invalid_iterator", msg) {}
	/* __________________________ */
};

class container_is_empty : public exception {
public:
	container_is_empty()
		: exception("container_is_empty", "") {}
	container_is_empty(const std::string &msg)
		: exception("container_is_empty", msg) {}
	/* __________________________ */
};
}

#endif
