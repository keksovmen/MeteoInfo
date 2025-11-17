#pragma once



#include <algorithm>
#include <span>
#include <stdio.h>
#include <string_view>

#include "m_display_writer.hpp"
#include "m_font_writer.hpp"



namespace display
{
	template<DisplayBasic D>
	class GraphDrawer
	{
		public:
			using DataArray = std::span<int32_t>;



			GraphDrawer(D& writer, FontWriter<D>& fontWriter)
				: _writer(writer), _fontWriter(fontWriter)
			{

			}

			void drawGraph(DataArray data)
			{
				_writer.addDrawAction([this, &data](){
					// define space between left side and Y axis and top space for Y axis
					const int offset = 4;
					// define space between bottom of the screen and X axis line
					const int offsetY = _fontWriter.template getFontHeight<FontWriter<D>::FontSize::SMALL>() + 1;
					// define size of dot
					const int blockSize = 6;
					const int halfBlockSize = blockSize / 2;
					// space unit of X
					const int entryOffset = 16;
					// 
					const int workingHeight = _writer.getHeight() - (offset + offsetY);
					// start positions
					const int startY = offset;
					const int startX = _writer.getWidth() - offset;
					// max displayable units at the same time
					const int MAX_SIZE = 6;

					// Y axis line
					_writer.drawLine(offset, offset, offset, _writer.getHeight() - offsetY);

					// X axis line
					_writer.drawLine(offset, _writer.getHeight() - offsetY, _writer.getWidth() - offset, _writer.getHeight() - offsetY);

					//need min and max values to calculate offsets
					int32_t max = *std::max_element(data.begin(), data.begin() + std::min<int>(data.size(), MAX_SIZE));
					int32_t min = *std::min_element(data.begin(), data.begin() + std::min<int>(data.size(), MAX_SIZE));

					int32_t middle = (max + min) / 2;
					int32_t range = max - min;
					range = (range == 0) ? 1 : range;

					char buff[16] = {0};
					sprintf(buff, "%d.%d", max / 100, abs(max % 100));

					_fontWriter.template changeSize<display::FontWriter<D>::FontSize::SMALL>();
					_fontWriter.drawStr(offset * 2.5, offset, buff);
					_writer.drawLine(offset / 2, startY, offset + offset / 2, startY);

					sprintf(buff, "%d.%d", min / 100, abs(min % 100));
					_fontWriter.drawStr(offset * 2.5, _writer.getHeight() - offsetY - 10, buff);

					sprintf(buff, "%d.%d", middle / 100, abs(middle % 100));
					_fontWriter.drawStr(offset * 2.5, startY + workingHeight / 2 - 4, buff);
					_writer.drawLine(offset / 2, startY + workingHeight / 2, offset + offset / 2, startY + workingHeight / 2);
	
					int xPrev = 0.0f;
					int yPrev = 0.0f;

					for(int i = 0; i < std::min<int>(data.size(), MAX_SIZE); i++){
						int yPosition = workingHeight - ((data[i] - min) * workingHeight / range) + startY;
						int xPosition = startX - entryOffset * i;

						_writer.drawRectangle(xPosition - halfBlockSize, yPosition - halfBlockSize, blockSize, blockSize);
						if(i == 0){
							xPrev = xPosition;
							yPrev = yPosition;
							continue;
						}
						//draw line from prev to current
						_writer.drawLine(xPrev, yPrev, xPosition, yPosition);
						xPrev = xPosition;
						yPrev = yPosition;
					}

					int beginingX = (_writer.getWidth() - _fontWriter.template calcLength<FontWriter<D>::FontSize::SMALL>(_label)) / 2;
					_fontWriter.drawStr(beginingX, _writer.getHeight() - (_fontWriter.template getFontHeight<FontWriter<D>::FontSize::SMALL>()), _label);
				});

				_writer.flush();
			}

			void setLabel(std::string_view label)
			{
				_label = label;
			}


		
		private:
			D& _writer;
			FontWriter<D>& _fontWriter;
			std::string_view _label = {};
	};
}