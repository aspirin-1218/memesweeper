#pragma once
#include "Graphics.h"
#include "RectI.h"
class MemeField
{
private:
	class Tile
	{
	public:
		enum class State
		{
			Hidden,
			Flagged,
			Revealed
		};

		void SpawnMine();
		bool HasMeme() const;
		void Draw(const Vei2 & screenPos, bool fucked, Graphics & gfx) const;
		void Reveal();
		bool IsRevealed() const;
		void ToggleFlag();
		bool IsFlagged() const;
		void SetNeighborMemeCount(int memeCount);
	private:
		State state = State::Hidden;
		bool hasMeme = false;
		int nNeighborMemes = -1;
	};
public:
	MemeField( int nMines);
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
	void OnRevealClick(const Vei2& screenPos);
	void OnFlagClick(const Vei2& screenPos);
	Vei2 Screen2Grid(const Vei2& screenPos);
	int countNeighborMemes(const Vei2& gridPos);
	~MemeField();
private:
	Tile& TileAt(const Vei2 &gridPos);
	const Tile& TileAt(const Vei2 &gridPos) const;
private:
	static constexpr int width = 20; //width shall be initialized as constant at compile-time.
	static constexpr int height = 16;
	Tile field[width*height];
	bool isFucked = false;
};

