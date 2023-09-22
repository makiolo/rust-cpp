// handle.h - Handles to C++ objects
// Copyright (c) KALX, LLC. All rights reserved. No warranty made.
#pragma once
//#include <memory>
#include <set>
#include <windows.h>
#include "XLCALL.H"

using HANDLEX = double;

namespace xll {

	// HANDLEX that defaults to NaN
	class handlex {
		HANDLEX h_;
	public:
		handlex()
			: h_(std::numeric_limits<double>::quiet_NaN())
		{ }
		operator HANDLEX()
		{
			return h_;
		}
		HANDLEX operator=(HANDLEX h)
		{
			return h_ = h;
		}
	};

	// Pointers to objects encoded as doubles.
	// Use first pointer allocated as a base offset
    // because 64-bit pointers are not always valid doubles.
	template<class T>
	class handle {
        class set {
            static std::map<HANDLEX,T*>& handle_map()
            {
                struct map_ {
                    std::map<HANDLEX,T*> m;
                    ~map_()
                    {
                        for (auto& p : m) 
                            delete p.second;
                    }
                };
                static map_ m;

                return m.m;
            }
        public:
            // Convert pointer to (small) number based on first allocation.
            static HANDLEX lookup(T* p)
            {
                static T* base = nullptr;
                if (base == nullptr) {
                    base = 8 + p;
                }
                return static_cast<HANDLEX>(p - base);
            }
            static void insert(T* p)
            {
                HANDLEX h = lookup(p);

                // delete if old handle in cell points at something
                OPER oldh = Excel(xlCoerce, Excel(xlfCaller));
                if (oldh.isNum() && oldh.val.num != 0) {
                    auto i = handle_map().find(h);
                    if (i != handle_map().end()) {
                        delete i->second;
                        handle_map().erase(i);
                    }
                }

                handle_map().insert(std::make_pair(h, p));
            }
            static T* find(HANDLEX h)
            {
                auto i = handle_map().find(h);

                return i == handle_map().end() ? nullptr : i->second;
            }
        };
		T* pt;
    public:
        //!!! use ptr sink
        handle(T* p)
            : pt(p)
        {
            set::insert(p);
        }
		handle(HANDLEX h)
		{
            //!!! check if h in handles
			pt = set::find(h);
            ensure(pt != nullptr || !"invalid handle");
		}
		handle(const handle&) = delete;
		handle& operator=(const handle&) = delete;
		~handle()
		{ }
        template<class U>
        bool operator==(const handle<U>& h) const
        {
            return pt == h.pt;
        }
		HANDLEX get() const
		{
			return set::lookup(pt);
		}
		operator HANDLEX()
		{
			return get();
		}
		T& operator*()
		{
			return *pt;
		}
		T* operator->()
		{
			return pt;
		}
		T* ptr()
		{
			return pt;
		}
    };
} // xll namespace
