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
#include "npv.h"
#include "IterableRepository.h"

enum SerieType
{
    unknown_type = 0,
	basic_none_type,
	string_type,
	basic_numpy_type,
    integer_numpy_type,

	map_string_serie_type,
	map_serie_serie_type,

	set_serie_type,

	//
	vector_none_type,
    // vector_long_type,
	vector_string_type,

	vector_map_string_serie_type,
	vector_map_serie_serie_type,

	vector_set_serie_type,

	//

	vector_serie_type,

    //

    calculation_type,

	//
	total_types
};

struct NoneType
{

};

/*
enum DType {
    //
    DISCRETE = 0,
    CONTINUOUS,
    CATEGORICAL,
    //
    DISCRETE_MUTABLE,
    CONTINUOUS_MUTABLE,
    CATEGORICAL_MUTABLE,
};
*/

class Serie : public std::enable_shared_from_this<Serie>
{
public:
    using Buffer = nc::NdArray<double>;

    Serie() = default;
    ~Serie() = default;

    Serie(const Serie& other)
        : values(other.values)
        , type(other.type)
        , write_notification(other.write_notification)
    {
        //std::cout << "Serie copy constructor" << std::endl;
    }

    Serie(Serie&& other) noexcept
        : values(std::move(other.values))
        , type(other.type)
        , write_notification(std::move(other.write_notification))
    {
        //std::cout << "Serie move constructor" << std::endl;
    }

    Serie& operator=(const Serie& other)
    {
        if (this != &other) {
            //std::cout << "Serie operator= copy" << std::endl;
            values = other.values;
            type = other.type;
            write_notification = other.write_notification;
        }
        return *this;
    }

    Serie& operator=(Serie&& other) noexcept
    {
        if (this != &other) {
            //std::cout << "Serie operator= move" << std::endl;
            values = std::move(other.values);
            type = other.type;
            write_notification = std::move(write_notification);
        }
        return *this;
    }

    explicit Serie(const NoneType& none)
		: values(none)
		, type(SerieType::basic_none_type)
	{
		//std::cout << "create None" << std::endl;
	}

    /*
    Serie(double data, DType dtype)
    {
        switch(dtype)
        {
            case CATEGORICAL:
            case CATEGORICAL_MUTABLE:
            {
                throw std::runtime_error("Invalid type, not is categorical");
                break;
            }
            case DISCRETE:
            case DISCRETE_MUTABLE:
            {
                values = BufferInt({static_cast<int>(data)});
                type = SerieType::integer_numpy_type;
                break;
            }
            case CONTINUOUS:
            case CONTINUOUS_MUTABLE:
            default:
            {
                values = Buffer({data});
                type = SerieType::basic_numpy_type;
            }
        }
    }
    */

	Serie(double data)
        : values(Buffer({data}))
        , type(SerieType::basic_numpy_type)
	{
		//std::cout << "create double (numpy)" << std::endl;
	}

	explicit Serie(const std::string& data)
		: values(data)
		, type(SerieType::string_type)
	{
		//std::cout << "create string" << std::endl;
	}

    explicit Serie(const double* xx, int xx_n)
            : values(Buffer(xx, xx_n))
            , type(SerieType::basic_numpy_type)
    {
        //std::cout << "create numpy1d" << std::endl;
    }

	explicit Serie(double* xx, int xx_n)
		: values(Buffer(xx, xx_n))
		, type(SerieType::basic_numpy_type)
	{
        //std::cout << "create numpy1d" << std::endl;
	}

    explicit Serie(double* xx, int xx_n, bool takeOwnerShip)
            : values(Buffer(xx, xx_n, takeOwnerShip))
            , type(SerieType::basic_numpy_type)
    {
        //std::cout << "create numpy1d" << std::endl;
    }

    explicit Serie(const Buffer& data)
		: values(data)
		, type(SerieType::basic_numpy_type)
    {
		//std::cout << "create numpy1d from const&" << std::endl;
	}

