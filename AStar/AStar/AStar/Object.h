#pragma once

class Object
{
private:
	POSITION m_pos;
	float m_gCost = 0; // 이동 거리 비용
	float m_hCost = 0; // 휴리스틱 비용 

public:
	Object(int y = 0, int x = 0) : m_pos{ y, x } {};

	float GetFCost() const { return m_gCost + m_hCost; }
	
	void SetPosition(int y, int x) { m_pos = { y, x }; }
	POSITION GetPosition() { return m_pos; }

	void SetGCost(float gcost) { m_gCost = gcost; }
	void SetHCost(float hcost) { m_hCost = hcost; }
	float GetGCost() const { return m_gCost; }
	float GetHCost() const { return m_hCost; }

	
	bool operator>(const Object& rhs) const {
		return GetFCost() > rhs.GetFCost();
	}
};

