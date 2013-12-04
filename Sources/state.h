#ifndef STATE_H
#define STATE_H

class State
{
protected:
	bool m_is_paused;
public:
	State(){m_is_paused = false;}
	virtual ~State(){};

	virtual bool init()=0;
	virtual bool run()=0;
	virtual bool exit()=0;

	virtual bool is_paused()
	{
		return m_is_paused;
	}

	virtual bool pause()
	{
		m_is_paused = true;
		return true;
	}

	virtual bool resume()
	{
		m_is_paused = false;
		return true;
	}
};

#endif // STATE_H