    explicit Serie(const std::initializer_list<double>& data)
        : values(Buffer(std::forward<Buffer>(data)))
        , type(SerieType::basic_numpy_type)
    {
        //std::cout << "create numpy1d array from initializer list" << std::endl;
    }

    explicit Serie(const std::unordered_map<std::string, std::shared_ptr<Serie> >& data)
		: values(data)
		, type(SerieType::map_string_serie_type)
	{
        //std::cout << "create map < string, Serie >" << std::endl;
	}

    explicit Serie(const std::unordered_map<std::shared_ptr<Serie>, std::shared_ptr<Serie> >& data)
		: values(data)
		, type(SerieType::map_serie_serie_type)
	{
		//std::cout << "create map < Serie, Serie >" << std::endl;
	}

    explicit Serie(const std::unordered_set<std::shared_ptr<Serie> >& data)
		: values(data)
		, type(SerieType::set_serie_type)
	{
        //std::cout << "create set < Serie >" << std::endl;
	}

    explicit Serie(rp::Calculation<Serie>&& data)
            : values(std::make_shared<rp::Calculation<Serie>>(std::move(data)))
            , type(SerieType::calculation_type)
    {
        //std::cout << "create serie from RValue calculation&&" << std::endl;
    }

	explicit Serie(const std::vector<NoneType>& none)
		: values(none)
		, type(SerieType::vector_none_type)
	{
		//std::cout << "create vector of None" << std::endl;
	}

	explicit Serie(const std::vector<double>& data)
        : values(Buffer(data.data(), data.size()))
        , type(SerieType::basic_numpy_type)
	{
        ;
	}

    explicit Serie(std::vector<double>&& data)
            : values(Buffer(data.data(), data.size()))
            , type(SerieType::basic_numpy_type)
    {
        ;
    }

	explicit Serie(const std::vector<std::string>& data)
		: values(data)
		, type(SerieType::vector_string_type)
	{
		//std::cout << "create vector < string >" << std::endl;
	}

	explicit Serie(const std::vector<std::shared_ptr<Serie> >& data)
		: values(data)
		, type(SerieType::vector_serie_type)
	{
		//std::cout << "create vector < Serie >" << std::endl;
	}

	explicit Serie(const std::vector<std::unordered_map<std::string, std::shared_ptr<Serie> > >& data)
		: values(data)
		, type(SerieType::vector_map_string_serie_type)
	{
        //std::cout << "create vector < map < string, Serie > >" << std::endl;
	}

	explicit Serie(const std::vector<std::unordered_map<std::shared_ptr<Serie>, std::shared_ptr<Serie> > >& data)
		: values(data)
		, type(SerieType::vector_map_serie_serie_type)
	{
        //std::cout << "create vector < map < Serie, Serie > >" << std::endl;
	}

	explicit Serie(const std::vector<std::unordered_set<std::shared_ptr<Serie> > >& data)
		: values(data)
		, type(SerieType::vector_set_serie_type)
	{
        //std::cout << "create vector < set < Serie > >" << std::endl;
	}

//	std::shared_ptr<Serie> __getitem__(const std::string& index) const
//	{
//        //std::cout << "getitem = " << index << std::endl;
//
//		if (type == SerieType::map_string_serie_type)
//		{
//			auto& data = get_map_string_serie();
//			return data.at(index);
//		}
//		else if (type == SerieType::vector_serie_type)
//		{
//			auto& data = get_vector_serie();
//			auto result = data[stoi(index)];
//			return result;
//		}
//		else
//		{
//			throw std::runtime_error("Invalid type __getitem__");
//		}
//	}
//
//	void __setitem__(const std::string& index, long d)
//	{
//	    std::cout << "setitem " << index << std::endl;
//	}

	/*
	void __setitem__(const std::string& index, const std::vector<long>& d)
	{
		std::cout << "set serie vector long" << std::endl;
	}

	void __setitem__(const std::string& index, const std::shared_ptr<Serie>& d)
	{
		std::cout << "set serie generic " << index << std::endl;
	}
	*/

