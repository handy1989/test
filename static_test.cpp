#include <stdio.h>

class Base
{
public:
    Base() : m_data(0){}
    static Base* getInstance()
    {
        return m_instance;
    }
    inline void add()
    {
        ++m_data;
    }
    inline void dec()
    {
        --m_data;
    }
    inline int get()
    {
        return m_data;
    }
    void test();
private:
    int m_data;
    static Base* m_instance;
};

void Base::test()
{
    printf("m_data:%d m_instance data:%d\n", m_data, m_instance->m_data);
    m_instance->add();
    printf("m_data:%d m_instance data:%d\n", m_data, m_instance->m_data);
}

Base* Base::m_instance = new Base();
int main()
{
    Base *p = new Base();
    p->test();
    return 0;
}
