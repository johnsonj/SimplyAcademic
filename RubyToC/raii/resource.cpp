#include<vector>
#include<iostream>

const int c_ErrorValue = -1;

template<typename ValueType>
class Database
{
public:
	ValueType GetValue(int pos)
	{
		return values[pos];
	}
	int AppendValue(ValueType val)
	{
		values.push_back(val);
		return values.size() - 1;
	}

private:
	std::vector<ValueType> values;
};

typedef Database<int> IntDb;

// FFI Interface
extern "C"
{

struct ManagedInstance
{
	void* pIntDb;
};

void* CreateIntDb() throw()
{
	try
	{
		return new IntDb;
	}
	catch(...)
	{
		return nullptr;
	}
}

void* CreateInstance() throw()
{
	ManagedInstance* mi = nullptr;
	try
	{
		mi = new ManagedInstance;
		mi->pIntDb = new IntDb;
		return mi;
	}
	catch(...)
	{
		if (mi)
			delete mi;

		return nullptr;
	}
}

bool DestroyIntDb(void* pIntDb) throw()
{
	std::cout << "Destroying instance" << std::endl;
	try
	{
		delete static_cast<IntDb*>(pIntDb);
		return true;
	}
	catch(...)
	{
		return false;
	}
}

int GetValue(void* pIntDb, int pos) throw()
{
	try
	{
		return static_cast<IntDb*>(pIntDb)->GetValue(pos);
	}
	catch(...)
	{
		return c_ErrorValue;
	}
}

int AppendValue(void* pIntDb, int val) throw()
{
	try
	{
		return static_cast<IntDb*>(pIntDb)->AppendValue(val);
	}
	catch(...)
	{
		return c_ErrorValue;
	}
}

}