#include "MemeField.h"
#include <assert.h>
#include <random>
#include "Vei2.h"
#include "SpriteCodex.h"
#include <algorithm>


MemeField::MemeField(int nMemes)
{
	assert(nMemes > 0 && nMemes < width*height);
	std::random_device rd;
	std::mt19937 rng(rd());  //generate a random 32 bit number
	std::uniform_int_distribution<int> xDist(0, width - 1);
	std::uniform_int_distribution<int> yDist(0, height - 1);

	for (int nSpawned = 0; nSpawned < nMemes; ++nSpawned) {
		Vei2 spawnPos;
		do {
			spawnPos = { xDist(rng), yDist(rng) };
		} while (TileAt(spawnPos).HasMeme());

		TileAt(spawnPos).SpawnMine();
	}

	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++) {
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++) {
			TileAt(gridPos).SetNeighborMemeCount(countNeighborMemes(gridPos));
		}
	}


}

void MemeField::Draw(Graphics & gfx) const
{
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++) {
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++) {
			TileAt(gridPos).Draw(gridPos*SpriteCodex::tileSize, isFucked, gfx);
		}
	}

}

RectI MemeField::GetRect() const
{
	return RectI(0, width*SpriteCodex::tileSize, 0, height*SpriteCodex::tileSize);
}

void MemeField::OnRevealClick(const Vei2 & screenPos)
{
	if (!isFucked)
	{
		const Vei2 & gridPos = MemeField::Screen2Grid(screenPos);
		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed() && !tile.IsFlagged()) {
			tile.Reveal();
			if (tile.HasMeme())
			{
				isFucked = true;
			}
		}
	}
}

void MemeField::OnFlagClick(const Vei2 & screenPos)
{
	if (!isFucked)
	{
		const Vei2 & gridPos = MemeField::Screen2Grid(screenPos);
		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed()) {
			tile.ToggleFlag();
		}
	}
}


Vei2 MemeField::Screen2Grid(const Vei2 & screenPos)
{
	return screenPos / SpriteCodex::tileSize;
}

int MemeField::countNeighborMemes(const Vei2 & gridPos)
{
	const int xStart = std::max(0, gridPos.x - 1);
	const int yStart = std::max(0, gridPos.y - 1);
	const int xEnd = std::min(width - 1, gridPos.x + 1);
	const int yEnd = std::min(height - 1, gridPos.y + 1);

	int count = 0;
	for (Vei2 gridPos = { xStart, yStart }; gridPos.y <= yEnd; gridPos.y++)
	{
		for (gridPos.x = xStart; gridPos.x <= xEnd; gridPos.x++)
		{
			if (TileAt(gridPos).HasMeme())
			{
				count++;
			}
		}
	}
	return count;
}


MemeField::~MemeField()
{
}

MemeField::Tile& MemeField::TileAt(const Vei2 & gridPos)
{
	return field[gridPos.y*width + gridPos.x];
}

const MemeField::Tile & MemeField::TileAt(const Vei2 & gridPos) const
{
	return field[gridPos.y*width + gridPos.x];
}

void MemeField::Tile::SpawnMine()
{
	assert(!hasMeme);
	hasMeme = true;
}

bool MemeField::Tile::HasMeme() const
{
	return hasMeme;
}

void MemeField::Tile::Draw(const Vei2 & screenPos, bool fucked, Graphics & gfx) const
{
	if (!fucked) {
		switch (state) {
		case State::Flagged:
			SpriteCodex::DrawTileButton(screenPos, gfx);
			SpriteCodex::DrawTileFlag(screenPos, gfx);
			break;
		case State::Hidden:
			SpriteCodex::DrawTileButton(screenPos, gfx);
			break;
		case State::Revealed:
			if (!HasMeme()) {
				SpriteCodex::DrawTileNumber(screenPos, nNeighborMemes, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			break;
		}
	}
	else
	{
		switch (state) {
		case State::Flagged:
			if(HasMeme())
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileFlag(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(screenPos, gfx);
				SpriteCodex::DrawTileCross(screenPos, gfx);
			}
			break;
		case State::Hidden:
			if (!HasMeme())
			{
				SpriteCodex::DrawTileNumber(screenPos, nNeighborMemes, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBombRed(screenPos, gfx);
			}
			break;
		case State::Revealed:
			if (!HasMeme()) {
				SpriteCodex::DrawTileNumber(screenPos, nNeighborMemes, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			break;
		}
	}
}

void MemeField::Tile::Reveal()
{
	assert(State::Hidden == state);
	state = State::Revealed;
}

bool MemeField::Tile::IsRevealed() const
{
	return state == State::Revealed;
}

void MemeField::Tile::ToggleFlag()
{
	assert(!IsRevealed());
	if (State::Flagged == state)
	{
		state = State::Hidden;
	}
	else
	{
		state = State::Flagged;
	}
}

bool MemeField::Tile::IsFlagged() const
{
	return state == State::Flagged;
}

void MemeField::Tile::SetNeighborMemeCount(int memeCount)
{
	assert(nNeighborMemes == -1); //uninitialized yet
	nNeighborMemes = memeCount;
}
