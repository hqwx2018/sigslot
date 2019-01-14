#pragma once

/*
自定义简单signal slot类
notes: 线程不安全
*/
#include <set>
#include <list>

namespace simplesignalslot
{
	//前置声明has_slot class
	class has_slots;
	//连接管理信息虚基类
	class _connection_base
	{
	public:
		virtual has_slots *getdest() const = 0;
		virtual void emit() = 0;
	};

	//信号的虚基类
	class _signal_base
	{
	public:
		virtual void slot_disconnect(has_slots* pslot) = 0;
	};


	//has_slots class
	//所有实现了槽函数的用户代码必须继承了该类
	class has_slots
	{
		typedef std::set<_signal_base *> sender_set;
		typedef sender_set::const_iterator const_iterator;
	public:
		has_slots(){}
		has_slots(const has_slots&){}
		virtual ~has_slots()
		{
			// 含有信号与槽的对象析构时，断开所有连接的信号与槽
			
		}
		//断开所有连接的信号与槽
		void disconnect_all()
		{
			const_iterator it = senders_.begin();
			const_iterator end = senders_.end();
			while(it!=end)
			{
				(*it)->slot_disconnect(this);
				++it;
			}
			senders_.clear();
		}

		//仅信号相关类回调 不允许用户调用
		void signal_connect(_signal_base *sender)
		{
			senders_.insert(sender);
		}

		//仅提供信号相关的类回调，不允许用户调用
		void signal_disconnect(_signal_base* sender)
		{
			senders_.erase(sender);
		}
	private:
		sender_set senders_;
	};

	// 封装了一个类和其成员函数的指针
	template <typename dest_type>
	class _connection : public _connection_base
	{
	private:
		// 泛型类指针
		dest_type *pobject_;

		// 泛型成员函数指针
		void (dest_type::*pmemfun_)();

	public:
		_connection(dest_type *pobject, void (dest_type::*pmemfun)())
		{
			pobject_ = pobject;
			pmemfun_ = pmemfun;
		}

		virtual has_slots *getdest() const
		{
			return pobject_;
		}

		virtual void emit()
		{
			(pobject_->*pmemfun_)();
		}
	};

	class signal : public _signal_base
	{
		typedef std::list<_connection_base *> connections_list;
		typedef connections_list::const_iterator const_iterator;

	protected:
		connections_list connected_slots_;

	public:
		signal()
		{ }

		// 信号对象销毁时自动断开所有槽函数的连接
		~signal()
		{
			disconnect_all();
		}

		// 当含有信号的对象被拷贝时只对信号进行初始化
		// 不拷贝信号和槽的绑定关系
		signal(const signal &)
		{ }

		void disconnect_all()
		{
			const_iterator it = connected_slots_.begin();
			const_iterator end = connected_slots_.end();
			while (it != end) {
				(*it)->getdest()->signal_disconnect(this);
				delete *it;
				++it;
			}
			connected_slots_.clear();
		}

		template <typename desttype>
		void connect(desttype *pclass, void (desttype::*pmemfun)())
		{
			_connection_base *conn = new _connection<desttype>(pclass, pmemfun);
			connected_slots_.push_back(conn);

			pclass->signal_connect(this);
		}

		void disconnect(has_slots *pclass)
		{
			const_iterator it = connected_slots_.begin();
			const_iterator end = connected_slots_.end();
			while (it != end) {
				if ((*it)->getdest() == pclass) {
					delete *it;
					connected_slots_.erase(it);
					pclass->signal_disconnect(this);
					return;
				}
				++it;
			}
		}

		void emit()
		{
			const_iterator it = connected_slots_.begin();
			const_iterator end = connected_slots_.end();
			while (it != end) {
				(*it)->emit();
				++it;
			}
		}

		void operator()()
		{
			emit();
		}

		void slot_disconnect(has_slots *pslot)
		{
			const_iterator it = connected_slots_.begin();
			const_iterator end = connected_slots_.end();
			while (it != end) {
				if ((*it)->getdest() == pslot) {
					connected_slots_.erase(it);
				}
				++it;
			}
		}

	};
}