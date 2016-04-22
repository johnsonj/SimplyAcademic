// Skyline.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <memory>
#include <array>
#include <functional>
#include <random>

namespace DataStructures
{
	using std::push_heap;
	using std::pop_heap;
	using std::make_heap;

	template<typename T>
	class ErasableHeap
	{
	public:
		ErasableHeap(std::function<bool(const T&, const T&)> compare) : m_compare(compare) {}
		void emplace(T val)
		{
			m_data.push_back(val);
			push_heap(m_data.begin(), m_data.end(), m_compare);
		}

		const T& max() const
		{
			return *m_data.begin();
		}

		bool empty() const
		{
			return m_data.empty();
		}

		void pop_max()
		{
			pop_heap(m_data.begin(), m_data.end(), m_compare);
		}

		void erase(T val)
		{
			auto val_iter = std::find(m_data.begin(), m_data.end(), val);
			assert(val_iter != m_data.end());
			m_data.erase(val_iter);
			// TODO: can we just reheapify underneath our deletion?
			make_heap(m_data.begin(), m_data.end(), m_compare);
		}

	private:
		std::vector<T> m_data;
		std::function<bool(const T&, const T&)> m_compare;
	};
}

namespace Skyline
{
	using std::vector;
	using std::sort;
	using std::find;
	using std::shared_ptr;
	using std::make_shared;
	using std::array;
	using std::cout;

	struct building
	{
		int left;
		int right;
		int height;
	};

	bool operator==(const building& a, const building& b)
	{
		return a.left == b.left && a.right == b.right && a.height == b.height;
	}

	struct inflection
	{
		int x;
		building* first_building;
		int height;
	};

	struct line
	{
		int start;
		int end;
		int height;
	};

	vector<building> static_data()
	{
		vector<building> v {
			{ 1, 5, 2 },
			{ 2, 3, 4 },
			{ 4, 8, 1 },
			{ 3, 6, 8 }
		};

		return v;
	}

	vector<building> generate_data(int width, int height, int density)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		const int board_area = width * height;
		const int count = (board_area / density) * std::generate_canonical<double, 1>(gen);

		std::uniform_int_distribution<int> width_dist(0, width / 2);
		std::uniform_int_distribution<int> offset_pos_dist(0, width / 2);
		std::normal_distribution<double> height_dist(0, height - 1);

		vector<building> buildings;
		for (int i = 0; i < count; ++i)
		{
			int bld_width = width_dist(gen);
			int bld_offset = offset_pos_dist(gen);
			int bld_height = height_dist(gen);
			buildings.emplace_back(building{ bld_offset, bld_width + bld_offset, bld_height });
		}

		return buildings;
	}

	bool building_start(const inflection& inf)
	{
		return inf.first_building->left == inf.x;
	}

	int tallest(const vector<building*>& buildings)
	{
		int height = 0;
		for (const auto& bld : buildings)
			height = std::max(height, bld->height);

		return height;
	}


	vector<line> find_heights(vector<building> buildings)
	{
		vector<inflection> inflections;
		// find all possible inflection points
		// O(n)
		for (auto& bld : buildings)
		{
			// Place the first and last point of each building as an inflection
			inflections.emplace_back(inflection{ bld.left, &bld, {} });
			inflections.emplace_back(inflection{ bld.right, &bld, {} });
		}

		// Sort all inflections by x
		// O( 2n log 2n) assuming a good sorting algorithm
		sort(inflections.begin(), inflections.end(),
			[](inflection& a, inflection& b) { return a.x < b.x; });

		// at each inflection, determine all of the buildings at that point
		// 2n - # of inflections
		// each time we need to remove an active building or recalc height: m number of overlapping buildings
		// O(2nlog(m)), m <= n
		vector<building*> active_buildings;
		int tallest_building = 0;
		std::function<bool(const building&, const building&)> compare_heights([](const building& a, const building& b) -> bool { return a.height < b.height; });
		DataStructures::ErasableHeap<building> active_bldgs(compare_heights);
		for (auto& infl : inflections)
		{
			if (building_start(infl))
				active_bldgs.emplace(*infl.first_building);
			else
				active_bldgs.erase(*infl.first_building);

			infl.height = active_bldgs.empty() ? 0 : active_bldgs.max().height;
		}

		// connect the higest inflections
		// o(n)
		vector<line> lines{ { 0, 0, 0 } };
		int current = 0;
		for (auto& infl : inflections)
		{
			// extend the pervious line to this point
			lines[current].end = infl.x - 1;

			// if the height has changed since last inflection we need a new line
			if (lines[current].height != infl.height)
			{
				current = lines.size();
				lines.emplace_back(line{ infl.x, infl.x, infl.height });
			}
		}

		return lines;
	}

	template<size_t size_x, size_t size_y>
	array<array<bool, size_x>, size_y> rasterize(vector<line> lines)
	{
		// zero out the arrays (output buffer if you will)
		array<array<bool, size_x>, size_y> output { false };

		// draw all of the (horizontal) lines
		for (const auto& line : lines)
		{
			//assert(line.height < size_y);
			for (int i = line.start; i <= line.end && i < size_x; ++i)
			{
				output[std::min(line.height, static_cast<int>(size_y) - 1)][i] = true;
			}
		}

		return output;
	}

	template<size_t size>
	void display_with_labels(array<array<bool, size>, size> renderedBitmap)
	{
		size_t current_line = size - 1;
		// from the top to the bottom
		for (auto line_itr = renderedBitmap.rbegin(); line_itr < renderedBitmap.rend(); ++line_itr)
		{
			std::cout << current_line-- << ": ";

			// from left to right
			for (auto& horiz_itr : *line_itr)
					std::cout << (horiz_itr ? "= " : ". ");

			std::cout << "\n";
		}

		std::cout << "   ";
		for (int i = 0; i < size; i++)
			std::cout << i << " ";

		// new line & flush the buffer
		std::cout << std::endl;
	}

	template<size_t size_x, size_t size_y>
	void display(array<array<bool, size_x>, size_y> renderedBitmap)
	{
		// from the top to the bottom
		for (auto line_itr = renderedBitmap.rbegin(); line_itr < renderedBitmap.rend(); ++line_itr)
		{
			// from left to right
			for (auto& horiz_itr : *line_itr)
				std::cout << (horiz_itr ? "=" : ".");

			std::cout << "\n";
		}
		// new line & flush the buffer
		std::cout << std::endl;
	}

	void main()
	{
		//auto lines = find_heights(static_data());

		const int width = 100;
		const int height = 25;
		int density = 100;
		auto lines = find_heights(generate_data(width, height, density));
		auto raster = rasterize<width, height>(lines);
		display(raster);
	}
}


int main()
{
	Skyline::main();
    return 0;
}