    std::shared_ptr<Serie> operator+(const std::shared_ptr<Serie>& other);
    std::shared_ptr<Serie> operator-(const std::shared_ptr<Serie>& other);
    std::shared_ptr<Serie> operator*(const std::shared_ptr<Serie>& other);
    std::shared_ptr<Serie> operator/(const std::shared_ptr<Serie>& other);

    Serie operator+(const Serie& other) const;
    Serie operator-(const Serie& other) const;
    Serie operator*(const Serie& other) const;
    Serie operator/(const Serie& other) const;
    Serie operator-() const;

	const char* __str__() const
	{
        const static size_t BUF_SIZE = 8192;
		static char tmp[BUF_SIZE];

		if (type == SerieType::calculation_type)
		{
			return get_calc()->__str__();
		}
		else if (type == SerieType::basic_numpy_type)
		{
			const auto& n0 = get< Buffer >();
			std::string numpy_str = n0.str();
			const char* ptr = numpy_str.c_str();
			snprintf(tmp, std::min<size_t>(numpy_str.length(), BUF_SIZE), "%s\n", ptr);
			return tmp;
		}
        else if (type == SerieType::string_type)
        {
            const auto& obj_str = get< std::string >();
            const char* ptr = obj_str.c_str();
            snprintf(tmp, std::min<size_t>(obj_str.length() + 1, BUF_SIZE), "%s\n", ptr);
            return tmp;
        }
		else
		{
			return "Serie class";
		}
	}

    double read(int offset) const
    {
        return raw_data()[offset];
    }

    void write(int offset, double data)
    {
        raw_data()[offset] = data;
        write_notification(shared_from_this());
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

//    inline const Buffer& get() const
//    {
//        if(type == SerieType::calculation_type)
//            return get_calc()->get< Buffer >();
//        else
//            return get< Buffer >();
//    }

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
        if(type == SerieType::calculation_type)
            return get_calc()->get<T>();
        else
            throw std::runtime_error("No calculation type.");
    }

    const Buffer& data() const
    {
        if(type == SerieType::calculation_type)
            return get_calc()->get<Buffer >();
        else
            return get<Buffer >();
    }

    const double* raw_data() const
    {
        return data().data();
    }

    double* raw_data()
    {
        if(type == SerieType::calculation_type)
            throw std::runtime_error("rp::Calculation<Serie> type can't use non-const raw_data().");
        else
            return get<Buffer >().data();
    }

    std::shared_ptr<Serie> sub(int stop)
    {
//        if(type == SerieType::calculation_type)
//            throw std::runtime_error("Serie is calculation type.");
        const auto& array_data = data();
        return std::make_shared<Serie>( array_data.at( nc::Slice(stop) ) );
    }

    std::shared_ptr<Serie> sub(int start, int stop)
    {
//        if(type == SerieType::calculation_type)
//            throw std::runtime_error("Serie is calculation type.");
        const auto& array_data = data();
        return std::make_shared<Serie>( array_data.at( nc::Slice(start, stop) ) );
    }

    std::shared_ptr<Serie> sub(int start, int stop, int step)
    {
//        if(type == SerieType::calculation_type)
//            throw std::runtime_error("Serie is calculation type.");
        const auto& array_data = data();
        return std::make_shared<Serie>( array_data.at( nc::Slice(start, stop, step) ) );
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
        if(type == SerieType::calculation_type)
        {
            const auto &calculation = get<std::shared_ptr<rp::Calculation<Serie>> >();
            calculation->wait();
        }
        else
        {
            throw std::runtime_error("No calculation type.");
        }
	}

    void wait(long long duration) const
	{
        if(type == SerieType::calculation_type)
        {
            auto calculation = get< std::shared_ptr<rp::Calculation<Serie>> >();
            calculation->wait(duration);
        }
        else
        {
            throw std::runtime_error("No calculation type.");
        }
	}

