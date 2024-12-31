#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstdint>
#include <memory>
#include <string>
namespace oit::ist::nws::adhoc_routing{
class matrix{
	protected:
	int d;
	std::unique_ptr<std::int32_t[]> mat;
	public:
	matrix();
	matrix(int d);
	~matrix();
	void clear();
	void put(int r, int c, std::int32_t v);
	std::int32_t get(int r, int c);
	virtual std::int32_t det(std::int32_t p)=0;
	std::string to_string();
};
class matrix33:public matrix{
	protected:
	std::int32_t det_elem(std::int32_t a, std::int32_t b, std::int32_t c, std::int32_t p);
	public:
	matrix33():matrix(3){};
	virtual std::int32_t det(std::int32_t p);
};
class matrix44:public matrix{

	protected:
	void cofactor_matrix(int r, int c, matrix33 &m33);
	public:
	matrix44():matrix(4){};
	virtual std::int32_t det(std::int32_t p);
	void adjugate_matrix(matrix44 &m44,std::int32_t p);
};
};


#endif