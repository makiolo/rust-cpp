#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <variant>
#include <string>
#include <future>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <NumCpp.hpp>
#include <algorithm>
#include <experimental/mdspan>
#include <cmath>
//
#include "calculation.h"
#include "fes/sync.h"

enum CategoricalType
{
    categorical_unknown_type = 0,
    categorical_basic_none_type,
    categorical_string_type,
    categorical_basic_numpy_type,

    categorical_map_string_serie_type,
    categorical_map_serie_serie_type,

    categorical_set_serie_type,

	//
    categorical_vector_none_type,
    // vector_long_type,
    categorical_vector_string_type,

    categorical_vector_map_string_serie_type,
    categorical_vector_map_serie_serie_type,

    categorical_vector_set_serie_type,

	//

    categorical_vector_serie_type,

    //

    categorical_calculation_type,

	//
    categorical_total_types
};

struct CategoricalNoneType
{

};

/*
class Categorical : public std::enable_shared_from_this<Categorical>
{
public:
    using CategoricalBuffer = nc::NdArray<long>;

    Categorical() = default;
    ~Categorical() = default;

//    explicit Categorical()
//        : type(CategoricalType::unknown_type)
//    {
//        // std::cout << "create empty Categorical" << std::endl;
//    }
//	~Categorical() noexcept { ; }

    // TODO: add long and vector<long>

    Categorical(const Categorical& other)
        : values(other.values)
        , type(other.type)
    {
        //std::cout << "copy constructor" << std::endl;
    }

    Categorical(Categorical&& other) noexcept
        : values(std::move(other.values))
        , type(other.type)
    {
        //std::cout << "move constructor" << std::endl;
    }

    Categorical& operator=(const Categorical& other)
    {
        //std::cout << "operator= copy" << std::endl;
        values = other.values;
        type = other.type;
        return *this;
    }

    Categorical& operator=(Categorical&& other) noexcept
    {
        //std::cout << "operator= move" << std::endl;
        values = std::move(other.values);
        type = other.type;
        return *this;
    }

    explicit Categorical(const CategoricalNoneType& none)
		: values(none)
		, type(CategoricalType::categorical_basic_none_type)
	{
		//std::cout << "create None" << std::endl;
	}

	Categorical(long data)
        : values(CategoricalBuffer({data}))
        , type(CategoricalType::categorical_basic_numpy_type)
	{
		//std::cout << "create long (numpy)" << std::endl;
	}

	explicit Categorical(const std::string& data)
		: values(data)
		, type(CategoricalType::categorical_string_type)
	{
		//std::cout << "create string" << std::endl;
	}

    explicit Categorical(const long* xx, int xx_n)
            : values(CategoricalBuffer(xx, xx_n))
            , type(CategoricalType::categorical_basic_numpy_type)
    {
        //std::cout << "create numpy1d" << std::endl;
    }

	explicit Categorical(long* xx, int xx_n)
		: values(CategoricalBuffer(xx, xx_n))
		, type(CategoricalType::categorical_basic_numpy_type)
	{
        //std::cout << "create numpy1d" << std::endl;
	}

    explicit Categorical(long* xx, int xx_n, bool takeOwnerShip)
            : values(CategoricalBuffer(xx, xx_n, takeOwnerShip))
            , type(CategoricalType::categorical_basic_numpy_type)
    {
        //std::cout << "create numpy1d" << std::endl;
    }

    explicit Categorical(const CategoricalBuffer& data)
		: values(data)
		, type(CategoricalType::categorical_basic_numpy_type)
    {
		//std::cout << "create numpy1d from const&" << std::endl;
	}

    explicit Categorical(const std::initializer_list<long>& data)
        : values(CategoricalBuffer(std::forward< CategoricalBuffer >(data)))
        , type(CategoricalType::categorical_basic_numpy_type)
    {
        //std::cout << "create numpy1d array from initializer list" << std::endl;
    }

    explicit Categorical(const std::unordered_map<std::string, std::shared_ptr<Categorical> >& data)
		: values(data)
		, type(CategoricalType::categorical_map_string_serie_type)
	{
        //std::cout << "create map < string, Categorical >" << std::endl;
	}

    explicit Categorical(const std::unordered_map<std::shared_ptr<Categorical>, std::shared_ptr<Categorical> >& data)
		: values(data)
		, type(CategoricalType::categorical_map_serie_serie_type)
	{
		//std::cout << "create map < Categorical, Categorical >" << std::endl;
	}

    explicit Categorical(const std::unordered_set<std::shared_ptr<Categorical> >& data)
		: values(data)
		, type(CategoricalType::categorical_set_serie_type)
	{
        //std::cout << "create set < Categorical >" << std::endl;
	}

    explicit Categorical(rp::Calculation<Categorical>&& data)
            : values(std::make_shared<rp::Calculation<Categorical> >(std::move(data)))
            , type(CategoricalType::categorical_calculation_type)
    {
        //std::cout << "create Categorical from RValue calculation&&" << std::endl;
    }

	explicit Categorical(const std::vector<CategoricalNoneType>& none)
		: values(none)
		, type(CategoricalType::categorical_vector_none_type)
	{
		//std::cout << "create vector of None" << std::endl;
	}

	explicit Categorical(const std::vector<long>& data)
        : values(CategoricalBuffer(data.data(), data.size()))
        , type(CategoricalType::categorical_basic_numpy_type)
	{
        ;
	}

    explicit Categorical(std::vector<long>&& data)
            : values(CategoricalBuffer(data.data(), data.size()))
            , type(CategoricalType::categorical_basic_numpy_type)
    {
        ;
    }

	explicit Categorical(const std::vector<std::string>& data)
		: values(data)
		, type(CategoricalType::categorical_vector_string_type)
	{
		//std::cout << "create vector < string >" << std::endl;
	}

	explicit Categorical(const std::vector<std::shared_ptr<Categorical> >& data)
		: values(data)
		, type(CategoricalType::categorical_vector_serie_type)
	{
		//std::cout << "create vector < Categorical >" << std::endl;
	}

	explicit Categorical(const std::vector<std::unordered_map<std::string, std::shared_ptr<Categorical> > >& data)
		: values(data)
		, type(CategoricalType::categorical_vector_map_string_serie_type)
	{
        //std::cout << "create vector < map < string, Categorical > >" << std::endl;
	}

	explicit Categorical(const std::vector<std::unordered_map<std::shared_ptr<Categorical>, std::shared_ptr<Categorical> > >& data)
		: values(data)
		, type(CategoricalType::categorical_vector_map_serie_serie_type)
	{
        //std::cout << "create vector < map < Categorical, Categorical > >" << std::endl;
	}

	explicit Categorical(const std::vector<std::unordered_set<std::shared_ptr<Categorical> > >& data)
		: values(data)
		, type(CategoricalType::categorical_vector_set_serie_type)
	{
        //std::cout << "create vector < set < Categorical > >" << std::endl;
	}

//    std::shared_ptr<Categorical> operator+(const std::shared_ptr<Categorical>& other);
//    std::shared_ptr<Categorical> operator-(const std::shared_ptr<Categorical>& other);
//    std::shared_ptr<Categorical> operator*(const std::shared_ptr<Categorical>& other);
//    std::shared_ptr<Categorical> operator/(const std::shared_ptr<Categorical>& other);
//
//    Categorical operator+(const Categorical& other) const;
//    Categorical operator-(const Categorical& other) const;
//    Categorical operator*(const Categorical& other) const;
//    Categorical operator/(const Categorical& other) const;
//    Categorical operator-() const;

	const char* __str__() const
	{
        const static size_t BUF_SIZE = 8192;
		static char tmp[BUF_SIZE];

		if (type == CategoricalType::categorical_calculation_type)
		{
			return get_calc()->__str__();
		}
		else if (type == CategoricalType::categorical_basic_numpy_type)
		{
			const auto& n0 = get< CategoricalBuffer >();
			std::string numpy_str = n0.str();
			const char* ptr = numpy_str.c_str();
			snprintf(tmp, std::min<size_t>(numpy_str.length(), BUF_SIZE), "%s\n", ptr);
			return tmp;
		}
        else if (type == CategoricalType::categorical_string_type)
        {
            const auto& obj_str = get< std::string >();
            const char* ptr = obj_str.c_str();
            snprintf(tmp, std::min<size_t>(obj_str.length() + 1, BUF_SIZE), "%s\n", ptr);
            return tmp;
        }
		else
		{
			return "Categorical class";
		}
	}

	template <typename T>
	inline const T& get() const  // throw(std::bad_variant_access)
	{
        try {
            return std::get<T>(values);
        } catch(const std::bad_variant_access& e)
        {
            // debug point
            throw;
        }
	}

    template <typename T>
    inline T& get()
    {
        try
        {
            return std::get<T>(values);
        }
        catch(const std::bad_variant_access&)
        {
            // debug point
            throw;
        }
    }

    rp::Calculation<Serie>::result_type get_calc() const
    {
        auto calculation = get< std::shared_ptr<rp::Calculation<Serie>> >();
        return calculation->get();
    }

    template <typename T>
    const T& get_result() const
    {
        if(type == CategoricalType::categorical_calculation_type)
            return get_calc()->get<T>();
        else
            throw std::runtime_error("No calculation type.");
    }

    const CategoricalBuffer& data() const
    {
        if(type == CategoricalType::categorical_calculation_type)
            return get_calc()->get<CategoricalBuffer>();
        else
            return get<CategoricalBuffer>();
    }

    const long* raw_data() const
    {
        return data().data();
    }

    long* raw_data()
    {
        if(type == CategoricalType::categorical_calculation_type)
            throw std::runtime_error("rp::Calculation<Categorical> type can't use non-const raw_data().");
        else
            return get<CategoricalBuffer>().data();
    }

    std::shared_ptr<Categorical> sub(int stop)
    {
        if(type == CategoricalType::categorical_calculation_type)
            throw std::runtime_error("Categorical is calculation type.");
        const auto& array_data = data();
        return std::make_shared<Categorical>( array_data.at( nc::Slice(stop) ) );
    }

    std::shared_ptr<Categorical> sub(int start, int stop)
    {
        if(type == CategoricalType::categorical_calculation_type)
            throw std::runtime_error("Categorical is calculation type.");
        const auto& array_data = data();
        return std::make_shared<Categorical>( array_data.at( nc::Slice(start, stop) ) );
    }

    std::shared_ptr<Categorical> sub(int start, int stop, int step)
    {
        if(type == CategoricalType::categorical_calculation_type)
            throw std::runtime_error("Categorical is calculation type.");
        const auto& array_data = data();
        return std::make_shared<Categorical>( array_data.at( nc::Slice(start, stop, step) ) );
    }

#if !defined(SWIGPYTHON) && !defined(SWIGJAVASCRIPT)

    auto reshape(int x, int y) const
    {
        return std::experimental::mdspan(raw_data(), x, y);
    }

    auto reshape(int x, int y, int z) const
    {
        return std::experimental::mdspan(raw_data(), x, y, z);
    }

    auto reshape(int x, int y)
    {
        return std::experimental::mdspan(raw_data(), x, y);
    }

    auto reshape(int x, int y, int z)
    {
        return std::experimental::mdspan(raw_data(), x, y, z);
    }
#endif

    size_t size() const
    {
        return data().numCols();
    }

	void wait() const
	{
        if(type == CategoricalType::categorical_calculation_type)
        {
            const auto &calculation = get<std::shared_ptr<rp::Calculation<Categorical> > >();
            calculation->wait();
        }
        else
        {
            throw std::runtime_error("No calculation type.");
        }
	}

    void wait(long long duration) const
	{
        if(type == CategoricalType::categorical_calculation_type)
        {
            const auto& calculation = get< std::shared_ptr<rp::Calculation<Categorical>> >();
            calculation->wait(duration);
        }
        else
        {
            throw std::runtime_error("No calculation type.");
        }
	}

	bool is_calculated() const
	{
        if(type == CategoricalType::categorical_calculation_type)
        {
            const auto &calculation = get< std::shared_ptr<rp::Calculation<Categorical>> >();
            return calculation->is_ready();
        }
        else
        {
            throw std::runtime_error("No calculation type.");
        }
	}

	inline const std::string& get_basic_string() const
	{
		return get<std::string>();
	}

	inline const std::unordered_map<std::string, std::shared_ptr<Categorical> >& get_map_string_serie() const
	{
		return get<std::unordered_map<std::string, std::shared_ptr<Categorical> > >();
	}

	inline const std::unordered_map<std::shared_ptr<Categorical>, std::shared_ptr<Categorical> >& get_map_serie_serie() const
	{
		return get<std::unordered_map<std::shared_ptr<Categorical>, std::shared_ptr<Categorical> > >();
	}

	inline const std::vector<CategoricalNoneType>& get_vector_none() const
	{
		return get<std::vector<CategoricalNoneType> >();
	}

	inline const std::vector<std::string>& get_vector_string() const
	{
		return get<std::vector<std::string> >();
	}

	inline const std::vector<std::shared_ptr<Categorical> >& get_vector_serie() const
	{
		return get<std::vector<std::shared_ptr<Categorical> > >();
	}

	inline const std::vector<std::unordered_map<std::string, std::shared_ptr<Categorical> > >& get_vector_map_string_serie() const
	{
		return get<std::vector<std::unordered_map<std::string, std::shared_ptr<Categorical> > > >();
	}

	inline const std::vector<std::unordered_map<std::shared_ptr<Categorical>, std::shared_ptr<Categorical> > >& get_vector_map_serie_serie() const
	{
		return get<std::vector<std::unordered_map<std::shared_ptr<Categorical>, std::shared_ptr<Categorical> > > >();
	}

    // DEPRECATED
    inline std::vector<long> get_basic_numpy() const
    {
        return data().toStlVector();
    }

    inline std::vector<long> to_vector() const
    {
        return data().toStlVector();
    }

    inline const std::unordered_set< std::shared_ptr<Categorical> >& get_set_serie() const
    {
        return get< std::unordered_set< std::shared_ptr<Categorical> > >();
    }

    inline const std::vector< std::unordered_set< std::shared_ptr<Categorical> > >& get_vector_set_serie() const
    {
        return get< std::vector< std::unordered_set< std::shared_ptr<Categorical> > > >();
    }

    /////////////////////////////////////


    void set_basic_string(const std::string& data)
    {
        type = CategoricalType::categorical_string_type;
        values = data;
    }

    void set_basic_none()
    {
        type = CategoricalType::categorical_basic_none_type;
    }

    void set_basic_numpy(long data)
    {
        type = CategoricalType::categorical_basic_numpy_type;
        values = CategoricalBuffer({data});
    }

    void set_basic_numpy(long* xx, int xx_n)
    {
        type = CategoricalType::categorical_basic_numpy_type;
        values = CategoricalBuffer(xx, xx_n);
    }

    void set_basic_numpy(const CategoricalBuffer& data)
    {
        type = CategoricalType::categorical_basic_numpy_type;
        values = data;
    }

    void set_basic_numpy(const std::initializer_list<long>& data)
    {
        type = CategoricalType::categorical_basic_numpy_type;
        values = CategoricalBuffer(std::forward< CategoricalBuffer >(data));
    }

    void set_basic_numpy(const std::vector<long>& data)
    {
        type = CategoricalType::categorical_basic_numpy_type;
        values = CategoricalBuffer(data.data(), data.size());
    }

    void set_vector_string(const std::vector<std::string>& data)
    {
        type = CategoricalType::categorical_vector_string_type;
        values = data;
    }

public:
	CategoricalType type;

protected:
	std::variant<
		// basic
		CategoricalNoneType,
        // bool
        long,
		std::string,
        CategoricalBuffer,

		// dict
		std::unordered_map<std::string, std::shared_ptr<Categorical> >,
		std::unordered_map<std::shared_ptr<Categorical>, std::shared_ptr<Categorical> >,
		// set
		std::unordered_set<std::shared_ptr<Categorical> >,

		// list of basic
		std::vector<CategoricalNoneType>,
		std::vector<std::string>,
        std::vector<std::shared_ptr<Categorical> >,
		// dict
		std::vector<std::unordered_map<std::string, std::shared_ptr<Categorical> > >,
		std::vector<std::unordered_map<std::shared_ptr<Categorical>, std::shared_ptr<Categorical> > >,
		// set
		std::vector< std::unordered_set<std::shared_ptr<Categorical> > >,

        // special
        std::shared_ptr<rp::Calculation<Categorical> >

	> values;
};

*/

//std::shared_ptr<Categorical> operator+(const std::shared_ptr<Categorical>& one, const std::shared_ptr<Categorical>& other);
//std::shared_ptr<Categorical> operator-(const std::shared_ptr<Categorical>& one, const std::shared_ptr<Categorical>& other);
//std::shared_ptr<Categorical> operator*(const std::shared_ptr<Categorical>& one, const std::shared_ptr<Categorical>& other);
//std::shared_ptr<Categorical> operator/(const std::shared_ptr<Categorical>& one, const std::shared_ptr<Categorical>& other);
//std::shared_ptr<Categorical> operator-(const std::shared_ptr<Categorical>& one);
//bool operator==(const std::shared_ptr<Categorical>& one, const std::shared_ptr<Categorical>& other);

// print operator
//std::ostream& operator<<(std::ostream &out, const std::shared_ptr<Categorical>& s);
//std::ostream& operator<<(std::ostream &out, const Categorical& s);
