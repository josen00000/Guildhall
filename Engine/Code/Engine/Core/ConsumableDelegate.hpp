#pragma once
#include <functional>
#include <vector>

template<typename ...ARGS>
class ConsumableDelegate {
public:
	using function_t = std::function<bool( ARGS... )>;
	using c_callback_t = bool(*)(ARGS...);

	struct sub_t {
		void const* func_id;
		void const* obj_id;
		function_t callable;

		inline bool operator==( sub_t const& other ) const { return func_id == other.func_id; }
	};


	void subscribe( c_callback_t const& cb ) {
		sub_t sub;
		sub.func_id = &cb;
		sub.callable = cb;
		subscribe( sub );
	}

	bool invoke( ARGS const&... args ) {
		for( sub_t& cb : m_callbacks ) {
			if( cb.callable( args... ) ) {
				return true;
			}
		}
		return false;
	}

	void unsubscribe( c_callback_t const& cb ) {
		sub_t sub;
		sub.func_id = cb;
		unsubscribe( sub );
	}

	template <typename OBJ_TYPE>
	void subscribe_method( OBJ_TYPE* obj, bool(OBJ_TYPE::* mcb)(ARGS...) ) {
		sub_t sub;
		sub.obj_id = obj;
		sub.func_id = *(void const**)&mcb;
		sub.callable = [ = ]( ARGS ...args ) -> bool { return (obj->*mcb)(args...); };

		subscribe( sub );
	}

	template <typename OBJ_TYPE>
	void unsubscribe_method( OBJ_TYPE* obj, bool(OBJ_TYPE::* mcb)(ARGS...) ) {
		sub_t sub;
		sub.obj_id = obj;
		sub.func_id = *(void const**)&mcb;

		unsubscribe( sub );
	}

	void operator() ( ARGS const&... args ) {
		invoke( args... );
	}

private:
	void subscribe( sub_t const& sub ) {
		m_callbacks.push_back( sub );
	}
	void unsubscribe( sub_t const& sub ) {
		for( int i = 0; i < m_callbacks.size(); ++i ) {
			if( m_callbacks[i] == sub ) {
				m_callbacks.erase( m_callbacks.begin() + i );
				return;
			}
		}
	}
	std::vector<sub_t> m_callbacks;
};

