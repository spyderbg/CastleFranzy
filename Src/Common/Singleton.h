///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

template <class T> 
class CSingleton
{
public:
	static T* GetInstance()
	{	
		static T instance;
		return &instance;
	}

protected:
	CSingleton(){}

private:
	CSingleton(const CSingleton&);
	CSingleton& operator = (const CSingleton&);
};

