#pragma once

/** Generic stack. Not sure why they called it matrix-stack. */
template <typename T, int SIZE> 
class zCMatrixStack {
    int		m_StackPosition;
    T		m_Stack[SIZE];
public:
    void Push(const T& m) 
	{
        m_Stack[m_StackPosition++] = m;
    }

    T Pop(void) 
	{
        return m_Stack[--m_StackPosition];
    }
};