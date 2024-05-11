
//
// File:           WarpBlendSample.cpp
// Description:    Sample implementation of Nvidia warp and blend API
// Target OS:      Windows 7 only
// Notes:          Now available in the public version of NVAPI
//

//Version 1.2 Changelog
//- Uses displayIds instead of outputIds (deprecated)
//- Removed mosaic calls to get gisplay handle. Replace with NvAPI_DISP_GetDisplayHandleFromdisplayId instead
//- Added NvAPI_GPU_GetScanoutConfigurationEx for the case when viewport rect!=display rect

//Version 1.3 Changelog
//- Added example of Warp and Blend Antialiasing - requires R352 or later
//- Added 64bit config

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




/*
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
		if (status == NVAPI_OK)
		{
			for (NvU32 igrid = 0; igrid < gcount; igrid++)
			{
				NvU32 numdisplays = gridstopo[igrid].displayCount; //no of displays in this grid
				NvU32 numrows = gridstopo[igrid].rows; //no of rows in this grid
				NvU32 numcols = gridstopo[igrid].columns; //no of columns in this grid
				printf("number of diaplays in this grid is = %lu \n ", numdisplays);
				printf("number of rows in this grid is= %lu \n", numrows);
				printf("number of cols in this grid is= %lu \n", numcols);

				// query the current mosaic topology
				error = NvAPI_Mosaic_GetCurrentTopo(&topo, &dispsetting, &overlapx, &overlapy);

				printf("over lap region of grid %d is %d :\n", igrid, overlapx);
			}
		}

		NvU32* display_ids = NULL;

		display_ids = (NvU32*)malloc(2 * sizeof(NvU32));

		printf("size of the memory allocated is = %lu\n", sizeof(display_ids));

			///////////////////////////////////////////////////////////////////////////////////for mosic grid overlap query////////////




			///////////////////////////////////////////////////////////////////////////////////

			//demo over lapping of the projectors to check the display values and all
		printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
		printf("number of diaplys in grid is =%lu\n", gridstopo[1].displayCount);
		gridstopo[1].displays[0].overlapX = 0;
		gridstopo[1].displays[1].overlapX = 0;

		NvAPI_Status ret = NvAPI_Mosaic_SetDisplayGrids(gridstopo, gcount, NV_MOSAIC_SETDISPLAYTOPO_FLAG_CURRENT_GPU_TOPOLOGY);

		NvU32 enable = false;
		system("pause");

			// at this point we have a list of accessible physical nvidia gpus in the system.
			// loop over all gpus

			for (gpu = 0; gpu < gpucount; gpu++)
			{


				
				error = NvAPI_Mosaic_GetCurrentTopo(&topo, &dispsetting, &overlapx, &overlapy);

				if (error != NVAPI_OK)
				{
					NvAPI_GetErrorMessage(error, estring);
					printf("nvapi_gpu_getcurrenttopo: %s\n", estring);
				}
				


				NvU32 dispidcount = 0;

				// query the active physical display connected to each gpu.
				error = NvAPI_GPU_GetConnectedDisplayIds(nvgpuhandles[gpu], NULL, &dispidcount, 0);
				if ((error != NVAPI_OK) || (dispidcount == 0))
				{
					NvAPI_GetErrorMessage(error, estring);
					printf("NvAPI_GPU_GetConnectedDisplayIds: %s\n", estring);
					printf("display count %d\n", dispidcount);
					return error;
				}

				NV_GPU_DISPLAYIDS* dispids = NULL;
				dispids = new NV_GPU_DISPLAYIDS[dispidcount];
				dispids->version = NV_GPU_DISPLAYIDS_VER;
				error = NvAPI_GPU_GetConnectedDisplayIds(nvgpuhandles[gpu], dispids, &dispidcount, 0);
				if (error != NVAPI_OK)
				{
					delete[] dispids;
					NvAPI_GetErrorMessage(error, estring);
					printf("NvAPI_GPU_GetConnectedDisplayIds: %s\n", estring);
				}


				for (int dispindex = 0; (dispindex < dispidcount) && dispids[dispindex].isActive; dispindex++)
				{

					ZeroMemory(&desktoprect, sizeof(desktoprect));
					ZeroMemory(&scanoutrect, sizeof(scanoutrect));
					ZeroMemory(&osrect, sizeof(osrect));
					ZeroMemory(&viewportrect, sizeof(viewportrect));
					printf("gpu %d, displayid 0x%08x\n", gpu, dispids[dispindex].displayId);

					// query the desktop and scanout portion of each physical active display.
					error = NvAPI_GPU_GetScanoutConfiguration(dispids[dispindex].displayId, &desktoprect, &scanoutrect);
					if (error != NVAPI_OK)
					{
						NvAPI_GetErrorMessage(error, estring);
						printf("NvAPI_GPU_GetScanoutConfiguration: %s\n", estring);
					}


					//the below is optional for r331+ in cases where the viewport rect!=scanoutrect
					NV_SCANOUT_INFORMATION scanoutinformation;

					scanoutinformation.version = NV_SCANOUT_INFORMATION_VER;
					error = NvAPI_GPU_GetScanoutConfigurationEx(dispids[dispindex].displayId, &scanoutinformation);
					//if this new interface is supported fetch scanout data from it
					if (error == NVAPI_OK)
					{
						scanoutrect.sWidth = scanoutinformation.targetDisplayWidth;
						scanoutrect.sHeight = scanoutinformation.targetDisplayHeight;
						viewportrect = scanoutinformation.targetViewportRect;
					}
					if (topo.enabled == false)
					{
						// extended mode
						// warp texture coordinates are defined in desktoprect coordinates
						printf("mosaic is disabled\n");
						srcleft_1 = (float)scanoutinformation.sourceDesktopRect.sX;
						srctop_1 = (float)scanoutinformation.sourceDesktopRect.sY;
						srcwidth_1 = (float)scanoutinformation.sourceDesktopRect.sWidth;
						srcheight_1 = (float)scanoutinformation.sourceDesktopRect.sHeight;
					}
					else
					{
						// mosaic -- we only want the pixels under the physical display here
						printf("mosaic is enabled\n");
						srcleft_1 = (float)scanoutinformation.sourceViewportRect.sX;
						srctop_1 = (float)scanoutinformation.sourceViewportRect.sY;
						srcwidth_1 = (float)scanoutinformation.sourceViewportRect.sWidth;
						srcheight_1 = (float)scanoutinformation.sourceViewportRect.sHeight;
					}
					//remove later
					//printf("srcleft_1:%f\n", srcleft_1);
					//printf("srctop_1:%f\n", srctop_1);
					//printf("srcwidth_1:%f\n", srcwidth_1);
					//printf("srcheight_1:%f\n", srcheight_1);
					
					//need to get osrect for this we need the nvdisplayhandle to get access to the displayname to be pased to win32
					NvDisplayHandle disp = NULL;
					NvAPI_DISP_GetDisplayHandleFromDisplayId(dispids[dispindex].displayId, &disp);
					//todo errhandle
					NvAPI_ShortString displayname;
					error = NvAPI_GetAssociatedNvidiaDisplayName(disp, displayname);
					//todo errhandle

					DEVMODEA dm = { 0 };
					dm.dmSize = sizeof(DEVMODEA);
					if (!EnumDisplaySettingsA(displayname, ENUM_CURRENT_SETTINGS, &dm)) {
						//todo handle error
					}

					osrect.sX = dm.dmPosition.x;
					osrect.sY = dm.dmPosition.y;
					osrect.sWidth = dm.dmPelsWidth;
					osrect.sHeight = dm.dmPelsHeight;
					//remove later
					//printf(" desktoprect: sx = %6d, sy = %6d, swidth = %6d sheight = %6d\n", desktoprect.sx, desktoprect.sy, desktoprect.swidth, desktoprect.sheight);
					//printf(" scanoutrect: sx = %6d, sy = %6d, swidth = %6d sheight = %6d\n", scanoutrect.sx, scanoutrect.sy, scanoutrect.swidth, scanoutrect.sheight);
					//printf(" viewportrect: sx = %6d, sy = %6d, swidth = %6d sheight = %6d\n", viewportrect.sx, viewportrect.sy, viewportrect.swidth, viewportrect.sheight);
					//printf(" osrect: sx = %6d, sy = %6d, swidth = %6d sheight = %6d\n", osrect.sx, osrect.sy, osrect.swidth, osrect.sheight);
					

					// desktoprect should now contain the area of the desktop which is scanned
					// out to the display given by the displayid.
					// scanoutrect should now contain the area of the scanout to which the 
					// desktoprect is scanned out. that will give information about the gpu 
					// internal output scaling before applying the warping and intensity control
					// however that doesn't give information about the scaling performed on 
					// the display side.

					// with this information the intensity and warping coordinates can be 
					// computed. the example here warps the desktoprect to a trapezoid similar
					// to the one seen in the diagram.	


					// warp vertices are defined in scanoutrect coordinates    					
					float	dstwidth = scanoutrect.sWidth / 2.0f;
					float	dstheight = scanoutrect.sHeight;
					float	dstxshift = dstwidth / 2.0f;
					float	dstyshift = dstheight / 2.0f;
					float	dstleft = (float)scanoutrect.sX + dstxshift;
					float	dsttop = (float)scanoutrect.sY; //todo play

					// triangle strip with 4 vertices
					// the vertices are given as a 2d vertex strip because the warp
					// is a 2d operation. to be able to emulate 3d perspective correction, 
					// the texture coordinate contains 4 components, which needs to be
					// adjusted to get this correction.
					//
					// a trapezoid needs 4 vertices.
					// format is xy for the vertex + yurq for the texture coordinate.
					// so we need 24 floats for that.
					//float vertices [4*6];
					//
					//  (0)  ----------------  (2)
					//       |             / |
					//       |            /  |
					//       |           /   |
					//       |          /    |
					//       |         /     |
					//       |        /      |
					//       |       /       |
					//       |      /        | 
					//       |     /         |
					//       |    /          |
					//       |   /           |
					//       |  /            |
					//       | /             |  
					//   (1) |---------------- (3)
					//


					// texture coordinates
					// warp texture coordinates are defined in desktoprect coordinates
					//
					//float srcleft = (float)desktoprect.sx;
					//float srctop = (float)desktoprect.sy;
					//float srcwidth = desktoprect.swidth;
					//float srcheight = desktoprect.sheight;

					//float vertices[] =
					//	//     x                        y                       u                           v                   r       q
					//{

					//	dstleft,				         dsttop,			    srcleft,					srctop,				0.0f,	 1.0f,     // 0
					//	dstleft - dstxshift,             dsttop + dstheight,	srcleft,					srctop + srcheight,	0.0f,	 0.5f,     // 1
					//	dstleft + dstwidth + dstxshift,	 dsttop,			    srcleft + srcwidth,			srctop,				0.0f,	 1.0f,      // 2
					//	dstleft + dstwidth,	             dsttop + dstheight,	srcleft + srcwidth,			srctop + srcheight,	0.0f,	 1.5f,     // 3
					//};
					//int maxnumvert = 4;
					////tl,bl,tr,br
					//float vertices_1[] =
					//	//     x                        y                  u                           v                   r       q
					//{

					//	80,	                     258,			(srcleft_1)*1.0,					    (srctop_1)*1.0,			           0.0f,	 1.0*1.0f,     // 0
					//	76,                      1021,	        (srcleft_1)*1.0,					    (srctop_1 + srcheight_1)*1.0,	   0.0f,	 1.0*1.0f,     // 1
					//	1919,	                 408,			(srcleft_1 + srcwidth_1)*1.0,			(srctop_1)*1.0,				       0.0f,	 1.0*1.0f,      // 2
					//	1919,	                 1079,	        (srcleft_1 + srcwidth_1)*1.0,			(srctop_1 + srcheight_1)*1.0,	   0.0f,	 1.1*1.0f,     // 3
					//};

					//float vertices_2[] =
					//	//     x                        y                  u                           v                   r       q
					//{

					//	40,	               193,			        srcleft_1,					srctop_1,				0.0f,	 1.0*1.0f,     // 0
					//	45,                983,	                srcleft_1,					srctop_1 + srcheight_1,	0.0f,	 1.0*1.0f,     // 1
					//	1813,	           81,			        srcleft_1 + srcwidth_1,		srctop_1,				0.0f,	 1.0*1.0f,      // 2
					//	1831,	           955,	                srcleft_1 + srcwidth_1,		srctop_1 + srcheight_1,	0.0f,	 1.0*1.0f,     // 3
					//};

					//float vertices_2_og[] =
					//	//     x                        y                  u                           v                   r       q
					//{

					//	0,	                     394,			    1920,					0,				0.0f,	 1.0f,     // 0
					//	0,                       1045,	            1920,					1080,	        0.0f,	 1.0f,     // 1
					//	1906,	                 47,			    3840,			        0,				0.0f,	 1.0f,      // 4
					//	1906,	                 945,	            3840,			        0,	           0.0f,	 1.0f,     // 3
					//};

					//printf("vertices: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_2[0], vertices_2[1], vertices_2[2], vertices_2[3], vertices_2[4], vertices_2[5]);
					//printf("vertices: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_2[6], vertices_2[7], vertices_2[8], vertices_2[9], vertices_2[10], vertices_2[11]);
					//printf("vertices: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_2[12], vertices_2[13], vertices_2[14], vertices_2[15], vertices_2[16], vertices_2[17]);
					//printf("vertices: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_2[18], vertices_2[19], vertices_2[20], vertices_2[21], vertices_2[22], vertices_2[23]);

					//printf("vertices: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_1[0], vertices_1[1], vertices_1[2], vertices_1[3], vertices_1[4], vertices_1[5]);
					//printf("vertices: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_1[6], vertices_1[7], vertices_1[8], vertices_1[9], vertices_1[10], vertices_1[11]);
					//printf("vertices: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_1[12], vertices_1[13], vertices_1[14], vertices_1[15], vertices_1[16], vertices_1[17]);
					//printf("vertices: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n", vertices_1[18], vertices_1[19], vertices_1[20], vertices_1[21], vertices_1[22], vertices_1[23]);
					////	printf("vertices: %6.0f, %6.0f, %6.0f, %6.0f, %6.0f, %6.0f\n",vertices[24],vertices[25],vertices[26],vertices[27],vertices[28],vertices[29]);

					//// demo warping
					//printf("demo warping\n");
					//warpingdata.version = nv_scanout_warping_ver;
					//warpingdata.numvertices = maxnumvert;
					//warpingdata.vertexformat = nv_gpu_warping_vertice_format_trianglestrip_xyuvrq;
					//warpingdata.texturerect = &osrect;

					//if (dispids[dispindex].displayid == 0x80061081)
					//{
					//	warpingdata.vertices = vertices_1;
					//}
					//else if (dispids[dispindex].displayid == 0x80061083)
					//{
					//	warpingdata.vertices = vertices_2;
					//}
					//else
					//{
					//	warpingdata.vertices = NULL;
					//}

					////warpingdata.vertices = vertices;

					//// this call does the warp
					//error = nvapi_gpu_setscanoutwarping(dispids[dispindex].displayid, &warpingdata, &maxnumvertices, &sticky);
					//if (error != NVAPI_OK)
					//{
					//	NvAPI_GetErrorMessage(error, estring);
					//	printf("nvapi_gpu_setscanoutwarping: %s\n", estring);
					//}
					
                    //remove later
					//system("pause");

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
					   
					std::vector<float>intensity_data_1;
					std::vector<float>intensity_data_2;
					std::vector<float>offset_data_1;
					std::vector<float>offset_data_2;

					std::vector<float>checker_board_intensity_1;
					std::vector<float>checker_board_intensity_2;
					checker_board_intensity_1.reserve(height*width*3);
					checker_board_intensity_2.reserve(height*width*3);

					int num_square_x = 25;
					int num_square_y = 25;
					int square_height = height / num_square_y;
					int square_width = width / num_square_x;
					
					//float int_data_1;
					//float int_data_2;
					//float offset_data_1;
					//float offst_data_2;
					//for checker_board_values//

					for (int y = 0; y < height; ++y) {
						for (int x = 0; x < width; ++x)
						{
							//
							//float pixelvalue = static_cast<float>(checker_board.at<uchar>(y, x)) ;
							//checker_board_intensity_1.push_back(pixelvalue);
							//
							
							for (int c = 0; c < channels; ++c) {
								float pixelvalue = static_cast<float>(checker_board.at<cv::Vec3b>(y, x)[c])/255.0f;
								checker_board_intensity_1.push_back(pixelvalue);

							}
							
						}
					}

					for (int y = 0; y < height; ++y) {
						for (int x = 0; x < width; ++x)
						{
							//
							//float pixelvalue = static_cast<float>(black.at<uchar>(y, x));
							//checker_board_intensity_2.push_back(pixelvalue);
							//
							
							for (int c = 0; c < channels; ++c) {
								float pixelvalue = static_cast<float>(black.at<cv::Vec3b>(y, x)[c])/255.0f;
								checker_board_intensity_2.push_back(pixelvalue);

							}
							
						}
					}

					float* check_data_1 = checker_board_intensity_1.data();
					float* check_data_2 = checker_board_intensity_2.data();

					printf("check_data_1:\n",size(checker_board_intensity_1));



					printf("check_data_2:\n", size(checker_board_intensity_2));

					checker_board_data_1.version = NV_SCANOUT_INTENSITY_DATA_VER;
					checker_board_data_1.width = 1920;
					checker_board_data_1.height = 1080;

					checker_board_data_1.blendingTexture = check_data_1;
					checker_board_data_1.offsetTexture = NULL;
					checker_board_data_1.offsetTexChannels = 1;

					checker_board_data_2.version = NV_SCANOUT_INTENSITY_DATA_VER;
					checker_board_data_2.width = 1920;
					checker_board_data_2.height = 1080;

					checker_board_data_2.blendingTexture = check_data_2;
					checker_board_data_2.offsetTexture = NULL;
					checker_board_data_2.offsetTexChannels = 1;

					int stick_chess_board=0;
					//uncooment only for the checker board pattern arrive on to the window application
					
					error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[0].displayId, &checker_board_data_1, &stick_chess_board);
					error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[1].displayId, &checker_board_data_2, &stick_chess_board);

					system("pause");
					error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[0].displayId, &checker_board_data_2, &stick_chess_board);
					error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[1].displayId, &checker_board_data_1, &stick_chess_board);

					system("pause");
					checker_board_data_1.blendingTexture = NULL;
					checker_board_data_2.blendingTexture = NULL;
					error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[0].displayId, &checker_board_data_2, &stick_chess_board);
					error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[1].displayId, &checker_board_data_1, &stick_chess_board);

					//uncomment for normal thing
					
					//
					//intensity_data_1.reserve(height*width*channels);
					//intensity_data_2.reserve(height*width*channels);
					//offset_data_1.reserve(height*width*channels);
					//offset_data_2.reserve(height*width*channels);


					//for (int y = 0; y < height; ++y) {
					//	for (int x = 0; x < width; ++x) {
					//		for (int c = 0; c < channels; ++c) {
					//			float pixelvalue = static_cast<float>(blend_1.at<cv::Vec3b>(y, x)[c]) / 255.0f;

					//			intensity_data_1.push_back(pixelvalue);
					//			offset_data_1.push_back(1.0f - pixelvalue);

					//		}
					//	}
					//}

					//for (int y = 0; y < height; ++y) {
					//	for (int x = 0; x < width; ++x) {
					//		for (int c = 0; c < channels; ++c) {
					//			float pixelvalue = static_cast<float>(blend_2.at<cv::Vec3b>(y, x)[c]) / 255.0f;
					//			intensity_data_2.push_back(pixelvalue);
					//			offset_data_2.push_back(1.0f - pixelvalue);

					//		}
					//	}
					//}

     //               ///for threee channelssss/////
					//

					////float intensity_data_1[1920 * 1080 * 3] = { 0.0f };
					////float intensity_data_2[1920 * 1080 * 3] = { 0.0f };

					//
					////float offset_data_1[1920 * 1080 * 3] = { 0.0f };
					////float offset_data_2[1920 * 1080 * 3] = { 0.0f };

					//

					//float* int_data_1 = intensity_data_1.data();
					////std::size_t vector_size = intensity_data_1.size();

					//float* int_data_2 = intensity_data_2.data();


					//printf("for  demo warping\n");
					//intensitydata.version = NV_SCANOUT_INTENSITY_DATA_VER;
					//intensitydata.width = 1920;
					//intensitydata.height = 1080;

					//intensitydata.blendingTexture = int_data_1;
					//intensitydata.offsetTexture = NULL;
					//intensitydata.offsetTexChannels = 3;

					//intensitydata_1.version = NV_SCANOUT_INTENSITY_DATA_VER;
					//intensitydata_1.width = 1920;
					//intensitydata_1.height = 1080;
					//intensitydata_1.blendingTexture = int_data_2;
					//intensitydata_1.offsetTexture = NULL;
					//intensitydata_1.offsetTexChannels = 3;



					//
					//int sticky = 0;
					//int sticky_2 = 0;

					//printf("grid setting after the warp\n");
					//error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[0].displayId, &intensitydata, &sticky);
					//error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[1].displayId, &intensitydata_1, &sticky);
					//system("pause");

					//intensitydata.blendingTexture = NULL;
					//error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[0].displayId, &intensitydata, &sticky);
					//intensitydata_1.blendingTexture = NULL;
					//error = NvAPI_GPU_SetScanoutIntensity(gridstopo[1].displays[1].displayId, &intensitydata_1, &sticky);

					//NvAPI_Status status = NvAPI_Mosaic_EnumDisplayGrids(NULL, &gcount_1);
					//if (status == NVAPI_OK && gcount_1 > 0)
					//{
					//	gridstopo_1 = (NV_MOSAIC_GRID_TOPO*)malloc(gcount_1 * sizeof(NV_MOSAIC_GRID_TOPO));

					//	for (NvU32 i = 0; i < gcount_1; ++i)
					//	{
					//		gridstopo_1[i].version = NV_MOSAIC_GRID_TOPO_VER;
					//	}

					//	NvAPI_Status status = NvAPI_Mosaic_EnumDisplayGrids(gridstopo_1, &gcount_1);
					//	if (status == NVAPI_OK)
					//	{
					//		for (NvU32 igrid = 0; igrid < gcount_1; igrid++)
					//		{
					//			NvU32 numdisplays = gridstopo_1[igrid].displayCount; //no of displays in this grid
					//			NvU32 numrows = gridstopo_1[igrid].rows; //no of rows in this grid
					//			NvU32 numcols = gridstopo_1[igrid].columns; //no of columns in this grid
					//			printf("number of diaplays in this grid is = %lu \n ", numdisplays);
					//			printf("number of rows in this grid is= %lu \n", numrows);
					//			printf("number of cols in this grid is= %lu \n", numcols);
					//		}
					//	}
					//}
				
					system("pause");
					//warpingdata.vertices = NULL;
					//warpingdata.numvertices = 0;

					//error = nvapi_gpu_setscanoutwarping(dispids[dispindex].displayId, &warpingdata, &maxnumvertices, &sticky);

					//if (error != NVAPI_OK)  
					//{
					//	NvAPI_GetErrorMessage(error, estring);
					//	printf("nvapi_gpu_setscanoutwarping: %s\n", estring);
					//} 

					//// demo warp and blend antialiasing
					//printf("demo warping and blend antialiasing\n");

					//warpingdata.version =  nv_scanout_warping_ver; 
					//warpingdata.numvertices = maxnumvert;
					//warpingdata.vertexforMat = nv_gpu_warping_vertice_forMat_trianglestrip_xyuvrq;
					//warpingdata.texturerect = &osrect; 


					//// this call does the warp

					//error = nvapi_gpu_setscanoutwarping(dispids[dispindex].displayId, &warpingdata, &maxnumvertices, &sticky);
					//if (error != NVAPI_OK)  
					//{ 
					//	NvAPI_GetErrorMessage(error, estring);
					//	printf("nvapi_gpu_setscanoutwarping: %s\n", estring);
					//}

		   //         // this call changes the warp filter function to bicubic triangular
		   //         printf("changing warp resample method to  bicubic triangular\n");
		   //         float unusedcontainer;
		   //         nv_gpu_scanout_composition_parameter_value parametervalue = nv_gpu_scanout_composition_parameter_value_warping_resampling_method_bicubic_triangular;
		   //         error = nvapi_gpu_setscanoutcompositionparameter(dispids[dispindex].displayId, nv_gpu_scanout_composition_parameter_warping_resampling_method, parametervalue, &unusedcontainer);

		   //                  if (error != NVAPI_OK)  
		   //                  { 
		   //                        NvAPI_GetErrorMessage(error, estring);
		   //                        printf("nvapi_gpu_setscanoutcompositionparameter: %s\n", estring);
		   //                  }

					//system("pause");

		   //         // after a pause, set back to unwarped and un-antialiased
		   //         parametervalue = nv_gpu_scanout_composition_parameter_value_warping_resampling_method_bilinear;
		   //         error = nvapi_gpu_setscanoutcompositionparameter(dispids[dispindex].displayId, nv_gpu_scanout_composition_parameter_warping_resampling_method, parametervalue, &unusedcontainer);

		   //                  if (error != NVAPI_OK)  
		   //                  { 
		   //                        NvAPI_GetErrorMessage(error, estring);
		   //                        printf("nvapi_gpu_setscanoutcompositionparameter: %s\n", estring);
		   //                  }

					//warpingdata.vertices = NULL;
					//warpingdata.numvertices = 0;

					//
					//error = nvapi_gpu_setscanoutwarping(dispids[dispindex].displayId, &warpingdata, &maxnumvertices, &sticky);

					//if (error != NVAPI_OK)  
					//{
					//	NvAPI_GetErrorMessage(error, estring);
					//	printf("nvapi_gpu_setscanoutwarping: %s\n", estring);
					//} 

					//// demonstration of intensity adjustment
					//printf("demo intensity adjustment\n");

					//nv_scanout_intensity_data intensitydata; 
					//// as per pixel intensity control example we specify only one intensity 
					//// value which is interpolated over the scanout area and affecting 
					//// all pixels by that. it's also possible to specify much more intensity
					//// values than scanout pixels.

					//float intensitytexture[12]        = { 0.95f, 0.95f, 0.95f, 0.75f, 0.75f, 0.75f, 0.25f, 0.25f, 0.25f, 0.05f, 0.05f, 0.05f} ;


					//float offsetTexture[2]        = { 0.0f, 0.1f} ;
					//intensitydata.version           = nv_scanout_intensity_data_ver;
					//intensitydata.width             = 4;
					//intensitydata.height            = 1;
					//intensitydata.blendingTexture   = intensitytexture;
					//intensitydata.offsetTexture		= offsetTexture;
					//intensitydata.offsetTexChannels = 1;kl

					//int sticky = 0;

					//// this call does the intensity map
					//error =  NvAPI_GPU_SetScanoutIntensity(dispids[dispindex].displayId, &intensitydata, &sticky);

					//if (error != NVAPI_OK)  
					//{
					//	NvAPI_GetErrorMessage(error, estring);
					//	printf("NvAPI_GPU_SetScanoutIntensity: %s\n", estring);
					//} 

					//system("pause");

					//// after a pause, set back to normal intensity

					//intensitydata.blendingTexture = NULL;

					//error =  NvAPI_GPU_SetScanoutIntensity(dispids[dispindex].displayId, &intensitydata, &sticky);
					//if (error != NVAPI_OK)  
					//{
					//	NvAPI_GetErrorMessage(error, estring);
					//	printf("NvAPI_GPU_SetScanoutIntensity: %s\n", estring);
					//} 
				} //end of for displays

				//
				//warpingdata.vertices = NULL;

				//error = nvapi_gpu_setscanoutwarping(0x80061081, &warpingdata, &maxnumvertices, &sticky);
				//if (error != NVAPI_OK)
				//{
				//	NvAPI_GetErrorMessage(error, estring);
				//	printf("nvapi_gpu_setscanoutwarping: %s\n", estring);
				//}
				//system("pause");
				//error = nvapi_gpu_setscanoutwarping(0x80061083, &warpingdata, &maxnumvertices, &sticky);
				//if (error != NVAPI_OK)
				//{
				//	NvAPI_GetErrorMessage(error, estring);
				//	printf("nvapi_gpu_setscanoutwarping: %s\n", estring);
				//}
				//system("pause");
				//
				delete[] dispids;
			}	//end of loop gpus

		}

	}
*/
