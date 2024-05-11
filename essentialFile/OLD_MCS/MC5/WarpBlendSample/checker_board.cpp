#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <assert.h>
#include "include\nvapi.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#define no_init_all

using namespace std;
using namespace cv;

int main(int argc, char **argv)
	{
		
		Mat img = imread("blended_screen_1.jpg");
		namedWindow("image", WINDOW_NORMAL);
		imshow("image", img);
		waitKey(0);
		destroyAllWindows();


		NvAPI_Status error;
		NvPhysicalGpuHandle nvgpuhandles[NVAPI_MAX_PHYSICAL_GPUS];

		NvU32 gpucount = 0;
		NvU32 gpu;
		NvU32 outputmask = 0;
		NvSBox desktoprect;
		NvSBox scanoutrect; //portion of the desktop
		NvSBox viewportrect; //the viewport which is a subregion of the scanout
		NvSBox osrect; //os coordinates of the desktop

		NV_SCANOUT_WARPING_DATA warpingdata;
		NvAPI_ShortString estring;
		int maxnumvertices = 0;
		int sticky = 0;
		NV_MOSAIC_TOPO_BRIEF  topo;

		printf("vwsdvcwvcwvcapp version: 1.3\n");

		// initialize nvapi, get gpu handles, etc.
		error = NvAPI_Initialize();
		ZeroMemory(&nvgpuhandles, sizeof(nvgpuhandles));
		error = NvAPI_EnumPhysicalGPUs(nvgpuhandles, &gpucount);

		NV_MOSAIC_GRID_TOPO* gridstopo = NULL;
		NV_MOSAIC_GRID_TOPO* gridstopo_1 = NULL;
		NvU32 gcount = 0;
		NvU32 gcount_1 = 0;
		NvAPI_Status status = NvAPI_Mosaic_EnumDisplayGrids(NULL, &gcount);

		topo.version = NVAPI_MOSAIC_TOPO_BRIEF_VER;
		NV_MOSAIC_DISPLAY_SETTING dispsetting;
		dispsetting.version = NVAPI_MOSAIC_DISPLAY_SETTING_VER;
		NvS32 overlapx, overlapy;
		float srcleft_1, srctop_1, srcwidth_1, srcheight_1;

		if (status == NVAPI_OK && gcount > 0)
		{
			gridstopo = (NV_MOSAIC_GRID_TOPO*)malloc(gcount * sizeof(NV_MOSAIC_GRID_TOPO));
			for (NvU32 i = 0; i < gcount; ++i)
			{
				gridstopo[i].version = NV_MOSAIC_GRID_TOPO_VER;
			}
			NvAPI_Status status = NvAPI_Mosaic_EnumDisplayGrids(gridstopo, &gcount);
			NvU32 numdisplays = gridstopo[1].displayCount; //no of displays in this grid
			NvU32 numrows = gridstopo[1].rows; //no of rows in this grid
			NvU32 numcols = gridstopo[1].columns; //no of columns in this grid
			printf("number of diaplays in this grid is = %lu \n ", numdisplays);
			printf("number of rows in this grid is= %lu \n", numrows);
			printf("number of cols in this grid is= %lu \n", numcols);

			// query the current mosaic topology
			error = NvAPI_Mosaic_GetCurrentTopo(&topo, &dispsetting, &overlapx, &overlapy);

			printf("over lap region of grid %d is %d :\n", 1, overlapx);

			NvU32* display_ids = NULL;

			display_ids = (NvU32*)malloc(2 * sizeof(NvU32));

			printf("size of the memory allocated is = %lu\n", sizeof(display_ids));

			/////////////////////for mosic grid overlap query////////////
			//demo over lapping of the projectors to check the display values and all
			printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
			printf("number of diaplys in grid is =%lu\n", gridstopo[1].displayCount);
			gridstopo[1].displays[0].overlapX = 0;
			gridstopo[1].displays[1].overlapX = 301;

			NvAPI_Status ret = NvAPI_Mosaic_SetDisplayGrids(gridstopo, gcount, NV_MOSAIC_SETDISPLAYTOPO_FLAG_CURRENT_GPU_TOPOLOGY);

			NvU32 enable = false;

			system("pause");
			Mat blend_1 = imread("blended_screen_1.jpg");
			Mat blend_2 = imread("blended_screen_2.jpg");
			Size sz = blend_1.size();
			Mat checker_board = imread("checker_board_25_25.jpg");
			Mat black = imread("black.jpg");
			

			int width = blend_1.cols;
			int height = blend_1.rows;
			int channels = blend_1.channels();

			cout << "image width: " << width << " pixels: " << endl;
			cout << "image height: " << height << " pixels: " << endl;
			cout << "channels: " << channels << " pixels: " << endl;
			printf("demo intensity adjustment\n");

			NV_SCANOUT_INTENSITY_DATA intensitydata;
			NV_SCANOUT_INTENSITY_DATA intensitydata_1;
			NV_SCANOUT_INTENSITY_DATA checker_board_data_1;
			NV_SCANOUT_INTENSITY_DATA checker_board_data_2;
			NV_SCANOUT_INTENSITY_DATA checker_board_height_1;
			NV_SCANOUT_INTENSITY_DATA checker_board_height_2;
			


			std::vector<float>intensity_data_1;
			std::vector<float>intensity_data_2;
			std::vector<float>offset_data_1;
			std::vector<float>offset_data_2;
			std::vector<float>checker_board_intensity_1;
			std::vector<float>checker_board_intensity_2;
			std::vector<float>checker_board_offset_1;
			std::vector<float>checker_board_offset_2;

			
			checker_board_intensity_1.reserve(height*width * 3);
			checker_board_intensity_2.reserve(height*width * 3);
			checker_board_offset_1.reserve(height*width * 3);
			checker_board_offset_2.reserve(height*width * 3);

			int num_square_x = 25;
			int num_square_y = 25;
			int square_height = height / num_square_y;
			int square_width = width / num_square_x;

			//float int_data_1;
			//float int_data_2;
			//float offset_data_1;
			//float offst_data_2;
			//for checker_board_values//

			float intensityTexture[6] = { 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f};
			float offsetTexture[2] = { 1.0f, 1.0f };

			intensitydata.version = NV_SCANOUT_INTENSITY_DATA_VER;
			intensitydata.width = 2;
			intensitydata.height = 1;
			intensitydata.blendingTexture = intensityTexture;
			intensitydata.offsetTexture = offsetTexture;
			intensitydata.offsetTexChannels = 1;
			//for weights to work make ofset as 0 and if we want to multiply the values by p
			//for checker board and some other image to disply on it properly make offset same as pixel value
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < width; ++x)
				{
					//
					//float pixelvalue = static_cast<float>(checker_board.at<uchar>(y, x)) ;
					//checker_board_intensity_1.push_back(pixelvalue);
					//

					for (int c = 2; c >=0; --c) {
						float pixelvalue = static_cast<float>(checker_board.at<cv::Vec3b>(y, x)[c]) / 255.0f;
						checker_board_intensity_1.push_back(pixelvalue);
						


					}
					float PV= static_cast<float>(checker_board.at<cv::Vec3b>(y, x)[0]) / 255.0f;
					//checker_board_offset_1.push_back(0.0f);
					checker_board_offset_1.push_back(PV);

				}
			}
			
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < width; ++x)
				{
					//
					//float pixelvalue = static_cast<float>(black.at<uchar>(y, x));
					//checker_board_intensity_2.push_back(pixelvalue);
					//

					for (int c = channels-1; c >=0; --c) {
						float pixelvalue = static_cast<float>(black.at<cv::Vec3b>(y, x)[c]) / 255.0f;
						checker_board_intensity_2.push_back(pixelvalue);
						

					}
					float PV = static_cast<float>(black.at<cv::Vec3b>(y, x)[0]) / 255.0f;
					//checker_board_offset_2.push_back(0.0f);
					checker_board_offset_2.push_back(PV);

				}
			}

			float* check_data_1 = checker_board_intensity_1.data();
			float* check_data_2 = checker_board_intensity_2.data();
			float* check_offset_data_1 = checker_board_offset_1.data();
			float* check_offset_data_2 = checker_board_offset_2.data();


			float data[25*25*3] = { 
				              1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f

			};

			float data_16[16* 9* 3] = {
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,

							  0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f,
							  0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f,

							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
                              
							  0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f,
							  0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f,

							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
                              
							  0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f,
							  0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f,

							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
                              
							  0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f,
							  0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f,

							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,






			};
			float data_blend[625] = { 
				                     1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 
				                     1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f,0.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f


			};

			float data_blend_16[16*9] = {
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,
			                         0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,
									 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,
									 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,
									 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,
			                         };

			float data_25[25 * 3] = {
				              1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 0.0f,0.0f,0.0f,
							  1.0f,1.0f,1.0f

			};

			float data_blen_25[25] = {
									 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,0.0f,
									 1.0f
			};

			printf("check_data_1:%d\n", sizeof(check_data_1));
			printf("check_offset_intensity_1:%d\n", sizeof(check_offset_data_1));
			printf("check_offset_intensity_2:%d\n", sizeof(check_offset_data_2));
			printf("check_data_2:%d\n", sizeof(check_data_2));



			float black_texture[3] = { 0.0f,0.0f,0.0f };
			float black_offsetTexture[1] = { 0.0f};

			float white_texture[3] = { 1.0f,1.0f,1.0f};
			float white_offsetTexture[1] = { 1.0f};

			checker_board_data_1.version = NV_SCANOUT_INTENSITY_DATA_VER;
			checker_board_data_1.width =  1920;
			checker_board_data_1.height = 1080;
			//checker_board_data_1.width = 25;
			//checker_board_data_1.height = 25;

			//checker_board_data_1.blendingTexture = data;
			//checker_board_data_1.offsetTexture = data_blend;

			checker_board_data_1.blendingTexture = check_data_1;
			checker_board_data_1.offsetTexture = check_offset_data_1;


			checker_board_data_1.offsetTexChannels = 1;

			checker_board_data_2.version = NV_SCANOUT_INTENSITY_DATA_VER;
			checker_board_data_2.width = 1920;
			checker_board_data_2.height = 1080;

			//checker_board_data_2.width = 25;
			//checker_board_data_2.height = 25;

			checker_board_data_2.blendingTexture = check_data_2;
			checker_board_data_2.offsetTexture = check_offset_data_2;

			//checker_board_data_2.blendingTexture = check_data_2;
			//checker_board_data_2.offsetTexture = check_offset_data_2;


			checker_board_data_2.offsetTexChannels = 1;
		
			checker_board_height_1.version = NV_SCANOUT_INTENSITY_DATA_VER;
			checker_board_height_1.width = 1;
			checker_board_height_1.height = 25;

			checker_board_height_1.blendingTexture = data;
			checker_board_height_1.offsetTexture = data_blend;
			checker_board_height_1.offsetTexChannels = 1;



			int stick_chess_board = 0;
			//uncooment only for the checker board pattern arrive on to the window application

			error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[0].displayId, &checker_board_data_1, &stick_chess_board);

			//error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[0].displayId, &checker_board_height_1, &stick_chess_board);
			error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[1].displayId, &checker_board_data_2, &stick_chess_board);

			system("pause");
			
			error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[0].displayId, &checker_board_data_2, &stick_chess_board);
			error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[1].displayId, &checker_board_data_1, &stick_chess_board);

			system("pause");
			error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[0].displayId, &checker_board_data_1, &stick_chess_board);
			error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[1].displayId, &checker_board_data_1, &stick_chess_board);
			system("pause");

			checker_board_data_1.blendingTexture = NULL;
			//checker_board_data_1.offsetTexChannels = NULL;

			checker_board_data_2.blendingTexture = NULL;
			//checker_board_data_2.offsetTexChannels = NULL;

			error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[0].displayId, &checker_board_data_2, &stick_chess_board);
			error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[1].displayId, &checker_board_data_1, &stick_chess_board);



		}
		
}

