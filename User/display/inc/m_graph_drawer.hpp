#pragma once



#include <algorithm>
#include <span>
#include <stdio.h>

#include "m_display_writer.hpp"
#include "m_font_writer.hpp"



namespace display
{
	class GraphDrawer
	{
		public:
			using DataArray = std::span<float>;



			GraphDrawer(DisplayWriter& writer, FontWriter& fontWriter)
				: _writer(writer), _fontWriter(fontWriter)
			{

			}

			void drawGraph(DataArray data)
			{
				_writer.addDrawAction([this, &data](){
					const int offset = 4;
					const int blockSize = 6;
					const int halfBlockSize = blockSize / 2;
					const int entryOffset = 16;
					const int workingHeight = _writer.getHeight() - offset * 2;
					const int startY = 4;
					const int startX = _writer.getWidth() - offset;
					const int MAX_SIZE = 6;
					// int workingWidth = _writer.getWidth() - offset * 2;

					// Y axis line
					_writer.drawLine(offset, offset, offset, _writer.getHeight() - offset);

					// X axis line
					_writer.drawLine(offset, _writer.getHeight() - offset, _writer.getWidth() - offset, _writer.getHeight() - offset);

					//need min and max values to calculate offsets
					float max = *std::max_element(data.begin(), data.begin() + std::min<int>(data.size(), MAX_SIZE));
					float min = *std::min_element(data.begin(), data.begin() + std::min<int>(data.size(), MAX_SIZE));
					float middle = (max + min) / 2;
					float range = max - min;

					char buff[16] = {0};
					sprintf(buff, "%d.%d", (int) max, (int) ((max - (int) max) * 100));

					_fontWriter.changeSize<display::FontWriter::FontSize::SMALL>();
					_fontWriter.drawStr(offset * 2.5, offset, buff);
					_writer.drawLine(offset / 2, startY, offset + offset / 2, startY);

					sprintf(buff, "%d.%d", (int) min, (int) ((min - (int) min) * 100));
					_fontWriter.drawStr(offset * 2.5, _writer.getHeight() - offset * 4, buff);

					sprintf(buff, "%d.%d", (int) middle, (int) ((middle - (int) middle) * 100));
					_fontWriter.drawStr(offset * 2.5, startY + workingHeight / 2 - 4, buff);
					_writer.drawLine(offset / 2, startY + workingHeight / 2, offset + offset / 2, startY + workingHeight / 2);
	
					float xPrev = 0.0f;
					float yPrev = 0.0f;

					for(int i = 0; i < std::min<int>(data.size(), MAX_SIZE); i++){
						float yPosition = (workingHeight - ((data[i] - min) / range) * workingHeight) + startY;
						float xPosition = startX - entryOffset * i;

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
				});

				_writer.flush();
			}


		
		private:
			DisplayWriter& _writer;
			FontWriter& _fontWriter;
	};
}