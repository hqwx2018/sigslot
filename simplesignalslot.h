#pragma once

/*
�Զ����signal slot��
notes: �̲߳���ȫ
*/
#include <set>
#include <list>

namespace simplesignalslot
{
	//ǰ������has_slot class
	class has_slots;
	//���ӹ�����Ϣ�����
	class _connection_base
	{
	public:
		virtual has_slots *getdest() const = 0;
		virtual void emit() = 0;
	};

	//�źŵ������
	class _signal_base
	{
	public:
		virtual void slot_disconnect(has_slots* pslot) = 0;
	};


	//has_slots class
	//����ʵ���˲ۺ������û��������̳��˸���
	class has_slots
	{
		typedef std::set<_signal_base *> sender_set;
		typedef sender_set::const_iterator const_iterator;
	public:
		has_slots(){}
		has_slots(const has_slots&){}
		virtual ~has_slots()
		{
			// �����ź���۵Ķ�������ʱ���Ͽ��������ӵ��ź����
			
		}
		//�Ͽ��������ӵ��ź����
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

		//���ź������ص� �������û�����
		void signal_connect(_signal_base *sender)
		{
			senders_.insert(sender);
		}

		//���ṩ�ź���ص���ص����������û�����
		void signal_disconnect(_signal_base* sender)
		{
			senders_.erase(sender);
		}
	private:
		sender_set senders_;
	};

	// ��װ��һ��������Ա������ָ��
	template <typename dest_type>
	class _connection : public _connection_base
	{
	private:
		// ������ָ��
		dest_type *pobject_;

		// ���ͳ�Ա����ָ��
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

		// �źŶ�������ʱ�Զ��Ͽ����вۺ���������
		~signal()
		{
			disconnect_all();
		}

		// �������źŵĶ��󱻿���ʱֻ���źŽ��г�ʼ��
		// �������źźͲ۵İ󶨹�ϵ
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