	bool is_calculated() const
	{
        if(type == SerieType::calculation_type)
        {
            auto calculation = get< std::shared_ptr<rp::Calculation<Serie>> >();
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

	inline const std::unordered_map<std::string, std::shared_ptr<Serie> >& get_map_string_serie() const
	{
		return get<std::unordered_map<std::string, std::shared_ptr<Serie> > >();
	}

	inline const std::unordered_map<std::shared_ptr<Serie>, std::shared_ptr<Serie> >& get_map_serie_serie() const
	{
		return get<std::unordered_map<std::shared_ptr<Serie>, std::shared_ptr<Serie> > >();
	}

	inline const std::vector<NoneType>& get_vector_none() const
	{
		return get<std::vector<NoneType> >();
	}

	inline const std::vector<std::string>& get_vector_string() const
	{
		return get<std::vector<std::string> >();
	}

	inline const std::vector<std::shared_ptr<Serie> >& get_vector_serie() const
	{
		return get<std::vector<std::shared_ptr<Serie> > >();
	}

	inline const std::vector<std::unordered_map<std::string, std::shared_ptr<Serie> > >& get_vector_map_string_serie() const
	{
		return get<std::vector<std::unordered_map<std::string, std::shared_ptr<Serie> > > >();
	}

	inline const std::vector<std::unordered_map<std::shared_ptr<Serie>, std::shared_ptr<Serie> > >& get_vector_map_serie_serie() const
	{
		return get<std::vector<std::unordered_map<std::shared_ptr<Serie>, std::shared_ptr<Serie> > > >();
	}

    inline std::vector<double> to_vector() const
    {
        return data().toStlVector();
    }

    inline const std::unordered_set< std::shared_ptr<Serie> >& get_set_serie() const
    {
        return get< std::unordered_set< std::shared_ptr<Serie> > >();
    }

    inline const std::vector< std::unordered_set< std::shared_ptr<Serie> > >& get_vector_set_serie() const
    {
        return get< std::vector< std::unordered_set< std::shared_ptr<Serie> > > >();
    }

    /////////////////////////////////////

    void set_basic_string(const std::string& data)
    {
        if(type == SerieType::string_type || type == SerieType::basic_none_type)
        {
            values = data;
            write_notification(shared_from_this());
        }
        else
        {
            throw std::runtime_error("type expected is string_type.");
        }
    }

    void set_basic_none()
    {
        type = SerieType::basic_none_type;
        write_notification(shared_from_this());
    }

    void set_basic_numpy(double data)
    {
        if(type == SerieType::basic_numpy_type || type == SerieType::basic_none_type)
        {
            values = Buffer({data});
            write_notification(shared_from_this());
        }
        else
        {
            throw std::runtime_error("type expected is basic_numpy_type.");
        }
    }

    void set_basic_numpy(double* xx, int xx_n)
    {
        if(type == SerieType::basic_numpy_type || type == SerieType::basic_none_type)
        {
            values = Buffer(xx, xx_n);
            write_notification(shared_from_this());
        }
        else
        {
            throw std::runtime_error("type expected is basic_numpy_type.");
        }
    }

    void set_basic_numpy(const Buffer& data)
    {
        if(type == SerieType::basic_numpy_type || type == SerieType::basic_none_type)
        {
            values = data;
            write_notification(shared_from_this());
        }
        else
        {
            throw std::runtime_error("type expected is basic_numpy_type.");
        }
    }

    void set_basic_numpy(const std::initializer_list<double>& data)
    {
        if(type == SerieType::basic_numpy_type || type == SerieType::basic_none_type)
        {
            values = Buffer(std::forward< Buffer >(data));
            write_notification(shared_from_this());
        }
        else
        {
            throw std::runtime_error("type expected is basic_numpy_type.");
        }
    }

    void set_basic_numpy(const std::vector<double>& data)
    {
        if(type == SerieType::basic_numpy_type || type == SerieType::basic_none_type)
        {
            values = Buffer(data.data(), data.size());
            write_notification(shared_from_this());
        }
        else
        {
            throw std::runtime_error("type expected is basic_numpy_type.");
        }
    }

    void set_vector_string(const std::vector<std::string>& data)
    {
        if(type == SerieType::vector_string_type || type == SerieType::basic_none_type)
        {
            values = data;
            write_notification(shared_from_this());
        }
        else
        {
            throw std::runtime_error("type expected is vector_string_type.");
        }
    }

    void set_calculation(const std::shared_ptr<rp::Calculation<Serie> >& data)
    {
        if(type == SerieType::calculation_type)
        {
            values = data;
        }
        else
        {
            throw std::runtime_error("type expected is set_calculation.");
        }
    }

public:
	SerieType type;
    mutable fes::sync<std::shared_ptr<Serie> > write_notification;

protected:
	std::variant<
		// basic
		NoneType,
        // bool
        long,
        // double
		std::string,
        Buffer,
        // BufferInt,

		// dict
		std::unordered_map<std::string, std::shared_ptr<Serie> >,
		std::unordered_map<std::shared_ptr<Serie>, std::shared_ptr<Serie> >,
		// set
		std::unordered_set<std::shared_ptr<Serie> >,

		// list of basic
		std::vector<NoneType>,
		std::vector<std::string>,
        std::vector<std::shared_ptr<Serie> >,
		// dict
		std::vector<std::unordered_map<std::string, std::shared_ptr<Serie> > >,
		std::vector<std::unordered_map<std::shared_ptr<Serie>, std::shared_ptr<Serie> > >,
		// set
		std::vector< std::unordered_set<std::shared_ptr<Serie> > >,

        // special
        std::shared_ptr<rp::Calculation<Serie>>

	> values;
};


namespace rp {

extern Serie ZERO;
/*
extern Serie HALF;
extern Serie ONE;
extern Serie TWO;
extern Serie PI;
extern Serie TWO_PI;
*/

}



std::shared_ptr<Serie> operator+(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other);
std::shared_ptr<Serie> operator-(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other);
std::shared_ptr<Serie> operator*(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other);
std::shared_ptr<Serie> operator/(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other);
std::shared_ptr<Serie> operator-(const std::shared_ptr<Serie>& one);
//std::shared_ptr<Serie> operator+=(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other);
//std::shared_ptr<Serie> operator*=(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other);
bool operator==(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other);
// std::shared_ptr<Serie> operator==(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other);

// print operator
std::ostream& operator<<(std::ostream &out, const std::shared_ptr<Serie>& s);
std::ostream& operator<<(std::ostream &out, const Serie& s);


namespace rp {

    using internal_array = Serie::Buffer;
    using column = Serie;
    using column_ptr = std::shared_ptr<column>;
    using dataframe = std::vector<column_ptr>;
    using function_ptr = std::function< column_ptr(const column_ptr&, const column_ptr&) >;
    using transformer_ptr = std::function< column_ptr(const column_ptr&) >;
    using indicator_ptr = std::function< dataframe(const dataframe&, int) >;

    column_ptr array(const column& data);
    column_ptr array(const internal_array& data);
    column_ptr array(const std::initializer_list<double>& data);
    column_ptr array(const std::vector<double>& data);
    column_ptr array(double* xx, int xx_n);
    column_ptr array(double* xx, int xx_n, bool takeOwnerShip);

    template <typename T>
    column_ptr calculate(T&& calculation)
    {
        if constexpr ( std::is_same_v<Serie, std::remove_cvref_t<T> > ) {
            if(calculation.type == SerieType::calculation_type)
                return calculation.get_calc();
            else
                return rp::array(calculation);
        } else {
            if(calculation->type == SerieType::calculation_type)
                return calculation->get_calc();
            else
                return calculation;
        }
    }
}
